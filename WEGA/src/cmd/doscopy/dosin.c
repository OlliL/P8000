/**************************************************************************
***************************************************************************
 
	W E G A - Quelle	(C) TU KMSt / ZFT/KEAW  Abt. WAE - 1988
	Programm: dosin.c
 
	Bearbeiter	: P. Hoge
	Datum		: 24.11.89
	Version		: 1.3
 
***************************************************************************
**************************************************************************/

#include "dos.h"
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

/* dosin
 * Schreibt das WEGA-File 'in' auf den
 * Datentraeger dp unter dem Namen fn.
 * Ist fn schon vorhanden wird die alte
 * Version geloescht.
 * Liefert 0 falls das uebergeordnete
 * Verzeichnis nicht existiert,
 *	  -1 bei Lesefehler,
 *	  -2 bei Schreibfehler und
 *	  -3 falls fn ein Verzeichnis oder
 *	     schreibgeschuetzt ist.
 *	  -4 Lesefehler in wega
 */
dosin(in, fn, dp, conv)
char *in, *fn;
register struct dosdisc *dp;
int conv;
{
 register clustsiz = dp->clustsiz;
 register i, j, res;
 int infd, len, nl, mode, z;
 long adr, depos;
 int free, follow; 
 unsigned dnum, nxt, dosnxt();
 int dmax, dcnt = 0;
 struct stat instat;
 struct dostat fnrec, dirrec;
 struct tm *sttime, *localtime();
 char *p, *file, s[LMAX];
 char *strchr(), *strrchr();
 char *buf, *outbuf, *malloc();

 buf = malloc(clustsiz);
 outbuf = buf;
 if (conv)
	outbuf = malloc(clustsiz);

 strcpy(s, fn);
 fn = s;
 z = 1;
 if ((res = dosopn(fn, dp, &fnrec)) < 0)
	return(res);

 /* Das DCP-File wird geloescht, falls es bereits existiert */
 if (res > 0) {
	if ((res = dosdel(fn, dp)) < 1)
		return(res);
 }
 if ((file=strrchr(fn, '/')) == 0) { /* main directory */
	file = fn;
	dmax = dnum = dp->dirnum;
	adr = dp->maindir;
	nxt = 0;
 }
 else { /* sub directory */
	fn[strlen(fn)-strlen(file)] = '\0';
	file++;

	if ((res = dosopn(fn, dp, &dirrec)) < 1)
		return(res);
	if (dirrec.mode != _SDIR)
		return(0);
	adr = DOSADR(dirrec.first, clustsiz) + dp->data;
	/* Bestimmen der Anzahl von Verzeichniseintraegen */
	nxt = dirrec.first;
	while ((nxt = dosnxt(dp->fd, nxt, dp->fat1, dp->fatlen)) > 0)
		z++;
	dnum = clustsiz / 32;
	dmax = z * dnum;
	nxt = dirrec.first;
 }
 lseek(dp->fd, adr, 0);
 /* Suche des ersten freien Eintrages */
 for (i = 0; i < z; i++) {
	j = 1;
	while (j++ <= dnum && (res = getde(dp->fd, &fnrec)) > 0) {
		dcnt++;
		if (fnrec.name[0] == 0xe5) {
			i = z;
			depos = lseek(dp->fd, (long)-32, 1);
			break;
		}
	}
	if (res == 0) {
		depos = lseek(dp->fd, (long)-32, 1);
		break;
	}
	if (nxt != 0) {
		nxt = dosnxt(dp->fd, nxt, dp->fat1, dp->fatlen);
		if (nxt < 1)
			break;
		else { /* Naechsten Cluster lesen */
			adr = DOSADR(nxt, clustsiz) + dp->data;
			lseek(dp->fd, adr, 0);
			dirrec.first = nxt;
		}
	}
 }
 if (res < 0)
	return(res);
 if (fn == file && dcnt >= dmax)
	return(-2);		/* Stammverzeichnis voll */
 if (dcnt >= dmax) {		/* Verzeichniscluster voll */
	if ((free = fatfree(dp)) < 0)
		return(free);
	if ((res = fatput(dirrec.first, free, dp)) < 0)
		return(res);
	if ((res = fatput(free, 0xffff, dp)) < 0)
		return(res);
	adr = DOSADR(free, clustsiz) + dp->data;

	for (i = 0; i < 512; i++)
		buf[i] = 0x00;
	lseek(dp->fd, adr, 0);
	z = clustsiz / 512;
	for (i = 0; i < z; i++)
		if (write(dp->fd, buf, 512) != 512)
			return(-2);
	/* depos zeigt auf den zu belegenden Eintrag */
	depos = lseek(dp->fd, adr, 0);
 }

 /* Oeffnen des WEGA-Files */
 if ((infd = open(in, 0)) < 0)
	return(-4);
 if (fstat(infd, &instat) < 0)
	return(-4);
 mode = instat.st_mode;
 if ((mode & S_IFMT) == S_IFDIR)
	return(-4);

 /* fnrec wird mit Werten von infd belegt */
 if ((p = strchr(file, '.')) != 0) {
	p++;
	file[strlen(file) - strlen(p) - 1] = '\0';
	for (i = 0; i < 8; i++)
		if (i >= strlen(file))
			fnrec.name[i] = 0x20;
		else
			fnrec.name[i] = file[i];

	for (i = 8; i < 11; i++)
		if (i - 7 > strlen(p))
			fnrec.name[i] = 0x20;
		else
			fnrec.name[i] = p[i - 8];
 }
 else {
	for (i = 0; i < 11; i++)
		if (i >= strlen(file) || i > 7)
			fnrec.name[i] = 0x20;
		else
			fnrec.name[i] = file[i];
 }
 fnrec.mode = _FILE;
 fnrec.flag = RW;
 sttime = localtime(&(instat.st_mtime));

 fnrec.time.sec = sttime->tm_sec;
 fnrec.time.min = sttime->tm_min;
 fnrec.time.hour = sttime->tm_hour;

 fnrec.date.day = sttime->tm_mday;
 fnrec.date.month = sttime->tm_mon + 1;
 fnrec.date.year = sttime->tm_year;
 free = fatfree(dp);
 if (free < 0)
	return(free);
 fnrec.first = free;
 fatput(free, 0xffff, dp);
 fnrec.dim = instat.st_size;

 /* Uebertragen der Daten */
 adr = DOSADR(free, clustsiz) + dp->data;
 lseek(dp->fd, adr, 0);

 j = nl = 0;

 while ((len = read(infd, buf, clustsiz)) > 0) {
   if (conv) { /* ASCII-Copy */

	for (i = 0; i < len; i++) {
		if (buf[i] == 0x0a) { /* CR einfuegen */
			outbuf[j++] = 0x0d;
 			fnrec.dim++;
			nl++;
		}
		if (nl == 0)
lf:
			outbuf[j++] = buf[i];
		if (j == clustsiz) {
			j = 0;
			if (write(dp->fd, outbuf, clustsiz) != clustsiz) {
				fatdel(&fnrec, dp);
				return(-2);
			}
			if ((follow = fatfree(dp)) < 0) {
				fatdel(&fnrec, dp);
				return(follow);
			}
			fatput(free, follow, dp);
			fatput(follow, 0xffff, dp);
			free = follow;
			adr = DOSADR(free, clustsiz)+dp->data;
			lseek(dp->fd, adr, 0);
		}
		if (nl) {
			nl = 0;
			goto lf;
		}
	} /* for */
   } else { /* Binary Copy */
	if (len != clustsiz) {
		j = len;
		break;
	}
	if (write(dp->fd, outbuf, clustsiz) != clustsiz) {
		fatdel(&fnrec, dp);
		return(-2);
	}
	if ((follow = fatfree(dp)) < 0) {
		fatdel(&fnrec, dp);
		return(follow);
	}
	fatput(free, follow, dp);
	fatput(follow, 0xffff, dp);
	free = follow;
	adr = DOSADR(free, clustsiz)+dp->data;
	lseek(dp->fd, adr, 0);
   }
 } /* while */
 if (len < 0) {
	fatdel(&fnrec, dp);
	return(-4);
 }
 if (j)
	if (write(dp->fd, outbuf, j) != j) {
		fatdel(&fnrec, dp);
		return(-2);
	}
 close(infd);

 /* Schreiben eines Verzeichniseintrages */
 lseek(dp->fd, depos, 0);
 if (putde(&fnrec, dp) < 0) {
	fatdel(&fnrec, dp);
	return(-2);
 }
 return(1);
}

