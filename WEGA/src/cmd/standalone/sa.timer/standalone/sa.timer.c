/*
 * Copyright (c) 2009 Oliver Lehmann <oliver@FreeBSD.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id: sa.timer.c,v 1.9 2009/08/12 16:29:42 olivleh1 Exp $
 */
 

#include "time.h"
#include "u130.h"
#include "rtc72421.h"
#include "timer.h"

long		timegm();
struct tm	*gmtime();
long		u130_get();

int		(*time_init)();
long		(*time_get)();
void		(*time_set)();
void		(*time_start)();

main()
{
	register long time;
	register i;
	struct clock_type *t;
	int a,b; 	 

	t = clock_devs;

	for( i = 0 ; i < sizeof(clock_devs) / sizeof(clock_devs[0]); i++, t++) {
		a = inb(t->clock_addr)&~t->clock_det_msk;
		b = t->clock_det_str;
 		if(a == b) {
			printf("%s found\n",t->clock_name);
			time_init  = t->time_init;
			time_get   = t->time_get;
			time_set   = t->time_set;
			time_start = t->time_start;
			break;
		}
	}

	if (time_init > 0) {		/* function pointer assigned? */
		time_init();		/* init RTC */
		time = time_get();	/* Uhrenmodul lesen */
		outtime(time);		/* Datum/Zeit ausgeben */
		settimer(time);		/* Uhrenmodul neu programmieren */
		time = time_get();	/* Uhrenmodul lesen */
		outtime(time);		/* Datum/Zeit ausgeben */
		exit(0);
	} else {
		printf("Timer not available\n");
		exit(1);
	}

}

getdat(k)
register k;
{
	register i, c, n;

	n = 0;
	for (i = 0; c = estring[i]; i++) {
		if (k-1 == 0) {
			if (c == '/' || c== ':') return(n);
			if (c < '0' || c > '9') return(-1);
			n = n*10 + (c-'0');
		} else
			if (c == '/' || c == ':') k--;
	}
	return(n);
}

outtime(time)
long time;
{
	struct tm *clktime;

	clktime = gmtime(&time);
	clktime->tm_year += 1900;
	clktime->tm_mon ++;
	printf("Date is: %d/%d/%d\n",     clktime->tm_mon,  clktime->tm_mday, clktime->tm_year);
	printf("Time is: %d:%d:%d GMT\n", clktime->tm_hour, clktime->tm_min,  clktime->tm_sec);
}

#ifdef DEBUG
prtime(time)
long time;
{
	long time1,time3;
	time1 = time/100000L;
	time3 = time%100000L;
	printf("time: %d|%d|%d|%d\n",(int)(time1/100L),(int)(time1%100L),(int)(time3/100L),(int)(time3%100L));
}
#endif

settimer(time)
long time;
{
	struct tm *clktime;
	long time2;
	register i;

	clktime = gmtime(&time);
loop1:
	printf("Enter new Date (MM/DD/YY) : ");
	gets(estring, sizeof estring);
	if (estring[0] == 0)
		goto loop2;
	clktime->tm_mon  = getdat(1)-1;
	clktime->tm_mday = getdat(2);
	clktime->tm_year = getdat(3);
	if (clktime->tm_year < 70 )
		clktime->tm_year += 100;

	if (clktime->tm_mon<0 || clktime->tm_mon>11 || clktime->tm_mday<1 || clktime->tm_mday>31)
		goto loop1;
		
loop2:
	printf("Enter new Time in GMT (HH:MM) : ");
	gets(estring, sizeof estring);
	if (estring[0] == 0)
		return;
	clktime->tm_hour = getdat(1);
	clktime->tm_min  = getdat(2);
	clktime->tm_sec  = 0;
	if (clktime->tm_min<0 || clktime->tm_min>59 || clktime->tm_hour<0 || clktime->tm_hour>23)
		goto loop2;


	time2 = timegm(clktime);
	time_set(time2);
	printf("Enter Return to start time : ");
	gets(estring, 1);
	time_start();
}
