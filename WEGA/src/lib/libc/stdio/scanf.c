/* @[$] scanf.c	2.1  09/12/83 11:16:36 - 87wega3.2 */

#include	<stdio.h>

scanf(fmt, a1, a2, a3, a4, a5, args)
char *fmt;
{
	int regs[5];

	regs[0] = a1;
	regs[1] = a2;
	regs[2] = a3;
	regs[3] = a4;
	regs[4] = a5;
	return(_doscan(stdin, fmt, regs, &args));
}

fscanf(iop, fmt, a1, a2, a3, a4, a5, args)
FILE *iop;
char *fmt;
{
	int regs[5];

	regs[0] = a1;
	regs[1] = a2;
	regs[2] = a3;
	regs[3] = a4;
	regs[4] = a5;
	return(_doscan(iop, fmt, regs, &args));
}

sscanf(str, fmt, a1, a2, a3, a4, a5, args)
register char *str;
char *fmt;
{
	FILE _strbuf;
	int regs[5];

	_strbuf._flag = _IOREAD|_IOSTRG;
	_strbuf._ptr = _strbuf._base = str;
	_strbuf._cnt = 0;
	while (*str++)
		_strbuf._cnt++;
	regs[0] = a1;
	regs[1] = a2;
	regs[2] = a3;
	regs[3] = a4;
	regs[4] = a5;
	return(_doscan(&_strbuf, fmt, regs, &args));
}
