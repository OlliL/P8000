/* @[$] fdopen.c	2.2  09/12/83 11:07:09 - 87wega3.2 */

/*
 * Unix routine to do an "fopen" on file descriptor
 * The mode has to be repeated because you can't query its
 * status
 */

#include	<stdio.h>
#include	<errno.h>

FILE *
fdopen(fd, mode)
	register char *mode;
{
	register FILE *iop;
	FILE *_findiop();

	if ((iop = _findiop()) == NULL)
		return(NULL);

	iop->_cnt = 0;
	iop->_file = fd;
	switch (*mode) {

	case 'r':
		iop->_flag |= _IOREAD;
		break;

	case 'a':
		lseek(fd, 0L, 2);
		/* No break */
	case 'w':
		iop->_flag |= _IOWRT;
		break;

	default:
		return(NULL);
	}

	if (mode[1] == '+' || mode[1] == 'w') {
		iop->_flag &= ~(_IOREAD|_IOWRT);
		iop->_flag |= _IORW;
	}

	return(iop);
}
