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
 * $Id: timer.c,v 1.6 2009/08/22 15:45:33 olivleh1 Exp $
 */


#include <time.h>
#include "rtc72421.h"
#include "u130.h"
#include "timer.h"

long
gettime()
{
	register long time = 0;
	register i;
	struct clock_type *t;
	int a,b;
	int		clock_min_year;
	int		clock_max_year;
	int		(*clock_finit)();
	long		(*clock_fget)();
	void		(*clock_fset)();
	void		(*clock_fstart)();
	int		clock_found = 0;

	t = clock_devs;

	for( i = 0 ; i < sizeof(clock_devs) / sizeof(clock_devs[0]); i++, t++) {
		a = inb(t->clock_addr)&~t->clock_det_msk;
		b = t->clock_det_str;
 		if(a == b) {
			printf("%s found\n",t->clock_name);
			clock_found    = 1;
			clock_finit    = t->clock_finit;
			clock_fget     = t->clock_fget;
			clock_fset     = t->clock_fset;
			clock_fstart   = t->clock_fstart;
			clock_min_year = t->clock_min_year;
			clock_max_year = t->clock_max_year;
			break;
		}
	}

	/* function pointer assigned? => RTC found?*/
	if (clock_found == 1) {
		clock_finit();
		time = clock_fget();
	}

	return(time);
}
