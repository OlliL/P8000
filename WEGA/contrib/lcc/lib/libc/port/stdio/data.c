#include <stdio.h>

/* some slop is allowed at the end of the buffers in case an upset in
 * the synchronization of _cnt and _ptr (caused by an interrupt or other
 * signal) is not immediately detected.
 */
unsigned char _sibuf[BUFSIZ+8], _sobuf[BUFSIZ+8];
/*
 * Ptrs to start of preallocated buffers for stdin, stdout.
 */
unsigned char *_stdbuf[] = { _sibuf, _sobuf };

unsigned char _smbuf[_NFILE+1][_SBFSIZ];

FILE _iob[_NFILE] = {
	{ NULL, 0, NULL, _IOREAD, 0},
	{ NULL, 0, NULL, _IOWRT, 1},
	{ _smbuf[2], 0, _smbuf[2], _IOWRT+_IONBF, 2},
};
/*
 * Ptr to end of io control blocks
 */
FILE *_lastbuf = { &_iob[_NFILE] };

/*
 * Ptrs to end of read/write buffers for each device
 * There is an extra bufend pointer which corresponds to the dummy
 * file number _NFILE, which is used by sscanf and sprintf.
 */
unsigned char *_bufendtab[_NFILE+1] = { NULL, NULL, _smbuf[2]+_SBFSIZ, };
