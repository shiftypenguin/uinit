/*
 * This code is in public domain
 */

#define _XOPEN_SOURCE 700
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reboot.h>

typedef void (*sighandler_t)(int);

static void sighandl(int sig)
{
	if (fork()) return;

	switch (sig) {
		case SIGINT:
		execv("/etc/init/cad", (char *[]){"cad", NULL});
		break;

		case SIGALRM:
		execv("/etc/init/reboot", (char *[]){"reboot", NULL});
		break;

		case SIGQUIT:
		execv("/etc/init/poweroff", (char *[]){"poweroff", NULL});
		break;

		case SIGABRT:
		execv("/etc/init/shutdown", (char *[]){"shutdown", NULL});
		break;

#ifdef SIGPWR
		case SIGPWR:
		execv("/etc/init/pwrfail", (char *[]){"pwrfail", NULL});
		break;
#endif
	}
}

int main(void)
{
	sigset_t set;
	int status;

	if (getpid() != 1) return 1;

	if (!access("/etc/init/altinit", X_OK) && !getenv("_INIT"))
		execv("/etc/init/altinit", (char *[]){"init", NULL});

	reboot(RB_DISABLE_CAD);

	sigfillset(&set);
	sigprocmask(SIG_BLOCK, &set, NULL);

	if (fork()) {
		sigprocmask(SIG_UNBLOCK, &set, NULL);
		sigdelset(&set, SIGINT);
		sigdelset(&set, SIGALRM);
		sigdelset(&set, SIGQUIT);
		sigdelset(&set, SIGABRT);
#ifdef SIGPWR
		sigdelset(&set, SIGPWR);
#endif
		sigprocmask(SIG_BLOCK, &set, NULL);
		signal(SIGINT, sighandl);
		signal(SIGALRM, sighandl);
		signal(SIGQUIT, sighandl);
		signal(SIGABRT, sighandl);
#ifdef SIGPWR
		signal(SIGPWR, sighandl);
#endif

		for (;;) wait(&status);
	}

	sigprocmask(SIG_UNBLOCK, &set, NULL);

	setsid();
	setpgid(0, 0);
	return execv("/etc/init/boot", (char *[]){"boot", NULL});
}
