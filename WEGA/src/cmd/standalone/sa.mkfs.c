/***************************************************************************
 ***************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1985
	Standalone:	sa.mkfs.c
 
 
	Bearbeiter:	P. Bala / P.Hoge
	Datum:		04/01/87 11:16:06
	Version:	1.1
 
 ***************************************************************************
 
  Hinweise:
	mkfs kann sowohl als Standalone- als auch als Kommandoversion
	generiert werden (Konstante STANDALONE =1 bzw. 0).
 
 ***************************************************************************
 ***************************************************************************/
 
/*--------------------------------------------------------------------------
 *
 * Aufbau eines Filesystems-Prototyps
 * Aufruf im Kommando: mkfs filsys proto/size [ m n ]
 * Bei Standalone-Version erfolgt Parameter-Angabe ueber Konsole
 *
 *--------------------------------------------------------------------------
 */
/*------------------------------------------------- Konstanten -------------*/
#define STANDALONE 1
#define	NIPB	(BSIZE/sizeof(struct dinode))
#define	NDIRECT	(BSIZE/sizeof(struct direct))
#define	LADDR	10		/* Anzahl der Direktadressen im Inode */
#define	MAXFN	72		/* Anzahl Blocks per Disk-Zyinder */
#define VERSATZ 1		/* kein Blockversatz */
/*---------------------------------------------------------------------------*/
 
#ifndef STANDALONE
#include <stdio.h>
#include <s.out.h>
#endif
#include <sys/param.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <sys/filsys.h>
#include <sys/fblk.h>
#include <sys/dir.h>
 
/*------------------------------------------------------- Variable --------*/
time_t	utime;
#ifndef STANDALONE
FILE 	*fin;
#else
int	fin;
#endif
int	fsi;
int	fso;
char	*charp;
char	buf[BSIZE];
union {
	struct fblk fb;
	char pad1[BSIZE];
} fbuf;
#ifndef STANDALONE
struct s_exec head;
#endif
char	string[50];
union {
	struct filsys fs;
	char pad2[BSIZE];
} filsys;
char	*fsys;
char	*proto;
int	f_n	= MAXFN;
int	f_m	= VERSATZ;	/* Standard-Blockversatz */
int	error;
ino_t	ino;
long	getnum();
daddr_t	alloc();

/*-------------------------------------------------------------------------
 *
 *		Haupt - Programm
 *
 *-------------------------------------------------------------------------
 */
 
