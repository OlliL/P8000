/**************************************************************************
***************************************************************************
 
	W E G A - Quelle	(C) TU KMSt / ZFT/KEAW  Abt. WAE - 1988
	Programm: dosrmdir.c
 
	Bearbeiter	: P. Hoge
	Datum		: 24.11.89
	Version		: 1.3
 
***************************************************************************

	Loeschen von DOS-Verzeichnissen

***************************************************************************
**************************************************************************/

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
	fprintf(stderr,"usage: dosrmdir dosdev:dirname\n");
	exit(1);
 }
 while (argc-- > 0) {
	if ((res=dosplit(*++argv,upath,dpath)) < 0) {
		status = 2;
		continue;
	}
	if (res == 2) {
		strcpy(dpath, "/bin/rmdir ");
		strcat(dpath, upath);
		status = system(dpath);
		continue;
	}
	if (*dpath == '\0') {
		fprintf(stderr,"%s: falsches Verzeichnis angegeben\n",arg0);
		status = 2;
		continue;
	}
	if (dosdev(upath,2,&dos) < 0) {
		fprintf(stderr,"%s: Fehler in %s\n",arg0,upath);
		status = 2;
		continue;
	}
	res = deldir(dpath,&dos);
	switch (res) {
	case 0:
		fprintf(stderr,"%s: %s nicht gefunden\n",arg0,dpath);
		status = 2;
		break;
	case -1:
		fprintf(stderr,"%s: %s - Lesefehler\n",arg0,upath);
		status = 2;
		break;
	case -2:
		fprintf(stderr,"%s: %s - Schreibfehler\n",arg0,upath);
		status = 2;
		break;
	case -3:
		fprintf(stderr,"%s: %s ist kein Verzeichnis oder schreibgeschuetzt\n",arg0,dpath);
		status = 2;
		break;
	case -4:
		fprintf(stderr,"%s: %s ist nicht leer\n",arg0,dpath);
		status = 2;
		break;
	}
 }
 exit(status);
}

/* deldir
 * Loescht das DCP-Verzeichnis dir auf dem Datentraeger dp.
 * Liefert 0 ,wenn nicht gefuden ,
 *	  -1 ,bei Lesefehler ,
 *	  -2 ,bei Schreibfehler
 *	  -3 ,falls dir kein Verzeichnis bzw
 *	      schreibgeschuetzt und
 *	  -4 ,falls dir nicht leer ist.
 */
deldir(dir,dp)
char *dir;
struct dosdisc *dp;
{
 int fatdel(),res,i,z=1,empty=1;
 struct dostat fnrec,dirrec;
 long adr,eadr;
 unsigned char c,delsign=0xe5;
 char *file,*p,*strchr();
 unsigned dnum,nxt,dosnxt();

 if ((res=dosopn(dir,dp,&fnrec))<1)
	return(res);
 if (fnrec.mode != _SDIR || fnrec.flag == RO)
	return(-3);
 file = dir;
 while ((p=strchr(file,'/')) != 0)
	file = ++p;
 dir[strlen(dir)-strlen(file)-1] = '\0';

 if (file == dir) {
	dnum = dp->dirnum;
	adr = dp->maindir;
	nxt = 0;
 }
 else {	/* Das uebergeordnete Verzeichnis ist nicht das
	 * Stammverzeichnis
	 */
	if ((res=dosopn(dir,dp,&dirrec)) < 1)
		return(res);
	adr = DOSADR(dirrec.first,dp->clustsiz)+dp->data;
	nxt = dirrec.first;
	/* Bestimmen der Anzahl von Verzeichnisclustern */
	while ((nxt=dosnxt(dp->fd,nxt,dp->fat1,dp->fatlen)) > 0)
		z++;
	dnum = dp->clustsiz/32;
 }
 /* Suchen des zu loeschenden Eintrages */
 for (i=0;i<z;i++) {
	if ((res=dosrde(dp->fd,file,dnum,adr,&dirrec)) < 0)
		return(res);
	else
		eadr = lseek(dp->fd,(long)-32,1);
	if (nxt != 0) {
		if ((nxt=dosnxt(dp->fd,nxt,dp->fat1,dp->fatlen)) < 1)
			break;
		/* naechsten Cluster lesen */
		adr = DOSADR(nxt,dp->clustsiz)+dp->data;
	}
 }
 adr = DOSADR(fnrec.first,dp->clustsiz)+dp->data;
 nxt = fnrec.first;
 z = 1;
 while ((nxt=dosnxt(dp->fd,nxt,dp->fat1,dp->fatlen)) > 0)
	z++;
 lseek(dp->fd,adr,0);
 for (i=0;i<z;i++) {
	while ((res=getde(dp->fd,&dirrec)) > 0 && empty)
		if ((c=dirrec.name[0]) != 0xe5 && c != '.')
			empty--;
	if (res < 0)
		return(res);
	if (!empty)
		return(-4);
	if (res == 0)
		break;
	if ((nxt=dosnxt(dp->fd,nxt,dp->fat1,dp->fatlen)) < 1)
		break;
	else {
		adr = DOSADR(nxt,dp->clustsiz)+dp->data;
		lseek(dp->fd,adr,0);
	}
 }
 /* Verzeichnis ist leer -> als geloescht markieren */
 lseek(dp->fd,eadr,0);
 if (write(dp->fd,&delsign,1) != 1)
	return(-2);
 return(fatdel(&fnrec,dp));
}
