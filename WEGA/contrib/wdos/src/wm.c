/**************************************************************************
***************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW    Abt. Basissoftware - 1989
	Programm: wm.c
 
	Bearbeiter	: P. Hoge
	Datum		: 11.12.89
	Version		: 1.2
 
***************************************************************************

	WDOS - Menue-System

***************************************************************************
**************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include <curses.h>
#include <screen.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>

extern errno;
extern char *getenv();
extern char *mktemp();
extern char *strchr();

char *home;
char *disks;
char *lp;

WINDOW *win1, *win2;

char buf[80];
char buf1[80];
char buf2[80];
char buf3[80];

char mbuf[6][50];

char idos[] = {"A=\nB=\nC=\nD=\nE=\nF=\n"};

main()
{
	register i;
	register fd;
	register FILE *fp;

	home = getenv("HOME");
	disks = getenv("WDOS_DISKS");
	lp = getenv("WDOS_LP");
	strcpy(buf, home);
	strcat(buf, "/.dos");
	if ((fd = open(buf, O_WRONLY|O_CREAT|O_EXCL, 0666)) != -1)
		write(fd, idos, sizeof(idos)); /* $HOME/.dos anlegen */
	close(fd);
	fp = fopen(buf, "r");
	for (i = 0; i < 6; i++)
		fgets(&mbuf[i][0], 50, fp);
	fclose(fp);
	init();

	printw("WDOS-Menue Vers. 1.2");
	refresh();
	menu1();
	while (1) {
		input("Kommando eingeben:\n");
		switch(buf[0]) {
		case 0x0d:
		case 0x0a:
			mountout();
			continue;
		case 0x1a:
			wrefresh(curscr);
			continue;
		case 'h':
		case '?':
			helping();
			continue;
		case '1':
			menu1();
			continue;
		case '2':
			menu2();
			continue;
		case 's':
			exec3("wdos", 0);
			continue;
		case 'f':
			format();
			continue;
		case 'n':
			new();
			continue;
		case 'u':
			unmount();
			continue;
		case 'x':
			rm();
			continue;
		case 'q':
			done();
		case '!':	/* WEGA Kommando ausfuehren */
			exec3(&buf1[1], 1);
			continue;
		}
		if (strncmp(buf1, "dir", 3) == 0) {
			exec1("dosdir ");
			continue;
		}
		if (strncmp(buf1, "ls", 2) == 0) {
			exec1("dosls ");
			continue;
		}
		if (strcmp(buf1, "cp") == 0) {
			exec2("doscp ");
			continue;
		}
		if (strncmp(buf1, "cpb", 3) == 0) {
			exec2("doscp -r ");
			continue;
		}
		if (strcmp(buf1, "r") == 0) {
			exec3("wdos -r", 0);
			continue;
		}
		if (strcmp(buf1, "rm") == 0) {
			exec2("dosrm ");
			continue;
		}
		if (strncmp(buf1, "mkdir", 5) == 0) {
			exec2("dosmkdir ");
			continue;
		}
		if (strncmp(buf1, "rmdir", 5) == 0) {
			exec2("dosrmdir ");
			continue;
		}
		if (strncmp(buf1, "cat", 3) == 0) {
			exec3("doscat", 1);
			continue;
		}
		if (strncmp(buf1, "ed", 2) == 0) {
			edit();
			continue;
		}
		if (strncmp(buf1, "mf", 2) == 0) {
			mounta("/dev");
			continue;
		}
		if (strncmp(buf1, "mg", 2) == 0) {
			mounta("/z/wdoslib/disks");
			continue;
		}
		if (strncmp(buf1, "ml", 2) == 0) {
			mounta(disks);
			continue;
		}
		leaveok(stdscr, TRUE);
		mesg("\007Ungueltiges Kommando\n", NULL);
		sleep(1);
	}
}

/*
 * Initialisierung Menue
 */
init()
{
	int done();
	int i;

	signal(SIGINT, done);
	initscr();
	SO = (char *)0;	/* Standout-Mode funktioniert nicht korrekt */
	SE = (char *)0;
	US = (char *)0;
	UE = (char *)0;
	goraw();
	win1 = subwin(stdscr,4,80,2,0);
	win2 = subwin(stdscr,stdscr->_maxy-7,80,7,0);
}

/*
 * Menue verlassen
 */
done()
{
	signal(SIGINT, done);
	highlight(FALSE);
	clear();
	refresh();
	gonormal();
	endwin();
	exit(0);
}

/*
 * Eingabe von der Tastatur
 */
input(s)
char *s;
{
	register char *b = buf;

	leaveok(stdscr, TRUE);
	mesg(s, NULL);
	leaveok(stdscr, FALSE);
	buf1[0] = 0;
	buf2[0] = 0;
	while (1) {
		*b = getch();
		if (*b == 0x08) {
			if (b > buf) {
				b--;
				addch(0x08);
				addch(0x20);
				addch(0x08);
				refresh();
			}
			continue;
		}
		if (*b < 0x20) {
			*(++b) = 0;
			break;
		}
		addch(*b++);
		refresh();
	}
	sscanf(buf,"%s %[^\n]", buf1, buf2);
}

