/*
 *	blk_no_hdl.c
 *	Behandlung der im Directory enthaltenen Blocknummern
 *	es werden zur Zeit 2 verschiedene Addressformate unterstuetzt
 */

#include <stdio.h>
#include "cpmio.h"
#include "cpmfio.h"

extern int flp_fmt;


/*
 *	getblno
 *	bereitstellen der naechsten Blocknummer aus dem Directory
 */
getblno( fptr )
	C_FILE *fptr;
{
	int blockno;
	int *blp;

	if( fptr->c_blk == 16 )
		return( EOF );
	switch( flp_fmt ) {
	case 1:	/* 8 bit Blockaddressen */
		blockno = fptr->c_dirp->pointers[ fptr->c_blk++ ] &0xff;
		break;
	case 2:	/* 16 bit Blockaddressen */
		blp = ( int * )&(fptr->c_dirp->pointers[ fptr->c_blk ] );
		fptr->c_blk += 2;
#ifdef p8000
		blockno = *blp;
		blockno = numswab( blockno );
#else
		blockno = *blp;
#endif p8000
		break;
	default:
		fprintf( stderr, "getblno: unknown addressformat %d\n",
		flp_fmt );
		exit( 1 );
		break;
	}
#ifdef DEBUG
	fprintf( stderr, "getblno: block = %d\n", blockno );
#endif
	return( blockno );
}

/*
 *	putblno
 *	eintragen der naechsten Blocknummer in das Verzeichniss
 */
putblno( fptr, block )
	C_FILE *fptr;
	int block;
{
	int *blp;

	if( fptr->c_blk == 16 )
		return( EOF );
	switch( flp_fmt ) {
	case 1:	/* 8 Bit Blockaddressen */
		fptr->c_dirp->pointers[ fptr->c_blk ] = block & 0xff;
		fptr->c_blk++;
		break;
	case 2:	/* 16 Bit Blockaddressen */
		blp = ( int * )&(fptr->c_dirp->pointers[ fptr->c_blk ] );
#ifdef p8000
		*blp = numswab( block );
#else
		*blp = block;
#endif p8000
		fptr->c_blk += 2;
		break;
	default:
		fprintf( stderr, "putblno: unknown addressformat %d\n",
		flp_fmt );
		exit( 1 );
		break;
	}
#ifdef DEBUG
	fprintf( stderr, "putblno: block = %d\n", block );
#endif
	return( block );
}

/*
 *	extblno
 *	liefert blocknummern fuer build_bmap
 */

extblno( pointer, ind )
	char pointer[];
	int ind;
{
	int *blp;
	int blk;

	switch( flp_fmt ) {
	case 1:
		return( pointer[ind] & 0xff );
		break;
	case 2:
		blp = ( int * )&pointer[ind];
#ifdef p8000
		blk = *blp;
		blk = numswab( blk );
#ifdef DEBUG
		fprintf( stderr, "extblno: block = %d\n", blk );
#endif DEBUG
		return(  blk );
#else
#ifdef DEBUG
		fprintf( stderr, "extblno: block = %d\n", blk );
#endif DEBUG
		return( *blp );
#endif p8000
		break;
	default:
		fprintf( stderr, "extblno: unknown addressformat %d\n",
		flp_fmt );
		exit( 1 );
		break;
	}
}	

#ifdef p8000
numswab( in )
	int in;
{
	char *ip, *op;
	int out;

	ip = ( char * ) &in;
	op = ( char * ) &out;
	op[1] = ip[0] & 0xff;
	op[0] = ip[1] & 0xff;
	return( out );
}
#endif p8000
