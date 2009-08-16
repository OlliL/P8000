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
 * $Id: timer.h,v 1.4 2009/08/16 11:11:00 olivleh1 Exp $
 */

#ifndef TIMER_H
#define TIMER_H 1

struct clock_type {
        int		clock_addr;
        int		clock_det_msk;
        int		clock_det_str;
	int		clock_min_year;
	int		clock_max_year;
        char		*clock_name;
	int		(*clock_finit)();
	long		(*clock_fget)();
	void		(*clock_fset)();
	void		(*clock_fstart)();
};

char    estring[20];

static struct clock_type clock_devs[] = {
        { U130BASE, MSK_U130, STR_U130, 80, 119,
	  "RFT U130X RTC",
	  u130init, u130get, u130set, u130start
	},
	{ R421BASE, MSK_R421, STR_R421, 70, 137,
	  "Seiko Epson RTC-72421",
	  r421init, r421get, r421set, r421start
	},
};

#endif