main(argc, argv)
char *argv[];
{
	register int f, c;
	register long n;

#ifndef STANDALONE
	time(&utime);
	if(argc < 3) {
		printf("usage: mkfs filsys proto/size [ m n ]\n");
		exit(1);
	}
	fsys = argv[1];
	proto = argv[2];
#else
/* Definiere Filesystem-Groesse ueber Konsole */
	{
		static char protos[60];

		printf("file system size: ");
		gets(protos, sizeof protos);
		proto = protos;
	}
#endif
#ifdef STANDALONE
	{
		char fsbuf[100];

/* Definiere Name und Nr. des 1. Blocks des Filesystems ueber Konsole */
		do {
			printf("file system: ");
			gets(fsbuf, sizeof fsbuf);
			fso = open(fsbuf, 1);
			fsi = open(fsbuf, 0);
		} while (fso < 0 || fsi < 0);
	}
	fin = 0;
	argc = 0;
#else
	fso = creat(fsys, 0666);
	if(fso < 0) {
		printf("%s: cannot create\n", fsys);
		exit(1);
	}
	fsi = open(fsys, 0);
	if(fsi < 0) {
		printf("%s: cannot open\n", fsys);
		exit(1);
	}
	fin = fopen(proto, "r");
#endif
/* Festlegen der Anzahl der inode-Bloecke des Filesystems */
	if(fin == 0) {
		n = 0;
		for(f=0; c=proto[f]; f++) {
			if(c<'0' || c>'9') {
				printf("%s: cannot open\n", proto);
				exit(1);
			}
			n = n*10 + (c-'0');
		}
		filsys.s_fsize = n;
		n = n/25;
		if(n <= 0)
			n = 1;
		if(n > 65500/NIPB)
			n = 65500/NIPB;
		filsys.s_isize = n + 2;
		printf("isize = %d\n", filsys.s_isize*NIPB);
/*
 * Kommando-String zur Generierung einer leeren Directory
 */
		charp = "d--777 0 0 $ ";
		goto f3;
	}

#ifndef STANDALONE

	getstr();
	f = open(string, 0);
	if(f < 0) {
		printf("%s: cannot  open init\n", string);
		goto f2;
	}
	read(f, (char *)&head, sizeof head);
	if(head.s_magic != N_MAGIC1) {
		printf("%s: bad format\n", string);
		goto f1;
	}
	c = head.s_imsize;
	if(c > BSIZE) {
		printf("%s: too big\n", string);
		goto f1;
	}
	read(f, buf, c);
	wtfs((long)0, buf);

f1:
	close(f);


f2:
	filsys.s_fsize = getnum();
	n = getnum();
	n /= NIPB;
	filsys.s_isize = n + 3;

#endif
f3:
	if(argc >= 5) {
		f_m = atoi(argv[3]);
		f_n = atoi(argv[4]);
		if(f_n <= 0 || f_n >= 1000)
			f_n = 1000;
		if(f_m <= 0 || f_m > f_n)
			f_m = 3;
	}
	filsys.s_m = f_m;
	filsys.s_n = f_n;
	printf("m/n = %d %d\n", f_m, f_n);
	if(filsys.s_isize >= filsys.s_fsize) {
		printf("%ld/%ld: bad ratio\n", filsys.s_fsize, filsys.s_isize-2);
		exit(1);
	}
	filsys.s_tfree = 0;
	filsys.s_tinode = 0;
	for(c=0; c<BSIZE; c++)
		buf[c] = 0;
	for(n=2; n!=filsys.s_isize; n++) {
		wtfs(n, buf);
		filsys.s_tinode += NIPB;
	}
	ino = 0;

/* Anlegen der Freiblock-Liste des Filesystems */
	bflist();
 
/* Generierung der Root-Directory entspr. der Kommando-String */
	cfile((struct inode *)0);

	filsys.s_time = utime;
	filsys.s_mach = 0;
/* Schreibe Superblock (Blocknr. 1 des Filesystems) auf Disk */
	wtfs((long)1, (char *)&filsys);
	exit(error);
}

/*-----------------------------------------------------------------------
 * Generiere einen File unter wega-Verwaltung anhand einer
 * Kommando-String
 *-----------------------------------------------------------------------
 */
