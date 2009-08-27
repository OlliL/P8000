/* @[$] putw.c	2.1  09/12/83 11:15:49 - 87wega3.2 */

#include	<stdio.h>

putw(i, iop)
register i;
register struct _iobuf *iop;
{
	putc(i, iop);
	putc(i>>8, iop);
}
