/*   Kopieren von DCP-Dateien in MUTOS-Files
 *   und umgekehrt 
*/

#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "dos.h"


main (argc, argv)
char  **argv;
{
    char  **arg0;
    int     conv = 1;		/* Newlinekonvertierung */
    int     status = 0,
            res;
    int     fd,
            fdf;		/* MUTOS-Filedeskriptor */
    char    mutos[40],
            dcp[50],
            h[50],
           *p,
           *strchr (), *ph, tempo[80], t[50];
    struct dosdisc  disc;
    struct dostat   dstat;
    struct stat mstat;
    int     dtm = 1;		/* DCP -> MUTOS */
    int     dir = 0;		/* letztes Argument ist Verzeichnis */
    int     arg;		/* Nummer des letzten Argumentes */
    int     mode;


    arg = --argc;
    arg0 = argv;
    for (; argc > 0 && argv[1][0] == '-'; argc--, arg--, argv++)
	switch (argv[1][1]) {
	    case 0: 
		break;
	    case 'r': 
		--conv;		/* keine Newlinekonv. */
		break;
	    default: 
		fprintf (stderr, "%s -falsche Option %c \n", *arg0, argv[1][1]);
		exit (1);
	}

    if (argc < 2) {
	fprintf (stderr, "usage: doscp [-r] dosdev:filename wegafilename\n");
	fprintf (stderr, "       doscp [-r] dosdev:filename [dosdev:filename] [...] wegadirname\n");
	fprintf (stderr, "       doscp [-r] wegafilename dosdev:filename \n");
	fprintf (stderr, "       doscp [-r] wegafilename [wegafilename] [...] dosdev:filename \n");
	exit (1);
    }
    if ((p = strchr (argv[arg], ':')) != 0) {
	dtm--;			/* MUTOS -> DCP */
	if ((res = dosplit (argv[arg], mutos, dcp)) < 0)
	    exit (1);
	if (*dcp == '\0')
	    dir++;
	else {
	    if ((res = dosdev (mutos, 2, &disc)) < 0) {
		fprintf (stderr, "%s -Fehler im Laufwerk %s \n", *arg0, mutos);
		exit (1);
	    }
	    strcpy (h, dcp);
	    if ((res = dosopn (dcp, &disc, &dstat)) < 1)
		if (res == 0 && argc > 2) {
		    fprintf (stderr, "%s -%s nicht gefunden \n", *arg0, h);
		    exit (1);
		}
		else
		    if (res < 0) {
			fprintf (stderr, "%s -%s -Lesefehler \n", *arg0, mutos);
			exit (1);
		    }
	    if (dstat.mode == _SDIR)
		dir++;
	}
    }
    else {			/* letztes Argument ist kein DCP-Pfad */
	if (stat (argv[arg], &mstat) < 0 && argc > 2) {
	    fprintf (stderr, "%s -%s nicht gefunden \n", *arg0, argv[arg]);
	    exit (1);
	}
	mode = mstat.st_mode;
	if ((mode & S_IFMT) == S_IFDIR)
	    dir++;
    }
    if (!dir) {
	if (argc != 2) {
	    fprintf (stderr, "usage: doscp [-r] dosdev:filename wegafilename\n");
	    fprintf (stderr, "       doscp [-r] dosdev:filename [dosdev:filename] [...] wegadirname\n");
	    fprintf (stderr, "       doscp [-r] wegafilename dosdev:filename \n");
	    fprintf (stderr, "       doscp [-r] wegafilename [wegafilename] [...] dosdev:filename \n");
	    exit (1);
	}
	if (dtm) {
	    if ((fd = creat (argv[arg], 0664)) < 0) {
		fprintf (stderr, "%s -%s nicht erstellbar \n", *arg0, argv[arg]);
		exit (1);
	    }
	    if ((res = dosplit (argv[--arg], mutos, dcp)) < 0)
		exit (1);
	    if (res == 2) {
		strcpy (tempo, "cp ");
		strcat (tempo, argv[arg]);
		strcat (tempo, " "),
		    strcat (tempo, argv[++arg]);
		res = system (tempo);
		exit (res);
	    }
	    if (*dcp == '\0') {
		fprintf (stderr, "%s -keine DCP-Datei \n", *arg0);
		exit (1);
	    }
	    if ((res = dosdev (mutos, 0, &disc)) < 0) {
		fprintf (stderr, "%s -Fehler im Laufwerk %s \n", *arg0, mutos);
		exit (1);
	    }

	    strcpy (h, dcp);
	    if ((res = dosopn (dcp, &disc, &dstat)) < 1)
		if (res == 0) {
		    fprintf (stderr, "%s -%s nicht gefunden \n", *arg0, h);
		    exit (1);
		}
		else {
		    fprintf (stderr, "%s -%s Lesefehler \n", *arg0, mutos);
		    exit (1);
		}
	    if ((res = dosout (&dstat, fd, &disc, conv)) < 1)
		switch (res) {
		    case -1: 
			fprintf (stderr, "%s -%s -Lesefehler \n", *arg0, mutos);
			exit (1);
		    case -2: 
			fprintf (stderr, "%s -Schreibfehler in MUTOS \n", *arg0);
			exit (1);
		}

	}
	else {			/* MUTOS -> DCP */
	    if ((res = dosplit (argv[arg], mutos, dcp)) < 0)
		exit (1);

	    if ((res = dosdev (mutos, 2, &disc)) < 0) {
		fprintf (stderr, "%s -Fehler im Laufwerk %s \n", *arg0, mutos);
		exit (1);
	    }
	    strcpy (h, dcp);
	    res = dosin (argv[--arg], dcp, &disc, conv);
	    if (res < 1)
		switch (res) {
		    case 0: 
			fprintf (stderr, "%s -Verzeichnis nicht gefunden \n", *arg0);
			exit (1);
		    case -1: 
			fprintf (stderr, "%s -Lesefehler in %s \n", *arg0, argv[arg]);
			exit (1);
		    case -2: 
			fprintf (stderr, "%s -Schreibfehler in %s \n", *arg0, mutos);
			exit (1);
		    case -3: 
			fprintf (stderr, "%s -%s ist Verzeichnis oder schreibgeschuetzt \n", *arg0, h);
			exit (1);
		}

	}

    }
    else {			/* Kopieren in ein Verzeichnis */
	argv++;
	strcpy (tempo, argv[--arg]);
	if (dtm) {
	    strcpy (t, tempo);	/* Zielverzeichnis */
	    strcat (t, "/");
	    while (strcmp (*argv, tempo)) {
		if ((res = dosplit (*argv, mutos, dcp)) < 0)
		    exit (1);
		if (*dcp == '\0') {
		    fprintf (stderr, "%s -kein DCP Pfad \n", *arg0);
		    status = 2;
		    argv++;
		    continue;
		}
		if ((res = dosdev (mutos, 0, &disc)) < 0) {
		    fprintf (stderr, "%s -Fehler im Laufwerk %s \n", *arg0, mutos);
		    status = 2;
		    argv++;
		    continue;
		}
		strcpy (h, dcp);
		if ((res = dosopn (dcp, &disc, &dstat)) < 1) {
		    if (res == 0) {
			fprintf (stderr, "%s -%s nicht gefunden \n", *arg0, h);
			status = 2;
			argv++;
			continue;
		    }
		    else {
			fprintf (stderr, "%s -%s -Lesefehler \n", *arg0, mutos);
			status = 2;
			argv++;
			continue;
		    }
		}

		ph = h;
		while ((p = strchr (ph, '/')) != 0) {
		    ph = ++p;
		}
		p = ph;
		while (*ph != '\0')
		    *ph++ = isupper (*ph) ? tolower (*ph) : *ph;
		strcat (t, p);	/* p ist letzter Teil des DCP-Pfades */
		if ((fdf = creat (t, 0664)) < 0) {
		    fprintf (stderr, "%s -%s nicht erzeugbar \n", *arg0, t);
		    status = 2;
		    argv++;
		    continue;
		}

		if ((res = dosout (&dstat, fdf, &disc, conv)) < 1)
		    switch (res) {
			case -1: 
			    fprintf (stderr, "%s -%s Lesefehler \n", *arg0, mutos);
			    status = 2;
			    argv++;
			    break;
			case -2: 
			    fprintf (stderr, "%s -% Schreibfehler \n", *arg0, t);
			    status = 2;
			    argv++;
			    break;
		    }
		strcpy (t, tempo);
		strcat (t, "/");
		argv++;
		close (fdf);
	    }			/* while */
	}
	else {			/* MUTOS -> DCP */

	    if ((res = dosplit (tempo, mutos, dcp)) < 0)
		exit (1);
	    strcpy (h, dcp);
	    if (*dcp == '\0')
		strcpy (t, dcp);
	    else {
		strcpy (t, h);
		strcat (t, "/");
	    }
	    if ((res = dosdev (mutos, 2, &disc)) < 0) {
		fprintf (stderr, "%s -Fehler im Laufwerk %s \n", *arg0, mutos);
		exit (1);
	    }
	    while (strcmp (*argv, tempo)) {
		strcpy (h, *argv);
		ph = *argv;
		while ((p = strchr (ph, '/')) != 0)
		    ph = ++p;
		p = ph;
		while (*ph != '\0')
		    *ph++ = islower (*ph) ? toupper (*ph) : *ph;
		strcat (t, p); 

		if ((res = dosin (h, t, &disc, conv)) < 1)
		    switch (res) {
			case 0: 
			    fprintf (stderr, "%s -Verzeichnis nicht gefunden \n", *arg0);
			    status = 2;
			    argv++;
			    break;
			case -1: 
			    fprintf (stderr, "%s -Lesefehler in %s \n", *arg0, h);
			    status = 2;
			    argv++;
			    break;
			case -2: 
			    fprintf (stderr, "%s -Schreibfehler in %s \n", *arg0, mutos);
			    status = 2;
			    argv++;
			    break;
			case -3: 
			    fprintf (stderr, "%s -%s ist Verzeichnis oder schreibgeschuetzt \n", *arg0, t);
			    status = 2;
			    argv++;
			    break;
		    }

                strcpy(h,dcp); /* dg */
		if (*h == '\0')
		    strcpy (t, h);
		else {
		    strcpy (t, h);
		    strcat (t, "/");
		}
		argv++;

	    }
	}

    }
    exit (status);
}