cfile(par)
register struct inode *par;
{
	struct inode in;
	int dbc, ibc;
	char db[BSIZE];
	daddr_t ib[NINDIR];
	register int i, f, c;

	/*
	 * Eingabe mode, uid und gid
	 */

	getstr();
	in.i_mode = gmode(string[0], "-bcd", IFREG, IFBLK, IFCHR, IFDIR);
	in.i_mode |= gmode(string[1], "-u", 0, ISUID, 0, 0);
	in.i_mode |= gmode(string[2], "-g", 0, ISGID, 0, 0);
	for(i=3; i<6; i++) {
		c = string[i];
		if(c<'0' || c>'7') {
			printf("%c/%s: bad octal mode digit\n", c, string);
			error = 1;
			c = 0;
		}
		in.i_mode |= (c-'0')<<(15-3*i);
	}
	in.i_uid = getnum();
	in.i_gid = getnum();


/* Loesche zugewiesenes inode */
	ino++;
	in.i_number = ino;
	for(i=0; i<BSIZE; i++)
		db[i] = 0;
	for(i=0; i<NINDIR; i++)
		ib[i] = (daddr_t)0;
	in.i_nlink = 1;
	in.i_size = 0;
	for(i=0; i<NADDR; i++)
		in.i_addr[i] = (daddr_t)0;
	if(par == (struct inode *)0) {
		par = &in;
		in.i_nlink--;
	}
	dbc = 0;
	ibc = 0;
	switch(in.i_mode&IFMT) {
/* regular file
 * die in der Kommando-String angegebene Datei wird in das
 * Filesystem uebertragen
 */

	case IFREG:
		/*
		 * Regular File
		 * Inhalt ist ein File-Name
		 */

		getstr();
		f = open(string, 0);
		if(f < 0) {
			printf("%s: cannot open\n", string);
			error = 1;
			break;
		}
		while((i=read(f, db, BSIZE)) > 0) {
			in.i_size += i;
			newblk(&dbc, db, &ibc, ib);
		}
		close(f);
		break;

/* special file
 * die in der Kommando-String notierte Devicenr. wird in das
 * inode uebernommen
 */
	case IFBLK:
	case IFCHR:
		/*
		 * Special File
		 * Inhalt ist eine major/minor - Devicenummer
		 */

		i = getnum() & 0377;
		f = getnum() & 0377;
		in.i_addr[0] = (i<<8) | f;
		break;

	case IFDIR:
/* directory
 * Anlegen eines File-Verzeichnisses bei rekursiver Definition
 * der in der akt. Directory notierten Files entsprechend der 
 * Kommando-String.
 * Ende der Eintragungen in die akt. Directory, wenn der File-
 * Name "$" gefunden wurde
 */

		par->i_nlink++;
		in.i_nlink++;
		entry(in.i_number, ".", &dbc, db, &ibc, ib);
		entry(par->i_number, "..", &dbc, db, &ibc, ib);
		in.i_size = 2*sizeof(struct direct);
		for(;;) {
			getstr();
			if(string[0]=='$' && string[1]=='\0')
				break;
			entry(ino+1, string, &dbc, db, &ibc, ib);
			in.i_size += sizeof(struct direct);
			cfile(&in);
		}
		break;
	}
/* Schreibe inode auf Disk */
	if(dbc != 0)
		newblk(&dbc, db, &ibc, ib);
	iput(&in, &ibc, ib);
}

/*-------------------------------------------------------------------------
 * Eintragen einer der drei Parameter m0 .. m3 entsprechend
 * dem Kommando-Steuerzeichen in c anhand der vorgegebenen
 * Moeglichkeiten s[0] .. s[3]
 *-------------------------------------------------------------------------
 */
gmode(c, s, m0, m1, m2, m3)
char c;
register char *s;
{
	if(c == s[0])
		return(m0);
	if(c == s[1])
		return(m1);
	if(c == s[2])
		return(m2);
	if(c == s[3])
		return(m3);
	printf("%c/%s: bad mode\n", c, string);
	error = 1;
	return(0);
}

/*----------------------------------------------------------------------
 * Eingabe-Konvertierung dezimal aus Kommando-String 
 *----------------------------------------------------------------------
 */
long
getnum()
{
	register int i, c;
	register long n;

	getstr();
	n = 0;
	for(i=0; c=string[i]; i++) {
		if(c<'0' || c>'9') {
			printf("%s: bad number\n", string);
			error = 1;
			return((long)0);
		}
		n = n*10 + (c-'0');
	}
	return(n);
}

/*----------------------------------------------------------------------
 * Uebertrage akt. Term aus Kommando-String nach string[]
 *----------------------------------------------------------------------
 */
getstr()
{
	register int i, c;

loop:
	switch(c=getch()) {

	case ' ':
	case '\t':
	case '\n':
		goto loop;

	case '\0':
		printf("EOF\n");
		exit(1);

	case ':':
		while(getch() != '\n');
		goto loop;

	}
	i = 0;

	do {
		string[i++] = c;
		c = getch();
	} while(c!=' '&&c!='\t'&&c!='\n'&&c!='\0');
	string[i] = '\0';
}

/*--------------------------------------------------------------------
 * Read Block von Disk
 *--------------------------------------------------------------------
 */
rdfs(bno, bf)
daddr_t bno;
char *bf;
{
	int n;

	lseek(fsi, bno*BSIZE, 0);
	n = read(fsi, bf, BSIZE);
	if(n != BSIZE) {
		printf("read error: %ld\n", bno);
		exit(1);
	}
}

/*----------------------------------------------------------------------
 * Write Block auf Disk
 *----------------------------------------------------------------------
 */
