/* @[$] getw.c	2.1  09/12/83 11:14:03 - 87wega3.2 */

#include	<stdio.h>

getw(iop)
register struct _iobuf *iop;
{
	register i;

	i = getc(iop);
	if (iop->_flag&_IOEOF)
		return(-1);
	return(i<<8 | getc(iop));
}
