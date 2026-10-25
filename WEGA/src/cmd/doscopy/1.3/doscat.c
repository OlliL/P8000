/**************************************************************************
***************************************************************************
 
	W E G A - Quelle	(C) TU KMSt / ZFT/KEAW  Abt. WAE - 1988
	Programm: doscat.c
 
	Bearbeiter	: P. Hoge
	Datum		: 24.11.89
	Version		: 1.3
 
***************************************************************************

	Kopieren von DOS-Dateien auf die Standardausgabe

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
 int conv = 1;		/* Newlinekonvertierung */
 register int status = 0,res;
 char upath[LMAX],dpath[LMAX];
 struct dosdisc dos;
 struct dostat dstat;

 argc--;
 for (; argc > 0 && argv[1][0] == '-';argc--,argv++)
	switch (argv[1][1]) {
	case 0:
		break;
	case 'r':
		conv--;	/* keine Newlinekonv. */
		break;
	default:
		fprintf(stderr,"%s: falsche Option %c\n",arg0,argv[1][1]);
		exit(1);
 }
 if (argc == 0) {
	fprintf(stderr,"usage: doscat [-r] dosdev:filename [dosdev:filename] [...]\n");
	exit(1);
 }
 while (argc-- > 0) {
 	if (dosplit(*++argv,upath,dpath) < 0) {
		status = 2;
		continue;
	}
	if (*dpath == '\0') {
		strcpy(dpath, "/bin/cat ");
		strcat(dpath, upath);
		status = system(dpath);
		continue;
	}
	if (dosdev(upath,0,&dos) < 0) {
		fprintf(stderr,"%s: Fehler in %s\n",arg0,upath);
		status = 2;
		continue;
	}
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
	res = dosout(&dstat,1,&dos,conv);
	switch (res) {
	case -1:
		fprintf(stderr,"%s: %s - Lesefehler\n",arg0,upath);
		status = 2;
		break;
	case -2:
		fprintf(stderr,"%s: Fehler bei der Standardausgabe\n",arg0);
		status = 2;
		break;
	}
 }
 exit(status);
}