wtfs(bno, bf)
daddr_t bno;
char *bf;
{
	int n;

	lseek(fso, bno*BSIZE, 0);
	n = write(fso, bf, BSIZE);
	if(n != BSIZE) {
		printf("write error: %D\n", bno);
		exit(1);
	}
}

/*------------------------------------------------------------------------
 * Der Return-Wert von alloc ist die Nummer des naechsten Blocks
 * aus der Freiblockliste des Superblocks. Ist diese leer, wird
 * der naechste Ankerblock in den Superblock gelesen und die Nr.
 * des Ankerblocks selbst zurueckgegeben. Ist kein solcher Anker-
 * Block mehr vorhanden, wird mkfs beendet
 *------------------------------------------------------------------------
 */
daddr_t
alloc()
{
	register int i;
	register daddr_t bno;

	filsys.s_tfree--;
	bno = filsys.s_free[--filsys.s_nfree];
	if(bno == 0) {
		printf("out of free space\n");
		exit(1);
	}
	if(filsys.s_nfree <= 0) {
		rdfs(bno, (char *)&fbuf);
		filsys.s_nfree = fbuf.df_nfree;
		for(i=0; i<NICFREE; i++)
			filsys.s_free[i] = fbuf.df_free[i];
	}
	return(bno);
}

/*-----------------------------------------------------------------------
 * Eintragen der Blocknr. bno in die Freiblockliste des Superblocks
 * Ist dieses Teil voll, so wird der akt. freizugebende Block als
 * Ankerblock benutzt, um die volle Freiblockliste des Superblocks 
 * auf die Disk auszulagern
 *-----------------------------------------------------------------------
 */
bfree(bno)
register daddr_t bno;
{
	register int i;

	filsys.s_tfree++;
	if(filsys.s_nfree >= NICFREE) {
		fbuf.df_nfree = filsys.s_nfree;
		for(i=0; i<NICFREE; i++)
			fbuf.df_free[i] = filsys.s_free[i];
		wtfs(bno, (char *)&fbuf);
		filsys.s_nfree = 0;
	}
	filsys.s_free[filsys.s_nfree++] = bno;
}

/*---------------------------------------------------------------------
 * Eintragen Filename incl. inode-Nummer in akt. Directory
 *---------------------------------------------------------------------
 */
entry(inum, str, adbc, db, aibc, ib)
ino_t inum;
char *str;
register int *adbc, *aibc;
char *db;
daddr_t *ib;
{
	register struct direct *dp;
	register int i;

	dp = (struct direct *)db;
	dp += *adbc;
	(*adbc)++;
	dp->d_ino = inum;
	for(i=0; i<DIRSIZ; i++)
		dp->d_name[i] = 0;
	for(i=0; i<DIRSIZ; i++)
		if((dp->d_name[i] = str[i]) == 0)
			break;
	if(*adbc >= NDIRECT)
		newblk(adbc, db, aibc, ib);
}

/*----------------------------------------------------------------------
 * Schreibe gefuellten internen Datenblock auf Disk und loesche
 * den internen Datenblock
 *----------------------------------------------------------------------
 */
newblk(adbc, db, aibc, ib)
register int *adbc, *aibc;
char *db;
daddr_t *ib;
{
	register int i;
	daddr_t bno;

	bno = alloc();
	wtfs(bno, db);
	for(i=0; i<BSIZE; i++)
		db[i] = 0;
	*adbc = 0;
	ib[*aibc] = bno;
	(*aibc)++;
	if(*aibc >= NINDIR) {
		printf("indirect block full\n");
		error = 1;
		*aibc = 0;
	}
}

/*-----------------------------------------------------------------
 * Lies akt. Zeichen von Kommando-String
 *-----------------------------------------------------------------
 */
getch()
{

#ifndef STANDALONE
	if(charp)
#endif
		return(*charp++);
#ifndef STANDALONE
	return(getc(fin));
#endif
}

/*---------------------------------------------------------------------
 * Aufbau der Freiblock-Liste des akt. Filesystems
 *---------------------------------------------------------------------
 */
