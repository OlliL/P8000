/*
 * time - time a command
 *
 * WEGA's time (no source; written from /bin/time, 01/83): the System III start (fork, execvp, wait), its own output:
 * user and system seconds with tenths, the real time as [h:]mm:ss, the share of the processor; -v names the fields.
 * The names of the functions are guesses.
 */

static char sccsid[] = "@[$]time.c\t2.1  01/22/83 01:33:50 - 87wega3.2";

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/times.h>

int	vflag = 0;
extern	int errno;
extern	char *sys_errlist[];

main(argc, argv)
char **argv;
{
	struct tms buffer;
	int status;
	int i = 1;
	long before, after;
	register p;

	if(argc <= 1)
		exit(0);
	if(argv[1][0] == '-' && argv[1][1] == 'v') {
		vflag++;
		i = 2;
	}
	time(&before);
	p = fork();
	if(p == -1) {
		fprintf(stderr, "time: cannot fork -- try again.\n");
		exit(2);
	}
	if(p == 0) {
		execvp(argv[i], &argv[i]);
		fprintf(stderr, "%s: %s\n", argv[i], sys_errlist[errno]);
		exit(2);
	}
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	wait(&status);
	time(&after);
	if((status & 0377) != 0)
		fprintf(stderr, "Command terminated abnormally.\n");
	times(&buffer);
	printt(after - before, &buffer);
	exit(status >> 8);
}

long
cputime(t)
struct tms *t;
{
	return(t->tms_cutime + t->tms_cstime);
}

printt(real, t)
long real;
register struct tms *t;
{
	prtime(t->tms_cutime);
	fprintf(stderr, vflag ? " user-sec., " : "u ");
	prtime(t->tms_cstime);
	fprintf(stderr, vflag ? " system-sec., " : "s ");
	prreal(real);
	fprintf(stderr, vflag ? " %d%% of capacity\n" : " %d%%\n",
		(int)(cputime(t) * 5 / ((real ? real : 1L) * 3)));
}

prtime(ticks)
long ticks;
{
	ticks += 3;
	fprintf(stderr, "%d.%d", (int)(ticks / 60), (int)(ticks % 60 * 10 / 6));
}

prreal(t)
long t;
{
	register h;

	if((h = t / 3600) != 0) {
		fprintf(stderr, "%d:", h);
		h = t % 3600;
		pr2(h / 60);
	} else {
		h = t;
		fprintf(stderr, "%d", h / 60);
	}
	h %= 60;
	fprintf(stderr, ":");
	pr2(h);
	fprintf(stderr, vflag ? " real-time," : "");
}

pr2(n)
register n;
{
	fprintf(stderr, "%d%d", n / 10, n % 10);
}
