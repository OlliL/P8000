/* @[$] printf.c	2.4  09/12/83 11:14:45 - 87wega3.2 */

#include <stdio.h>
extern FILE *_pfile;
int rc;

printf(fmt, a1, a2, a3, a4, a5, args)
register char *fmt;
{
	register int x, n = 0;
	int regs[5];

	regs[0] = a1;
	regs[1] = a2;
	regs[2] = a3;
	regs[3] = a4;
	regs[4] = a5;
	_pfile = stdout;
	rc = _doprnt(fmt, regs, &args, 0, stdout);
	return(rc);
}

fprintf(iop, fmt, a1, a2, a3, a4, args)
FILE *iop;
register char *fmt;
{
	register int x, n = 0;
	int regs[5];

	regs[0] = a1;
	regs[1] = a2;
	regs[2] = a3;
	regs[3] = a4;
	_pfile = iop;
	rc = _doprnt(fmt, regs, &args, 1, iop);
	return(rc);
}

sprintf(str, fmt, a1, a2, a3, a4, args)
char *str;
register char *fmt;
{
	register int x, n = 0;
	FILE _strbuf;
	long l;
	int regs[5];

	regs[0] = a1;
	regs[1] = a2;
	regs[2] = a3;
	regs[3] = a4;
	_strbuf._flag = _IOWRT+_IOSTRG;
	_strbuf._ptr = str;
	_strbuf._cnt = 32767;
	_pfile = &_strbuf;
	rc = _doprnt(fmt, regs, &args, 1, &_strbuf);
	putc('\0', &_strbuf);
	return(rc);
}
