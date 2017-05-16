/*
 * This code is in public domain
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/reboot.h>
#include <libgen.h>

static int usage(void)
{
	puts("usage: shutdown [-rhpcCS] [-t sec]");
	puts("usage: reboot");
	puts("usage: halt");
	puts("usage: poweroff");
	puts("  -c: c-a-d INTs init");
	puts("  -C: c-a-d quickly reboots");
	puts("  -S: do not call sync()");
	puts("  -t sec: delay shutdown by sec");
	return 1;
}

int main(int argc, char **argv)
{
	int c, r, h, p, S;
	int t;

	t = r = h = p = S = 0;

	if (!strcmp(basename(*argv), "reboot")) { kill(1, SIGALRM); return 0; }
	if (!strcmp(basename(*argv), "halt")) { kill(1, SIGABRT); return 0; }
	if (!strcmp(basename(*argv), "poweroff")) { kill(1, SIGQUIT); return 0; }

	while ((c = getopt(argc, argv, "rhpcCSt:")) != -1) {
		switch (c) {
			case 'r': r = 1; break;
			case 'h': h = 1; break;
			case 'p': p = 1; break;
			case 'c': r = 2; break;
			case 'C': r = 3; break;
			case 'S': S = 1; break;
			case 't': t = atoi(optarg); break;
			default: return usage();
		}
	}

	if (t) sleep(t);

	if (!S) sync();
	if (r) {
		if (r == 2) reboot(RB_DISABLE_CAD);
		else if (r == 3) reboot(RB_ENABLE_CAD);
		else reboot(RB_AUTOBOOT);
	}
	else if (h) reboot(RB_HALT_SYSTEM);
	else if (p) reboot(RB_POWER_OFF);
	else return usage();

	return 0;
}
