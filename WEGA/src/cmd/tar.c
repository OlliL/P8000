/* WEGA P8000 'tar' Version: 1.8.88 */

/* notes for migration


  +--------- pos in shipped tar
  |      +-- name here
  v      v
%81FA = BLOCKS
%81FC = Blocks
%81FE = disks
%4634 = nblock
%81EE = recno
%81E0 = dflag

%2868 = printf
*/



#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <signal.h>
#include <sgtty.h>

#define TBLOCK	512	/* Blocklaenge (512 Byte) */
#define NBLOCK	20	/* max. Blockung (512 * 20 Byte) */
#define NAMSIZ	100

union hblock {
    char dummy[TBLOCK];
    struct header {
	char name[NAMSIZ];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char mtime[12];
	char chksum[8];
	char linkflag;
	char linkname[NAMSIZ];
    } dbuf;
} dblock, tbuf[NBLOCK];

struct linkbuf {
    ino_t  inum;
    dev_t  devnum;
    int	   count;
    char   pathname[NAMSIZ];
    struct linkbuf *nextp;
} *ihead;

struct stat stbuf;

char	*sprintf();
char	*strcat();
char	*malloc();
daddr_t	bsrch();

int	rflag, xflag, vflag, tflag, dflag, cflag, mflag;
int	fd, term, chksum, wflag, recno, first, linkerrok;
int	archuid, rhead, timeout;
int	freemem = 1;
int	nblock = 1;	/* default: nur bei tardev */
int	BLOCKS, Blocks, disks = 1;
int	xbuf[TBLOCK];

daddr_t	low;
daddr_t	high;

FILE	*tfile;
char	tname[] = "/tmp/tarXXXXXX";

/* default: Disk.-Drive 1, cpi 96, double-sided, 9 sector */
/*          Typ 6 = rfd196ds9   (Blocklaenge : 512 Byte ) */
/*				(Blockanzahl : 1440 )     */

char	*usefile;
char	tardev[]	= "/dev/tardev";
char	devrfd1[]	= "/dev/rfd1";

main(argc, argv)
int	argc;
char	*argv[];
{
    char *cp;
    int onintr(), onquit(), onhup(), onterm();

    if (argc < 2)
	usage();

    tfile = NULL;
    usefile =  tardev;
    argv[argc] = 0;
    argv++;
    for (cp = *argv++; *cp; cp++) 
	switch(*cp) {
	case 'f':
	    usefile = *argv++;
   	    if (nblock == 1) nblock = 0;
	    break;
   	case 'c':
	    cflag++;
	    rflag++;
	    break;
	case 'u':
	    mktemp(tname);
	    if ((tfile = fopen(tname, "w")) == NULL) {
		fprintf(stderr, "tar: cannot create temporary file (%s)\n", tname);
		done(1); }
	    fprintf(tfile, "!!!!!/!/!/!/!/!/!/! 000\n");
	    /* kein 'break' !!! */
	case 'r':
	    rflag++;
	    if (nblock != 1 && cflag == 0) {
noupdate:
		fprintf(stderr, "tar: Blocked archiv cannot be updated (yet)\n");
		done(1); }
	    break;
	case 'v':
	    vflag++;
	    break;
	case 'd':		/* TEST */
	    dflag++;
	    break;
	case 'w':
	    wflag++;
	    break;
	case 'X':		/* Lese mit eigenen uid/gid's */
	    archuid++;
	case 'x':
	    xflag++;
	    break;
	case 't':
	    tflag++;
	    break;
	case 'm':
	    mflag++;
	    break;
	case '-':
	    break;
	case '0':
	case '1':
	    devrfd1[8] = *cp;    /* d.h. /dev/rfd0 oder 1 */
	    usefile = devrfd1;
	    break;
	case 'b':
	    nblock = atoi(*argv++);
	    if (nblock > NBLOCK || nblock <= 0) {
		fprintf(stderr, "Invalid blocksize (Max %d)\n", NBLOCK);
		done(1); }
	    if (rflag && !cflag)
		goto noupdate;
	    break;
	case 'l':
	    linkerrok++;
	    break;
	default:
	    fprintf(stderr, "tar: %c: unknown option\n", *cp);
	    usage();
	}

if (dflag) fprintf(stderr, "tar: Version 1.8.88\n");	/* TEST */

    if (rflag) {
	if (cflag && tfile != NULL) { usage(); done(1); }
	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
	    signal(SIGINT, onintr);
	if (signal(SIGHUP, SIG_IGN) != SIG_IGN)
	    signal(SIGHUP, onhup);
	if (signal(SIGQUIT, SIG_IGN) != SIG_IGN)
	    signal(SIGQUIT, onquit);
/* WOFUER ??? 
	if (signal(SIGTERM, SIG_IGN) != SIG_IGN)
	    signal(SIGTERM, onterm);
*/
	if (strcmp(usefile, "-") == 0) {
	    if (cflag == 0) {
		fprintf(stderr, "Can only create standard output archives\n");
		done(1); }
	    fd = dup(1);
	    nblock = 1; }
	else
	nonex1:
	if ((fd = open(usefile, 2)) < 0) {
	    if (cflag == 0 || (fd =  creat(usefile, 0666)) < 0) {
	    fprintf(stderr, "tar: cannot open %s\n", usefile);
	    if (strcmp(usefile,tardev) == 0) {
	        usefile = devrfd1;
	        fprintf(stderr, "tar: tries open %s\n", usefile);
	        goto nonex1; }
	        done(1); } }
	inBL();
	if (cflag == 0 && nblock == 0) nblock = 1;
	dorep(argv);
    }
    else if (xflag)  {
	if (dflag)
	    if (archuid) fprintf(stderr, "Take your own uid/gid\n");
	    else fprintf(stderr, "Get uid/gid from Archiv\n");

	if (strcmp(usefile, "-") == 0) {
	    fd = dup(0); nblock = 1; }
	else
	nonex2:
	if ((fd = open(usefile, 0)) < 0) {
	    fprintf(stderr, "tar: cannot open %s\n", usefile);
	    if (strcmp(usefile,tardev) == 0) {
	    usefile = devrfd1;
	    fprintf(stderr, "tar: tries open %s\n", usefile);
	    goto nonex2; }
	    done(1); }
	inBL();
	doxtract(argv);
    }
    else if (tflag) {
	if (strcmp(usefile, "-") == 0) {
	    fd = dup(0); nblock = 1; }
	else
	nonex3:
	if ((fd = open(usefile, 0)) < 0) {
	    fprintf(stderr, "tar: cannot open %s\n", usefile);
	    if (strcmp(usefile,tardev) == 0) {
	    usefile = devrfd1;
	    fprintf(stderr, "tar: tries open %s\n", usefile);
	    goto nonex3; }
	    done(1); }
	inBL();
	if(BLOCKS)nblock = 1;
	dotable(); }
    else    usage();
    done(0);
}

