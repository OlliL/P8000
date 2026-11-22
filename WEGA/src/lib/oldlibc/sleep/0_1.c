/* sleep of V7 (alarm and pause, setret/longret for setjmp/longjmp); the sleep of bin/sleep and bin/tail (07/82) */
#include <signal.h>

static int jmp[9];	/* the 9 words setret stores (ret_buf of setret.h has 10) */

sleep(n)
unsigned n;
{
	int sleepx();
	unsigned altime;
	int (*alsig)() = SIG_DFL;

	if (n==0)
		return;
	altime = alarm(1000);	/* time to maneuver */
	if (setret(jmp)) {
		signal(SIGALRM, alsig);
		alarm(altime);
		return;
	}
	if (altime) {
		if (altime > n)
			altime -= n;
		else {
			n = altime;
			altime = 1;
		}
	}
	alsig = signal(SIGALRM, sleepx);
	alarm(n);
	for(;;)
		pause();
	/*NOTREACHED*/
}

static
sleepx()
{
	longret(jmp, 1);
}
