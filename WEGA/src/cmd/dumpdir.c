/* WEGA P8000 'dumpdir' 1.Version (6.11.87) */
#define MAXINO	3000
#define BITS	8
#define MAXXTR	60
#define NCACHE	3

#include <sgtty.h>
#include <stdio.h>
#include <sys/param.h>
#include <sys/inode.h>
#include <sys/ino.h>
#include <sys/fblk.h>
#include <sys/filsys.h>
#include <sys/dir.h>
#include <dumprestor.h>

#define	MWORD(m,i) (m[(unsigned)(i-1)/MLEN])
#define	MBIT(i)	(1<<((unsigned)(i-1)%MLEN))
#define	BIS(i,w)	(MWORD(w,i) |=  MBIT(i))
#define	BIC(i,w)	(MWORD(w,i) &= ~MBIT(i))
#define	BIT(i,w)	(MWORD(w,i) & MBIT(i))

int	fd;
char	archiv[]  = "/dev/dumpdev";
char	devrfd1[] = "/dev/rfd1";
char	*readfrom = archiv;

daddr_t	seekpt;
int	ofile;
FILE	*df;
char	dirfile[] = "rstXXXXXX";

struct {
	ino_t	t_ino;
	daddr_t	t_seekpt;
		} inotab[MAXINO];
int	ipos;

#define ONTAPE	1
#define XTRACTD	2
#define XINUSE	4

short	dumpmap[MSIZ];		/* MSIZ = 4096 */
short	clrimap[MSIZ];


int	bct = NTREC+1;
char	tbf[NTREC*BSIZE];

char	prebuf[512];

int	dflag;
int	BLOCKS, nblock;
int	volno = 1;

main(argc, argv)
char *argv[];
{
    register char *cp;
    extern char *ctime();

    mktemp(dirfile);
    argv++;
    if (argc >= 2) {
    argc -= 2;
    for (cp = *argv++; *cp; cp++) {
	switch (*cp) {
	case 'd':
	    dflag++;
	    break;
	case 'f':
	    readfrom = *argv++;
	    argc--;
	    break;
	default:
	    printf("bad key '%c%'\n", *cp);
	    usage(); } } }

if (dflag) printf("dumpdir: Version 6.11.87\n");

    nonex:
    if ((fd = open(readfrom, 0)) < 0) {
	printf("dumpdir: cannot open %s\n", readfrom);
	if (strcmp(readfrom,archiv) == 0) {
	    readfrom = devrfd1;
	    printf("dumpdir: tries open %s\n", readfrom);
	    goto nonex; }
	exit(1); }
    inBL();

    if (readhdr(&spcl) == 0) {
	printf("Not a dump-archiv\n");
	exit(1); }

    df = fopen(dirfile, "w");
    if (df == NULL) {
	printf("dumpdir: %s - cannot create directory temporary\n", dirfile);
	exit(1); }

    printf("Dump   date: %s", ctime(&spcl.c_date));
    printf("Dumped from: %s", ctime(&spcl.c_ddate));
    if (checkvol(&spcl, 1) == 0) {
	printf("Tape is not volume 1 of the dump\n");
	exit(1); }
    pass1();  /* This sets the various maps on the way by */
    freopen(dirfile, "r", df);
    strcpy(prebuf, "/");
    printem(prebuf, (ino_t) 2);
    unlink(dirfile);
    exit(0);
}

usage()
{
    printf("dumpdir: usage dumpdir [f archivdev]\n");
    exit(1);
}

/* Hole BLOCKS ueber ioctl() */
inBL()
{
register i;
    if (i=ioctl(fd,SHOWFD,&BLOCKS)) BLOCKS = 0;
if (dflag && BLOCKS) fprintf(stderr, "tar: disk capacity %d blocks\n", BLOCKS);
}

/*
 * Read the archiv, bulding up a directory structure for extraction
 * by name
 */
