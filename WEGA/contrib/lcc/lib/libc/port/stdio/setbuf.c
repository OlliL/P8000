#include <stdio.h>

extern void free();
extern int isatty();
extern unsigned char _smbuf[][_SBFSIZ];
extern unsigned char *_stdbuf[];

void
setbuf(iop, buf)
register FILE *iop;
char	*buf;
{
	register int fno = fileno(iop);  /* file number */

	if(iop->_base != NULL && iop->_flag & _IOMYBUF)
		free((char*)iop->_base);
	iop->_flag &= ~(_IOMYBUF | _IONBF | _IOLBF);
	if((iop->_base = (unsigned char*)buf) == NULL) {
		iop->_flag |= _IONBF; /* file unbuffered except in fastio */
		if (fno < 2)  /* for stdin, stdout, use the existing bufs */
			_bufend(iop) = (iop->_base = _stdbuf[fno]) + BUFSIZ;

		else   /* otherwise, use small buffers reserved for this */
			_bufend(iop) = (iop->_base = _smbuf[fno]) + _SBFSIZ;
	}
	else {  /* regular buffered I/O, standard buffer size */
		_bufend(iop) = iop->_base + BUFSIZ;
		if (isatty(fno))
			iop->_flag |= _IOLBF;
	}
	iop->_ptr = iop->_base;
	iop->_cnt = 0;
}
