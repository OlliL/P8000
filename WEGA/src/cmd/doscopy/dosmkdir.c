/**************************************************************************
***************************************************************************
 
	W E G A - Quelle	(C) TU KMSt / ZFT/KEAW  Abt. WAE - 1988
	Programm: dosmkdir.c
 
	Bearbeiter	: P. Hoge
	Datum		: 24.1.89
	Version		: 1.3
 
***************************************************************************

	Erzeugen von DCP-Verzeichnissen

***************************************************************************
**************************************************************************/

#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "dos.h"

main(argc,argv)
register char **argv;
{
 register char *arg0 = argv[0];
 register int status = 0,res;
 char upath[LMAX],dpath[LMAX];
 struct dosdisc dos;
 struct dostat dstat;

 argc--;
 if (argc == 0) {
	fprintf(stderr,"usage: dosmkdir dosdev:dirname\n");
	exit(1);
 }
 while (argc-- > 0) {
	if ((res=dosplit(*++argv,upath,dpath)) < 0) {
		status = 2;
		continue;
	}
	if (res == 2) {
		strcpy(dpath, "/bin/mkdir ");
		strcat(dpath, upath);
		status = system(dpath);
		continue;
	}
	if (*dpath == '\0') {
		fprintf(stderr,"%s: kein DCP-Pfad angegeben\n",arg0);
		status = 2;
		continue;
	}
	if (dosdev(upath,2,&dos) < 0) {
		fprintf(stderr,"%s: Fehler in %s\n",arg0,upath);
		status = 2;
		continue;
	}
	res = msubdir(dpath,&dos);
	switch (res) {
	case 0:
		fprintf(stderr,"%s: uebergeordnetes Verzeichnis fehlt\n",arg0);
		status = 2;
		break;
	case -1:
		fprintf(stderr,"%s: %s - existiert bereits\n",arg0,dpath);
		status = 2;
		break;
	case -2:
		fprintf(stderr,"%s: %s - Schreibfehler\n",arg0,upath);
		status = 2;
		break;
	}
 }
 exit(status);
}

/* msubdir
 * Traegt das Unterverzeichnis subdir
 * auf dem Datentraeger dp ein.
 * Liefert 0 falls das uebergeordnete
 * Verzeichnis nicht existiert ,
 *	  -1 falls subdir bereits existiert und
 *	  -2 bei Schreibfehler
 */
