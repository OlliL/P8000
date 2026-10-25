/**************************************************************************
***************************************************************************
 
	W E G A - Quelle	(C) TU KMSt / ZFT/KEAW  Abt. WAE - 1988
	Programm: dosopn.c
 
	Bearbeiter	: P. Hoge
	Datum		: 24.11.89
	Version		: 1.3
 
***************************************************************************
**************************************************************************/

#include "dos.h"

/* dosopn
 * Durchmustert ein DCP-Verzeichnis,
 * uebergibt die Adresse  einer Struktur dostat
 * fuer die durch path beschriebene Datei.
 * Liefert -1 bei Fehler und 0 wenn nicht gefunden
 */
dosopn(path,dp,stp)
register char *path;
register struct dosdisc *dp;
register struct dostat *stp;
{
 char s[LMAX],*p;
 long dadr;
 unsigned next,dosnxt();
 int res,dn;
 char *strchr();

 dn = dp->dirnum;
 dadr = dp->maindir;
 next = 0;
 strcpy(s,path);
 path = s;
 if (path[0] == '/')
	path++;
 while (path != 0 && path[0] != '\0') {
 	if ((p=strchr(path,'/')) != 0)
		path[strlen(path)-strlen(p++)] = '\0';
	while (-1) {
		if ((res=dosrde(dp->fd,path,dn,dadr,stp)) < 0)
			return(res);
		if (res == 1) {
			next = stp->first;
			dadr = DOSADR(next,dp->clustsiz)+dp->data;
			break;
		}
		dn = dp->clustsiz/32;
		if ((next=dosnxt(dp->fd,next,dp->fat1,dp->fatlen))<1)
			return(0);
		dadr = DOSADR(next,dp->clustsiz)+dp->data;
	}
	path = p;
 }
 return(1);
}


/* dosrde
 * Sucht einen Verzeichniseintrag und
 * belegt eine Struktur dostat mit Werten
 */
dosrde(fd,fn,dnum,adr,stp)
int fd;
register char *fn;	/* Name des zu suchenden DCP-Files */
int dnum;		/* max. moegliche Anzahl von Verzeichniseintraegn */
register long adr;	/* Startadresse des Verzeichnisses */
register struct dostat *stp;
{
 int res;
 char *strchr(),*ext, c='\0';

 lseek(fd,adr,0);
 ext = strchr(fn,'.');
 if (ext)
	ext++;
 else
	ext = &c;

 while ((res=getde(fd,stp)) > 0 ) {
	if (dnum-- == 0)
		return(0);
	if (ncmp(fn, stp->name, 8) && ncmp(ext, &stp->name[8], 3))
		break;
 }
 return(res);
}

ncmp(a,b,n)
register char *a,*b;
register int n;
{
 while (n--) {
	if (*b == 0x20 && (*a == '\0' || *a == '.'))
		break;
	if (*a++ != *b++)
		return(0);
 }
 return(1);
}


/* getde
 * liest einen 32-Byte Verzeichniseintrag und belegt
 * dostat mit den Dateiparametern.
 * Beachte: Der Bytezaehler muss vorher auf das erste
 * Byte des Eintrages zeigen !
 * Die Funktion liefert 0, wenn keine gueltigen Eintraege
 * mehr im Verzeichnis stehen und -1 bei Lesefehler.
 */
getde(fd,stat_buf)
register int fd;
register struct dostat *stat_buf;
{
 register int i;
 register unsigned date,time;
 unsigned char dirbuf[32];

 if (read(fd,dirbuf,32)!=32)
	return(-1);

 for (i=0;i<11;i++)
	stat_buf->name[i]=dirbuf[i];
 stat_buf->name[i]='\0';

 if (stat_buf->name[0]=='\0')
	return(0);

 if (dirbuf[11]&0x01)
	stat_buf->flag = RO;
 else
	stat_buf->flag = RW;
 stat_buf->attr = dirbuf[11];
 if (dirbuf[11]&0x02)
	stat_buf->mode = _BLOCKED;
 else if (dirbuf[11]&0x04)
	stat_buf->mode = _SYSDAT;
 else if (dirbuf[11]&0x10)
	stat_buf->mode = _SDIR;
 else
	stat_buf->mode = _FILE;

 stat_buf->first=dirbuf[27]*256+dirbuf[26];
 stat_buf->dim=dirbuf[31]*16777216+dirbuf[30]*65536+dirbuf[29]*256+dirbuf[28];
 date=dirbuf[25]*256 + dirbuf[24];
 time=dirbuf[23]*256 + dirbuf[22];

 stat_buf->date.year=date/512 + 1980;
 stat_buf->date.month=(date&0x01ff)/32;
 stat_buf->date.day=date&0x001f;

 stat_buf->time.hour=time/2048;
 stat_buf->time.min=(time&0x07ff)/32;
 stat_buf->time.sec=(time&0x001f)*2;
 return(1);
}


/* Bereitstellen der naechsten Clusternummer
 * liefert 0 bei Dateiende
 */
unsigned dosnxt(fd,first,fat,fatlen)
register int fd;
register unsigned first;
register unsigned long fat;
register int fatlen;
{
 register unsigned nxt;
 unsigned char b[2];

 if (fatlen == 12) {
	lseek(fd,fat+(long)(1.5*first),0);
	read(fd,b,2);

	if (first % 2 == 0)
		nxt = (b[1]&0x0f)*256 + b[0];
	else
		nxt = b[1]*16 + b[0]/16;

	if (nxt >= 0x0ff8 && nxt <= 0x0fff)
		nxt = 0;
 }
 else {
	lseek(fd,fat+(long)(2*first),0);
	read(fd,b,2);
	nxt = b[1]*256+b[0];
	if (nxt >= 0xfff8 && nxt <= 0xffff)
		nxt = 0;
 }
 return(nxt);
}
