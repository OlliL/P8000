/* sleep of V7 (alarm and pause, setret/longret for setjmp/longjmp) as 0_1, with the ret_buf of setret.h (10 words); the sleep
   of bin/nice (01/83, ccv1) */
#include <signal.h>
#include <setret.h>

static ret_buf jmp;

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
