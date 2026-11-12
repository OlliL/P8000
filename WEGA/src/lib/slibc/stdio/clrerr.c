/* @[$] clrerr.c	2.1 09/12/83 11:04:39 -87wega3.2. */

#include	<stdio.h>

clearerr(iop)
register struct _iobuf *iop;
{
	iop->_flag &= ~(_IOERR|_IOEOF);
}
