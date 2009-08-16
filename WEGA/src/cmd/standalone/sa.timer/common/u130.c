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
 * $Id: u130.c,v 1.11 2009/08/16 14:52:52 olivleh1 Exp $
 */
 
#include <time.h>
#include "u130.h"

int			rdvalue();
int			invalue();
void			outvalue();
extern long		timegm();
extern struct tm	*gmtime();

int
u130init()
{
	return(0);
}

void
u130set(time)
long time;
{
	struct tm *clktime;

	clktime = gmtime(&time);
	if(clktime->tm_year > 100)
		clktime->tm_year -= 100;
	clktime->tm_mon ++;

	outvalue(0x02);		/* Datum einstellen */
	outvalue(0x03);		/* Korrektur Monat */
	outvalue(0x00);
	while ((invalue(U130MMx1,1) + invalue(U130MM1x,1)*10) != clktime->tm_mon)
		;
	outvalue(0x01);		/* Korrektur Zehnerstelle Tag */
	outvalue(0x00);
	while (invalue(U130DD1x,1) != clktime->tm_mday/10)
		;
	outvalue(0x01);		/* Korrektur Einerstelle Tag */
	outvalue(0x00);
	while (invalue(U130DDx1,1) != clktime->tm_mday%10)
		;
	outvalue(0x01);		/* Korrektur Zehnerstelle Jahr */
	outvalue(0x00);
	while (invalue(U130YY1x,1) != clktime->tm_year/10)
		;
	outvalue(0x01);		/* Korrektur Einerstelle Jahr */
	outvalue(0x00);
	while (invalue(U130YYx1,1) != clktime->tm_year%10)
		;
	outvalue(0x02);		/* Korrektur Ende */
	outvalue(0x00);

	outvalue(0x01);		/* Zeit stellen */
	outvalue(0x00);
	outvalue(0x02);		/* Sekunden auf Null */
	outvalue(0x00);
	outvalue(0x01);
	outvalue(0x00);
	outvalue(0x01);		/* Korrektur Stunden */
	outvalue(0x00);
	while ((invalue(U130HHx1,1) + invalue(U130HH1x,1)*10) != clktime->tm_hour)
		;
	outvalue(0x01);		/* Korrektur Zehner-Minuten */
	outvalue(0x00);
	while (invalue(U130MI1x,1) != clktime->tm_min/10)
		;
	outvalue(0x01);		/* Korrektur Einer-Minuten */
	outvalue(0x00);
	while (invalue(U130MIx1,1) != clktime->tm_min%10)
		;
	outvalue(0x01);		/* Korrektur Stop */
	outvalue(0x00);
}

void
u130start()
{
	outvalue(0x01);		/* Start der Zeit */
	outvalue(0x00);
}

long
u130get()
{
	long time;
	int tm_year, tm_mday, tm_mon, tm_sec, tm_min, tm_hour;

	outvalue(0x02);				/* Umschalten auf Datum */
	tm_year = rdvalue(U130YYx1);
	if (tm_year < 70)
		tm_year+=100;
	tm_mday = rdvalue(U130DDx1);
	tm_mon = rdvalue(U130MMx1);
	tm_mon --;
	outvalue(0x00);
	tm_sec = inb(U130MM1x) & 0x28;	/* Umschalten auf Zeit */
	if (tm_sec == 0x08 || tm_sec == 0x20) {
		outvalue(0x02);
		outvalue(0x00);
	}
	while ((tm_sec = rdvalue(U130SSx1)) == 59);
	tm_min = rdvalue(U130MIx1);
	tm_hour = rdvalue(U130HHx1);

	time = timegm(tm_year,tm_mon,tm_mday,tm_hour,tm_min,tm_sec);
	return(time);
}

void
outvalue(t)
register t;
{
	outb(U130BASE, t);
	t = 10000;
	while (t--)
		;
}

int
invalue(addr, flag)
register addr;
register flag;
{
	register seg7, takt;
	
	seg7 = inb(addr);
	if (seg7 != inb(addr))
		seg7 = inb(addr);
	takt = seg7 & 0x80;		/* nur gueltig bei 6. Stelle */
	if (seg7 & 0x20)
		seg7 = ~seg7;
	if (addr == U130MM1x)		/* 6.Stelle */
		seg7 &= 0x07;
	seg7 &= 0x1f;
	if (seg7 == 0x0f)
		return(0);
	if (seg7 == 0x00) {
		if (flag == 0)
			return(0); 
		if (addr == U130MM1x && takt)
			return(0);
	}
	if (seg7 == 0x02)
		return(1);
	if (seg7 == 0x17 || seg7 == 0x05)
		return(2);
	if (seg7 == 0x13)
		return(3);
	if (seg7 == 0x1a)
		return(4);
	if (seg7 == 0x19)
		return(5);
	if (seg7 == 0x1d)
		return(6);
	if (seg7 == 0x03)
		return(7);
	if (seg7 == 0x1f)
		return(8);
	if (seg7 == 0x1b)
		return(9);
	return(0xff);
}

int
rdvalue(addr)
unsigned addr;
{
	return(invalue(addr,0) + invalue(addr+2,0)*10);
}
