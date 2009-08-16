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
 * $Id: timer.c,v 1.1 2009/08/16 11:11:00 olivleh1 Exp $
 */
 

#include "time.h"
#include "rtc72421.h"

void			wrnibble();
void			wrbyte();
int			rdnibble();
int			rdbyte();

static	int  dmsize[] =		/* Tage/Monat (kumulierend) */
	{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };

long
gettime()
{
	register long time;
	register d1, d2, d3;
	register i;

	time = 0;
	r421init();

	d1 = rdbyte(R421YY1x,R421YYx1);		/* Jahr */
	d2 = rdbyte(R421DD1x,R421DDx1);		/* Tag */
	d3 = rdbyte(R421MM1x,R421MMx1);		/* Monat */
	if (d1 > 100 || d2 > 31 || d3 > 12)
		return(time);
	i = dmsize[d3-1];
	if (d1%4 == 0 && d3 > 2) i++;
	if (d1 < 70) d1 += 100;
	if (d1 > 137) return(time);
	time = (d1-69)/4;
	time += 365*(d1-70)+i;
	time += d2 - 1;

	while ((d1 = rdbyte(R421SS1x,R421SSx1)) == 59)
		;			/* Sekunden */
	d2 = rdbyte(R421MI1x,R421MIx1);		/* Minuten */
	d3 = rdbyte(R421HH1x,R421HHx1);		/* Stunden */
	if (d1 > 59 || d2 > 59 || d3 > 23)
		return(time);
	time = 24*time + d3;
	time = 60*time + d2;
	time = 60*time + d1;
	return(time);
}



r421init()
{
	outb(R421REGD,R421IRQF);
	outb(R421REGE,R421CLR);
	outb(R421REGF,R4212412);
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
