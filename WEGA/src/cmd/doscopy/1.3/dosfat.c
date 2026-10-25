/**************************************************************************
***************************************************************************
 
	W E G A - Quelle	(C) TU KMSt / ZFT/KEAW  Abt. WAE - 1988
	Programm: dosfat.c
 
	Bearbeiter	: P. Hoge
	Datum		: 24.11.89
	Version		: 1.3
 
***************************************************************************
**************************************************************************/

#include "dos.h"
#include <stdio.h>

/* fatfree
 * Liefert die naechste freie Clusternummer
 * auf dem Datentraeger dp.
 * Liefert -1 bei Lesefehler und
 *	   -3 falls die Disk voll ist
 */
fatfree(dp)
register struct dosdisc *dp;
{
 static long freeadr = 0;
 static FILE *fp;
 register unsigned nclust;
 unsigned char n[3];

 nclust = dp->nclust;
 if (fp == 0)
 	fp = fdopen(dp->fd, "r");

 if (dp->fatlen == 12) {
	fseek(fp, dp->fat1+freeadr+3, 0);
	while (nclust -= 2) {
		if (fread(n, sizeof(*n),3,fp) != 3)
			return (-1);

		if (n[0] == 0 && (n[1]&0x0f) == 0) {
			freeadr = ftell(fp) - dp->fat1 - 3;
			return(freeadr/1.5);
		}

		if ((n[1]&0xf0) == 0 && n[2] == 0) {
			freeadr = ftell(fp) - dp->fat1 - 3;
			return(freeadr/1.5 + 1);
		}
	}
 }
 else {
	fseek(fp, dp->fat1+freeadr+2, 0);
	while (nclust--) {
		if (fread(n,sizeof(*n),2,fp) != 2)
			return(-1);

		if (n[0] == 0 && n[1] == 0) {
			freeadr = ftell(fp) - dp->fat1 - 2;
			return(freeadr/2);
		}
	}
 }
 return(-3);
}
