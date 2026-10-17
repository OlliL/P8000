/**************************************************************************
***************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW    Abt. Basissoftware - 1988
	Programm: verify.c
 
	Bearbeiter	: P. Hoge
	Datum		: 30.11.89
	Version		: 1.6
 
***************************************************************************

	WEGA P8000 Floppy-Verify
	Syntax: verify /dev/rfd...

***************************************************************************
**************************************************************************/

#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FLOPPY 1	/* Major-Nummer des Floppy-Treibers */
#define ACK 0x06

main(argc, argv)
int argc;
char **argv;
{
	register char *progname = argv[0];
	register char *fdname = "/dev/rfd1";
	register dd;
	char buf;
	struct stat stbuf;

	if (argc > 2) {
		fprintf(stderr,"Usage: %s [/dev/rfd...]\n", progname);
		exit(1);
	}
	if (argc == 2)
		fdname = *++argv;
	if ((dd = open("/dev/ud", 2)) == -1) {
		fprintf(stderr,"%s: unable to work with floppy\n", progname);
		exit(1);
	}
	if (stat(fdname, &stbuf) == -1) {
		fprintf(stderr,"%s: unable to open %s\n", progname, fdname);
		exit(1);
	}
	if ((stbuf.st_mode & (S_IFCHR | S_IFBLK)) == 0 || /* special file ? */
	    (stbuf.st_rdev >> 8) != FLOPPY) {	/* floppy ? */
		fprintf("stderr,%s: %s is not a floppy device\n", progname, fdname);
		exit(1);
	}
	/* wenn im Hintergrund keine Ausgabe */
	if (strcmp(ttyname(0), "/dev/console") == 0) {
		printf("Verify %s, ready ? ", fdname);
  		if ((getchar() & 0x5f) != 'Y')
			exit(1);
	}

	/* Programm darf nicht abgebrochen werden */
	signal(SIGALRM, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	buf = 'V';
	write(dd, &buf, 1);	/* Start Request V: Verify */
	buf = stbuf.st_rdev;	/* Minornummer (Typ + Drivenr.) */
	write(dd, &buf, 1);
	read(dd, &buf, 1);	/* warten auf ACK oder Fehlerkode */
	if (buf != ACK) {
		fprintf(stderr,"%s: \007ERROR %x - ", progname, buf&0x0ff);
		if (buf == 0x0C2)
			fprintf(stderr,"drive not ready\n");
		else
			fprintf(stderr,"disk verification error\n");
		exit(2);
	}
	putchar(7);	/* Bell */
	exit(0);
}
