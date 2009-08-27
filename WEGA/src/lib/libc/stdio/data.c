/* @[$] data.c	2.1  09/12/83 11:04:52 - 87wega3.2 */

#include <stdio.h>
char	_sibuf[BUFSIZ];
char	_sobuf[BUFSIZ];

struct	_iobuf	_iob[_NFILE] = {
	{ _sibuf, 0, _sibuf, _IOREAD, 0},
	{ NULL, 0, NULL, _IOWRT, 1},
	{NULL, 0, NULL, _IOWRT+_IONBF, 2},
};
/*
 * Ptr to end of buffers
 */
struct	_iobuf	*_lastbuf = { &_iob[_NFILE] };
