/**************************************************************************
***************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW    Abt. Basissoftware - 1988
	Programm: format.c
 
	Bearbeiter	: P. Hoge
	Datum		: ??.??.??
	Version		: 1.5
 
***************************************************************************

	WEGA P8000 Floppy-Format
	Syntax: format [/dev/rfd...]

***************************************************************************
**************************************************************************/

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
	char *ttyname();

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
		fprintf(stderr,"%s: %s is not a floppy device\n",progname,fdname);
		exit(1);
	}
	/* wenn im Hintergrund keine Abfrage */
	if (strcmp(ttyname(0), "/dev/console") == 0) {
		printf("Format %s, ready ? ", fdname);
  		if ((getchar() & 0x5f) != 'Y')
			exit(1);
	}

	buf = 'F';
	write(dd, &buf, 1);	/* Start Request F: Format */
	buf = stbuf.st_rdev;	/* Minornummer (Typ + Drivenr.) */
	write(dd, &buf, 1);
	read(dd, &buf, 1);	/* warten auf ACK oder Fehlerkode */
	if (buf != ACK) {
		fprintf(stderr,"%s: \007ERROR %x - ", progname, buf&0x0ff);
		if (buf == 0x0C2)
			fprintf(stderr,"drive not ready\n");
		else if (buf == 0x0C3)
			fprintf(stderr,"disk is write protected\n");
		else
			fprintf(stderr,"disk verification error\n");
		exit(2);
	}
	putchar(7);	/* Bell */
	exit(0);
}
