/**************************************************************************
***************************************************************************
 
	W E G A - Quelle	(C) TU KMSt / ZFT/KEAW  Abt. WAE - 1988
	Programm: dosls.c
 
	Bearbeiter	: P. Hoge
	Datum		: 24.11.89
	Version		: 1.3
 
***************************************************************************

	Listet DCP-Dateien und -Verzeichnisse im WEGA-Format auf

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
 int z=1,i,j;
 unsigned char upath[LMAX],dpath[LMAX];
 struct dosdisc dos;
 struct dostat dstat;
 long adr;
 unsigned dnum,dosnxt(),nxt;

 argc--;
 if(argc == 0) {
   fprintf(stderr,"usage: dosls dosdev:[dirname] [dosdev:[dirname]] [...]\n");
   exit(1);
 }
 while (argc-- > 0) {
	if ((res=dosplit(*++argv,upath,dpath)) < 0) {
		status = 2;
		continue;
	}
	if (res == 2) {
		strcpy(dpath, "/bin/ls -l ");
		strcat(dpath, upath);
		status = system(dpath);
		continue;
	}
	if (dosdev(upath,0,&dos) < 0) {
		fprintf(stderr,"%s: Fehler in %s\n",arg0,upath);
		status = 2;
		continue;
	}
	if (*dpath != '\0') {
		/* Es handelt sich nicht um das Stammverzeichnis */
		res = dosopn(dpath,&dos,&dstat);
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
			pls(&dstat);
			continue;
		}
		if (dstat.mode == _SDIR) {
		    printf("\n%s:/%s\n",upath,dpath);
		    adr = DOSADR(dstat.first,dos.clustsiz)+dos.data;
		    /* Bestimmen Anzahl von Verzeichniseintraegen */
		    nxt = dstat.first;
		    while ((nxt=dosnxt(dos.fd,nxt,dos.fat1,dos.fatlen)) > 0)
			z++;
		    dnum = dos.clustsiz/32;
		}
 	}
	else { /* Ausgabe des Stammverzeichnisses */
		printf("\n%s:/\n",upath);
		adr = dos.maindir;
		dnum = dos.dirnum;
	}
	lseek(dos.fd,adr,0);
	/* Ausgabe aller gueltigen Eintraege */
	nxt = dstat.first;
	for (i=0;i<z;i++) {
		j = 1;
		while(j++ <= dnum && (getde(dos.fd,&dstat)) > 0)
			if (dstat.name[0] != 0xe5)
				pls(&dstat);
		if ((nxt=dosnxt(dos.fd,nxt,dos.fat1,dos.fatlen)) < 1)
			break;
		else { /* Naechsten Cluster bereitstellen */
			adr = DOSADR(nxt,dos.clustsiz)+dos.data;
			lseek(dos.fd,adr,0);
		}
	}
 } /* while */
 exit(status);
}

pls(pstat)
register struct dostat *pstat;
{
 if (pstat->mode == _SDIR)
	printf("d");
 else
	printf("-");
 if (pstat->attr & READ_ONLY)
 	printf("r--r--r-");
 else
 	printf("rw-rw-rw");
 printf("- 1 dpath        ");
 printf("%6ld ",pstat->dim);
 printf("%2d %2d %2d:%2d ",pstat->date.month,pstat->date.day,pstat->time.hour,pstat->time.min);
 printf("%8.8s %3.3s\n",pstat->name, &pstat->name[8]);
}
