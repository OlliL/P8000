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
 * $Id: rtc72421.c,v 1.6 2009/08/16 14:52:52 olivleh1 Exp $
 */
 
#include <time.h>
#include "rtc72421.h"

void			wrnibble();
void			wrbyte();
int			rdnibble();
int			rdbyte();
extern long		timegm();
extern struct tm	*gmtime();


int
r421init()
{
	outb(R421REGD,R421IRQF);
	outb(R421REGE,R421CLR);
	outb(R421REGF,R4212412);
}


void
r421set(time)
long time;
{
	struct tm *clktime;

	clktime = gmtime(&time);
	if(clktime->tm_year > 100)
		clktime->tm_year -= 100;
	clktime->tm_mon++;
	outb(R421REGF,R421RST|R421STOP|R4212412);
	wrbyte(R421SS1x,R421SSx1,clktime->tm_sec);
	wrbyte(R421MI1x,R421MIx1,clktime->tm_min);
	wrbyte(R421HH1x,R421HHx1,clktime->tm_hour);
	wrbyte(R421DD1x,R421DDx1,clktime->tm_mday);
	wrbyte(R421MM1x,R421MMx1,clktime->tm_mon);
	wrbyte(R421YY1x,R421YYx1,clktime->tm_year);
}

void
r421start()
{
	outb(R421REGF,R4212412);
}

long
r421get()
{
	long time = 0;
	int tm_year, tm_mday, tm_mon, tm_sec, tm_min, tm_hour;

	tm_sec  = rdbyte(R421SS1x,R421SSx1);
	tm_min  = rdbyte(R421MI1x,R421MIx1);
	tm_hour = rdbyte(R421HH1x,R421HHx1);
	tm_mday = rdbyte(R421DD1x,R421DDx1);
	tm_mon  = rdbyte(R421MM1x,R421MMx1);
	tm_year = rdbyte(R421YY1x,R421YYx1);

	if (tm_year < 70)
		tm_year+=100;
	tm_mon --;

	time = timegm(tm_year,tm_mon,tm_mday,tm_hour,tm_min,tm_sec);

	return(time);
}


r421bsy()
{
busy:
	outb(R421REGD,R421HOLD);
	if( rdnibble(R421REGD) & R421BUSY ) {
		outb(R421REGD,R421CLR);
		goto busy;
	}
	outb(R421REGD,R421CLR);
	return;	
}


void
wrnibble(addr,nibble)
unsigned addr;
int nibble;
{
	outb(addr, nibble);
}
int
rdnibble(addr)
unsigned addr;
{
	return (inb(addr)&~0xf0);
}


void
wrbyte(addr1,addr2,byte)
unsigned addr1;
unsigned addr2;
int byte;
{
	r421bsy();
	wrnibble(addr1,byte/10);
	r421bsy();
	wrnibble(addr2,byte%10);
}

int
rdbyte(addr1,addr2)
unsigned addr1;
unsigned addr2;
{
	register a,b;

	r421bsy();
	a=rdnibble(addr2);
	r421bsy();
	b=rdnibble(addr1);
        return(a + b*10);
}
