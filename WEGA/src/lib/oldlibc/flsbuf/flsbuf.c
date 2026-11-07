#include	<stdio.h>

#ifdef _PTRCHAR
#define UCP(p)	(p)
#else
#define UCP(p)	((unsigned char *)(p))
#endif

char	*malloc();

int
_flsbuf(c, iop)
	int c;
	register FILE *iop;
{
	register char *base;
	register n, rn;
	char c1;
	extern char _sobuf[];

	if (iop->_flag & _IORW) {
		iop->_flag |= _IOWRT;
		iop->_flag &= ~_IOEOF;
	}

tryagain:
	if (iop->_flag & _IONBF) {
		c1 = c;
		rn = 1;
		n = write(fileno(iop), &c1, rn);
		iop->_cnt = 0;
	} else {
		if ((base = iop->_base) == NULL) {
			if (iop == stdout) {
				if (isatty(fileno(stdout))) {
					iop->_flag |= _IONBF;
					goto tryagain;
				}
				iop->_base = _sobuf;
				iop->_ptr = UCP(_sobuf);
				goto tryagain;
			}
			if ((iop->_base = base = malloc(BUFSIZ)) == NULL) {
				iop->_flag |= _IONBF;
				goto tryagain;
			}
			iop->_flag |= _IOMYBUF;
			rn = n = 0;
		} else if((rn = n = (char *)iop->_ptr - base) > 0) {
			iop->_ptr = UCP(base);
			n = write(fileno(iop), base, n);
		}
		iop->_cnt = BUFSIZ - 1;
		*base++ = c;
		iop->_ptr = UCP(base);
	}
	if (rn != n) {
		iop->_flag |= _IOERR;
		return(EOF);
	}
	return(c);
}

int
fflush(iop)
	register FILE *iop;
{
	register char *base;
	register n;

	if ((iop->_flag & (_IONBF|_IOWRT)) == _IOWRT
	 && (base = iop->_base) != NULL && (n = (char *)iop->_ptr - base) > 0) {
		iop->_ptr = UCP(base);
		iop->_cnt = BUFSIZ;
		if (write(fileno(iop), base, n) != n) {
			iop->_flag |= _IOERR;
			return(EOF);
		}
	}
	return(0);
}

/*
 * Flush buffers on exit
 */

_cleanup()
{
	register FILE *iop;
	extern FILE *_lastbuf;

	for(iop = _iob; iop < _lastbuf; iop++)
		fclose(iop);
}

int
fclose(iop)
	register FILE *iop;
{
	register r;

	r = EOF;
	if (iop->_flag & (_IOREAD|_IOWRT|_IORW)
	    && (iop->_flag & _IOSTRG) == 0) {
		r = fflush(iop);
		if (close(fileno(iop)) < 0)
			r = EOF;
		if (iop->_flag & _IOMYBUF)
			free(iop->_base);
		if (iop->_flag & (_IOMYBUF|_IONBF))
			iop->_base = NULL;
	}
	iop->_flag &=
		~(_IOREAD|_IOWRT|_IONBF|_IOMYBUF|_IOERR|_IOEOF|_IOSTRG|_IORW);
	iop->_cnt = 0;
	return(r);
}
