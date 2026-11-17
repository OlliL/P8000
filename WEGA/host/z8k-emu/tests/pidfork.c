/* Z8K_PIDDIR (tests/pidfork.env): the process numbers of the guest are the same in every run: 100 for the first process, for a
   child a number made of its parent's number and the count of the parent's forks (the numbers 7051 and 7209 of the .out), wait gives the number of the child, getppid that of the parent, kill reaches a child by its number */
#include <stdio.h>
#include <signal.h>

main()
{
	int p, q, st, w, ppid;

	setbuf(stdout, (char *)0);
	printf("parent %d\n", getpid());
	p = fork();
	if (p == 0) {
		pause();
		exit(0);
	}
	q = fork();
	if (q == 0) {
		ppid = getppid();
		exit(ppid == 100 ? 3 : 4);
	}
	printf("forked %d %d\n", p, q);
	w = wait(&st);
	printf("wait gave %d status %d\n", w, (st >> 8) & 0xFF);
	kill(p, SIGTERM);
	w = wait(&st);
	printf("wait gave %d signal %d\n", w, st & 0x7F);
	exit(0);
}