usage()
{
    fprintf(stderr, "tar: usage  tar [-]{crutxX}[Nbflmvw] [blocksize] [archivdev] [name ... ]\n");
    fprintf(stderr, "     c: create archiv\n");
    fprintf(stderr, "     r: write after archivend\n");
    fprintf(stderr, "     u: update archiv\n");
    fprintf(stderr, "     t: display archiv\n");
    fprintf(stderr, "     x: read archiv \(get UID GID from archiv\)\n");
    fprintf(stderr, "     X: read archiv \(get your own UID GID\)\n");
    fprintf(stderr, "     N=0,1: archiv device number \(drive\)\n");
    fprintf(stderr, "     b blocksize: verify archivblocksize\n");
    fprintf(stderr, "     f archivdev: verify archivdevice\n");
    fprintf(stderr, "     l: diagnostic links\n");
    fprintf(stderr, "     m: time of modification for files becomes actual systemetime\n");
    fprintf(stderr, "     v: display in verbosemode\n");
    fprintf(stderr, "     w: interactive archivwork \(x,X with rename\)\n");
    done(1); }

/* Hole BLOCKS ueber ioctl() */
inBL()
{
register i;

    i = ioctl(fd,SHOWFD,&BLOCKS);
if (dflag >= 2) fprintf(stderr, "ioctl: i = %d BLOCKS = %d\n",i,BLOCKS);
    if (i) BLOCKS = 0;
    if (BLOCKS && !nblock) nblock = 1;
if (dflag && BLOCKS) fprintf(stderr, "tar: disk capacity %d blocks\n", BLOCKS);
}

/* Lese von Platte, Schreibe auf Archiv */
dorep(argv)
char	*argv[];
{
    register char *cp, *cp2;
    char wdir[60];

if (dflag >= 2) fprintf(stderr, "dorep(argv)\n");	/* TEST */
    if (!cflag) {		/* vorheriges Archivlesen (Opt.: u,r) */
	gethead();
	do {
	    passarchiv();
	    if (term)
		done(0);
	    gethead();
	} while (!endarchiv());
	if (tfile != NULL) {
	    char buf[200];

	    strcat(buf, "sort +0 -1 +1nr ");
	    strcat(buf, tname);
	    strcat(buf, " -o ");
	    strcat(buf, tname);
	    sprintf(buf, "sort +0 -1 +1nr %s -o %s; awk '$1 != prev {print; prev=$1}' %s >%sX;mv %sX %s",
		tname, tname, tname, tname, tname, tname);
	    fflush(tfile);
	    system(buf);
	    freopen(tname, "r", tfile);
	    fstat(fileno(tfile), &stbuf);
	    high = stbuf.st_size;
	}
    }
    if (*argv == 0) {
	fprintf(stderr, "tar: no write-arguments\n");
	done(1);
    }
    getwdir(wdir);
    while (*argv && ! term) {
	cp2 = *argv;
	for (cp = *argv; *cp; cp++)
	    if (*cp == '/')
		cp2 = cp;
	if (cp2 != *argv) {
	    *cp2 = '\0';
	    chdir(*argv);
	    *cp2 = '/';
	    cp2++; }
	putfile(*argv++, cp2);
	chdir(wdir); }
    putempty();
if (dflag >= 2) fprintf(stderr, "getwd ruft diskend\n");
    diskend();			/* NEU */
    write(fd, tbuf, TBLOCK*nblock);	    /* ehemals flushtape(); */
    if (linkerrok == 1)
        for (; ihead != NULL; ihead = ihead->nextp)
	if (ihead->count != 0)
	    fprintf(stderr, "Missing links to %s\n", ihead->pathname);
    if (BLOCKS) printblocks(Blocks-nblock+recno+1);
}

