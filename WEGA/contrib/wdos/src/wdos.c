/**************************************************************************
***************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW    Abt. Basissoftware - 1988
	Programm: wdos.c
 
	Bearbeiter	: P. Hoge
	Datum		: 24.9.89
	Version		: 1.3
 
***************************************************************************

	Koppelprogramm zu MP600-Erweiterungskarten

***************************************************************************
**************************************************************************/

#include <stdio.h>
#include <termio.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

extern errno;
extern char *getenv();
extern char *malloc();

#define BUFLEN 512
#define DISKBUF 40*512

int timeout;				/* Signal-Flag */
int parent, child1, child2, child3;	/* Prozess-Id's */
int cpu2tty, cpu2lp, cpu2disk, cpu2ram;
int fddisk[6];
struct diskrq {
	char		rcode;
	char		drive;
	long		blkoff;
	long		addr;
	unsigned	len;
} diskrq;

char *tty, *ttyname();
char *term;
char *lp;
unsigned termtyp = 0;
char *pname;
struct termio targ;		/* definiert in termio.h */
struct termio savterm;		/* original tty-Flags */
int  lpopenflg;
struct stat stbuf;
char mname[7][50];

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
 	targ.c_iflag = IGNBRK|IGNPAR;
	if (termtyp != 2)
 		targ.c_iflag |= IXON;
	targ.c_oflag = 0;
	targ.c_cflag &= ~PARENB;
	targ.c_cflag |= (CS8|CSTOPB|CLOCAL|CREAD);
	targ.c_lflag = 0;
	targ.c_cc[VMIN] = 1;
	targ.c_cc[VTIME] = 0;
	ioctl(0, TCSETA, &targ);
	if (termtyp == 1) {		/* P8000/WDOS-Terminal */
		putchar(0x1b);
		putchar('m');
		sleep(3);
	}
	if (termtyp == 2) {		/* Wyse WY-99GT */
		putchar(0x1b);
		putchar('~');
		putchar('5');
	}
}

/*
 * Restore tty-Parameter
 */
restty()
{
	if (termtyp == 1) {
		putchar(0x1b);
		putchar('1');
		sleep(2);
	}
	if (termtyp == 2) {
		printf("%s","\033G0\033.1\033v");
		putchar('+');
	}
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
		if ((termtyp && c == 0x58) ||	/* F12 beendet Programm */
		   (termtyp == 0 && c == 0x1c))	/* ^\  beendet Programm */
			intrupt();
		else
    			write(cpu2tty, &c, 1);
  	}
}

/*
 * CPU2-Terminalasgabe zum Terminal senden
 */
cpu2_to_term()
{
	register n_read;
	register char *buf;

	buf = malloc(0x800);
	while (1) {
		n_read = read(cpu2tty, buf, 0x800);
		write(1, buf, n_read);
	}
}

/*
 * CPU2-Druckerausgabe zum Drucker senden
 */
cpu2_to_lp()
{
	register n_read;
	register FILE *fp;

	n_read = read(cpu2lp, buffer, BUFLEN);
	if (lp) {
		if (fp = fopen(lp, "r+")) {
			if (strncmp(lp, "/dev", 4) == 0)
				setbuf(fp, (char *)0);
			goto lpwrite;
		}
		printf("\r\n%s: kann %s nicht oeffnen\r\n", pname, lp);
		printf("Druckerausgaben erfolgen zur Datei /tmp/wdoslp\r\n");
	}
	fp = fopen("/tmp/wdoslp", "w+");

lpwrite:
	while (1) {
		fwrite(buffer, sizeof(*buffer), n_read, fp);
		n_read = read(cpu2lp, buffer, BUFLEN);
	}
}

/*
 * Disk-Arbeit realisieren
 */