msubdir(subdir,dp)
register char *subdir;
register struct dosdisc *dp;
{
 struct dostat fnrec,dirrec,pkt,dpkt;
 int res,z=1,i,j,io=1;
 char *p,*file;
 char *strchr();
 long adr,depos,time(),zeit;
 unsigned dnum,nxt,dosnxt(),dpktfirst;
 unsigned char c,buf[512];
 struct tm *sttime,*localtime();
 register int free;
 int dmax,dcnt=0;

 if ((res=dosopn(subdir,dp,&fnrec)) < 0)
	return(-2);
 if (res > 0)
	return(-1);

 file = subdir;
 while ((p=strchr(file,'/')) != 0)
	file = ++p;

 subdir[strlen(subdir)-strlen(file)-1] = '\0';

 if (file == subdir) {
	dmax = dnum = dp->dirnum;
	adr = dp->maindir;
	dpktfirst = nxt = 0;
 }
 else {	/* Das uebergeordnete Verzeichnis ist nicht das
	 * Stammverzeichnis !
	 */
	if ((res=dosopn(subdir,dp,&dirrec)) < 1)
		return(res);
	if (dirrec.mode != _SDIR)
		return(0);
	dpktfirst = dirrec.first;
	adr = DOSADR(dirrec.first,dp->clustsiz)+dp->data;

	/* Bestimmen der Anzahl von Verzeichniseintraegen */
	nxt = dirrec.first;
	while ((nxt=dosnxt(dp->fd,nxt,dp->fat1,dp->fatlen)) > 0)
		z++;
	dnum = dp->clustsiz/32;
	dmax = z*dnum;
	nxt = dirrec.first;
 }
 lseek(dp->fd,adr,0);
 /* Suche des ersten freien Eintrages */
 for (i=0;i<z;i++) {
	j = 1;
	while (j++ <= dnum && (res=getde(dp->fd,&fnrec)) > 0) {
		dcnt++;
		if (fnrec.name[0] == 0xe5) {
			i = z;
			depos = lseek(dp->fd,(long)-32,1);
			break;
		}
	}
	if (res == 0) {
		depos = lseek(dp->fd,(long)-32,1);
		break;
	}
	if (nxt != 0) {
		if ((nxt=dosnxt(dp->fd,nxt,dp->fat1,dp->fatlen)) < 1)
			break;
		else { /* Naechsten Cluster lesen */
			adr = DOSADR(nxt,dp->clustsiz)+dp->data;
			lseek(dp->fd,adr,0);
			dirrec.first = nxt;
		}
	}
 }
 if (res < 0)
 	return(-2);
 if (subdir == file && dcnt >= dmax)
	return(-2);	/* Stammverzeichnis voll */
 if (dcnt >= dmax) {	/* Verzeichniscluster voll */
	if ((free=fatfree(dp)) < 0)
		return(-2);
	if (fatput(dirrec.first,free,dp) < 0)
		return(-2);
	if (fatput(free,0xffff,dp) < 0)
		return(-2);
	adr = DOSADR(free,dp->clustsiz)+dp->data;
	for (i=0;i<512;i++)
		buf[i] = 0x00;
	lseek(dp->fd,adr,0);
	z = dp->clustsiz/512;
	for (i=0;i<z;i++)
		if (write(dp->fd,buf,512) != 512)
			return(-2);
	/* depos zeigt auf den zu belegenden Eintrag */
	depos = lseek(dp->fd,adr,0);
 }
 /* fnrec wird mit Werten von subdir belegt */
 if ((p=strchr(file,'.')) != 0) {
 	p++;
	file[strlen(file)-strlen(p)-1] = '\0';
	for (i=0;i<8;i++)
		if (i >= strlen(file))
			fnrec.name[i] = 0x20;
		else
			fnrec.name[i] = file[i];
	for (i=8;i<11;i++)
		if (i-7 > strlen(p))
			fnrec.name[i] = 0x20;
		else
			fnrec.name[i] = p[i-8];
 }
 else {
	for (i=0;i<11;i++)
		if (i >= strlen(file) || i > 7)
			fnrec.name[i] = 0x20;
		else
			fnrec.name[i] = file[i];
 }

 fnrec.mode = _SDIR;
 fnrec.flag = RW;
 zeit = time(0);
 sttime = localtime(&zeit);

 fnrec.time.sec = sttime->tm_sec;
 fnrec.time.min = sttime->tm_min;
 fnrec.time.hour = sttime->tm_hour;

 fnrec.date.day = sttime->tm_mday;
 fnrec.date.month = sttime->tm_mon+1;
 fnrec.date.year = sttime->tm_year;

 if ((free = fatfree(dp)) < 0)
	return(-2);
 fnrec.first = free;
 fatput(free,0xffff,dp);
 fnrec.dim = 0;

 /* Uebernehmen der Eintraege . und .. */
 adr = DOSADR(free,dp->clustsiz)+dp->data;
 for (i=0;i<11;i++)
	if (i == 0)
		pkt.name[i] = 0x2e;
	else
		pkt.name[i] = 0x20;
 pkt.name[11] = '\0';
 pkt.mode = _SDIR;
 pkt.flag = RW;
 dpkt.time.sec = pkt.time.sec = fnrec.time.sec;
 dpkt.time.min = pkt.time.min = fnrec.time.min;
 dpkt.time.hour = pkt.time.hour = fnrec.time.hour;
 dpkt.date.day = pkt.date.day = fnrec.date.day;
 dpkt.date.month = pkt.date.month = fnrec.date.month;
 dpkt.date.year = pkt.date.year = pkt.date.year;
 pkt.dim = 0;
 pkt.first = fnrec.first;

 dpkt.name[0] = 0x2e;
 dpkt.name[1] = 0x2e;
 for (i=2;i<11;i++)
	dpkt.name[i] = 0x20;
 dpkt.name[11] = '\0';
 dpkt.mode = _SDIR;
 dpkt.flag = RW;
 dpkt.dim = 0;
 dpkt.first = dpktfirst;

 lseek(dp->fd,adr,0);
 for (i=0;i<512;i++)
	buf[i] = 0x00;
 z = dp->clustsiz/512;
 for (i=0;i<z;i++)
	if (write(dp->fd,buf,512) != 512)
		return(-2);

 lseek(dp->fd,adr,0);
 if (putdir(&pkt,dp) < 0)
	io--;
 if (io)
	if (putdir(&dpkt,dp) < 0)
		io--;
 if (!io) {
	fatdel(&fnrec,dp);
	return(-2);
 }

 /* Schreiben des Verzeichniseintrages subdir */
 lseek(dp->fd,depos,0);
 if (putdir(&fnrec,dp) < 0)
	return(-2);
 return(1);
}


putdir(stp,dp)
register struct dostat *stp;
register struct dosdisc *dp;
{
 register int res,i;
 unsigned char attrib=0x10,*h,buf[32];
 unsigned tv;

 for (i=0;i<11;i++)
	buf[i] = stp->name[i];
 buf[11] = attrib;
 for (i=12;i<22;i++)
	buf[i] = '\0';

 tv = stp->time.hour*2045 + stp->time.min*32 + stp->time.sec/2;
 buf[22] = tv % 256;
 buf[23] = tv / 256;

 tv = (stp->date.year-80)*512 + stp->date.month*32 + stp->date.day;
 buf[24] = tv % 256;
 buf[25] = tv / 256;
 buf[26] = stp->first % 256;
 buf[27] = stp->first / 256;
 buf[28] = stp->dim % 256;
 buf[29] = (stp->dim % 65536) / 256;
 buf[30] = (stp->dim % 16777216) / 65536;
 buf[31] = (stp->dim / 16777216);

 if (write(dp->fd,buf,32) != 32)
	return(-2);
 return(1);
}