/*
 * Menue 1 ausgeben
 */
menu1()
{
 wmove(win1,0,0);
 waddstr(win1,"s Start von WDOS    | mf mount reale Disk   | 2 Menue 2\n");
 waddstr(win1,"r Rueckkehr zu WDOS | mg mount globale Disk | n neue lokale Disk\n");
 waddstr(win1,"h Hilfe             | ml mount lokale Disk  | f Format Floppy\n");
 waddstr(win1,"q Verlassen Menue   | u  umount Disk        | x Disk loeschen\n");
 wrefresh(win1);
 mountout();
}

/*
 * Menue 2 ausgeben
 */
menu2()
{
 wmove(win1,0,0);
 waddstr(win1,"dir Directory | cp  Kopieren (text) | 1     Menue 1\n");
 waddstr(win1,"ls  Directory | cpb Kopieren (bin)  | mkdir Subdirectory erstellen\n");
 waddstr(win1,"cat Anzeigen  |                     | rmdir Subdirectory loeschen\n");
 waddstr(win1,"ed  Editieren | !   WEGA-Kommando   | rm    Datei loeschen\n");
 wrefresh(win1);
}

/*
 * gemountete WDOS-Disks ausgeben
 */
mountout()
{
	register i;

	wmove(win2,0,0);
	waddstr(win2,"Physische WDOS-Disks:\n\n");
	for (i = 0; i < 6; i++)
		waddstr(win2, &mbuf[i][0]);
	if (lp) {
		waddstr(win2, "\nDrucker: ");
		waddstr(win2, lp);
	}
	wclrtobot(win2);
	wrefresh(win2);
}

/*
 * Help-Datei ausgeben
 */
helping()
{
	register FILE *fp;

	wsavescrn(win2);
	fp = fopen("/z/wdoslib/help", "r");
	wpage(fp);
	fclose(fp);
	input("CR eingeben:");
	wresscrn(win2);
}

/*
 * externe Kommandos starten
 */
exec1(s)
register char *s;
{
	register FILE *fin;

	strcpy(buf, s);
	strcat(buf, buf2);
	wclear(win2);
	wrefresh(win2);
	wmesg(win2, buf, NULL);
	fin = popen(buf, "r");
	wpage(fin);
	if (pclose(fin)) {
		input("CR eingeben:");
		clearok(curscr, TRUE);
	}
	wmesg(win2, "\n", NULL);
}

/*
 * externe Kommandos starten
 */
exec2(s)
register char *s;
{
	wsavescrn(win2);
	exec21(s);
	wresscrn(win2);
}

exec21(s, flag)
register char *s;
{
	register FILE *fin;

	strcpy(buf, s);
	strcat(buf, buf2);
	wclear(win2);
	wrefresh(win2);
	wmesg(win2, buf, NULL);
	fin = popen(buf, "r");
	wpage(fin);
	if (pclose(fin)) {
		input("CR eingeben:");
		clearok(curscr, TRUE);
	}
}

/*
 * externe Kommandos starten
 */
exec3(s, flag)
register char *s;
register flag;
{
	register r;

	strcpy(buf, s);
	strcat(buf, " ");
	strcat(buf, buf2);
	savescrn();
	clear();
	refresh();
	mesg(buf, NULL);
	gonormal();
	r = csystem(buf);
	goraw();
	if (flag || r)
		input("CR eingeben:");
	clearok(curscr, TRUE);
	resscrn();
}

edit()
{
	register char *s;
	register char *p;

	savescrn();
	clear();
	refresh();
	buf1[0] = 0;
	sscanf(buf2, "%s", buf1);
	p = mktemp("/tmp/wdosed.XXXXXX");
	strcpy(buf, "doscp ");
	strcat(buf, buf1);
	strcat(buf, " ");
	strcat(buf,p);
	if (system(buf) == 0) {
		if ((s = getenv("wdosed")) == NULL)
			s = "vi";
		gonormal();
		strcpy(buf, s);
		strcat(buf, " ");
		strcat(buf, p);
		csystem(buf);
		goraw();
		strcpy(buf, "doscp ");
		strcat(buf, p);
		strcat(buf, " ");
		strcat(buf, buf1);
		if (system(buf) == 0)
			unlink(p);
	}
	/* input("CR eingeben:"); */
	sleep(1);
	clearok(curscr, TRUE);
	resscrn();
}

/*
 * Mount real, global oder local floppy
 */
mounta(p)
register char *p;
{
	register i;

	if (p == 0) {
		merror1();
		return;
	}
	if (buf2[0] == 0) {
		mout(p); /* nur Disks ausgeben */
		return;
	}
	if ((i = mtest()) == -1)
		return;
	buf1[0] = 0;
	sscanf(buf2, "%*[^= ]%s%s", buf1, buf1);
	if (buf1[0] == '=')
		strcpy(buf2, &buf1[1]);
	else
		strcpy(buf2, buf1);
	if (mname(p)) {
		strcpy(&mbuf[i][2], buf2);
		strcat(mbuf[i], "\n");
	}
	mwrite();
	mountout();
}

/*
 * Unmount Disk
 */
