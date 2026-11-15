#ifndef SEG
/* @[$] scanf.c	2.1  09/12/83 11:16:36 - 87wega3.2 */
#else
/* @[$] scanf.c	2.1  08/17/83 11:10:12 - 87wega3.2 */
#endif

#include	<stdio.h>

#ifndef SEG
scanf(fmt, a1, a2, a3, a4, a5, args)
#else
scanf(fmt, a1, a2, args)
#endif
char *fmt;
#ifdef SEG
int *a1, *a2;
#endif
{
#ifndef SEG
	int regs[5];
#else
	struct { int *r0, *r1, *r2, *r3, *r4; } regs;
#endif

#ifndef SEG
	regs[0] = a1;
	regs[1] = a2;
	regs[2] = a3;
	regs[3] = a4;
	regs[4] = a5;
	return(_doscan(stdin, fmt, regs, &args));
#else
	regs.r0 = a1;
	regs.r1 = a2;
	return(_doscan(stdin, fmt, &regs, &args));
#endif
}

#ifndef SEG
fscanf(iop, fmt, a1, a2, a3, a4, a5, args)
#else
fscanf(iop, fmt, a1, a2, args)
#endif
FILE *iop;
char *fmt;
#ifdef SEG
int *a1, *a2;
#endif
{
#ifndef SEG
	int regs[5];
#else
	struct { int *r0, *r1, *r2, *r3, *r4; } regs;
#endif

#ifndef SEG
	regs[0] = a1;
	regs[1] = a2;
	regs[2] = a3;
	regs[3] = a4;
	regs[4] = a5;
	return(_doscan(iop, fmt, regs, &args));
#else
	regs.r0 = a1;
	regs.r1 = a2;
	return(_doscan(iop, fmt, &regs, &args));
#endif
}

#ifndef SEG
sscanf(str, fmt, a1, a2, a3, a4, a5, args)
#else
sscanf(str, fmt, a1, a2, args)
#endif
register char *str;
char *fmt;
#ifdef SEG
int *a1, *a2;
#endif
{
	FILE _strbuf;
#ifndef SEG
	int regs[5];
#else
	struct { int *r0, *r1, *r2, *r3, *r4; } regs;
#endif

	_strbuf._flag = _IOREAD|_IOSTRG;
	_strbuf._ptr = (unsigned char *)(_strbuf._base = str);
	_strbuf._cnt = 0;
	while (*str++)
		_strbuf._cnt++;
#ifndef SEG
	regs[0] = a1;
	regs[1] = a2;
	regs[2] = a3;
	regs[3] = a4;
	regs[4] = a5;
	return(_doscan(&_strbuf, fmt, regs, &args));
#else
	regs.r0 = a1;
	regs.r1 = a2;
	return(_doscan(&_strbuf, fmt, &regs, &args));
#endif
}
