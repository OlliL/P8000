/**************************************************************************
***************************************************************************
 
	W E G A - Quelle	(C) TU KMSt / ZFT/KEAW  Abt. WAE - 1988
	Programm: dosdir.c
 
	Bearbeiter	: P. Hoge
	Datum		: 24.11.89
	Version		: 1.3
 
***************************************************************************

	Listet DCP-Dateien und -Verzeichnisse im DCP-Format auf

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
 int z=1,i,j,dcnt;
 unsigned char upath[LMAX],dpath[LMAX];
 char label[12];
 struct dosdisc dos;
 struct dostat dstat,estat;
 long adr;
 unsigned dnum,dosnxt(),nxt;

 argc--;
 if(argc == 0) {
   fprintf(stderr,"usage: dosdir dosdev:[dirname] [dosdev:[dirname]] [...]\n");
   exit(1);
 }
 while (argc-- > 0) {
	dcnt = 0;
	label[0] = '\0';
	if ((res=dosplit(*++argv,upath,dpath)) < 0) {
		status = 2;
		continue;
	}
	if (res == 2) {
		strcpy(dpath, "/bin/ls ");
		strcat(dpath, upath);
		status = system(dpath);
		continue;
	}
	if (dosdev(upath,0,&dos) < 0) {
		fprintf(stderr,"%s: Fehler in %s\n",arg0,upath);
		status = 2;
		continue;
	}
	adr = dos.maindir;
	dnum = dos.dirnum;
	lseek(dos.fd,adr,0);

	nxt = dstat.first;
	j = 1;
	while (j++ <= dnum && (getde(dos.fd,&estat)) > 0)
		if (estat.name[0] != 0xe5 && (estat.attr&VOLUME_LABEL) != 0) {
			strcpy(label,estat.name);
	}
	if (*dpath != '\0') {
		/* Es handelt sich nicht um das Stammverzeichnis */
		res=dosopn(dpath,&dos,&dstat);
		if (res == 0) {
			fprintf(stderr,"%s: %s nicht gefunden\n",arg0,dpath);
			status = 2;
			continue;
		}
		if (res < 0) {
			fprintf(stderr,"%s: %s - Lesefehler\n",arg0,upath);
			status = 2;
			continue;
		}
		if (dstat.mode == _FILE) {
			pkopf(upath,dpath,label);
			pdir(&dstat);
			bytefree(1,&dos);
			continue;
		}
		if (dstat.mode == _SDIR) {
		    pkopf(upath,dpath,label);
		    adr = DOSADR(dstat.first,dos.clustsiz)+dos.data;
		    /* Bestimmen Anzahl von Verzeichniseintraegen */
		    nxt = dstat.first;
		    while((nxt=dosnxt(dos.fd,nxt,dos.fat1,dos.fatlen)) > 0)
			z++;
		    dnum = dos.clustsiz/32;
		}
	}
	else { /* Ausgabe des Stammverzeichnisses */
		pkopf(upath, '\0', label);
		adr = dos.maindir;
		dnum = dos.dirnum;
	}
	lseek(dos.fd,adr,0);

	/* Ausgabe aller gueltigen Eintraege */
	nxt = dstat.first;
	for (i=0; i<z; i++) {
	    j = 1;
	    while (j++ <= dnum && (res=getde(dos.fd,&estat)) > 0)
		if (estat.name[0] != 0xe5 && (estat.attr&VOLUME_LABEL) == 0) {
			pdir(&estat);
			dcnt++;
		}
		if ((nxt=dosnxt(dos.fd,nxt,dos.fat1,dos.fatlen)) < 1)
			break;
		/* Naechsten Cluster bereitstellen */
		adr = DOSADR(nxt,dos.clustsiz)+dos.data;
		lseek(dos.fd,adr,0);
	}
	bytefree(dcnt,&dos);

 } /* while */
 exit(status);
}

pkopf(lw,dpath, label)
char *lw,*dpath, *label;
{
 printf("\nVolume in %s hat Label: %s\n",lw, label);
 if (dpath != '\0')
	printf("Verzeichnis von %s:/%s\n\n",lw,dpath);
 else
	printf("Verzeichnis von %s:/\n\n",lw);
}


pdir(pstat)
register struct dostat *pstat;
{
 if (pstat->mode == _SDIR) {
	printf("%8.8s%3.3s ",pstat->name, &pstat->name[8]);
	printf("  <DIR>   ");
 } else {
	printf("%8.8s %3.3s",pstat->name, &pstat->name[8]);
	printf("%9ld ",pstat->dim);
 }
 printf("%2d-%02d-%2d ",pstat->date.month,pstat->date.day,pstat->date.year-1900);
 printf("%2d:%2d\n",pstat->time.hour,pstat->time.min);
}

/* bytefree
 * Summiert alle FAT-Eintraege groesser Null
 * und gibt die so ermittelte Anzahl freier
 * Byte auf dem Bildschirm aus
 */
bytefree(dcnt,dp)
int dcnt;		/* Anz. Dateien im Verzeichnis */
register struct dosdisc *dp;
{
 register FILE *fp;
 register nclust, used=0;
 unsigned char n[3];
 long free;

 fp = fdopen(dp->fd, "r");
 nclust = dp->nclust;

 if (dp->fatlen == 12) {
	fseek(fp,dp->fat1+3,0);
	while ((nclust -=2) > 0) {
		if (fread(n, sizeof(*n),3,fp) != 3)
			return;
		if (n[0] || (n[1]&0x0f))
			used++;
		if ((n[1]&0xf0) || n[2])
			used++;
	}
 }
 else {
	fseek(fp,dp->fat1+4,0);
	while (nclust--) {
		if (fread(n, sizeof(*n),2,fp) != 2)
			return;
		if (n[0] || n[1])
			used++;
	}
 }
 free = (long)(dp->nclust-used) * dp->clustsiz;
 printf("       %d Datei(en)    %ld Bytes frei\n",dcnt, free);
}
