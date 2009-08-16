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
 * $Id: timegm.c,v 1.3 2009/08/16 14:52:52 olivleh1 Exp $
 */
 
#include <time.h>

long
timegm(tm_year,tm_mon,tm_mday,tm_hour,tm_min,tm_sec)
register tm_year;
register tm_mon;
register tm_mday;
register tm_hour;
register tm_min;
register tm_sec;
{
	register long time;
	register i;
	static  int  dmsize[] =         /* Tage/Monat (kumulierend) */
		{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };

	time = 0;
	if (tm_mday > 31 || tm_mon > 12)
		return(time);
	i = dmsize[tm_mon];
	if (tm_year%4 == 0 && tm_mon > 2) i++;
	if (tm_year > 137) return(time);
	time = (tm_year-69)/4;
	time += 365*(tm_year-70)+i;
	time += tm_mday - 1;

	if (tm_sec > 59 || tm_min > 59 || tm_hour > 23)
		return(time);
	time = 24*time + tm_hour;
	time = 60*time + tm_min;
	time = 60*time + tm_sec;
	return(time);
}
