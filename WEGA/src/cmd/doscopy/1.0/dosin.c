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
struct dosdisc *dp;
int conv;
{
 struct dostat fnrec, dirrec;
 int res, z = 1, infd, i, j, len, ok = 1, nl, mode;
 char *p, *file, s[LMAX];
 long adr, depos;
 unsigned dnum, nxt, dosnxt();
 unsigned free, follow;
 struct stat instat;
 unsigned char inbuf[1024], outbuf[1024];
 struct tm *sttime, *localtime();
 int dmax, dcnt = 0;
 char *strchr(), *strrchr();

 strcpy(s, fn);
 fn = s;
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
	adr = DOSADR(dirrec.first, dp->clustsiz) + dp->data;
	/* Bestimmen der Anzahl von Verzeichniseintraegen */
	nxt = dirrec.first;
	while ((nxt = dosnxt(dp->fd, nxt, dp->fat1, dp->fatlen)) != 0)
		z++;
	dnum = dp->clustsiz / 32;
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
			adr = DOSADR(nxt, dp->clustsiz) + dp->data;
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
	if ((res = fatput(dirrec.first, free, dp)) < 0)
		return(res);
	if ((res = fatput(free, 0xffff, dp)) < 0)
		return(res);
	adr = DOSADR(free, dp->clustsiz) + dp->data;

	for (i = 0; i < 512; i++)
		inbuf[i] = 0x00;
	lseek(dp->fd, adr, 0);
	z = dp->clustsiz / 512;
	for (i = 0; i < z; i++)
		if (write(dp->fd, inbuf, 512) != 512)
			return(-2);
	/* depos zeigt auf den zu belegenden Eintrag */
	depos = lseek(dp->fd, adr, 0);
 }

 /* Oeffnen des WEGA-Files */
 if ((infd = open(in, 0)) < 0)
	return(-1);
 if (fstat(infd, &instat) < 0)
	return(-1);
 mode = instat.st_mode;
 if ((mode & S_IFMT) == S_IFDIR)
	return(-1);

 /* fnrec wird mit Werten von infd belegt */
 if ((p = strchr(file, '.')) != 0) {
	p++;
	file[strlen(file) - strlen(p) - 1] = '\0';
	for (i = 0; i < 8; i++)
		if (strlen(file) <= i)
			fnrec.name[i] = 0x20;
		else
			fnrec.name[i] = file[i];

	for (i = 8; i < 11; i++)
		if (strlen(p) < i - 7)
			fnrec.name[i] = 0x20;
		else
			fnrec.name[i] = p[i - 8];
 }
 else {
	for (i = 0; i < 11; i++)
		if (strlen(file) <= i || i > 7)
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
 fnrec.first = free = fatfree(dp);
 fatput(free, 0xffff, dp);
 fnrec.dim = instat.st_size;

 /* Uebertragen der Daten */
 adr = DOSADR(free, dp->clustsiz) + dp->data;
 lseek(dp->fd, adr, 0);

 nl = z = 0;
 goto rd;
bad:
 fatdel(&fnrec, dp);
 return(-2);
next:
   if (!ok)
	goto done;
   for (i = 0; i < len; i++) {
	if (!ok)
		break;
	if (conv && inbuf[i] == 0x0a) { /* CR einfuegen */
		outbuf[j++] = 0x0d;
		nl++;
		z++;
		if (z == dp->clustsiz) {
			if (write(dp->fd, outbuf, j) < j)
				ok--;
			follow = fatfree(dp);
			fatput(free, follow, dp);
			fatput(follow, 0xffff, dp);
			free = follow;
			j = 0;
			adr = DOSADR(free, dp->clustsiz)+dp->data;
			lseek(dp->fd, adr, 0);
			z = 0;
		}
		if (j == 1024 && ok) {
			if (write(dp->fd, outbuf, 1024) < 1024)
				ok--;
			j = 0;
		}
	}
	outbuf[j++] = inbuf[i];
	z++;
	if (z == dp->clustsiz) {
		if (write(dp->fd, outbuf, j) < j)
			ok--;
		follow = fatfree(dp);
		fatput(free, follow, dp);
		fatput(follow, 0xffff, dp);
		free = follow;
		j = 0;
		adr = DOSADR(free, dp->clustsiz)+dp->data;
		lseek(dp->fd, adr, 0);
		z = 0;
	}
	if (j == 1024 && ok) {
		if (write(dp->fd, outbuf, 1024) < 1024)
			ok--;
		j = 0;
	}
   } /* for */
   if (z && j) {
	if (write(dp->fd, outbuf, j) < j)
		ok--;
   }
rd:
 j = 0;
 if ((len = read(infd, inbuf, 1024)) > 0)
	goto next;
done:
 close(infd);
 fnrec.dim += nl;
 if (!ok)
	goto bad;

 /* Schreiben eines Verzeichniseintrages */
 depos = lseek(dp->fd, depos, 0);
 if (putde(&fnrec, dp) < 0)
	return(-1);
 return(1);
}

/* fatfree
 * Liefert die naechste freie Clusternummer
 * auf dem Datentraeger dp.
 * Liefert -1 bei Lesefehler und
 *	   -3 falls die Disk voll ist
 */
fatfree(dp)
struct dosdisc *dp;
{
 long pos, off;
 unsigned char n[3], m[2];
 int v0, v1;
 unsigned size;
 int i = 1, res;

 size = (dp->maindir - dp->fat1) / dp->fatnum;

 if (dp->fatlen == 12) {
	size -= 3;
	lseek(dp->fd, dp->fat1+3, 0);
	while (size/3 > i++) {
		if ((res = read(dp->fd, n, 3)) < 3)
			return(-1);
		v0 = (n[1]&0x0f)*256 + n[0];
		v1 = n[2]*16 + ((unsigned)n[1]>>4);
		if (v0 == 0) {
			pos = lseek(dp->fd, -3L, 1);
			off = pos - dp->fat1;
			return(off/1.5);
		}
		if (v1 == 0) {
			pos = lseek(dp->fd, -3L, 1);
			off = pos - dp->fat1;
			return(off/1.5 + 1);
		}
	}
 }
 else {
	size -= 4;
	lseek(dp->fd, dp->fat1+4, 0);
	while (size/2 > i++) {
		if ((res = read(dp->fd, m, 2)) < 2)
			return(-1);
		if ((v0 = m[1]*256 + m[0]) == 0) {
			pos = lseek(dp->fd, -2L, 1);
			off = pos - dp->fat1;
			return(off/2);
		}
	}
 }
 return(-3);
}

/* putde
 * Schreibt fuer das File stp auf dem
 * Datentraeger dp einen 32-Byte
 * Verzeichniseintrag.
 * Liefert -2 bei Schreibfehler.
 */
putde(stp, dp)
struct dostat *stp;
struct dosdisc *dp;
{
 int i;
 unsigned char buf[32];
 unsigned tv;

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
