/*
 * signal.c - a test of the signal model of z8k-emu (wega.c: sig_deliver, sysiret; src/uts/sys/sig.c psig): built by
 * tests/run.sh with cc and with scc under the emulator, its output compared with tests/signal.out. What it shows:
 * the handler runs and gets the number; the handler goes back to the default after it ran (signal() still returns
 * the function: libc's own table, signal.s); ignore; pause and read end with EINTR; sleep (alarm and pause); the wait
 * status of a child killed by a signal has the WEGA number (15, 17); a handler in a child; registers and flags kept
 * across handlers; an ignored signal stays ignored through exec (sh does not die by kill -16).
 */
#include <stdio.h>
#include <signal.h>
#include <errno.h>
extern int errno;
int got, n;
long sum;
h(s) { got = s; n++; }
loop(s) { n++; signal(SIGALRM, loop); if (n < 3) alarm(1); }
main()
{
	int st, pid, fd[2], r, (*old)();
	char c;
	long i;
	setbuf(stdout, (char *)0);
	old = signal(SIGUSR1, h);
	printf("old %d\n", old == SIG_DFL);
	kill(getpid(), SIGUSR1);
	printf("1 got %d n %d\n", got, n);
	old = signal(SIGUSR1, SIG_IGN);
	printf("reset %d\n", old == SIG_DFL);
	kill(getpid(), SIGUSR1);
	printf("2 ignored n %d\n", n);
	signal(SIGALRM, h);
	alarm(1);
	r = pause();
	printf("3 pause %d errno %d got %d\n", r, errno, got);
	pipe(fd);
	signal(SIGALRM, h);
	alarm(1);
	r = read(fd[0], &c, 1);
	printf("4 read %d errno %d\n", r, errno);
	sleep(1);
	printf("5 slept\n");
	if ((pid = fork()) == 0) { pause(); exit(0); }
	kill(pid, SIGTERM); wait(&st); printf("6 status %o\n", st);
	if ((pid = fork()) == 0) { pause(); exit(0); }
	kill(pid, SIGUSR2); wait(&st); printf("7 status %o\n", st);
	if ((pid = fork()) == 0) { signal(SIGINT, h); kill(getpid(), SIGINT); exit(got); }
	wait(&st); printf("8 status %o\n", st);
	n = 0; signal(SIGALRM, loop); alarm(1);
	for (i = 0, sum = 0; n < 3; i++) sum += i * 3 + 1;
	{ long j, s2; for (j = 0, s2 = 0; j < i; j++) s2 += j * 3 + 1;
	printf("9 n %d check %d\n", n, sum == s2 ? 1 : 0); }
	if ((pid = fork()) == 0) { execl("/bin/sh", "sh", "-c", "kill -16 $$; echo alive", 0); }
	wait(&st); printf("10 status %o\n", st);
	exit(0);
}