/* putde
 * Schreibt fuer das File stp auf dem
 * Datentraeger dp einen 32-Byte
 * Verzeichniseintrag.
 * Liefert -2 bei Schreibfehler.
 */
putde(stp, dp)
register struct dostat *stp;
register struct dosdisc *dp;
{
 register i;
 register unsigned tv;
 unsigned char buf[32];

 for (i = 0; i < 11; i++)
	buf[i] = stp->name[i];
 buf[11] = 0x20; /* Attribut */
 for (i = 12; i < 22; i++)
	buf[i] = '\0';

 tv = stp->time.hour * 2045 + stp->time.min * 32 + stp->time.sec / 2;
 buf[22] = tv % 256;
 buf[23] = tv / 256;
 tv = (stp->date.year - 80) * 512 + stp->date.month * 32 + stp->date.day;
 buf[24] = tv % 256;
 buf[25] = tv / 256;
 buf[26] = stp->first % 256;
 buf[27] = stp->first / 256;
 buf[28] = stp->dim % 256;
 buf[29] = (stp->dim % 65536) / 256;
 buf[30] = (stp->dim % 16777216) / 65536;
 buf[31] = stp->dim / 16777216;
 if (write(dp->fd, buf, 32) != 32)
	return(-2);
 return(1);
}
