/* @[$] fputs.c	2.1  09/12/83 11:10:27 - 87wega3.2 */

#include	<stdio.h>

fputs(s, iop)
register char *s;
register FILE *iop;
{
	register r;
	register c;

	while (c = *s++)
		r = putc(c, iop);
	return(r);
}
