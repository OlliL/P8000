/* @[$] rdwr.c	2.1  09/12/83 11:16:03 - 87wega3.2 */

#include	<stdio.h>

fread(ptr, size, count, iop)
unsigned size, count;
register char *ptr;
register FILE *iop;
{
	register c;
	unsigned ndone, s;

	ndone = 0;
	if (size)
	for (; ndone<count; ndone++) {
		s = size;
		do {
			if ((c = getc(iop)) >= 0)
				*ptr++ = c;
			else
				return(ndone);
		} while (--s);
	}
	return(ndone);
}

fwrite(ptr, size, count, iop)
unsigned size, count;
register char *ptr;
register FILE *iop;
{
	register unsigned s;
	unsigned ndone;

	ndone = 0;
	if (size)
	for (; ndone<count; ndone++) {
		s = size;
		do {
			putc(*ptr++, iop);
		} while (--s);
		if (ferror(iop))
			break;
	}
	return(ndone);
}