printblocks(lastdskspace)
int lastdskspace;
{
	if (disks) {
	    fprintf(stderr, "\ntar: allocated space on %2d disks:  %4d blocks\n",
		disks+1, disks * BLOCKS + lastdskspace);
	    fprintf(stderr, "     allocated space on last disk: %4d blocks\n",
		lastdskspace);
	    fprintf(stderr, "       free    space on last disk: %4d from %4d blocks\n",
		BLOCKS-lastdskspace, BLOCKS);
	} else {
	    fprintf(stderr, "\ntar: allocated space on disk: %4d blocks\n", lastdskspace);
	    fprintf(stderr, "tar:   free    space on disk: %4d from %4d blocks\n",
		BLOCKS-lastdskspace, BLOCKS);
	}
}

endarchiv()
{
if (dflag >= 2) fprintf(stderr, "endarchiv\n");	/* TEST */
    if (dblock.dbuf.name[0] == '\0') {
	backarchiv();
	return(1);
    }
    else
	return(0);
}

/* Lese Head-Block vom Archiv und Werte seine Informationen aus */
gethead()
{
    register struct stat *sp;
    int i, j;

    j = 0;
readhead:
    rhead = 1;			/* an readarchiv: "Erwarte headblock" */
    readarchiv( (char *) &dblock);
    if (rhead > 1) goto readhead;	/* readarchiv-Fehler */
    for (i=0;i<TBLOCK && dblock.dummy[i] == '\0';i++) ;
    if (i==TBLOCK) return;
    sp = &stbuf;
    sscanf(dblock.dbuf.mode, "%o", &sp->st_mode);
    if (archuid) {		/* uid/gid vom tar-Benutzer nehmen */
	sp->st_uid = getuid();
	sp->st_gid = getgid(); }
    else {			/* uid/gid vom Archiv nehmen */
	sscanf(dblock.dbuf.uid, "%o", &sp->st_uid);
	sscanf(dblock.dbuf.gid, "%o", &sp->st_gid); }
    sscanf(dblock.dbuf.size, "%lo", &sp->st_size);
    sscanf(dblock.dbuf.mtime, "%lo", &sp->st_mtime);
    sscanf(dblock.dbuf.chksum, "%o", &chksum);
    if (chksum != checksum()) {
	fprintf(stderr, "headblock checksum error\n");
	if (dflag >= 2) new1();
	if (j) goto readhead;
	else {
	    fprintf(stderr, "test next blocks ? (y/n) : ");
	    if (response() == 'y') { j++; goto readhead; } }
	done(2); }
    if (tfile != NULL)
	fprintf(tfile, "%s %s\n", dblock.dbuf.name, dblock.dbuf.mtime);
    rhead = 0;
}

/* Archiv-Durchlauf zum Lesen der Header-Informationen */
/* FIXME: ADR in shipped version: L0t_08e2 */
passarchiv()
{
    long blocks;
    char buf[TBLOCK];

if (dflag >= 2) fprintf(stderr, "passarchiv\n");	/* TEST */
    if (dblock.dbuf.linkflag == '1')
	return;
    blocks = (stbuf.st_size + (TBLOCK-1)) / TBLOCK;
    if (BLOCKS && nblock == 1) {
	Blocks += blocks;
	while (Blocks >= BLOCKS) {
if (dflag >= 2) fprintf(stderr, "passarchiv: Blocks = Blocks + blocks (%d = %D + %D)\n",
	        Blocks, Blocks-blocks, blocks);
	    blocks = Blocks;
if (dflag >= 2) fprintf(stderr, "passarchiv ruft diskend\n");
	    diskend();
	    blocks -= BLOCKS;
	    Blocks = blocks; }
if (dflag >= 2) fprintf(stderr, "passarchiv: Blocks = %d : ", Blocks);
if (dflag >= 2) fprintf(stderr, "lseek mit blocks = %D\n", blocks);
	lseek(fd, (long)TBLOCK*blocks, 1);
	blocks = 0; }
    else while (blocks-- > 0)
	readarchiv(buf);
}