cpu2_disk()
{
	register struct diskrq *prq = &diskrq;
	register d;
	register unsigned len;
	register n_read;
	char comcode;
	char *buf;

	buf = malloc(DISKBUF);
	if (buf == 0) {
		printf("Malloc\n");
		sleep(4);
	}
	while (1) {
		read(cpu2disk, &diskrq, sizeof(struct diskrq));

		comcode = 0;
		if (((d = prq->drive) > 5) || prq->len == 0) {
			comcode = 1;
			goto diskend;
		}
		if (fddisk[d] == 0) {
		    if (mname[d][0]==0 || (fddisk[d]=open(mname[d],O_RDWR))==-1) {
			fddisk[d] = 0;
			comcode = 0x80;
			goto diskend;
		    }
		}
		lseek(fddisk[d], prq->blkoff, 0);
		lseek(cpu2ram, prq->addr, 0);
		while (prq->len != 0) {
			len = prq->len;
			if (len > DISKBUF)
				len = DISKBUF;
			prq->len -= len;
			switch (prq->rcode) {
			case 1:				/* Read */
				n_read = read(fddisk[d], buf, len);
				if (n_read == -1) {
					comcode = 0x10; /* Fehler */
					goto diskend;
			     	}
				while (len > n_read) {
					buf[n_read] = 0;
					n_read++;
				}
				if (write(cpu2ram, buf, n_read) != n_read) {
					comcode = 0x08;
					goto diskend;
			     	}
				break;

			case 2:				/* Write */
				n_read = read(cpu2ram, buf, len);
				if (n_read != len) {
					comcode = 0x08;
					goto diskend;
				}
				if (write(fddisk[d], buf, n_read) != n_read) {
					comcode = 0x10;
					goto diskend;
				}
				break;

			default:
				comcode = 1;
				goto diskend;
			}
		}
diskend:
		write(cpu2disk, &comcode, 1);
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

	if (child1 && child2 && child3) {	/* Vaterprozess */
		signal(SIGINT, SIG_IGN);	/* ignoriere weiterer SIGINT */ 
		kill(child1, SIGINT);		/* Soehne beenden */
		kill(child2, SIGINT);
		kill(child3, SIGINT);
		/*
		wait(&status);
		wait(&status);
		wait(&status);
		*/
		restty();
		print();
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

	pname = argv[0];
	oflag = O_RDWR;
	term = getenv("TERM");
	lp = getenv("WDOS_LP");
	if (strncmp(term,"w8",2) == 0 || strncmp(term,"W8",2) == 0)
		termtyp = 1;
	else if (strncmp(term,"WY",2) == 0)
		termtyp = 2;
	else if (strncmp(term,"p8",2) != 0 && strncmp(term,"P8",2) != 0) {
		printf("%s: Falscher Terminaltyp\n", pname);
		exit(1);
	}
	switch (--argc) {		/* Kommandozeilen-Argumente auswerten */
	case 0:
		filname = "/z/wdoslib/bios.p8";
		if (termtyp == 1)
			filname = "/z/wdoslib/bios";
		if (termtyp == 2)
			filname = "/z/wdoslib/bios.wy";
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
		printf("Usage: %s [-resume | proto_file]\n", pname);
		exit(1);
	}
	if ((cpu2ram = open("/dev/cpu2rram", oflag)) == -1) {
		printf("%s: kann /dev/cpu2rram nicht oeffnen\n",pname);
		exit(1);
	}
	if (filname)
		load(filname);
	else
		setreturn();

	disknames();
	setdate();

	if ((cpu2tty = open("/dev/cpu2tty", O_RDWR)) == -1) {
		printf("%s: kann /dev/cpu2tty nicht oeffnen\n", pname);
		exit(1);
	}
	if ((cpu2lp = open("/dev/cpu2lp", O_RDONLY)) == -1) {
		printf("%s: kann /dev/cpu2lp nicht oeffnen\n", pname);
		exit(1);
	}
	if ((cpu2disk = open("/dev/cpu2disk", O_RDWR)) == -1) {
		printf("%s: kann /dev/cpu2disk nicht oeffnen\n",pname);
		exit(1);
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
		cpu2_to_lp();
	if ((child3 = fork()) == 0)	/* Sohnprozess 3 */
		cpu2_disk();
	term_to_cpu2();			/* Vater */
}

/*
 * WDOS-BIOS (Datei proto.file)
 * in den RAM der Erweiterungskarten laden
 */
load(filname)
register char *filname;
{
	register int fdr;
	register int n_read;

	if ((fdr = open(filname, O_RDONLY)) == -1) {
	    printf("%s: kann %s nicht oeffnen\n", pname, filname);
	    exit(1);
	}
	fstat(fdr, &stbuf);
	if ((stbuf.st_mode & S_IFDIR)||	/* ist es ein Directory ? */
	    stbuf.st_size == 0L ||	/* Test der Laenge */
	    stbuf.st_size > 0x10000L) {
		printf("%s: Fehler in %s\n", pname, filname);
		exit(1);
	}
	/* Datei lesen und zu CPU2-RAM schreiben */
	lseek(cpu2ram, 0x100000L-stbuf.st_size, 0); /* RAM-Adresse einstellen */
	while ((n_read = read(fdr, buffer, BUFLEN)) > 0)
		write(cpu2ram, buffer, n_read);
	close(fdr);		/* Close Files */
}
/*
 * Return zu WDOS an CPU2 uebermitteln
 */
setreturn()
{
	lseek(cpu2ram, 0xfd000L, 0); /* Adresse reset */
	buffer[0] = 1;
	write(cpu2ram, buffer, 1);
}

/*
 * Datum fuer CPU2 setzen
 */
setdate()
{
	register struct tm *tp;
	long clock;
	struct tm *localtime();

	lseek(cpu2ram, 0xfd026L, 0); /* Adresse struct dos_date */
	time(&clock);
	tp = localtime(&clock);
	buffer[0] = tp->tm_sec;
	buffer[1] = tp->tm_min;
	buffer[2] = tp->tm_hour;
	buffer[3] = tp->tm_mday;
	buffer[4] = tp->tm_mon;
	buffer[5] = tp->tm_year;
	write(cpu2ram, buffer, 6);
}

/*
 * Ermitteln der gemounteten Disketten aus der Datei $HOME/.dos
 */
disknames()
{
	register FILE *fp;
	register i, m;
	register hd0 = 0;
	register hd1 = 0;
	char *p;
	char *strrchr();

	strcpy(buffer, getenv("HOME"));
	strcat(buffer, "/.dos");
	if ((fp = fopen(buffer, "r")) == NULL) {
		printf("%s: kann %s nicht oeffnen\n", pname,buffer);
		exit(1);
	}
	for (i = 0; i < 6; i++) {
		if (fgets(&mname[6][0], 50, fp) == NULL ||
		   (i < 6 && mname[6][0] != i+'A')) {
			printf("%s: Fehler in %s\n", pname, buffer);
			exit(1);
		}
		mname[6][strlen(mname[6])-1] = 0;
		if (mname[6][2] == 0)
			continue;
		strcpy(mname[i], &mname[6][2]);
		if (strncmp(&mname[6][2], "/dev/", 5) == 0) {
			mname[i][5] = 'r'; /* raw-Device benutzen */
			strcpy(&mname[i][6], &mname[6][7]);
		}
		m = 3;
		if ((p = strrchr(mname[i], '_'))) {
			if (strncmp(p, "_5", 2) == 0)
				m = 0;
			if (strncmp(p, "_10", 3) == 0)
				m = 1;
			if (strncmp(p, "_20", 3) == 0)
				m = 2;
		}
		if (i == 2 || i == 3) {	/* Harddisk Laufwerke */
		    if (m == 3) {
		    	printf("%s: Mount-Fehler Harddisk-Laufwerk %c\n", pname,i+'A');
			exit(1);
		    }
		    if (i == 2)
			hd0 = m;
		    else
			hd1 = m;
		} else {
		    if (m != 3) {
		    	printf("%s: Mount-Fehler Floppy-Laufwerk %c\n", pname,i+'A');
			exit(1);
		    }
		}
	}
	fclose(fp);

	lseek(cpu2ram, 0xfd024L, 0); /* Adresse hd0/hd1type */
	buffer[0] = hd0;
	buffer[1] = hd1;
	write(cpu2ram, buffer, 2);
}

/*
 * Ausgabe der Datei /tmp/wdoslp zum Drucker
 */
print()
{
	if (stat("/tmp/wdoslp", &stbuf) == -1)
		return; /* keine Daten vorhanden */
	printf("\nEs wurden %D Byte zum Drucken angesammelt, ausdrucken ? (j/n): ", stbuf.st_size);

	if (getchar() != 'j') {
		unlink("/tmp/wdoslp");
		return;
	}
 	system("/z/wdoslib/run_lpr /tmp/wdoslp");
}