bflist()
{
	struct inode in;
	daddr_t ib[NINDIR];
	int ibc;
	char flg[MAXFN];
	int adr[MAXFN];
	register int i, j;
	register daddr_t f, d;

	/* Aufbau der Musterverteilung der freien Bloecke auf
	 * einem Disk-Zylinder entsprechend des vorgegebenen
	 * Blockversatzes
	 */
	for(i=0; i<f_n; i++)
		flg[i] = 0;
	i = 0;
	for(j=0; j<f_n; j++) {
		while(flg[i])
			i = (i+1)%f_n;
		adr[j] = i+1;
		flg[i]++;
		i = (i+f_m)%f_n;
	}

	/* Anlegen des inodes 1 (bad blocks)
	 */
	ino++;
	in.i_number = ino;
	in.i_mode = IFREG;
	in.i_uid = 0;
	in.i_gid = 0;
	in.i_nlink = 0;
	in.i_size = 0;
	for(i=0; i<NADDR; i++)
		in.i_addr[i] = (daddr_t)0;

	for(i=0; i<NINDIR; i++)
		ib[i] = (daddr_t)0;
	ibc = 0;
	/* Markiere Ende der Freiblockliste
	 * (Adr. Ankerblock = 0)
	 */
	bfree((daddr_t)0);
	/* Generiere Freiblockliste anhand der Musterverteilung
	 * wobei die definierten inode-Bloecke nicht einbezogen 
	 * werden.
	 */
	d = filsys.s_fsize-1;
	while(d%f_n)
		d++;
	for(; d > 0; d -= f_n)
	for(i=0; i<f_n; i++) {
		f = d - adr[i];
		if(f < filsys.s_fsize && f >= filsys.s_isize)
			if(badblk(f)) {
				if(ibc >= NINDIR) {
					printf("too many bad blocks\n");
					error = 1;
					ibc = 0;
				}
				ib[ibc] = f;
				ibc++;
			} else
				bfree(f);
	}
	/* Schreibe "bad-blocks"-inode auf Disk
	 */
	iput(&in, &ibc, ib);
}

/*---------------------------------------------------------------------
 * Wandle interne Form des inodes ip in Diskform und Eintragen
 * dieser in den zugeordeten inode-Block
 *---------------------------------------------------------------------
 */
iput(ip, aibc, ib)
register struct inode *ip;
register int *aibc;
daddr_t *ib;
{
	register struct dinode *dp;
	daddr_t d;
	register int i;

	filsys.s_tinode--;
	d = itod(ip->i_number);
	if(d >= filsys.s_isize) {
		if(error == 0)
			printf("ilist too small\n");
		error = 1;
		return;
	}
	rdfs(d, buf);
	dp = (struct dinode *)buf;
	dp += itoo(ip->i_number);

	dp->di_mode = ip->i_mode;
	dp->di_nlink = ip->i_nlink;
	dp->di_uid = ip->i_uid;
	dp->di_gid = ip->i_gid;
	dp->di_size = ip->i_size;
	dp->di_atime = utime;
	dp->di_mtime = utime;
	dp->di_ctime = utime;

	switch(ip->i_mode&IFMT) {

	/* Fuer regular files und directorys werden die max. 128
	 * Blocknummern entspr. der unix-Konvention ( 10 direkte
	 * Adressen, Rest in einem einfach indexierten Ankerblock
	 * im inode) in eine 3-Byte-Form gewandelt
	 */
	case IFDIR:
	case IFREG:
		for(i=0; i<*aibc; i++) {
			if(i >= LADDR)
				break;
			ip->i_addr[i] = ib[i];
		}
		if(*aibc >= LADDR) {
			ip->i_addr[LADDR] = alloc();
			for(i=0; i<NINDIR-LADDR; i++) {
				ib[i] = ib[i+LADDR];
				ib[i+LADDR] = (daddr_t)0;
			}
			wtfs(ip->i_addr[LADDR], (char *)ib);
		}

	case IFBLK:
	case IFCHR:
		ltol3(dp->di_addr, ip->i_addr, NADDR);
		break;

	default:
		printf("bad mode %o\n", ip->i_mode);
		exit(1);
	}
	wtfs(d, buf);
}

badblk(bno)
daddr_t bno;
{

	return(0);
}
