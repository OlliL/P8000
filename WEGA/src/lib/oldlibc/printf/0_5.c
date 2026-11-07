#include <stdio.h>

printf(fmt, a1, a2, a3, a4, a5, args)
char *fmt;
{
	int regs[5];

	regs[0] = a1;
	regs[1] = a2;
	regs[2] = a3;
	regs[3] = a4;
	regs[4] = a5;
	_doprnt(fmt, regs, &args, 0, stdout);
	return(ferror(stdout)? EOF: 0);
}

fprintf(iop, fmt, a1, a2, a3, a4, args)
FILE *iop;
char *fmt;
{
	int regs[4];

	regs[0] = a1;
	regs[1] = a2;
	regs[2] = a3;
	regs[3] = a4;
	_doprnt(fmt, regs, &args, 1, iop);
	return(ferror(iop)? EOF: 0);
}

sprintf(str, fmt, a1, a2, a3, a4, args)
char *str, *fmt;
{
	FILE _strbuf;
	int regs[4];

	_strbuf._flag = _IOWRT+_IOSTRG;
	_strbuf._ptr = str;
	_strbuf._cnt = 32767;
	regs[0] = a1;
	regs[1] = a2;
	regs[2] = a3;
	regs[3] = a4;
	_doprnt(fmt, regs, &args, 1, &_strbuf);
	putc('\0', &_strbuf);
	return((int)str);
}