/* Schreibe Datei auf Archiv */
/* FIXME: ADR in shipped version: L0t_0a06 */
putfile(longname, shortname)
char *longname;
char *shortname;
{
    int infile;
    long blocks;
    char buf[TBLOCK];
    register char *cp, *cp2;
    struct direct dbuf;
    int i, j;

    infile = open(shortname, 0);
    if (infile < 0) {
	fprintf(stderr, "tar: %s: cannot open file\n", longname);
	return; }

    fstat(infile, &stbuf);

    if (tfile != NULL && checkupdate(longname) == 0) {
	close(infile);
	return; }
    if (checkw('r', longname) == 0) {
	close(infile);
	return; }

    if ((stbuf.st_mode & S_IFMT) == S_IFDIR) {		/* = Directory */
	for (i = 0, cp = buf; *cp++ = longname[i++];);
	*--cp = '/';
	cp++;
	i = 0;
	chdir(shortname);
	while (read(infile, (char *)&dbuf, sizeof(dbuf)) > 0 && !term) {
	    if (dbuf.d_ino == 0) {
		i++; continue; }
	    if (strcmp(".", dbuf.d_name) == 0 ||
		strcmp("..", dbuf.d_name) == 0)  {
		i++; continue; }
	    cp2 = cp;
	    for (j=0; j < DIRSIZ; j++)
		*cp2++ = dbuf.d_name[j];
	    *cp2 = '\0';
	    close(infile);
	    putfile(buf, cp);		/* rekursiv */
	    infile = open(".", 0);
	    i++;
	    lseek(infile, (long) (sizeof(dbuf) * i), 0);
	}
	close(infile);
	chdir("..");
	return;
    }
    if ((stbuf.st_mode & S_IFMT) != S_IFREG) {
	fprintf(stderr, "tar: %s is not a file. Not archived\n", longname);
	return;
    }

    tomodes(&stbuf);

    cp2 = longname;
    for (cp = dblock.dbuf.name, i=0; (*cp++ = *cp2++) && i < NAMSIZ; i++);
    if (i >= NAMSIZ) {
	fprintf(stderr, "%s: file name too long\n", longname);
	close(infile);
	return;
    }

    if (stbuf.st_nlink > 1) {
	struct linkbuf *lp;
	int found = 0;

	for (lp = ihead; lp != NULL; lp = lp->nextp) {
	    if (lp->inum == stbuf.st_ino && lp->devnum == stbuf.st_dev) {
		found++;
		break;
	    }
	}
	if (found) {
	    strcpy(dblock.dbuf.linkname, lp->pathname);
	    dblock.dbuf.linkflag = '1';
	    sprintf(dblock.dbuf.chksum, "%6o", checksum());
	    writearchiv( (char *) &dblock);
	    if (vflag) {
		fprintf(stderr, "a %s ", longname);
		fprintf(stderr, "link to %s\n", lp->pathname);
	    }
	    lp->count--;
	    close(infile);
	    return;
	}
	else {
	    lp = (struct linkbuf *) malloc(sizeof(*lp));
	    if (lp == NULL) {
		if (freemem) {
		    fprintf(stderr, "Out of memory. Link information lost\n");
		    freemem = 0;
		}
	    }
	    else {
		lp->nextp = ihead;
		ihead = lp;
		lp->inum = stbuf.st_ino;
		lp->devnum = stbuf.st_dev;
		lp->count = stbuf.st_nlink - 1;
		strcpy(lp->pathname, longname);
	    }
	}
    }

    blocks = (stbuf.st_size + (TBLOCK-1)) / TBLOCK;
    if (vflag) {
	fprintf(stderr, "a %s ", longname);
	fprintf(stderr, "%ld blocks\n", blocks);
    }
    sprintf(dblock.dbuf.chksum, "%6o", checksum());
    writearchiv( (char *) &dblock);

    while ((i = read(infile, buf, TBLOCK)) > 0 && blocks > 0) {
	writearchiv(buf);
	blocks--;
    }
    close(infile);
    if (blocks != 0 || i != 0)
	fprintf(stderr, "%s: file changed size\n", longname);
    while (blocks-- >  0)
	putempty();
}

