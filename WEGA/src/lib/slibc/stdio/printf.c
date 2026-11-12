/* @[$] printf.c	2.3  08/17/83 10:58:37 - 87wega3.2 */

#include <stdio.h>
extern FILE *_pfile;
int rc;

printf(fmt, a1, a2, a3, a4, args)
char *fmt;
{
	int regs[5];

	regs[0] = a1;
	regs[1] = a2;
	regs[2] = a3;
	regs[3] = a4;
	_pfile = stdout;
	rc = _doprnt(fmt, regs, &args, 1, stdout);
	return(rc);
}

fprintf(iop, fmt, a1, a2, args)
FILE *iop;
char *fmt;
{
	int regs[3];

	regs[0] = a1;
	regs[1] = a2;
	_pfile = iop;
	rc = _doprnt(fmt, regs, &args, 3, iop);
	return(rc);
}

sprintf(str, fmt, a1, a2, args)
char *str;
char *fmt;
{
	FILE _strbuf;
	int regs[3];

	_strbuf._flag = _IOWRT+_IOSTRG;
	_strbuf._ptr = str;
	_strbuf._cnt = 32767;
	regs[0] = a1;
	regs[1] = a2;
	_pfile = &_strbuf;
	rc = _doprnt(fmt, regs, &args, 3, &_strbuf);
	putc('\0', &_strbuf);
	return(rc);
}
