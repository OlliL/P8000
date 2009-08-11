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
 * $Id: rtc72421.h,v 1.3 2009/08/11 19:18:03 olivleh1 Exp $
 */

#ifndef R421_H
#define R421_H 1

#define STR_R421	0xc
#define MSK_R421	0xf0

#define R421BASE	0xfde1	/* base address */
#define R421YYx1	0xfdf5	/* 1-year digit register */
#define R421YY1x	0xfdf7	/* 10-year digit register */
#define R421DDx1	0xfded	/* 1-day digit register */
#define R421DD1x	0xfdef	/* 10-day digit register */
#define R421MMx1	0xfdf1	/* 1-month digit register */
#define R421MM1x	0xfdf3	/* 10-month digit register */
#define R421SSx1	0xfde1	/* 1-second digit register */
#define R421SS1x	0xfde3	/* 10-second digit register */
#define R421HHx1	0xfde9	/* 1-hour digit register */
#define R421HH1x	0xfdeb	/* 10-hour digit register */
#define R421MIx1	0xfde5	/* 1-minute digit register */
#define R421MI1x	0xfde7	/* 10-minute digit register */
#define R421DOW		0xfdfa	/* not used */
#define R421REGD	0xfdfb	/* 30-s ADJ, IRQ Flag, BUSY, HOLD */
#define R421REGE	0xfdfd	/* t1, t0, ITRPT/STND, MASK */
#define R421REGF	0xfdff	/* TEST, 24/12, STOP, RESET */

/* register F */
#define R421RST		0x01
#define	R421STOP	0x02
#define R4212412	0x04
#define R421TEST	0x08

/* register D */
#define R421HOLD	0x01
#define	R421BUSY	0x02
#define R421IRQF	0x04
#define R421ADJ		0x08

int 	rtc72421_init();
long	rtc72421_get();
void 	rtc72421_set();
void	rtc72421_start();

#endif
