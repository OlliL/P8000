/* printf 0_8 (libc-v14, local 02/84): ../../libc/common/stdio/printf.c without _pfile and the unused long of sprintf */
#ifndef SEG
/* @[$] printf.c	2.4  09/12/83 11:14:45 - 87wega3.2 */
#else
/* @[$] printf.c	2.3  08/17/83 10:58:37 - 87wega3.2 */
#endif

#include <stdio.h>
int rc;

#ifndef SEG
printf(fmt, a1, a2, a3, a4, a5, args)
register char *fmt;
#else
printf(fmt, a1, a2, a3, a4, args)
char *fmt;
#endif
{
#ifndef SEG
	register int x, n = 0;
	int regs[5];
#else
	struct { int r0, r1, r2, r3; } regs;
#endif

#ifndef SEG
	regs[0] = a1;
	regs[1] = a2;
	regs[2] = a3;
	regs[3] = a4;
	regs[4] = a5;
#else
	regs.r0 = a1;
	regs.r1 = a2;
	regs.r2 = a3;
	regs.r3 = a4;
#endif
#ifndef SEG
	rc = _doprnt(fmt, regs, &args, 0, stdout);
#else
	rc = _doprnt(fmt, &regs, &args, 1, stdout);
#endif
	return(rc);
}

#ifndef SEG
fprintf(iop, fmt, a1, a2, a3, a4, args)
#else
fprintf(iop, fmt, a1, a2, args)
#endif
FILE *iop;
#ifndef SEG
register char *fmt;
#else
char *fmt;
#endif
{
#ifndef SEG
	register int x, n = 0;
	int regs[5];
#else
	struct { int r0, r1; } regs;
#endif

#ifndef SEG
	regs[0] = a1;
	regs[1] = a2;
	regs[2] = a3;
	regs[3] = a4;
#else
	regs.r0 = a1;
	regs.r1 = a2;
#endif
#ifndef SEG
	rc = _doprnt(fmt, regs, &args, 1, iop);
#else
	rc = _doprnt(fmt, &regs, &args, 3, iop);
#endif
	return(rc);
}

#ifndef SEG
sprintf(str, fmt, a1, a2, a3, a4, args)
#else
sprintf(str, fmt, a1, a2, args)
#endif
char *str;
#ifndef SEG
register char *fmt;
#else
char *fmt;
#endif
{
#ifndef SEG
	register int x, n = 0;
#endif /* ! SEG */
	FILE _strbuf;
#ifndef SEG
	int regs[5];
#else
	struct { int r0, r1; } regs;
#endif

#ifndef SEG
	regs[0] = a1;
	regs[1] = a2;
	regs[2] = a3;
	regs[3] = a4;
#endif /* ! SEG */
	_strbuf._flag = _IOWRT+_IOSTRG;
	_strbuf._ptr = str;
	_strbuf._cnt = 32767;
#ifdef SEG
	regs.r0 = a1;
	regs.r1 = a2;
#endif
#ifndef SEG
	rc = _doprnt(fmt, regs, &args, 1, &_strbuf);
#else
	rc = _doprnt(fmt, &regs, &args, 3, &_strbuf);
#endif
	putc('\0', &_strbuf);
	return(rc);
}
