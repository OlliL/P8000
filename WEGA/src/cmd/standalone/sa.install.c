/******************************************************************************
*******************************************************************************

	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1986
	KOMMANDO  install.c

	Bearbeiter:	N. Taege
	Datum:		6.1.88
	Version:	3.0

*******************************************************************************
******************************************************************************/

/* notwendig: -rw-r--r-- 1 bin system 2188 Mar  5 13:53 /lib/libb.a      */

#include "sys/param.h"
#include "sys/ino.h"
#include "sys/inode.h"
#include "sys/filsys.h"
#include "sys/fblk.h"
#include "sys/dir.h"


/*--------------------------------------------- Konstanten ------------------*/


#define LCONT	 15		/* max. Laenge der CONTENTS */


/*---------------------------------------------  Variable  ------------------*/


int	fsi, fdi, fdo;		/* fopen-Werte von Source und Destination */

char	*contp, CONT[LCONT*BSIZE];    /* CONTENTS - Puffer */
char	CON[] = "CONTENTS";
char	conline[50];		      /* CONTENTS-Zeile */
char	fddname[40] = "ud(0,0)";      /* floppy-disk-driver- & udos-file-name */
char	hddname[20] = "md(0,0)";      /* hard-disk-driver-name (/usr) */
char	estring[10];		      /* Eingabe-Zeichenkette */

union {
	struct fblk fb;
	char pad1[BSIZE];
      } fbuf;
union {
	struct filsys fs;
	char pad2[BSIZE];
      } filsys;

struct	dinode pin;		/* inode der parent-Directory */
struct	dinode iin;		/* inode des zu installierenden Files */

