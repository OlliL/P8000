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
 * $Id: u130.h,v 1.2 2009/08/07 20:02:59 olivleh1 Exp $
 */

#ifndef U130_H
#define U130_H 1

#define STR_U130	0xbf
#define MSK_U130	0x00

#define U130BASE	0xfef1	/* base address */
#define U130YYx1	0xfef5	/* 1-year digit register */
#define U130YY1x	0xfef7	/* 10-year digit register */
#define U130DDx1	0xfef9	/* 1-day digit register */
#define U130DD1x	0xfefb	/* 10-day digit register */
#define U130MMx1	0xfefd	/* 1-month digit register */
#define U130MM1x	0xfeff	/* 10-month digit register */
#define U130SSx1	0xfef5	/* 1-second digit register */
#define U130SS1x	0xfef7	/* 10-second digit register */
#define U130HHx1	0xfefd	/* 1-hour digit register */
#define U130HH1x	0xfeff	/* 10-hour digit register */
#define U130MIx1	0xfef9	/* 1-minute digit register */
#define U130MI1x	0xfefb	/* 10-minute digit register */

int 	u130_init();
long	u130_get();
int 	u130_set();
void	u130_start();

#endif
