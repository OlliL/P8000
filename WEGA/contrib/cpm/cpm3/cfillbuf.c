/*	cfillbuf.c	1.6	83/05/13	*/
#include <stdio.h>
#include "cpmio.h"
#include "cpmfio.h"

c_fillbuf(fptr)
	register C_FILE *fptr;
{

	int	getnext(), nsect;
	int	block;

	if ( ( block = getblno( fptr ) )  == EOF )
		if (fptr->c_dirp->blkcnt ==  (char) 0x80) {
			/* find next extent (if it exists) */
			if (getnext(fptr) == NULL)
				return (EOF);
			if ((block = getblno(fptr)) == EOF)
				return(EOF);	/* Hahn */
		}
		else 
			return (EOF);
	nsect = (fptr->c_seccnt>blksiz/128) ? blksiz/128 : fptr->c_seccnt;
	if (nsect == 0) 
		return (EOF);
	fptr->c_seccnt -= nsect;
	if (getblock( block, fptr->c_base, nsect) == EOF)
		return (EOF);
	fptr->c_buf = fptr->c_base;
	fptr->c_cnt = nsect*128 -1;
	return (*fptr->c_buf++&0xff);
}
