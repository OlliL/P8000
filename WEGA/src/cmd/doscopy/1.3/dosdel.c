/**************************************************************************
***************************************************************************
 
	W E G A - Quelle	(C) TU KMSt / ZFT/KEAW  Abt. WAE - 1988
	Programm: dosdel.c
 
	Bearbeiter	: P. Hoge
	Datum		: 24.11.89
	Version		: 1.3
 
***************************************************************************
**************************************************************************/

#include "dos.h"

/* dosdel
 * Loescht die DCP-Datei fn auf dem Datentraeger dp.
 * Liefert 0, wenn nicht gefunden,
 *	  -1, bei Lesefehler,
 *	  -2, bei Schreibfehler und
 *	  -3, falls fn ein Verzeichnis bzw
 *	      schreibgeschuetzt ist.
 */
dosdel(fn,dp)
register char *fn;
register struct dosdisc *dp;
{
 register struct dostat fnrec, dirrec;
 long adr;
 unsigned char delsign=0xe5;
 char s[LMAX],*file,*strrchr();
 unsigned dnum,nxt,dosnxt();
 int res;

 strcpy(s,fn);
 fn = s;
 if ((res=dosopn(fn,dp,&fnrec)) < 1)
	return(res);
 if (fnrec.mode == _SDIR || fnrec.flag == RO)
	return(-3);

 if (fn[0] == '/')
	fn++;
 if ((file=strrchr(fn,'/')) == 0) { /* main directory */
 	file = fn;
	dnum = dp->dirnum;
	adr = dp->maindir;
	nxt = 0;
 }
 else { /* sub directory */
	fn[strlen(fn)-strlen(file)] = '\0';
	file++;

	if ((res=dosopn(fn,dp,&dirrec)) < 1)
		return(res);
	nxt = dirrec.first;
	adr = DOSADR(nxt,dp->clustsiz)+dp->data;
	dnum = dp->clustsiz/32;
 }
 /* Suchen des zu loeschenden Eintrages */
 while (-1) {
	if ((res=dosrde(dp->fd,file,dnum,adr,&fnrec)) < 0)
		return(res);
	if (res == 1) {
		lseek(dp->fd,(long)-32,1);
		if (write(dp->fd,&delsign,1) != 1)
			return(-2);
		return(fatdel(&fnrec,dp));
	}
	if (nxt == 0)
		return(0);
	if ((nxt=dosnxt(dp->fd,nxt,dp->fat1,dp->fatlen)) < 1)
		return(0);
	/* naechsten Cluster lesen */
	adr = DOSADR(nxt,dp->clustsiz)+dp->data;
 }
}

/* fatdel
 * Gibt die Belegungskette der DCP-Datei stp
 * frei. Liefert -1 bei Lese- und -2 bei Schreibfehler.
 */
fatdel(stp,dp)
register struct dostat *stp;
register struct dosdisc *dp;
{
 register int res;
 register unsigned first, next;

 next = stp->first;
 while (next) {
	first = next;
	next = dosnxt(dp->fd,first,dp->fat1,dp->fatlen);
	if ((res=fatput(first,0x0000,dp)) < 0)
		return(res);
 }
 return(1);
}

/* fatput
 * Schreibt in alle FAT-Versionen fuer den Wert
 * des Eintrages ent den Wert val.
 * Liefert -1 bei Lesefehler
 * und -2 bei Schreibfehler.
 */
fatput(ent,val,dp)
register unsigned ent,val;
register struct dosdisc *dp;
{
 register long fat=dp->fat1;
 unsigned char b[2];
 int n;

 for (n = dp->fatnum; n >= 1; n--) {
	if (dp->fatlen == 12) {
		val &= 0x0fff;
		lseek(dp->fd,fat+(long)(1.5*ent),0);
		if (read(dp->fd,b,2) != 2)
			return(-1);
		lseek(dp->fd,(long)-2,1);
		if (ent % 2 == 0) {
			b[0] = val%256;
			b[1] = (b[1]/16)*16+(val/256);
		}
		else {
			b[1] = val/16;
			b[0] = (val%16)*16+(b[0]&0x0f);
		}
	}
	else {
		lseek(dp->fd,fat+(long)(2*ent),0);
		b[0] = val% 256;
		b[1] = val/256;
	}
	if (write(dp->fd,b,2) != 2)
		return(-2);
	fat = (dp->maindir-fat)/n+fat;
 }
 return(1);
}