/* Lese von Archiv, Schreibe auf Platte */
/* FIXME: ADR in shipped version: L0t_0dd8 */
doxtract(argv)
char	*argv[];
{
    long blocks, bytes;
    char buf[TBLOCK];
    char **cp;
    int ofile, i;

    for (;;) {
	gethead();
	if (endarchiv())
	    break;

	if (*argv == 0)
	    goto gotit;

	for (cp = argv, i = 0; *cp; cp++, i++)
	    if (prefix(*cp, dblock.dbuf.name))
		goto vgotit;
	passarchiv();
	continue;

vgotit:
	xbuf[i]++;
gotit:
	if (checkw('x', dblock.dbuf.name) == 0) {
	    passarchiv();
	    continue;
	}

	checkdir(dblock.dbuf.name);

	if (dblock.dbuf.linkflag == '1') {
	    unlink(dblock.dbuf.name);
	    if (link(dblock.dbuf.linkname, dblock.dbuf.name) < 0) {
		fprintf(stderr, "%s: cannot link\n", dblock.dbuf.name);
		continue;
	    }
	    if (vflag)
		fprintf(stderr, "%s linked to %s\n", dblock.dbuf.name, dblock.dbuf.linkname);
	    continue;
	}
	if ((ofile = creat(dblock.dbuf.name, stbuf.st_mode & 07777)) < 0) {
	    fprintf(stderr, "tar: %s - cannot create\n", dblock.dbuf.name);
	    passarchiv();
	    continue;
	}

	chown(dblock.dbuf.name, stbuf.st_uid, stbuf.st_gid);

	blocks = ((bytes = stbuf.st_size) + TBLOCK-1)/TBLOCK;
	if (vflag)
	    fprintf(stderr, "x %s, %ld bytes, %ld archiv blocks\n", dblock.dbuf.name, bytes, blocks);
	while (blocks-- > 0) {
	    readarchiv(buf);
	    if (bytes > TBLOCK) {
		if (write(ofile, buf, TBLOCK) < 0) {
		    fprintf(stderr, "tar: %s: HELP1 - extract write error\n", dblock.dbuf.name);
		    done(2);
		}
	    } else
		if (write(ofile, buf, (int) bytes) < 0) {
		    fprintf(stderr, "tar: %s: HELP2 - extract write error\n", dblock.dbuf.name);
		    done(2);
		}
	    bytes -= TBLOCK;
	}
	close(ofile);
	if (mflag == 0) {
	    time_t timep[2];

	    timep[0] = time(NULL);
	    timep[1] = stbuf.st_mtime;
	    utime(dblock.dbuf.name, timep); }
    }
    if (*argv) {
	for (cp = argv, i = 0; *cp; cp++, i++)
	    if (!xbuf[i])
		fprintf(stderr, "tar: %s not in archiv\n", *cp); }
}

/* Lese von Archiv (Realisierung t-Option) */
/* FIXME: ADR in shipped version: L0t_1036 */
dotable()
{
    for (;;) {
	gethead();
	if (endarchiv()) break;
	if (vflag) longt(&stbuf);
	printf("%s", dblock.dbuf.name);
	if (dblock.dbuf.linkflag == '1')
	    printf(" linked to %s", dblock.dbuf.linkname);
	printf("\n");
	passarchiv();
    }
}

/* FIXME: ADR in shipped version: L0t_1078 */
putempty()
{
    char buf[TBLOCK];
    char *cp;

if (dflag >= 2) fprintf(stderr, "putempty\n");	/* TEST */
    for (cp = buf; cp < &buf[TBLOCK]; )
	*cp++ = '\0';
    writearchiv(buf);
}

/* v-Option fuer Terminalanzeige von: mode, uid, gid, size, ctime */
/* FIXME: ADR in shipped version: L0t_10ba */
longt(st)
register struct stat *st;
{
    register char *cp;
    char *ctime();

    pmode(st);
    printf("%3d/%1d", st->st_uid, st->st_gid);
    printf("%7D", st->st_size);
    cp = ctime(&st->st_mtime);
    printf(" %-12.12s %-4.4s ", cp+4, cp+20);
}

#define	SUID	04000
#define	SGID	02000
#define	ROWN	0400
#define	WOWN	0200
#define	XOWN	0100
#define	RGRP	040
#define	WGRP	020
#define	XGRP	010
#define	ROTH	04
#define	WOTH	02
#define	XOTH	01
#define	STXT	01000
int	m1[] = { 1, ROWN, 'r', '-' };
int	m2[] = { 1, WOWN, 'w', '-' };
int	m3[] = { 2, SUID, 's', XOWN, 'x', '-' };
int	m4[] = { 1, RGRP, 'r', '-' };
int	m5[] = { 1, WGRP, 'w', '-' };
int	m6[] = { 2, SGID, 's', XGRP, 'x', '-' };
int	m7[] = { 1, ROTH, 'r', '-' };
int	m8[] = { 1, WOTH, 'w', '-' };
int	m9[] = { 2, STXT, 't', XOTH, 'x', '-' };

int	*m[] = { m1, m2, m3, m4, m5, m6, m7, m8, m9};

