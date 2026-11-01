/* data.c, an older version (reconstruction): _sobuf declared in front of _sibuf, the bss order of /lib/cparse */

#include <stdio.h>
char	_sobuf[BUFSIZ];
char	_sibuf[BUFSIZ];

struct	_iobuf	_iob[_NFILE] = {
	{ _sibuf, 0, _sibuf, _IOREAD, 0},
	{ NULL, 0, NULL, _IOWRT, 1},
	{NULL, 0, NULL, _IOWRT+_IONBF, 2},
};
/*
 * Ptr to end of buffers
 */
struct	_iobuf	*_lastbuf = { &_iob[_NFILE] };
