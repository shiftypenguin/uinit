/*
 * This code is in public domain
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

typedef void (*sighandler_t)(int);

static void sighandl(int sig)
{
	switch (sig) {
		case SIGTERM:
		case SIGINT:
			exit(0);
		case SIGCHLD:
			while (waitpid(0, NULL, WNOHANG) >= 0);
			break;
	}
}

int main(int argc, char **argv)
{
	sigset_t set;
	pid_t x, y;
	int f;
	int c; char *tty = "/dev/console"; int t = 0, T = -1;

	opterr = 0;
	while ((c = getopt(argc, argv, "c:t:T:")) != -1) {
		switch (c) {
			case 'c': tty = optarg; break;
			case 't': t = atoi(optarg); break;
			case 'T': T = atoi(optarg); break;
			default: exit(1);
		}
	}

	if (!*(argv+optind)) return 1;

	sigfillset(&set);
	sigdelset(&set, SIGCHLD);
	sigdelset(&set, SIGTERM);
	sigdelset(&set, SIGINT);
	sigprocmask(SIG_BLOCK, &set, NULL);
	signal(SIGCHLD, sighandl);
	signal(SIGTERM, sighandl);
	signal(SIGINT, sighandl);

	if (fork()) return 0;

	setsid();
	while (1) {
		if ((x = fork())) {
			while (1) {
				y = waitpid(x, NULL, 0);
				if (y == x) break;
				if (y == -1 && errno != EINTR) break;
			}
		}
		else {
			setsid();
			if ((f = open(tty, O_RDWR | O_NOCTTY)) != -1) {
				close(0);
				dup2(f, 0);
				close(1);
				dup2(f, 1);
				close(2);
				dup2(f, 2);
				if (f > 2) close(f);
			}
			else return 2;

			sigfillset(&set);
			sigprocmask(SIG_UNBLOCK, &set, NULL);

			argv += optind;
			execv(*argv, argv);
			return 127;
		}

		usleep(t);
		if (T != -1) T--;
		if (T == 0) break;
	}

	return 0;
}