/* FIXME: ADR in shipped version: L0t_1106 */
pmode(st)
register struct stat *st;
{
    register int **mp;

    for (mp = &m[0]; mp < &m[9];)
	select(*mp++, st);
}

/* FIXME: ADR in shipped version: L0t_1132 */
select(pairp, st)
int *pairp;
struct stat *st;
{
    register int n, *ap;

    ap = pairp;
    n = *ap++;
    while (--n>=0 && (st->st_mode&*ap++)==0)
	ap++;
    printf("%c", *ap);
}

/* FIXME: ADR in shipped version: L0t_1174 */
checkdir(name)
register char *name;
{
    register char *cp;
    int i;
    for (cp = name; *cp; cp++) {
	if (*cp == '/') {
	    *cp = '\0';
	    if (access(name, 01) < 0) {
		if (fork() == 0) {
		    execl("/bin/mkdir", "mkdir", name, 0);
		    execl("/usr/bin/mkdir", "mkdir", name, 0);
		    fprintf(stderr, "tar: cannot find mkdir!\n");
		    done(0);
		}
		while (wait(&i) >= 0);
		chown(name, stbuf.st_uid, stbuf.st_gid);
	    }
	    *cp = '/';
	}
    }
}

onintr()
{
    signal(SIGINT, SIG_IGN);
    term++;
}

onquit()
{
    signal(SIGQUIT, SIG_IGN);
    term++;
}

onhup()
{
    signal(SIGHUP, SIG_IGN);
    term++;
}

onterm()
{
    signal(SIGTERM, SIG_IGN);
    term++;
}

/* FIXME: ADR in shipped version: L0t_1238 */
tomodes(sp)
register struct stat *sp;
{
    register char *cp;

    for (cp = dblock.dummy; cp < &dblock.dummy[TBLOCK]; cp++)
	*cp = '\0';
    sprintf(dblock.dbuf.mode, "%6o ", sp->st_mode & 07777);
    sprintf(dblock.dbuf.uid, "%6o ", sp->st_uid);
    sprintf(dblock.dbuf.gid, "%6o ", sp->st_gid);
    sprintf(dblock.dbuf.size, "%11lo ", sp->st_size);
    sprintf(dblock.dbuf.mtime, "%11lo ", sp->st_mtime);
}

/* FIXME: ADR in shipped version: L0t_12b0 */
checksum()
{
    register i;
    register char *cp;

    for (cp = dblock.dbuf.chksum; cp < &dblock.dbuf.chksum[sizeof(dblock.dbuf.chksum)]; cp++)
	*cp = ' ';
    i = 0;
    for (cp = dblock.dummy; cp < &dblock.dummy[TBLOCK]; cp++)
	i += *cp;
if (dflag >= 2) fprintf(stderr, "checksum = %o\n",i);	/* TEST */
    return(i);
}

/* FIXME: ADR in shipped version: L0t_1302 */
new1()
{
	int i1,i2;
	if(dflag)return(0);
	if(dflag)return(0);
	if(dflag)return(0);
	if(dflag)return(0);
	if(dflag)return(0);
	if(dflag)return(0);
	if(dflag)return(0);
	if(dflag)return(0);
	if(dflag)return(0);
	if(dflag)return(0);
	if(dflag)return(0);
	if(dflag)return(0);
	if(dflag)return(0);
	if(dflag)return(0);
	if(dflag)return(0);
	if(dflag)return(0);
	if(dflag)return(0);
	if(dflag)return(0);
	if(dflag+dflag)return(0);
	if(dflag+dflag)return(0);
	if(dflag+dflag)return(0);
/* FIXME ASM Code
L0t_1302:
	dec	r15,#6
	ldm	@r15,r12,#3
	ld	r12,#%584C
	jr	L0t_1314
L0t_130e:
	ldb	@r12,#%20
	inc	r12,#1
L0t_1314:
	cp	r12,#%5854
	jr	c,L0t_130e
	ldk	r13,#0
	ld	r14,r13
	ld	r12,#%57B8
	jr	L0t_136c
L0t_1324:
	ldb	rl2,@r12
	extsb	r2
	add	r14,r2
	inc	r13,#1
	ldb	rl5,@r12
	extsb	r5
	and	r5,#%00FF
	ld	r6,#%500A
	ld	r7,#%5520
	call	%2868
	cpb	@r12,#%1F
	jr	le,L0t_135e
	cpb	@r12,#%7F
	jr	ge,L0t_135e
	ldb	rl5,@r12
	extsb	r5
	and	r5,#%00FF
	ld	r6,#%5010
	ld	r7,#%5520
	jr	L0t_1366
L0t_135e:
	ld	r7,#%5520
	ld	r6,#%5016
L0t_1366:
	call	%2868
	inc	r12,#1
L0t_136c:
	cp	r12,#%59B8
	jr	c,L0t_1324
	ld	r7,#%5520
	ld	r6,#%501A
	ld	r5,%81EA
	ld	r4,r14
	call	%2868
	ldm	r12,@r15,#3
	inc	r15,#6
	ret
*/
}
/* FIXME: ADR in shipped version: L0t_138c */
checkw(c, name)
char *name;
{
char estring[100];
register i;

    if (wflag) {
	printf("%c ", c);
	if (vflag) longt(&stbuf);
	printf("%s: ", name);
	gets(estring);
	if (estring[0] == 'y') {
	    if (c == 'x' && estring[1] == ' ' && strlen(estring) > 2) {
		for (i=1; estring[i] == ' '; i++) ;
		for ( ; estring[i] != '\0'; i++) *name++ = estring[i];
		*name = '\0'; }
	    return(1); }
	else return(0); }
    return(1);
}

