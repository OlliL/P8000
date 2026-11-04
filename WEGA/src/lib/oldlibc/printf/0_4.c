#include <stdio.h>
extern FILE *_pfile;
int rc;
printf(fmt, a1, a2, a3, a4, a5, d1, d2, d3, d4, args)
char *fmt;
double d1, d2, d3, d4;
{
	int regs[5];
	double fregs[4];

	regs[0] = a1;
	regs[1] = a2;
	regs[2] = a3;
	regs[3] = a4;
	regs[4] = a5;
	fregs[0] = d1;
	fregs[1] = d2;
	fregs[2] = d3;
	fregs[3] = d4;
	_pfile = stdout;
	rc = _doprnt(fmt, regs, fregs, &args, 0, stdout);
	return(rc);
}

fprintf(iop, fmt, a1, a2, a3, a4, d1, d2, d3, d4, args)
FILE *iop;
char *fmt;
double d1, d2, d3, d4;
{
	int regs[4];
	double fregs[4];

	regs[0] = a1;
	regs[1] = a2;
	regs[2] = a3;
	regs[3] = a4;
	fregs[0] = d1;
	fregs[1] = d2;
	fregs[2] = d3;
	fregs[3] = d4;
	_pfile = iop;
	rc = _doprnt(fmt, regs, fregs, &args, 1, iop);
	return(rc);
}

sprintf(str, fmt, a1, a2, a3, a4, d1, d2, d3, d4, args)
char *str, *fmt;
double d1, d2, d3, d4;
{
	FILE _strbuf;
	int regs[4];
	double fregs[4];

	_strbuf._flag = _IOWRT+_IOSTRG;
	_strbuf._ptr = str;
	_strbuf._cnt = 32767;
	regs[0] = a1;
	regs[1] = a2;
	regs[2] = a3;
	regs[3] = a4;
	fregs[0] = d1;
	fregs[1] = d2;
	fregs[2] = d3;
	fregs[3] = d4;
	_pfile = &_strbuf;
	rc = _doprnt(fmt, regs, fregs, &args, 1, &_strbuf);
	putc('\0', &_strbuf);
	return(rc);
}
