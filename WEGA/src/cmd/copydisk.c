/**************************************************************************
***************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW    Abt. Basissoftware - 1988
	Programm: copydisk.c
 
	Bearbeiter	: P. Hoge
	Datum		: ??.??.??
	Version		: 1.5
 
***************************************************************************

	WEGA P8000 Floppy-COPYDISK
	Syntax: copydisk /dev/rfd... /dev/rfd...

***************************************************************************
**************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FLOPPY 1	/* Major-Nummer des Floppy-Treibers */
#define ACK 0x06

main(argc, argv)
int argc;
register char **argv;
{
	register char *progname = argv[0];
	register dd;
	char *sfdname = "/dev/rfd0";
	char *dfdname = "/dev/rfd1";
	char buf, sbuf, dbuf;
	struct stat stbuf;
	char *ttyname();

	if (argc == 2 || argc > 3) {
		fprintf(stderr,"Usage: %s [/dev/rfd... /dev/rfd...] (source to destination)\n", progname);
		exit(1);
	}
	if (argc == 3) {
		sfdname = *++argv;
		dfdname = *++argv;
	}
	if ((dd = open("/dev/ud", 2)) == -1) {
		fprintf(stderr,"%s: unable to work with floppy\n", progname);
		exit(1);
	}
	if (stat(sfdname, &stbuf) == -1) {
		fprintf(stderr,"%s: unable to open %s\n", progname, sfdname);
		exit(1);
	}
	if ((stbuf.st_mode & (S_IFCHR | S_IFBLK)) == 0 || /* special file ? */
	    (stbuf.st_rdev >> 8) != FLOPPY) {	/* floppy ? */
		fprintf(stderr,"%s: %s is not a floppy device\n", progname, sfdname);
		exit(1);
	}
	sbuf = stbuf.st_rdev;	/* Minornummer Source (Typ + Drivenr.) */
	if (stat(dfdname, &stbuf) == -1) {
		fprintf(stderr,"%s: unable to open %s\n", progname, dfdname);
		exit(1);
	}
	if ((stbuf.st_mode & (S_IFCHR | S_IFBLK)) == 0 || /* special file ? */
	    (stbuf.st_rdev >> 8) != FLOPPY) {	/* floppy ? */
		fprintf(stderr,"%s: %s is not a floppy device\n", progname, dfdname);
		exit(1);
	}
	dbuf = stbuf.st_rdev;	/* Minornummer Source (Typ + Drivenr.) */
	if ((sbuf & 0x0f) == (dbuf & 0x0f)) {
		fprintf(stderr,"%s: source and destination is the same drive\n", progname);
		exit(1);
	}
	if ((sbuf & 0xf0) != (dbuf & 0xf0)) {
		fprintf(stderr,"%s: source and destination have not the same floppy format\n", progname);
		exit(1);
	}
	/* wenn im Hintergrund keine Abfrage */
	if (strcmp(ttyname(0), "/dev/console") == 0) {
		printf("Copydisk %s to %s, ready? ",sfdname, dfdname);
  		if ((getchar() & 0x5f) != 'Y')
			exit(1);
	}

	buf = 'C';
	write(dd, &buf, 1);	/* Start Request C: COPYDISK */
	write(dd, &sbuf, 1);
	write(dd, &dbuf, 1);
	read(dd, &buf, 1);	/* warten auf ACK oder Fehlerkode */
	if (buf != ACK) {
		fprintf(stderr,"%s: \007ERROR %x - ", progname, buf&0x0df);
		if (buf == 0xe2)
			fprintf(stderr,"source drive not ready\n");
		else if (buf & 0x20)
			fprintf(stderr,"source disk read error\n");
		else if (buf == 0x0C2)
			fprintf(stderr,"destination drive not ready\n");
		else if (buf == 0x0C3)
			fprintf(stderr,"destination disk is write protected\n");
		else
			fprintf(stderr,"destination disk write or verification error\n");
		exit(2);
	}
	putchar(7);	/* Bell */
	exit(0);
}
