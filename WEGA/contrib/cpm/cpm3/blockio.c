/*
 *	blockio.c
 *	blockorientirte Funktionen von cpm
 */

#include <stdio.h>
#include "cpmio.h"

extern int n_systrk;
/*
 * Get a full block or a part of a block from floppy disk file
 * nsect gives the actual number of physical sectors to read.
 * if nsect is negative then always read a full block.
 */

getblock(blockno, buffer, nsect)
	int blockno;	/* Nummer des zu lesenden Blockes */
	int nsect;	/* Anzahl der zu lesenden logischen Sektoren */
	char buffer[];
{

	int sect, track, counter;

#ifdef DEBUG
	printf("block: %d\n",blockno);
#endif
	if (nsect < 0) 	/* lesen eines ganzen Blockes */
		nsect = blksiz/seclth;
	else {		/* lesen einer bestimmten Anzahl log. Sektoren */
			/* dass erfordert eine Umrechnung in phys. Sektoren */
		nsect *= 128;
		sect = nsect / seclth;
		sect += nsect % seclth ? 1 : 0;
		nsect = sect;
	}
#ifdef DEBUG
	printf( "getblock: nsect = %d block = %d\n", nsect, blockno );
#endif
	/* Translate block number into logical track/sector address */
	sect = (blockno*(blksiz/seclth)+ sectrk*n_systrk)%sectrk + 1;
	track = (blockno*(blksiz/seclth)+ sectrk*n_systrk)/sectrk ;
	/* read the block */
	for (counter = 0; counter < nsect; counter++) {
		if (getpsect(track, skewtab[sect++ -1], buffer+(seclth*counter))
				== EOF)
			return (EOF);
		if (sect > sectrk) {
			sect = 1;
			track++;
		}
	}
	return (0);
}

/*
 * Save a full block or a part of a block in floppy disk file
 * If nsects is nggative, write a full block.
 */

putblock(blockno, buffer, nsect)
	int blockno, nsect;
	char buffer[];
{

	int sect, track, counter;

	if (nsect < 0) 
		nsect = blksiz/seclth;
	else {
		nsect *= 128;
		sect = nsect / seclth;
		sect += nsect % seclth ? 1 : 0;
		nsect = sect;
	}
#ifdef DEBUG
	printf( "putblock: nsect = %d block = %d\n", nsect, blockno );
#endif
	/* Translate block number into logical track/sector address */
	sect = (blockno*(blksiz/seclth)+ sectrk*n_systrk)%sectrk + 1;
	track = (blockno*(blksiz/seclth)+ sectrk*n_systrk)/sectrk ;
	/* write the block */
	for (counter = 0; counter < nsect; counter++) {
		if (putpsect(track, skewtab[sect++ -1],buffer+(seclth*counter))
				== EOF) 
			return (EOF);
		if (sect > sectrk) {
			sect = 1;
			track++;
		}
	}
	return (0);
}
