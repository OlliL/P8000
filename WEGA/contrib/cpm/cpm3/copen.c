/*
 *	c_open
 *	in dieser und allen anderen Funktionen zu fileorientierten E/A
 *	wird nur noch mit log. Sektoren gearbeitet
 */

#include <stdio.h>
#include "cpmio.h"
#include "cpmfio.h"

C_FILE * c_open( name, ext, mode )
	char *name, *ext;
{
	int i, index, scnt;
	register C_FILE *fptr;
	char *malloc();

	if((index = searchdir(name, ext)) == -1 ) {
		fnfound( name, ext );
		return( NULL  );
	}
	if( !mode & RW ) {
		fprintf( stderr, "open: illegal mode - %d\n", mode );
		return ( NULL );
	}
#ifdef DEBUG
	printf( "directory index: %d\n", index );
#endif
	for( i = 0, fptr = c_iob; i < C_NFILE; i++, fptr++ ) {
		if(!(fptr->c_flag))
			break;
	}
	if( i== C_NFILE ) {
		fprintf( stderr, "too many open files\n");
		return(NULL );
	}
	if( ( fptr->c_buf = malloc( blksiz ) ) == NULL ) {
		printf("c_open: no memory!\n");
		return( NULL );
	}
	fptr->c_extno = 0;
	fptr->c_base = fptr->c_buf;
	fptr->c_flag = mode;
	fptr->c_blk = 0;
	fptr->c_ext = index;
	fptr->c_dirp = dirbuf+index;
	fptr->c_seccnt = 0xff & (dirbuf+index)->blkcnt;
	scnt = (fptr->c_seccnt > blksiz/128) ? blksiz/128 : fptr->c_seccnt;
#ifdef DEBUG
	printf( "c_open: scnt=%d\n", scnt );
#endif
	if( getblock( getblno(fptr), fptr->c_buf, scnt) == EOF )
		return( NULL );
	fptr->c_cnt = 128 * scnt;
	fptr->c_seccnt -= scnt;
	return( fptr );
}

fnfound( name, ext )
char name[], ext[];
{
	if( ext[0] == ' ' ) ext[0] = '\0';
	if( name[0] == ' ' ) name[0] = '\0';
	printf( "file not found: %s %s \n", name, ext );
}
