/*
 *	physio.c Version 3.0
 *
 *	physiche Zugriffsrtn. fuer cpm
 */

#include <stdio.h>
#include "cpmio.h"
#include <sys/types.h>
#include <sys/file.h>

extern int n_systrk;

/*
 * Write physical sector to floppy disk file
 */

putpsect(tr, sect, buf)
	int tr, sect;
	char buf[];
{

	long newpos;

	if (sect > sectrk || sect < 1) {
		fprintf(stderr, 
			"putpsect: sector number out of range: %d\n", sect);
		return (EOF);
	}
	if( tr > tracks || tr < 0 ) {
		fprintf( stderr, "putpsect: bad track number %d\n", tr );
		return( EOF );
	}
	newpos = (long) (sect + (tr * sectrk) -1 ) * seclth;
	if (lseek(fid, newpos, 0) == -1) {
		perror("putpsect");
		return (EOF);
	}
	if (write(fid, buf, seclth) == seclth) 
		return (1);
	perror("putpsect");
	fprintf(stderr, "track %d, sect %d\n", tr, sect);
	return (EOF);
}

/*
 * Read physical sector from floppy disk file
 */

getpsect(tr, sect, buf)
	char buf[];
{

	long newpos;

	if (sect > sectrk || sect < 1) {
		fprintf("getpsect: sector number out of range: %d\n",sect);
		return (EOF);
	}
	if( tr > tracks || tr < 0 ) {
		fprintf( stderr, "getpsect: bad track number %d\n", tr );
		return( EOF );
	}
	newpos = (long) (sect + (tr * sectrk) -1 ) * seclth;
	if (lseek(fid, newpos, 0) == -1) {
		perror("getpsect");
		return (EOF);
	}
	if (read(fid, buf, seclth) != seclth) {
		perror("getpsect");
		fprintf(stderr, "track %d, sect %d\n", tr, sect);
		return (EOF);
	}
	return (1);
}

/* 
 * Initialize a new floppy disk file in "name",
 * return its file pointer.
 */

initcpm(name)
	char *name;
{
	int f, i, lc;
	char *buf, *malloc();

	if ((f = open(name, 2)) < 0)
		if(( f = creat( name, 0644 )) < 0 )
			return (EOF);
		else {
			close( f );
			f = open( name , 2 );
		}
	if( ( buf = malloc( (unsigned)seclth ) ) == NULL ) {
		fprintf( stderr, "initcpm: out of memory\n" );
		return( EOF );
	}
	for (i=0; i<seclth; i++)
		buf[i] = '\345';
	/*
	 * Initialize (with 0xe5) the first four tracks
	 * on the `floppy'
	 */
	lc = ( n_systrk + 2 ) * ( seclth * sectrk );
	for (i=0; i < lc; i+= seclth)
		write(f, buf, seclth);
	free( buf );
	return(f);
}