/* FIXME: ADR in shipped version: L0t_1438 */
response()
{
    char c;

    c = getchar();
    if (c != '\n')
	while (getchar() != '\n');
    else c = 'n';
    return(c);
}

/* FIXME: ADR in shipped version: L0t_14a4 */
checkupdate(arg)
char	*arg;
{
    char name[100];
    long	mtime;
    daddr_t seekp;
    daddr_t	lookup();

    rewind(tfile);
    for (;;) {
	if ((seekp = lookup(arg)) < 0)
	    return(1);
	fseek(tfile, seekp, 0);
	fscanf(tfile, "%s %lo", name, &mtime);
	if (stbuf.st_mtime > mtime)
	    return(1);
	else
	    return(0);
    }
}

/* FIXME: ADR in shipped version: L0t_14fc */
done(n)
{
    unlink(tname);
    exit(n);
}

prefix(s1, s2)
register char *s1, *s2;
{
    while (*s1)
	if (*s1++ != *s2++)
	    return(0);
    if (*s2)
	return(*s2 == '/');
    return(1);
}

getwdir(s)
char *s;
{
    int	pipdes[2];

    pipe(pipdes);
    if (fork() == 0) {
	close(1);
	dup(pipdes[1]);
	execl("/bin/pwd", "pwd", 0);
	execl("/usr/bin/pwd", "pwd", 0);
	fprintf(stderr, "pwd failed!\n");
	printf("/\n");
	exit(1);
    }
    while (wait((int *)NULL) != -1)
	;
    read(pipdes[0], s, 50);
    while(*s != '\n')
	s++;
    *s = '\0';
    close(pipdes[0]);
    close(pipdes[1]);
}

#define	N	200
int	njab;
daddr_t
lookup(s)
char *s;
{
    register i;
    daddr_t a;

    for(i=0; s[i]; i++)
	if(s[i] == ' ')
	    break;
    a = bsrch(s, i, low, high);
    return(a);
}

daddr_t
bsrch(s, n, l, h)
daddr_t l, h;
char *s;
{
    register i, j;
    char b[N];
    daddr_t m, m1;

    njab = 0;

loop:
    if(l >= h)
	return(-1L);
    m = l + (h-l)/2 - N/2;
    if(m < l)
	m = l;
    fseek(tfile, m, 0);
    fread(b, 1, N, tfile);
    njab++;
    for(i=0; i<N; i++) {
	if(b[i] == '\n')
	    break;
	m++;
    }
    if(m >= h)
	return(-1L);
    m1 = m;
    j = i;
    for(i++; i<N; i++) {
	m1++;
	if(b[i] == '\n')
	    break;
    }
    i = cmp(b+j, s, n);
    if(i < 0) {
	h = m;
	goto loop;
    }
    if(i > 0) {
	l = m1;
	goto loop;
    }
    return(m);
}

cmp(b, s, n)
char *b, *s;
{
    register i;

    if(b[0] != '\n')
	exit(2);
    for(i=0; i<n; i++) {
	if(b[i+1] > s[i])
	    return(-1);
	if(b[i+1] < s[i])
	    return(1);
    }
    return(b[i+1] == ' '? 0 : -1);
}

