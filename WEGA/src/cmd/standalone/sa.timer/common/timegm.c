/*-
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
 * $Id: timegm.c,v 1.2 2009/08/12 19:27:31 olivleh1 Exp $
 */
 
#include <time.h>

extern struct tm	*gmtime();

long
timegm(clktime)
struct tm *clktime;
{
	register long time;
	register i;
	static  int  dmsize[] =         /* Tage/Monat (kumulierend) */
		{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };

	time = 0;
	if (clktime->tm_mday > 31 || clktime->tm_mon > 12)
		return(time);
	i = dmsize[clktime->tm_mon];
	if (clktime->tm_year%4 == 0 && clktime->tm_mon > 2) i++;
	if (clktime->tm_year > 137) return(time);
	time = (clktime->tm_year-69)/4;
	time += 365*(clktime->tm_year-70)+i;
	time += clktime->tm_mday - 1;

	if (clktime->tm_sec > 59 || clktime->tm_min > 59 || clktime->tm_hour > 23)
		return(time);
	time = 24*time + clktime->tm_hour;
	time = 60*time + clktime->tm_min;
	time = 60*time + clktime->tm_sec;
	return(time);
}
