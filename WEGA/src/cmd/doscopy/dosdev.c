/**************************************************************************
***************************************************************************
 
	W E G A - Quelle	(C) TU KMSt / ZFT/KEAW  Abt. WAE - 1988
	Programm: dosdev.c
 
	Bearbeiter	: P. Hoge
	Datum		: 24.11.89
	Version		: 1.3
 
***************************************************************************
**************************************************************************/

/* Bereitstellen von DCP-Disk Informationen */

#include "dos.h"
#include <signal.h>

/* dosdev() oeffnet DCP-Geraetefiles und
 * belegt Struktur dosdisc mit gueltigen Werten
 */
dosdev(fn,mode,dp)
char *fn;
int mode;
register struct dosdisc *dp;
{
 register int i, bps, fat=0;
 unsigned long off=0;
 unsigned char b[17];	/* Boot-Record-Informationen */

 if ((dp->fd=open(fn,mode)) < 0)
	return(-1);

 if (mode) {	/* nicht read only */
	signal(SIGALRM, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
 }

 /* Ermitteln ob Floppy oder Harddisk */

 lseek(dp->fd,(long)0,0);
 if (read(dp->fd,b,1) != 1)
	return(-1);
 if (b[0] != 0xeb) { /* Harddisk */
	lseek(dp->fd,(long)0x1be,0);
	i = 4;
	while (i--) {
		if (read(dp->fd,b,16) != 16)
			return(-1);
		if (b[4] == 1) { /* DCP-Partion */
			fat = 12;
			break;
		}
		if (b[4] == 4) {
			fat = 16;
			break;
		}
	}
	if (fat == 0)
		return(-1);
	off = ((b[9]*256) + b[8]) * 512;
 }

 lseek(dp->fd,off+11,0);
 if (read(dp->fd,b,17) != 17)
	return(-1);
 bps = b[1]*256+b[0];
 dp->fat1 = (b[4]*256+b[3])*bps + off;
 dp->fatnum = b[5];
 dp->maindir = off + ((b[4]*256+b[3]) + (b[12]*256+b[11]) * dp->fatnum) * bps;
 dp->dirnum = b[7]*256+b[6];
 dp->data = dp->maindir+(dp->dirnum * 32);
 dp->clustsiz = b[2] * bps;
 dp->vol = b[9]*256 + b[8];
 dp->nclust = ((long)dp->vol*bps - dp->data + off) / dp->clustsiz;

 if (fat == 0) {
	if (dp->nclust > 4095)
		fat = 16;
	else
		fat = 12;
 }
 dp->fatlen = fat;
 return(0);
}