readarchiv(buffer)
char *buffer;
{
    int i, j;

    if (recno >= nblock || first == 0) {
	if (first == 0 && nblock == 0) j = NBLOCK;
	else j = nblock;
if (dflag >= 2) fprintf(stderr, "readarchiv ruft diskend\n");
	diskend();			/* NEU */
	if ((i = read(fd, tbuf, TBLOCK*j)) < 0) {
	    fprintf(stderr, "tar: archiv read error\n");
	    if (rhead) { rhead++;	/* Fehlermeldung an 'gethead' */
		fprintf(stderr, "     in head-block\n"); }
	    if (!tflag)
		fprintf(stderr, "tar: file %s is not complete\n", dblock.dbuf.name);
	    fprintf(stderr, "     continue (y/n) : ");
	    if (response() != 'y') done(3);
	    lseek(fd, (long)(TBLOCK*j), 1); }		/*? *j ?*/
	if (first == 0) {
	    if (i % TBLOCK) {
		fprintf(stderr, "tar: archiv blocksize error\n");
		done(3);
	    }
	    i /= TBLOCK;
	    if (rflag && i != 1) {
		fprintf(stderr, "tar: Cannot update blocked archiv (yet)\n");
		done(4);
	    }
	    if (i != nblock && i != 1) {
if (dflag >= 2) fprintf(stderr, "i = %d, nblock = %d\n",i, nblock);
		fprintf(stderr, "tar: blocksize = %d\n", i);
		nblock = i;
	    }
	}
	recno = 0;
    }
    first = 1;
    copy(buffer, &tbuf[recno++]);
    return(TBLOCK);
}

writearchiv(buffer)
char *buffer;
{
    first = 1;
    if (nblock == 0)
	nblock = 1;
if (dflag >= 2) fprintf(stderr, "writearchiv: recno=%d nblock=%d\n",recno, nblock);
    if (recno >= nblock) {
if (dflag >= 2) fprintf(stderr, "writearchiv ruft diskend (1)\n");
	diskend();			/* NEU */
/* Hier muesste Test auf Diskettenformat rein, inkl. Format-Moeglichkeit */
	if (write(fd, tbuf, TBLOCK*nblock) < 0) {
	    fprintf(stderr, "tar: archiv write error\n");
	    done(2);
	}
	recno = 0;
    }
    copy(&tbuf[recno++], buffer);
    if (recno >= nblock) {
if (dflag >= 2) fprintf(stderr, "writearchiv ruft diskend (2)\n");
	diskend();			/* NEU */
	if (write(fd, tbuf, TBLOCK*nblock) < 0) {
	    fprintf(stderr, "tar: archiv write error\n");
	    done(2);
	}
	recno = 0;
    }
    return(TBLOCK);
}

/* Wird nur beim Archiv-Lesen aufgerufen (Optionen: r,u,x,t) */
backarchiv()
{
    Blocks -= nblock;
if (dflag >= 2) {					/* TEST */
    fprintf(stderr, "backarchiv\n");
    fprintf(stderr, "lseek: Blocks := Blocks-nblock = %d\n", Blocks); }

    lseek(fd, (long)(-TBLOCK*nblock), 1);
    if (recno >= nblock) {
	recno = nblock - 1;
if (dflag >= 2) fprintf(stderr, "backarchiv ruft diskend\n");
	diskend();			/* NEU */
	if (read(fd, tbuf, TBLOCK*nblock) < 0) {
	    fprintf(stderr, "tar: archiv read error after seek\n");
	    done(4);
	}
	Blocks -= nblock;
if (dflag >= 2)					/* TEST */
    fprintf(stderr, "lseek: Blocks := Blocks-nblock = %d\n", Blocks);
if (BLOCKS && tflag) {
    printf("\ntar: allocated space on disk %4d blocks with 512 bytes\n",
					    Blocks+3);
    printf("tar:   free    space on disk %4d blocks from %4d blocks\n",
					BLOCKS-Blocks-3, BLOCKS); }
	lseek(fd, (long)(-TBLOCK*nblock), 1);
    }
}

copy(to, from)
register char *to, *from;
{
    register i;

    i = TBLOCK;
    do {
	*to++ = *from++;
    } while (--i);
}

diskend()
{
#define SEC 2
int	time_out();

    if (BLOCKS == 0) return;
if (dflag >= 2) fprintf(stderr, "diskend: Blocks = %d\n", Blocks);	/* TEST */
    if (++Blocks == 0) {
	close(fd);
	fprintf(stderr, "\ninsert previous disk : ");
	if (disks > 1) disks--;
	Blocks = 1;	/* da 'diskend' mit ++Blocks beginnt */
	while (getchar() != '\n') ;
	if ((fd = open(usefile, 2)) < 0) {
	    if (cflag == 0 || (fd =  creat(usefile, 0666)) < 0) {
	        fprintf(stderr, "tar: cannot open %s\n", usefile);
	        done(1); } } }
    if (Blocks > BLOCKS) {
	close(fd);
	timeout = 0;
	signal(SIGALRM,time_out);
	alarm(SEC);
	while (!timeout) getchar();
	alarm(0);
	fprintf(stderr, "\ninsert next disk : ");
	disks++;
	Blocks = 1;
	while (getchar() != '\n') ;
	if ((fd = open(usefile, 2)) < 0) {
	    if (cflag == 0 || (fd =  creat(usefile, 0666)) < 0) {
	        fprintf(stderr, "tar: cannot open %s\n", usefile);
	        done(1); } } }
}

time_out()
{
    signal(SIGALRM,time_out);
    timeout++;
}
