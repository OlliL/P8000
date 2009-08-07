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
 * $Id: rtc72421.c,v 1.2 2009/08/07 20:02:59 olivleh1 Exp $
 */
 
#include <time.h>
#include "rtc72421.h"

void			wrnibble();
void			wrbyte();
int			rdnibble();
int			rdbyte();
extern long		timegm();
extern struct tm	*gmtime();


#ifdef NOTYET
int
rtc72421_init()
{
	return(0);
}


int
rtc72421_set(time)
long time;
{
	struct tm *clktime;

	clktime = gmtime(&time);
	if(clktime->tm_year > 100)
		clktime->tm_year -= 100;
	clktime->tm_mon++;

	return(0);
}

void
rtc72421_start()
{
}

long
rtc72421_get()
{
	long time;
	struct tm *clktime;
	clktime->tm_year = 0;
	clktime->tm_mday = 0;
	clktime->tm_mon  = 0;
	while ((clktime->tm_sec = 0) == 59);
	clktime->tm_min  = 0;
	clktime->tm_hour = 0;

	time = timegm(clktime);

	return(time);
}


void
wrnibble(addr,nibble)
unsigned addr;
int nibble;
{
}

void
wrbyte(addr1,addr2,byte)
unsigned addr1;
unsigned addr2;
int byte;
{
	wrnibble(addr1,byte/10);
	wrnibble(addr2,byte%10);
}

int
rdnibble(addr)
unsigned addr;
{
	return (0);
}
int
rdbyte(addr1,addr2)
unsigned addr1;
unsigned addr2;
{
        return(rdnibble(addr1) + rdnibble(addr2)*10);
}
#endif