unmount()
{
	register i;

	if ((i = mtest()) == -1)
		return;
	strcpy(&mbuf[i][2], "\n");
	mountout();
	mwrite();
}

/*
 * Mount Tabelle zu $HOME/.dos schreiben
 */
mwrite()
{
	register FILE *fp;
	register i;

	strcpy(buf, home);
	strcat(buf, "/.dos");

	fp = fopen(buf, "w+");
	for (i = 0; i < 6; i++)
		fputs(&mbuf[i][0], fp);
	fclose(fp);
}

/*
 * Test Laufwerk
 */
mtest()
{
	register i;

	i = (buf2[0] | 0x20) - 'a';
	if (i < 0 || i > 5) {
		merror("Ungueltiges Laufwerk\n");
		return(-1);
	}
	return(i);
}

/*
 * Fehler ausgeben
 */
merror(s)
register char *s;
{
	wmove(win2,0,0);
	waddstr(win2, s);
	wclrtobot(win2);
	wrefresh(win2);
	sleep(2);
	mountout();
}

merror1()
{
	merror("Kein lokales Disk-Directory vorhanden\n");
}

/*
 * Disks ausgeben
 */
mout(p)
register char *p;
{
	register char *m;
	register FILE *fin;

	wclear(win2);
	wrefresh(win2);
	m = "s *";
	if (strncmp(p, "/dev", 4) == 0)
		m = " wd* fd*ds*";
	strcpy(buf, "cd ");
	strcat(buf, p);
	strcat(buf, ";/bin/ls -Cx");
	strcat(buf, m);
	fin = popen(buf, "r");
	wpage(fin);
	pclose(fin);
}

/*
 * Floppy-Namen eingeben
 */
mname(p)
register char *p;
{
	register i;

	if (buf2[0] == 0) {
		mout(p);
		input("Disk-Namen eingeben:\n");
		strcpy(buf2, buf1);
		if (buf2[0] == 0)
			return(0);
	}
	if (buf2[1] == 0 || buf2[1] == ':') {
		if ((i = mtest()) == -1) {
			buf2[0] = 0;
			return(0);
		}
		if (mbuf[i][2] == '\n') {
			buf2[0] = 0;
			merror("Laufwerk nicht definiert\n");
			return(0);
		}
		strcpy(buf2, &mbuf[i][2]);
		buf2[strlen(buf2)-1] = 0;
	}
	if (buf2[0] != '/')	{ /* kein vollst. Pfadname */
		strcpy(buf1, buf2);
		strcpy(buf2, p);
		strcat(buf2, "/");
		strcat(buf2, buf1);
	}
	return(1);
}

/*
 * Pseudo-Floppy loeschen
 */
rm()
{
	register i;
	FILE *fin;

	if (disks == 0) {
		merror1();
		return;
	}
	if (mname(disks) == 0)
		return;
	if (strncmp(disks, buf2, strlen(disks)) != 0) {
		merror("Keine lokale Disk\n");
		return;
	}
	strcpy(buf3, buf2);
	input("\007Disk wird vollstaendig geloescht, ok ? (j/n): \n");
	if (buf[0] != 'j')
		return;
	if (unlink(buf3)) {
		merror("Fehler beim Loeschen\n");
		return;
	}
	for (i = 0; i < 6; i++) { /* Eintrag in Mount Tab suchen */
		if (strncmp(buf3, &mbuf[i][2], strlen(buf3)) == 0) {
			strcpy(&mbuf[i][2], "\n");
			mwrite();
		}
	}
	mout(disks);
}


/*
 * Pseudo-Floppy anlegen
 */
new()
{
	if (disks == 0) {
		merror1();
		return;
	}
	if (mname(disks) == 0)
		return;
	if (strncmp(disks, buf2, strlen(disks)) != 0) {
		merror("Keine lokale Disk\n");
		return;
	}
	strcpy(buf3, buf2);
	input("\007Pseudoiskette wird angelegt/formatiert, ok ? (j/n): \n");
	if (buf[0] != 'j')
		return;
	strcpy(buf2, buf3);
	exec21("dosformat ");
	mout(disks); /* lokale Disks ausgeben */
}

/*
 * reale Floppy formatieren
 */
format()
{
	if (buf2[0] == 0)
		strcpy(buf2, "/dev/rfd1");
	if (mname("/dev") == 0)
		return;
	if (strncmp(buf2, "/dev", 4) != 0) {
		merror("Keine reale Disk\n");
		return;
	}
	exec3("dosformat "); /* reale Floppy */
}

/*
 * Page in win2 ausgeben
 */
static char str5[]="++++++++  naechste Seite mit CR  |  Abbruch mit 'q'  +++++++++++";
wpage(fp)
register FILE *fp;
{
	register i;
	while (1) {
		wclear(win2);
		i = win2->_maxy-1;
		while (i--) {
			if (fgets(buf, 80, fp) == NULL) {
				wrefresh(win2);
				return;
			}
			waddstr(win2, buf);
		}
		wrefresh(win2);
		input("Naechste Seite mit ctrl-F");
		if (buf[0] != 0x06)
			return;
	}
}