long	time;			/* Zeit in Sekunden ab 1970 */
static	int  dmsize[12] =	/* Tage/Monat (kumulierend) */
	{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

long	zdb[12];		/* Blocknummern im inode */
long	adb[128];		/* einfach indirekt Adressblock */
long	iadb[128];		/* zweifach indirekt Adressblock */
daddr_t	aadb, aiadb;		/* Blocknummern der Adressbloecke */
int	padb, piadb, pzdb;	/* Pointer in den Adressbloecken */
ino_t	parinumb, inumb, oldinumb;

daddr_t	apardir;
char	pardir[BSIZE], parinob[BSIZE], inob[BSIZE];
int	ppnidb;
int	abfr;		/* 0 = abfragen, 1 = a, 2 = A, 3 = q, 4 = Q */
char	*aeintr;		  /* Adr(Dir.-Eintrag) */

daddr_t		alloc();
long		getnum();
ino_t		sun(), alloci();
unsigned int	get2b();
unsigned long	get3b();
/*---------------------------------------------  Funktion  ------------------*/

/*
 * Das Standalone Modul sa.install dient der Installation der File-Systeme
 * auf der Hard-Disk des P8000
 */

/*---------------------------------------------------------------------------*/



main()
{
    register i;

    indate();	/* Datum eingeben */
    rmopen(0);	/* remote open mit Abfrage */

for (i=1; i<5; i++) {
    printf("\noutput file system : ");
    gets(hddname, sizeof hddname);
    if (hddname[0] == '\0') {
	hddname[0] = 'm';
	printf("%s\n", hddname); }
    fdo = open(hddname, 1);
    fdi = open(hddname, 0);
    if (fdo<0 || fdi<0)
	printf("cannot open %s\n", hddname);
    else break; }
if (i == 5) exit(0);

for (i=0; i<128; pardir[i++]=0);	/* parent-Directory loeschen */
rdfs((long)1, (char *)&filsys);		/* read Superblock */

rddisk:
abfr = 0;
while (*contp != '\0') {
switch (*contp) {
    case '/':
	/*
	 * Directory - Definitions - Zeile
	 */
	if (abfr == 4) {
	    getstr();	/* bei eingegebenem 'Q' : Zeile ueberspringen */
	    break; }
	if (abfr == 1 || abfr == 3) abfr = 0;	/* 'a'- und 'q'-Mode beenden */
	wpar();		/* write parent-Directory-Block u. inode */
	parinumb = 2;
	rindi();	/* read root-inode-Block u. 1.root-Directory-Block */
	printf("%s\n", contp);
	contp++;
       rdloop:
	if (*contp == '\0') {
	    contp++;
	    break; }
	holdn();	/* Dir.-Namen aus CONT-Zeilenstueck in conline holen */
	parinumb = sun(0);	/* Directory-Namen in pardir suchen */
	if (parinumb == 0) {
	    printf("\ndirectory %s not found\n", conline);
	    insend(); }
	rindi();		/* Lese inode-Block u. 1.Directory-Block */
	goto rdloop;

    case 'l':
       /*
	* nlink - Zeile
	*/
	if (testab() == 0) break;
	getstr();
	getstr();
	if (sun(1)) {
	    printf("\nlink-file: %s already exist; cannot overwrite !\n", conline);
	    break; }
	iin.di_nlink++;
	cfilend();
	break;

    case 'd':
    case 'b':
    case 'c':
    case '-':
    case 'n':
       /*
	* create - file - Zeile
	*/
	if (testab() == 0) break;
	cfile();
	break;

    default:
	printf("syntax error in CONTENTS\n");
	printf("last instruction: %s\n",conline);
	printf("next characters (hexadecimal): %x ", *contp++ & 0xFF);
	printf("%x %x ", *contp++ & 0xFF, *contp++ & 0xFF);
	printf("%x %x etc.\n", *contp++ & 0xFF, *contp++ & 0xFF);
	insend();
    }
}

loopnd:
    printf("\n\007next input disk ? (y/n) : ");
    gets(estring, sizeof estring);
    switch (estring[0]) {
	case 'Y':
	case 'y':
	    rmopen(1);	/* remote open ohne Eingabe */
	    goto rddisk;
	case 'N':
	case 'n':
	    insend();
	default:
	    goto loopnd; }
}

/*---------------------------------------------------------------------------*/
/*
 * Aussprung aus sa.install
 */
/*---------------------------------------------------------------------------*/
insend()
{
    wpar();					/* write Parent-Dir. */
    filsys.s_time = time;			/* setze Superblock-Zeit */
    wtfs((long)1, (char *)&filsys);		/* write Superblock */
    exit(0);
}
/*---------------------------------------------------------------------------*/
/*
 * Lese Block (bno) von der Hard-Disk in den Puffer (bf)
 */
/*---------------------------------------------------------------------------*/
rdfs(bno, bf)
daddr_t bno;
char *bf;
{
    register n;

    lseek(fdi, bno*BSIZE, 0);
    n = read(fdi, bf, BSIZE);
    if (n != BSIZE) {
	printf("read error: %ld\n", bno);
	insend(); }
}

/*---------------------------------------------------------------------------*/
/*
 * Schreibe Puffer (bf) in den Block (bno) der Hard-Disk
 */
/*---------------------------------------------------------------------------*/
wtfs(bno, bf)
daddr_t bno;
char *bf;
{
    register n;

    lseek(fdo, bno*BSIZE, 0);
    n = write(fdo, bf, BSIZE);
    if (n != BSIZE) {
	printf("write error: %ld\n", bno);
	insend(); }
}

/*---------------------------------------------------------------------------*/
/*
 * Stelle einen freien Block (= Return-Wert) zu Verfuegung
 */
/*---------------------------------------------------------------------------*/
daddr_t
alloc()
{
    register i;
    register daddr_t bno;

    filsys.s_tfree--;
    bno = filsys.s_free[--filsys.s_nfree];
    if (bno == 0) {
	printf("\nout of free space\n");
	insend(); }
    if (filsys.s_nfree == 0) {
	rdfs(bno, (char *)&fbuf);
	filsys.s_nfree = fbuf.df_nfree;
	for (i=0; i < NICFREE; i++)
	    filsys.s_free[i] = fbuf.df_free[i];
	}
    return(bno);
}

/*---------------------------------------------------------------------------*/
/*
 * Speichere einen freigewordenen Block zurueck in die Freiblock-Liste
 */
/*---------------------------------------------------------------------------*/
bfree(bno)
register daddr_t bno;
{
    register i;

    filsys.s_tfree++;
    if (filsys.s_nfree == NICFREE) {
	clear(&fbuf, BSIZE);
	fbuf.df_nfree = filsys.s_nfree;
	for (i=0; i < NICFREE; i++)
	    fbuf.df_free[i] = filsys.s_free[i];
	wtfs(bno, (char *)&fbuf);
	filsys.s_nfree = 0; }
    filsys.s_free[filsys.s_nfree++] = bno;
}
/*---------------------------------------------------------------------------*/
/*
 * open remote
 */
/*---------------------------------------------------------------------------*/
rmopen(out)
register out;
{
    register size, c, i;

for (i=1; i<5; i++) {
    if (out == 0) {
	printf("\ninput file system : ");
	gets(fddname, sizeof fddname); }
    if (fddname[0] == '\0') {
	fddname[0] = 'u';
	printf("%s\n", fddname); }
    adname(CON);
    fsi = open(fddname, 0);
    if (fsi<0) {
	printf("cannot open %s\n", fddname);
	out = 0; }
    else break; }
if (i == 5) insend();

size = 0;
for (i=1; i <= LCONT; i++) {
    c = read(fsi, &CONT[size], BSIZE);
    if (c < 0) insend();
    size += c;
    if (c < BSIZE) break; }
if (i > LCONT) {
    printf("\nCONTENTS to long\n");
    insend(); }
close(fsi);

for (i=0; i <= size; i++)
    if (CONT[i] == '\r' || CONT[i] == '\n') CONT[i] = '\0';
for (--i; i < LCONT*BSIZE; i++) CONT[i] = '\0';

contp = CONT;
}

/*---------------------------------------------------------------------------*/
/*
 * Eingabe des Datums
 * Gueltig: 1.1.70 - 18.1.2038
 */
/*---------------------------------------------------------------------------*/
indate()
{
    int year, month, day, hour;

loop:
    printf("Enter Date (MM/DD/YY) : ");
    gets(estring, sizeof estring);
    month = getdat(1);
    day   = getdat(2);
    year  = getdat(3);
    hour = 6;	/* -1h fuer localtime und timezone */
    if (month<1 || month>12 || day<1 || day>31)
	goto loop;
    time = 0;
    if (year%4 == 0 && month > 2) ++dmsize[month-1];
    if (year < 70) year += 100;
    time = (year-69)/4;
    time += 365*(year-70)+dmsize[month-1];
    time += day - 1;
    if (time > 24854) {
	printf("can only create date from 1/1/(19)70 to 1/18/(20)38\n");
	goto loop; }
    time = 24*time + hour;
    time = 60*60*time;
}

getdat(k)
int k;
{
    register i, c, n;

    n = 0;
    for (i=0; c=estring[i]; i++) {
	if (k-1 == 0) {
	       if (c == '/') return(n);
	       if (c < '0' || c > '9') return(-1);
	       n = n*10 + (c-'0'); }
	else if (c == '/') k--; }
    return(n);
}
/*---------------------------------------------------------------------------*/
/*
 * Haenge UDOS-Dateinamen (*pt) an Drivernamen (fddname) an
 */
/*---------------------------------------------------------------------------*/
adname(pt)
char *pt;
{
    register i;

    for (i=0; (fddname[i] !=  ')') && (i < 20); i++);
    do { i++;
	 fddname[i] = *pt;
	 pt++;
       } while (*pt != '\0' && *pt != ' ');
    fddname[++i] = '\0';
}

/*---------------------------------------------------------------------------*/
/*
 * Lese Inode und zugehoerigen 1.Directory-Block
 */
/*---------------------------------------------------------------------------*/
rindi()
{

    rdfs((long)((parinumb-1)>>3)+2, parinob);
    copy(&pin, parinob+(parinumb-1)%8*64);

    ppnidb = 1;
    apardir = get3b(pin.di_addr);
    rdfs(apardir, pardir);
}

/*---------------------------------------------------------------------------*/
/*
 * Hole Dir.-Namen aus der Dir.-Definition-Zeile (CONT) in Puffer (conline)
 */
/*---------------------------------------------------------------------------*/
holdn()
{
    register i;

    for (i=0; i < 50; i++) {
	conline[i] = *(contp++); 
	if (*contp == '/' || *contp == '\0') {
	    conline[i+1] = '\0';
	    if (*contp == '/') contp++;
	    break; }
	}
}
/*---------------------------------------------------------------------------*/
/*
 * Suche Namen (conline) in den 10 dir. Bloecken der Directory (pardir)
 */
/*---------------------------------------------------------------------------*/
ino_t
sun(c)
register c;
{
    register i, j, k;
    register ino_t inonb;
    long z;

   loop:
    k=1;
    for (i=0; i<32; i++) {
	if ((inonb = get2b(aeintr = &pardir[i*16])) == 0) return(0);
	k=0;
	for (j=0; j<15; j++) {
	    if (pardir[i*16+2+j] != conline[j]) {
		k++;
		break; }
	    if (conline[j] == '\0') break; }
	if (k == 0) return(inonb); }
    if (ppnidb <= 10) {
	if ((z = get3b(&pin.di_addr[3*ppnidb])) == 0) {
	    if (c==0) return(0);		/* nur suchen */
	    wtfs(apardir, pardir);		/* creiere neuen Block*/
	    z=alloc();
	    rdfs(z,pardir);
	    for (j=0; j<BSIZE; j++) pardir[j]=0;
	    ltol3(&pin.di_addr[3*ppnidb], &z, 1); }
	else rdfs(z, pardir);
	ppnidb++;
	apardir = z;
	goto loop; }
    else {
	/* Directory hat mehr als 10 Bloecke (= 318 Files) */
	printf("\ndirectory too long !\n");
	insend(); }
return(0);
}

/*---------------------------------------------------------------------------*/
/*
 * Schreibe parent-Directory-Block und parent-Inode
 */
/*---------------------------------------------------------------------------*/
wpar()
{

    if (parinumb && oldinumb) {
    if (pardir[0] || pardir[1]) {		/* beim 1.Mal ist pardir 0 */
	wtfs(apardir, pardir);
	if (((parinumb-1)>>3) != ((oldinumb-1)>>3)) {
	    wtfs((long)((oldinumb-1)>>3)+2, inob);
	    rdfs((long)((parinumb-1)>>3)+2, parinob);
	    copy(parinob+(parinumb-1)%8*64, &pin);
	    wtfs((long)((parinumb-1)>>3)+2, parinob); }
	else {
	    copy(inob+(parinumb-1)%8*64, &pin);
	    wtfs((long)((oldinumb-1)>>3)+2, inob); }
    } }
}
/*---------------------------------------------------------------------------*/
/*
 * Test der Zelle abfr und Reaktion darauf
 */
/*---------------------------------------------------------------------------*/
testab()
{

    if (abfr > 2) {
	while (*contp != '\0') ++contp;
	contp++;
	return(0); }
    printf("%s", contp);
    if (abfr) printf("\n");
    else {
       loop:
	printf("  (n/y/a/A/q/Q) ? : ");
	gets(estring, sizeof estring);
	switch (estring[0]) {
	    case 'a':
		abfr = 1;
		goto caseY;
	    case 'A':
		abfr = 2;
	    case 'y':
	    case 'Y':
	    caseY:
		return(1);
	    case 'q':
		abfr = 3;
		goto caseN;
	    case 'Q':
		abfr = 4;
	    case 'n':
	    case 'N':
	    caseN:
		while (*contp != '\0') ++contp;
		contp++;
		return(0);
	    default:
		goto loop;
	    }
	}
    return(1);				/* fuer 'a' und 'A' */
}
/*---------------------------------------------------------------------------*/
/*
 * Directories und special-Files creieren und regular-Files laden
 */
/*---------------------------------------------------------------------------*/
cfile()
{
    char db[BSIZE];
    register i, c, nullfile;
    ino_t dupinumb;
    struct dinode dupin;
 
    nullfile = 0;
    for (i=0; i<BSIZE; db[i++]=0);		/* loesche Datenpuffer 'db' */

    clear(&iin, 64);				/* loesche Strucktur 'iin' */
    iin.di_atime = iin.di_mtime = iin.di_ctime = time;
    padb = piadb = pzdb = 0;
    for (i=0; i<12; zdb[i++]=0);

    getstr();
    if (conline[0] == 'n') {
	conline[0] = '-';
	nullfile++; }
    iin.di_mode =  gmode(conline[0], "-bcd", IFREG, IFBLK, IFCHR, IFDIR);
    iin.di_mode |= gmode(conline[1],  "-u" , 0,     ISUID, 0,     0);
    iin.di_mode |= gmode(conline[2],  "-g" , 0,     ISGID, 0,     0);
    iin.di_mode |= gmode(conline[3],  "-t" , 0,     ISVTX, 0,     0);
    for (i=4; i<7; i++) {
	c = conline[i];
	if (c < '0' || c > '7') {
	    printf("\nbad octal digit %x in %s\n", c, conline);
	    insend(); }
	iin.di_mode |= (c-'0')<<(18-3*i); }
    iin.di_uid = getnum();
    iin.di_gid = getnum();
    iin.di_nlink = 1;

    inumb = alloci();

    switch (iin.di_mode & IFMT) {
	case IFREG:
	    getstr();		/* UDOS-Filename */
	    adname(conline);
	    if (*--contp) getstr();	/* wega-Filename */
	    else contp++;
    	    if (dupinumb = sun(1)) {
		rdfs((long)((dupinumb-1)>>3)+2, db);
		copy(&dupin, db+(dupinumb-1)%8*64);
		if (dupin.di_nlink) {
		    printf("file: %s already exist\n", conline);
	           loop:
		    printf("overwrite (y/n/q) ? : ");
		    gets(estring, sizeof estring);
		    switch (estring[0]) {
		    case 'y':
			pin.di_size -= 0x10;
			if (dupin.di_nlink != 1)
			    iin.di_nlink = dupin.di_nlink;
			bfinfree(dupin.di_addr);
			if (((dupinumb-1)>>3) != ((oldinumb-1)>>3)) {
			    db[(dupinumb-1)%8*64] = 0;
			    db[(dupinumb-1)%8*64+1] = 0;
			    wtfs((long)((dupinumb-1)>>3)+2, db); }
			else {
			    inob[(dupinumb-1)%8*64] = 0;
			    inob[(dupinumb-1)%8*64+1] = 0; }
			ifree(inumb);	/* set inumber free */
			inumb = dupinumb;
			break;
		    case 'q':
			abfr = 3;	/* Abbruch fuer akt. Dir. */
		    case 'n':
			ifree(inumb);
			return;
		    default:
			goto loop;
	    } } }
	    if (nullfile) break;
	   loopopen:
	    fsi = open(fddname, 0);
/* Achtung: ab 'open' bis 'close' ist kein E/A-Verkehr ueber tty moeglich !!! */
	    if (fsi<0) {
	       loopno:
		printf("repeat (y/n/q) ? : ");
		gets(estring, sizeof estring);
		switch (estring[0]) {
		    case 'y':
			goto loopopen;
		    case 'q':
			abfr = 3;	/* Abbruch fuer akt. Dir. */
		    case 'n':
			ifree(inumb);
			return;
		    default:
			goto loopno; } }
	    while ((i=read(fsi, db, BSIZE)) > 0) {
		iin.di_size += i;
		newblk(db); }
	    close(fsi);
	    if (i < 0) {
		if (piadb) wtfs(aiadb, iadb);
		if (padb)  wtfs(aadb,   adb);
		if (pzdb) ltol3(iin.di_addr, zdb, pzdb);
		bfinfree(iin.di_addr);
	       loopre:
		printf("repeat (y/n/q) ? : ");
		gets(estring, sizeof estring);
		switch (estring[0]) {
		    case 'y':
			iin.di_size = 0;
			padb = piadb = pzdb = 0;
			for (i=0; i<12; zdb[i++]=0);
			goto loopopen;
		    case 'q':
			abfr = 3;	/* Abbruch fuer akt. Dir. */
		    case 'n':
			ifree(inumb);
			return;
		    default:
			goto loopre; } }

	    ltol3(iin.di_addr, zdb, pzdb);
	    if (padb)  wtfs(aadb,   adb);
	    if (piadb) wtfs(aiadb, iadb);
	    break;

	case IFBLK:
	case IFCHR:
	    i = getnum() & 0xFF;	/* majory number */
	    c = getnum() & 0xFF;	/* minory number */
	    iin.di_addr[1] = i;
	    iin.di_addr[2] = c;
	    getstr();
	    if(sun(1)) {
		printf("\nspecial-file: %s already exist; cannot overwrite !\n", conline);
		ifree(inumb);
		return; }
	    break;

	case IFDIR:
	    getstr();
	    if (sun(1)) {
		printf("\ndirectory: %s already exist; cannot overwrite !\n", conline);
		ifree(inumb);
		return; }
	    entry(&inumb, ".", db);
	    entry(&parinumb, "..", &db[16]);
	    newblk(db);
	    ltol3(iin.di_addr, zdb, pzdb);
	    iin.di_nlink++;
	    iin.di_size = 0x20;
	    ++pin.di_nlink;
	    break;

	default:
	    printf("\ndi_mode - Fehler\n");
	    insend(); }

    cfilend();
}
/*---------------------------------------------------------------------------*/
/*
 * cfile-Ende-Behandlung
 */
/*---------------------------------------------------------------------------*/
cfilend()
{

    if (oldinumb && (((inumb-1)>>3) != (oldinumb-1)>>3))
	wtfs((long)((oldinumb-1)>>3)+2, inob);
    if (((inumb-1)>>3) != ((oldinumb-1)>>3))
	rdfs((long)((inumb-1)>>3)+2, inob);
    copy(inob+(inumb-1)%8*64, &iin);
    oldinumb = inumb;

    pin.di_size += 0x10;
    entry(&inumb, conline, aeintr);
}
/*---------------------------------------------------------------------------*/
/*
 * Uebertrage akt. Teil einer Anweisung von CONT nach conline[]
 */
/*---------------------------------------------------------------------------*/
getstr()
{
    register i;
    register char c;

   loop:
    switch (c = *contp++) {
    case ' ':
	goto loop;  }

    i = 0;
    do { conline[i++] = c ; c = *contp++; }  
    while (c != ' ' && c != '\0');
    conline[i] = '\0';
}

/*---------------------------------------------------------------------------*/
/*
 * Eintragen des Parameters m0,...,m3 in den Return-Wert
 */
/*---------------------------------------------------------------------------*/
gmode(c, s, m0, m1, m2, m3)
char c;
register char *s;
{
    if (c == s[0])
	return(m0);
    if (c == s[1])
	return(m1);
    if (c == s[2])
	return(m2);
    if (c == s[3])
	return(m3);
    printf("\nbad charakter %x in %s\n", c, conline);
    insend();
    return(0);
}

/*---------------------------------------------------------------------------*/
/*
 * Dezimale Eingabekonvertierung aus conline
 */
/*---------------------------------------------------------------------------*/
long
getnum()
{
    register i, c;
    register long n;

    getstr();
    n = 0;
    for (i=0; c=conline[i]; i++) {
	if (c < '0' || c > '9') {
	    printf("\nbad decimal digit %x in %s\n", c, conline);
	    insend(); }
	n = n*10 + (c-'0'); }
    return(n);
}
/*---------------------------------------------------------------------------*/
/*
 * Loesche strukturierten Puffer *pt der Laenge l
 */
/*---------------------------------------------------------------------------*/
clear(pt, l)
register l;
register char *pt;
{
    register i;

    for (i=0; i<l; *pt++ = '\0', i++);
}

/*---------------------------------------------------------------------------*/
/*
 * Copiere Inode (str1) := (str2)
 */
/*---------------------------------------------------------------------------*/
copy(str1, str2)
register char *str1, *str2;
{
    register i;

    for (i=0; i<64; *str1++ = *str2++, i++);
}
/*---------------------------------------------------------------------------*/
/*
 * Gebe im Return-Wert freie inode-Nr. zurueck
 */
/*---------------------------------------------------------------------------*/
ino_t
alloci()
{
    register i, j, k;
    register ino_t inr;
    char     bf[BSIZE];

    filsys.s_tinode--;
    if (filsys.s_ninode == 0) {		/* inode Liste fuellen */
	for (i=0; i<filsys.s_isize; i++) {
	    if (i == (oldinumb-1)>>3)
		for (j=0; j<BSIZE; j++) bf[j] = inob[j];
	    else rdfs((long)i+2, bf);
	    for (j=0; j<8; j++) {
		if (get2b(&bf[64*j]) == 0) {
		    filsys.s_ninode++;
		    k = 100-filsys.s_ninode;
		    filsys.s_inode[k] = j+1+i*8;
		    if (filsys.s_ninode == 100)
			goto holinr; } } }

	printf("\nout of free inodes\n");
	insend(); }
   holinr:		/* hole inode-Nr. */
    inr = filsys.s_inode[--filsys.s_ninode];
    return(inr);
}

/*---------------------------------------------------------------------------*/
/*
 * Gebe eine freigewordene inode zurueck in die Frei-inode-Liste
 */
/*---------------------------------------------------------------------------*/
ifree(inb)
register ino_t inb;
{
    filsys.s_tinode++;
    if (filsys.s_ninode == NICINOD) return;
    filsys.s_inode[filsys.s_ninode++] = inb;
}

/*---------------------------------------------------------------------------*/
/*
 * Gebe Bloecke aus inode wieder frei
 */
/*---------------------------------------------------------------------------*/
bfinfree(pnt)
register pnt;
{
    daddr_t  bn;
    register i, j;
    long     addrb[12], bf[128], ibf[128];

    l3tol(addrb, pnt, 12);
    for (i=0; i<10; i++) {
	if (bn = addrb[i]) bfree(bn);
	else return; }

    if (bn = addrb[10]) {
	rdfs(bn, bf);
	bfree(bn);
	for (i=0; i<128; i++) {
	if (bn = bf[i]) bfree(bn);
	else return; } }
    else return;

    if (bn = addrb[11]) {
	rdfs(bn, ibf);
	bfree(bn);
	for (i=0; i<128; i++) {
	    if (bn = ibf[i]) {
		rdfs(bn, bf);
		bfree(bn);
		for (j=0; j<128; j++) {
		    if (bn = bf[j]) bfree(bn);
		    else return; } }
	    else return; } }
}
/*---------------------------------------------------------------------------*/
/*
 * Speicher Block (db) auf Hard-Disk und notiere es im inode (ib)
 */
/*---------------------------------------------------------------------------*/
newblk(db)
char *db;
{
    register i;
    register daddr_t bno;

    bno = alloc();
    wtfs(bno, db);
    for (i=0; i<BSIZE; db[i++]=0);

    if (pzdb < 10) {			/* direkte Bloecke */
	zdb[pzdb++] = bno;
	return; }

    /* dreifach indirekt wird nicht realisiert */
    if (piadb == 0 && padb == 0 && pzdb == 12) {
	printf("\nfile to long\n");
	insend(); }

    /* Start: zweifach indirekt */
    if (piadb == 0 && padb == 0 && pzdb == 11) {
	aiadb = alloc();
	for (i=0; i<128; iadb[i++]=0);
	zdb[pzdb++] = aiadb; }

    /* Start: einfach indirekt */
    if (padb == 0) {
	aadb = alloc();
	for (i=0; i<128; adb[i++]=0);
	if (pzdb == 12) iadb[piadb++] = aadb;	/* auch bei zweifach indirekt */
	else zdb[pzdb++] = aadb; }

    adb[padb++] = bno;

    if (piadb == 128) {
	wtfs(aiadb, iadb);
	piadb = 0; }

    if (padb == 128) {
	wtfs(aadb, adb);
	padb = 0; }
}

/*---------------------------------------------------------------------------*/
/*
 * Eintragen des Filenamen (incl. inode-Nummer) in die Directory
 */
/*---------------------------------------------------------------------------*/
entry(inum, str, direa)
register char *inum, *str, *direa;
{
    register i, j;

    j = 0;
    *direa++ = *inum++;
    *direa++ = *inum;
    for (i=0; i<14; i++) {
	if (*str == '\0' || j == 1)
	    { *direa++ = '\0'; j = 1; }
	else *direa++ = *str++; }
}
/*---------------------------------------------------------------------------*/
/*
 * Hole aus einem char-Buffer 2 oder 3 Byte
 */
/*---------------------------------------------------------------------------*/
unsigned
get2b(e)
char *e;
{
register unsigned i;
i = (*e << 8) | (*(e+1) & 0xFF);
return(i);
}
unsigned long
get3b(e)
char *e;
{
register unsigned long i;
i = ((unsigned long)*e << 16) & 0xFF0000;
i |= ((unsigned long)*(e+1) << 8) & 0xFF00;
i |= (unsigned long)*(e+2) & 0xFF;
return(i);
}
