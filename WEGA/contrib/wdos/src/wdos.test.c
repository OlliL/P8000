/**************************************************************************
***************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW    Abt. Basissoftware - 1988
	Programm: wdos.test.c
 
	Bearbeiter	: P. Hoge
	Datum		: 1.11.88
	Version		: 1.1
 
***************************************************************************

	Test-Programm fuer die MP600-Erweiterungskarten

***************************************************************************
**************************************************************************/


#include <termio.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

extern errno;

#define BUFLEN 5*512

int timeout;				/* Signal-Flag */
int parent, child1, child2;		/* Prozess-Id's */
int cpu2tty, cpu2lp, cpu2disk, cpu2ram;

char *tty, *ttyname();
char *progname;
struct termio targ;		/* definiert in termio.h */
struct termio savterm;		/* original tty-Flags */

char buffer[BUFLEN];

/*
 * Terminal Write-Protected setzen
 * tty-Parameter sichern und "Raw"-Mode setzen
 */
settty()
{
	tty = ttyname(1);		/* Terminal-Namen ermitteln */
	chmod(tty, 0600);
	ioctl(0, TCGETA, &savterm);
	ioctl(0, TCGETA, &targ);
 	targ.c_iflag = IXON|IXANY;
	targ.c_oflag = 0;
	targ.c_cflag &= ~PARENB;
	targ.c_cflag |= (CS8|CSTOPB|CLOCAL|CREAD);
	targ.c_lflag = 0;
	targ.c_cc[VMIN] = 1;
	targ.c_cc[VTIME] = 0;
	ioctl(0, TCSETA, &targ);
}

/*
 * Restore tty-Parameter
 */
restty()
{
	ioctl(0, TCSETA, &savterm);
	chmod(tty, 0622);
}

/*
 * Eingabe vom Terminal zu CPU2 senden
 */
term_to_cpu2()
{
	char c;

	while (1) {			/* Schleife endet nie */
		read(0, &c, 1);
		if ((c & 0x7f)==0x1c) {	/* ^Backslash beendet Programm */
			intrupt();
		} else
    			write(cpu2tty, &c, 1);
  	}
}

/*
 * Ausgabe von CPU2 zum Terminal senden
 */
cpu2_to_term()
{
	register n_read;

	while (1) {
		n_read = read(cpu2tty, buffer, BUFLEN);
		write(1, buffer, n_read);
	}
}

/*
 * Speicher-Lesen
 */
cpu2_ram()
{
	while (1) {
		lseek(cpu2ram, 0x1000L,0);
		read(cpu2ram, buffer, 512);
		write(cpu2ram, buffer, 512);
		lseek(cpu2ram, 0xf1000L,0);
		read(cpu2ram, buffer, 512);
		write(cpu2ram, buffer, 512);
	}
}


/*
 * Behandlung Alarm-Interrupt
 */
time_out()
{
	signal(SIGALRM, time_out);
	timeout++;
}

/*
 * benutzt von Vater und Soehnen
 */
intrupt(sig)
int sig;
{
	int status;

	if (child1 && child2) {			/* Vaterprozess */
		signal(SIGINT, SIG_IGN);	/* ignoriere weiterer SIGINT */ 
		kill(child1, SIGINT);		/* Soehne beenden */
		kill(child2, SIGINT);
		wait(&status);			/* auf Soehne */
		wait(&status);			/* auf Soehne */
		restty();
	} else					/* einer der Sohnprozesse */
		kill(parent, SIGINT);		/* Vater beendet dann alle */
						/* Soehne und sich selbst */
	exit(0);
}

main(argc, argv)
register argc;
register char *argv[];
{
	register char *filname = (char *)0;
	unsigned oflag;

	progname = argv[0];
	oflag = O_RDWR;
	switch (--argc) {		/* Kommandozeilen-Argumente auswerten */
	case 0:
		filname = "/z/wdoslib/bios.test";
		break;
	case 1:
	    if ((*++argv)[0] != '-') {	/* Optionen ? */
		filname = *argv;	/* Argument ist Dateiname */
		break;
	    } else {
		if ((*argv)[1] == 'r') {
			oflag = O_RDWR | O_NDELAY; /* O_NDELAY hier:      */
			break;			   /* kein Reset fuer CPU */
		}
	    }
	default:
		printf ("Usage: %s [-resume | proto_test]\n", progname);
		exit(1);
		return;
	}
	if ((cpu2ram = open("/dev/cpu2rram", oflag)) == -1) {
		printf ("%s: unable to open /dev/cpu2rram\n", progname);
		exit(1);
		return;
	}
	if (filname)
		load(filname);

	if ((cpu2tty = open("/dev/cpu2tty", O_RDWR)) == -1) {
		printf ("%s: unable to open /dev/cpu2tty\n", progname);
		exit(1);
		return;
	}
	/* folgende 2 Open sind fuer den Start des Prozessors
	 * erforderlich, werden aber im Test nicht benoetigt
	 */
	if ((cpu2lp = open("/dev/cpu2lp", O_RDONLY)) == -1) {
		printf ("%s: unable to open /dev/cpu2lp\n", progname);
		exit(1);
		return;
	}
	if ((cpu2disk = open("/dev/cpu2disk", O_RDWR)) == -1) {
		printf ("%s: unable to open /dev/cpu2disk\n", progname);
		exit(1);
		return;
	}
	settty();
	signal(SIGALRM, time_out);
	signal(SIGHUP, intrupt);
	signal(SIGINT, intrupt);
	signal(SIGQUIT, intrupt);
	signal(SIGTERM, intrupt);

	parent = getpid();		/* Prozess-Id dieses Prozesses */
	if ((child1 = fork()) == 0)	/* Sohnprozess 1 */
		cpu2_to_term();
	if ((child2 = fork()) == 0)	/* Sohnprozess 2 */
		cpu2_ram();
	term_to_cpu2();			/* Vater */
}

/*
 * WDOS-Test (Datei bios.test)
 * in den RAM der Erweiterungskarten laden
 */
load(filname)
register char *filname;
{
	register int fdr;
	register int n_read;
	struct stat stbuf;

	if ((fdr = open(filname, O_RDONLY)) == -1) {
	    printf ("%s: unable to open proto_file %s\n", progname, filname);
	    exit(1);
	    return;
	}
	fstat(fdr, &stbuf);
	if ((stbuf.st_mode & S_IFDIR)||	/* ist es ein Directory ? */
	    stbuf.st_size == 0L ||	/* Test der Laenge */
	    stbuf.st_size > 0x10000L) {
		printf("%s: invalid proto_file %s\n", progname, filname);
		exit(1);
		return;
	}
	/* Datei lesen und zu CPU2-RAM schreiben */
	lseek(cpu2ram, 0x100000L-stbuf.st_size, 0); /* RAM-Adresse einstellen */
	while ((n_read = read(fdr, buffer, BUFLEN)) > 0)
		write(cpu2ram, buffer, n_read);
	close(fdr);		/* Close Files */
}