pass1()
{
    register	  i;
    struct dinode *ip;
    int		  putdir(), null();

    while (gethead(&spcl) == 0) {
	printf("Can't find directory header!\n"); }
    for (;;) {
	if (checktype(&spcl, TS_BITS) == 1) {
	    readbits(dumpmap);
	    continue; }
	if (checktype(&spcl, TS_CLRI) == 1) {
	    readbits(clrimap);
	    continue; }
	if (checktype(&spcl, TS_INODE) == 0) {
	finish:
	    fflush(df);
	    close(fd);
	    return; }
	ip = &spcl.c_dinode;
	i = ip->di_mode & IFMT;
	if (i != IFDIR) goto finish;
	inotab[ipos].t_ino = spcl.c_inumber;
	inotab[ipos++].t_seekpt = seekpt;
	getfile(spcl.c_inumber, putdir, null, spcl.c_dinode.di_size);
	putent("\000\000/");
    }
}

printem(prefix, inum)
char *prefix;
ino_t	inum;
{
    struct direct dir;
    register int i;

    for (i = 0; i < MAXINO; i++)
	if (inotab[i].t_ino == inum) goto found;
    printf("PANIC - can't find directory %d\n", inum);
    return;
    found:
    mseek(inotab[i].t_seekpt);
    for (;;) {
	getent((char *) &dir);
	if (direq(dir.d_name, "/"))
	    return;
	if (search(dir.d_ino) != 0 &&
	    direq(dir.d_name, ".") == 0 &&
	    direq(dir.d_name, "..") == 0) {
	    int len;
	    FILE *tdf;

	    tdf = df;
	    df = fopen(dirfile, "r");
	    len = strlen(prefix);
	    strncat(prefix, dir.d_name, sizeof(dir.d_name));
	    strcat(prefix, "/");
	    printem(prefix, dir.d_ino);
	    prefix[len] = '\0';
	    fclose(df);
	    df = tdf;
	}
	else
	    if (BIT(dir.d_ino, dumpmap))
		printf("%5d	%s%-.14s\n", dir.d_ino, prefix, dir.d_name);
    }
}
/*
 * Do the file extraction, calling the supplied functions
 * with the blocks
 */
getfile(n, f1, f2, size)
ino_t	n;
int	(*f2)(), (*f1)();
long	size;
{
    register i;
    struct spcl addrbl;
    char buf[BSIZE];

    addrbl = spcl;
    goto start;
    for (;;) {
	if (gethead(&addrbl) == 0) {
	    printf("Missing address (header) block\n");
	    goto eloop; }
	if (checktype(&addrbl, TS_ADDR) == 0) {
	    spcl = addrbl;
	    return; }
	start:
	for (i = 0; i < addrbl.c_count; i++) {
	    if (addrbl.c_block.c_addr[i]) {
		readarchiv(buf);
		(*f1)(buf, size > BSIZE ? (long) BSIZE : size); }
	    else {
		clearbuf(buf);
		(*f2)(buf, size > BSIZE ? (long) BSIZE : size); }
	    if ((size -= BSIZE) <= 0) {
		eloop:
		while (gethead(&spcl) == 0) ;
		if (checktype(&spcl, TS_ADDR) == 1)
		    goto eloop;
		return; }
	}
    }
}

/*
 * Do the archiv i\/o, dealling with volume changes
 * etc..
 */
readarchiv(b)
char *b;
{
    register i;
    struct spcl tmpbuf;

    if (bct >= NTREC) {
	for (i = 0; i < NTREC; i++)
	    ((struct spcl *)&tbf[i*BSIZE])->c_magic = 0;
	bct = 0;
	if (BLOCKS && nblock < BLOCKS)
	if ((i = read(fd, tbf, NTREC*BSIZE)) < 0) exit(1);
if (dflag > 1 && BLOCKS) printf("Ich las Block %d bis %d\n",nblock*NTREC,(nblock+1)*NTREC-1);
	nblock++;
	if (i == 0 || (BLOCKS && nblock > BLOCKS)) {
	    bct = NTREC+1;
	    volno++;
    loop:
	    bct = NTREC+1;	/* ehemals: flsht(); */
	    close(fd);
	    nblock = 0;
	    if (BLOCKS) printf("insert disk %d : ",volno);
	    else	printf("change archiv %d : ", volno);
	    while (getchar() != '\n') ;
	    if ((fd = open(readfrom, 0)) == -1) 
		printf("Cannot open archiv!\n");
	    if (readhdr(&tmpbuf) == 0) {
		printf("Not a dump-archiv. Try again\n");
		goto loop; }
	    if (checkvol(&tmpbuf, volno) == 0) {
		printf("Wrong archiv. Try again\n");
		goto loop; }
	    readarchiv(b);
	    return; }
    }
    copy(&tbf[(bct++*BSIZE)], b, BSIZE);
}

