/* @[$] scanf.c	2.1  08/17/83 11:10:12 - 87wega3.2 */

#include	<stdio.h>

scanf(fmt, a1, a2, args)
char *fmt;
int *a1, *a2;
{
	int *regs[5];

	regs[0] = a1;
	regs[1] = a2;
	return(_doscan(stdin, fmt, regs, &args));
}

fscanf(iop, fmt, a1, a2, args)
FILE *iop;
char *fmt;
int *a1, *a2;
{
	int *regs[5];

	regs[0] = a1;
	regs[1] = a2;
	return(_doscan(iop, fmt, regs, &args));
}

sscanf(str, fmt, a1, a2, args)
register char *str;
char *fmt;
int *a1, *a2;
{
	FILE _strbuf;
	int *regs[5];

	_strbuf._flag = _IOREAD|_IOSTRG;
	_strbuf._ptr = (unsigned char *)(_strbuf._base = str);
	_strbuf._cnt = 0;
	while (*str++)
		_strbuf._cnt++;
	regs[0] = a1;
	regs[1] = a2;
	return(_doscan(&_strbuf, fmt, regs, &args));
}
