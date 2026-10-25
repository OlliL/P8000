/**************************************************************************
***************************************************************************
 
	W E G A - Quelle	(C) TU KMSt / ZFT/KEAW  Abt. WAE - 1988
	Programm: dosrm.c
 
	Bearbeiter	: P. Hoge
	Datum		: 24.11.89
	Version		: 1.3
 
***************************************************************************

	Loeschen von DOS-Dateien

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
    fprintf(stderr,"usage: dosrm dosdev:filename [dosdev:filename] [...]\n");
    exit(1);
 }
 while (argc-- > 0) {
	if (dosplit(*++argv,upath,dpath) < 0) {
		status = 2;
		continue;
	}
	if (*dpath == '\0') {
		fprintf(stderr,"%s: %s kein DCP-Pfad\n",arg0,upath);
		status = 2;
		continue;
	}
	if (dosdev(upath,2,&dos) < 0) {
		fprintf(stderr,"%s: Fehler in %s\n",arg0,upath);
		status = 2;
		continue;
	}
	res = dosdel(dpath,&dos);
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
		fprintf(stderr,"%s: %s ist Verzeichnis oder schreibgeschuetzt\n",arg0,dpath);
		status = 2;
		break;
	}
 }
 exit(status);
}