copy(f, t, s)
register char *f, *t;
{
    register i;

    i = s;
    do *t++ = *f++; while (--i);
}

clearbuf(cp)
register char *cp;
{
    register i;

    i = BSIZE;
    do *cp++ = 0; while (--i);
}

/*
 * Put and get the directory entries from the compressed
 * directory file
 */
putent(cp)
char	*cp;
{
    register i;

    for (i = 0; i < sizeof(ino_t); i++)
	writec(*cp++);
    for (i = 0; i < DIRSIZ; i++) {
	writec(*cp);
	if (*cp++ == 0) return; }
}

getent(bf)
register char *bf;
{
    register i;

    for (i = 0; i < sizeof(ino_t); i++)
	*bf++ = readc();
    for (i = 0; i < DIRSIZ; i++)
	if ((*bf++ = readc()) == 0)
	    return;
}

/*
 * read/write te directory file
 */
writec(c)
char c;
{
    seekpt++;
    fwrite(&c, 1, 1, df);
}

readc()
{
    char c;

    fread(&c, 1, 1, df);
    return(c);
}

mseek(pt)
daddr_t pt;
{
    fseek(df, pt, 0);
}

/*
 * search the directory inode ino
 * looking for entry cp
 */
search(inum)
ino_t	inum;
{
    register low, high, probe;

    low = 0;
    high = ipos-1;

    while (low != high) {
	probe = (high - low + 1)/2 + low;
/*
printf("low = %d, high = %d, probe = %d, ino = %d, inum = %d\n", low, high, probe, inum, inotab[probe].t_ino);
*/
	if (inum >= inotab[probe].t_ino)
	    low = probe;
	else
	    high = probe - 1; }
    return(inum == inotab[low].t_ino);
}

direq(s1, s2)
register char *s1, *s2;
{
    register i;

    for (i = 0; i < DIRSIZ; i++)
	if (*s1++ == *s2) {
	    if (*s2++ == 0)
		return(1); }
	else return(0);
    return(1);
}

/*
 * read the archiv into buf, then return whether or
 * or not it is a header block.
 */
gethead(buf)
struct spcl *buf;
{
    readarchiv((char *)buf);
    if (buf->c_magic != MAGIC || checksum((int *) buf) == 0)
	return(0);
    return(1);
}

/*
 * return whether or not the buffer contains a header block
 */
checktype(b, t)
struct	spcl *b;
int	t;
{
    return(b->c_type == t);
}


checksum(b)
int *b;
{
    register i, j;

    j = BSIZE/sizeof(int);
    i = 0;
    do i += *b++; while (--j);
    if (i != CHECKSUM) {
	printf("Checksum error %o\n", i);
	return(0); }
    return(1);
}

checkvol(b, t)
struct spcl *b;
int t;
{
    if (b->c_volume == t) return(1);
    return(0);
}

readhdr(b)
struct	spcl *b;
{
    if (gethead(b) == 0) return(0);
    if (checktype(b, TS_TAPE) == 0) return(0);
    return(1);
}

putdir(b)
char *b;
{
    register struct direct *dp;
    register i;

    for (dp = (struct direct *)b, i = 0; i < BSIZE; dp++, i += sizeof(*dp)) {
	if (dp->d_ino == 0)
	    continue;
	putent((char *) dp); }
}

/*
 * read a bit mask from the archiv into m.
 */
readbits(m)
short	*m;
{
    register i;

    i = spcl.c_count;

    while (i--) {
	readarchiv((char *) m);
	m += (BSIZE/(MLEN/BITS)); }
    while (gethead(&spcl) == 0) ;
}

null() { ; }
