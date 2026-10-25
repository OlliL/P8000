/**************************************************************************
***************************************************************************
 
	W E G A - Quelle	(C) TU KMSt / ZFT/KEAW  Abt. WAE - 1988
	Programm: doscp.c
 
	Bearbeiter	: P. Hoge
	Datum		: 24.11.89
	Version		: 1.3
 
***************************************************************************

	Kopieren von DCP-Dateien in WEGA-Files und umgekehrt

***************************************************************************
**************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "dos.h"

main(argc, argv)
register char **argv;
{
 register char *arg0 = argv[0];
 char *argl;			/* letztes Argumentes */
 register int status = 0, res;
 int conv = 1;			/* Newlinekonvertierung */
 int fd;			/* WEGA-Filedeskriptor */
 char upath[LMAX], dpath[LMAX];
 char *p, t[2*LMAX];
 char *strrchr(), *strcat();
 struct dosdisc dos;
 struct dostat dstat;
 struct stat mstat;
 int dtm = 1;			/* DCP -> WEGA */
 int dir = 0;			/* letztes Argument ist Verzeichnis */

 argc--;
 argl = argv[argc];
 for (; argc > 0 && argv[1][0] == '-'; argc--, argv++) {
	switch (argv[1][1]) {
	case '\0':
		break;
	case 'r':
		--conv;		/* keine Newlinekonv. */
		break;
	default:
		fprintf(stderr,"%s: falsche Option %c\n",arg0,argv[1][1]);
		exit(1);
	}
 }
 if (argc < 2)
	usage();
 if ((p = strrchr(argl, ':')) != 0) {
	dtm--;		/* WEGA -> DCP */
	if (dosplit(argl, upath, dpath) < 0)
		exit(1);
	if (dosdev(upath, 2, &dos) < 0) {
		fprintf(stderr, "%s: Fehler in %s\n", arg0, upath);
		exit(1);
	}
	if (*dpath == '\0')
		dir++;
	else {
		res = dosopn(dpath, &dos, &dstat);
		if (res == 0 && argc > 2) {
			fprintf(stderr, "%s: %s nicht gefunden\n", arg0, dpath);
			exit(1);
		}
		if (res < 0) {
			fprintf(stderr, "%s: %s - Lesefehler\n", arg0, upath);
			exit(1);
		}
		if (dstat.mode == _SDIR)
		dir++;
	}
 }
 else {	/* letztes Argument ist kein DCP-Pfad */
	if (stat(argl, &mstat) < 0) {
	    if (argc > 2) {
		fprintf(stderr, "%s: %s nicht gefunden\n", arg0, argl);
		exit(1);
	    }
	}
	else
		if ((mstat.st_mode & S_IFMT) == S_IFDIR)
			dir++;
 }
 if (!dir) {
	if (argc != 2)
		usage();
 	argv++;
	if (dtm) {
	    if ((fd = creat(argl, 0664)) < 0) {
		fprintf(stderr, "%s: %s nicht erstellbar\n", arg0, argl);
		exit(1);
	    }
	    if ((res = dosplit(*argv, upath, dpath)) < 0)
		exit(1);
	    if (res == 2) {
		strcpy(t, "/bin/cp ");
		strcat(t, *argv);
		strcat(t, " "),
		strcat(t, argl);
		exit(system(t));
	    }
	    if (*dpath == '\0') {
		fprintf(stderr, "%s: keine DCP-Datei\n", arg0);
		exit(1);
	    }
	    if ((dosdev(upath, 0, &dos)) < 0) {
		fprintf(stderr, "%s: Fehler in %s\n", arg0, upath);
		exit(1);
	    }
	    res = dosopn(dpath, &dos, &dstat);
	    if (res == 0) {
		fprintf(stderr, "%s: %s nicht gefunden\n", arg0, dpath);
		exit(1);
	    }
	    if (res < 0) {
		fprintf(stderr, "%s: %s Lesefehler\n", arg0, upath);
		exit(1);
	    }
	    switch (dosout(&dstat, fd, &dos, conv)) {
	    case -1:
		fprintf(stderr, "%s: %s - Lesefehler\n", arg0, upath);
		exit(1);
	    case -2:
		fprintf(stderr, "%s: Schreibfehler in %s\n", arg0,argl);
		exit(1);
	    }
	}
	else {	/* WEGA -> DCP */
	    if (strrchr(*argv, ':') != 0)
		usage();
	    switch (dosin(*argv, dpath, &dos, conv)) {
	    case 0:
		fprintf(stderr,"%s: Verzeichnis nicht gefunden\n",arg0);
		exit(1);
	    case -1:
		fprintf(stderr,"%s: Lesefehler in %s\n",arg0,upath);
		exit(1);
	    case -2:
		fprintf(stderr,"%s: Schreibfehler in %s\n",arg0,upath);
		exit(1);
	    case -3:
		fprintf(stderr,"%s: %s ist Verzeichnis oder schreibgeschuetzt\n",arg0,dpath);
		exit(1);
	    case -4:
		fprintf(stderr,"%s: Lesefehler in %s\n",arg0,*argv);
		exit(1);
	    }
	}
 }
 else {	/* Kopieren in ein Verzeichnis */
	if (dtm) {
	    while (--argc) {
		argv++;
		strcpy(t, argl);	/* Zielverzeichnis */
		strcat(t, "/");
		if ((res = dosplit(*argv, upath, dpath)) < 0)
			exit(1);
		if (res == 2) {
			strcpy(t, "cp ");
			strcat(t, *argv);
			strcat(t, " "),
			strcat(t, argl);
			system(t);
			continue;
		}
		if (*dpath == '\0') {
			fprintf(stderr, "%s: kein DCP Pfad\n", arg0);
			status = 2;
			continue;
		}
		if (dosdev(upath, 0, &dos) < 0) {
			fprintf(stderr,"%s: Fehler in %s\n",arg0,upath);
			status = 2;
			continue;
		}
		res = dosopn(dpath, &dos, &dstat);
		if (res == 0) {
			fprintf(stderr, "%s: %s nicht gefunden\n", arg0, dpath);
			status = 2;
			continue;
		}
		if (res < 0) {
			fprintf(stderr, "%s: %s - Lesefehler\n", arg0, upath);
			status = 2;
			continue;
		}
		if ((p = strrchr(dpath, '/')) != 0) /* letzter Teil DCP-Pfad */
			p++;
		else
			p = dpath;
		p = strcat(t, p);
		while (*p != '\0')
			*p++ = isupper(*p) ? tolower(*p) : *p;
		if ((fd = creat(t, 0664)) < 0) {
			fprintf(stderr, "%s: %s nicht erzeugbar\n", arg0, t);
			status = 2;
			continue;
		}
		switch (dosout(&dstat, fd, &dos, conv)) {
		case -1:
			fprintf(stderr,"%s: %s Lesefehler\n",arg0,upath);
			status = 2;
			break;
		case -2:
			fprintf(stderr,"%s: %s Schreibfehler\n",arg0,t);
			status = 2;
			break;
		}
		close(fd);
	    }	/* while */
	}
	else { /* WEGA -> DCP */
	    while (--argc) {
		argv++;
		strcpy(t, dpath);
		if (*t != '\0')
			strcat(t, "/");
		if (strrchr(*argv, ':') != 0)
			usage();
		if ((p = strrchr(*argv, '/')) != 0)
			p++;
		else
			p = *argv;
		p = strcat(t, p);
		while (*p != '\0')
			*p++ = islower(*p) ? toupper(*p) : *p;

		switch (dosin(*argv, t, &dos, conv)) {
		case 0:
		    fprintf(stderr,"%s: Verzeichnis nicht gefunden\n",arg0);
		    status = 2;
		    break;
		case -1:
		    fprintf(stderr, "%s: Lesefehler in %s\n", arg0,upath);
		    status = 2;
		    break;
		case -2:
		    fprintf(stderr,"%s: Schreibfehler in %s\n",arg0,upath);
		    status = 2;
		    break;
		case -3:
		    fprintf(stderr,"%s: %s ist schreibgeschuetzt\n",arg0,t);
		    status = 2;
		    break;
		case -4:
		    fprintf(stderr, "%s: Lesefehler in %s\n", arg0, *argv);
		    status = 2;
		    break;
		}
	    } /* while */
	}
 }
 exit(status);
}

usage()
{
 fprintf(stderr,"usage: doscp [-r] dosdev:filename wegafilename\n");
 fprintf(stderr,"       doscp [-r] dosdev:filename [dosdev:filename] [...] wegadirname\n");
 fprintf(stderr,"       doscp [-r] wegafilename dosdev:filename\n");
 fprintf(stderr,"       doscp [-r] wegafilename [wegafilename] [...] dosdev:dirname\n");
 exit(1);
}
