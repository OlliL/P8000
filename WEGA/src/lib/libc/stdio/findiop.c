/* @[$] findiop.c	2.1  09/12/83 11:08:06 - 87wega3.2 */

#include <stdio.h>

FILE *
_findiop()
{
	extern FILE *_lastbuf;
	register FILE *iop;

	for(iop = _iob; iop->_flag & (_IOREAD|_IOWRT|_IORW); iop++)
		if (iop >= _lastbuf)
			return(NULL);

	return(iop);
}
