static char sccsid[] = "@[$]ls.c\t1.7  02/23/83 23:37:28 - 87wega3.2";
#define UCB			/* Controls output format for -F */

/*
 * ls - list file or directory
 *
 * Modified by Bill Joy UCB May/August 1977
 * Modified by Dave Presotto BTL Feb/80
 * Modified by Bill Joy and Mark Horton Summer 1980
 * Modified by Paul Fronberg June 1982 (SYS III)
 */

#include <sys/param.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <stdio.h>
#include <pwd.h>
#include <grp.h>

#define	printf	lsprintf	/* the own printf at the end (name guessed): the printf member of libc comes for fprintf */
#define MAXFILEWIDTH 14
#define NFILES	1024
FILE	*pwdf, *dirf;

struct lbuf {
	union {
		char	lname[15];
		char	*namep;
	} ln;
	char	ltype;
	ino_t	lnum;
	short	lflags;
	short	lnl;
	short	luid;
	short	lgid;
	long	lsize;
	long	lmtime;
};

struct dchain {
	char *dc_name;		/* the path name */
	struct dchain *dc_next; /* the next directory on the chain */
};

struct dchain *dfirst;		/* the start of the directory chain */
struct dchain *cdfirst;		/* the end of the directory chain */
struct dchain *dtemp;		/* temporary used when linking */
char *curdir;			/* the current directory */

int	aflg, dflg, lflg, sflg, tflg, uflg, iflg, fflg, gflg, cflg, ogflg, oflg;
int	Aflg, nflg, qflg, Fflg, Rflg, across, Cflg, Dflg;
int	nopad;
int	tabflg;
int	rflg	= 1;
int	lastuid	= -1;
long	year;
int	flags;
long	tblocks;
int	statreq;
int	xtraent;		/* for those switches which print out a total */
struct	lbuf	*flist[NFILES];
struct	lbuf	**lastp = flist;
struct	lbuf	**firstp = flist;
char	*dotp	= ".";

char	*makename();
struct	lbuf *gstat();
char	*ctime();
long	time();
long	nblock();

#define ISARG	0100000
int	colwidth;
int	filewidth;
int	fixedwidth;
int	outcol;

char	obuf[BUFSIZ];

main(argc, argv)
int argc;
char *argv[];
{
#include <sgtty.h>

	int i, width;
	register struct lbuf *ep;
	register struct lbuf **slastp;
	struct lbuf **epp;
	struct lbuf lb;
	char *t;
	char *cp;
	int compar();
	struct sgttyb sgbuf;

	Fflg = 0;
	tabflg = 0;
	Aflg = getuid() == 0;
	setbuf(stdout, obuf);
	lb.lmtime = time((long *) 0);
	year = lb.lmtime - 6L*30L*24L*60L*60L; /* 6 months ago */
	qflg = gtty(1, &sgbuf) == 0;

	/* guarantee at least on column width */
	fixedwidth = 1;

	/*
	 * If the standard output is not a teletype,
	 * then we default to one-per-line format
	 * otherwise decide between stream and
	 * columnar based on our name.
	 */
	if (qflg) {
		Cflg = 1;
		if ((sgbuf.sg_flags & XTABS) == 0)
			tabflg++;
		for (cp = argv[0]; cp[0] && cp[1]; cp++)
			continue;
		/*
		 * Name ends in l => stream
		 */
		if (cp[0] == 'l')
			nopad = 1, Cflg = 0;
		/*
		 * ... if doesn't end in l or s ==> columns sorted across
		 */
		else if (cp[0] == 'x')
			across = 1;
		else if (cp[0] == 'f')
			Fflg = 1;
	}
	while (--argc > 0 && *argv[1] == '-') {
		argv++;
		while (*++*argv) switch (**argv) {
		/*
		 * C - force columnar output
		 */
		case 'C':
			Cflg = 1;
			nopad = 0;
			continue;
		/*
		 * m - force stream output
		 */
		case 'm':
			Cflg = 0;
			nopad = 1;
			continue;
		/*
		 * x - force sort across
		 */
		case 'x':
			across = 1;
			nopad = 0;
			Cflg = 1;
			continue;
		/*
		 * q - force ?'s in output
		 */
		case 'q':
			qflg = 1;
			continue;
		/*
		 * 1 - force 1/line in output
		 */
		case '1':
			Cflg = 0;
			nopad = 0;
			continue;
		/* STANDARD FLAGS */
		case 'a':
			aflg++;
			continue;

		case 'A':
			Aflg = !Aflg;
			continue;

		case 'c':
			cflg++;
			continue;

		case 's':
			fixedwidth += 5;
			sflg++;
			statreq++;
			xtraent++;
			continue;

		case 'd':
			dflg++;
			continue;

		/*
		 * n - don't look in password file
		 */
		case 'n':
			nflg++;
		case 'l':
			lflg++;
			statreq++;
			xtraent++;
			gflg++;
			continue;

		case 'o':
			oflg++;
			statreq++;
			xtraent++;
			continue;

		case 'r':
			rflg = -1;
			continue;

		case 't':
			tflg++;
			statreq++;
			continue;

		case 'u':
			uflg++;
			continue;

		case 'i':
			fixedwidth += 5;
			iflg++;
			continue;

		case 'f':
			fflg++;
			continue;

		case 'g':
			ogflg++;
			continue;

		case 'F':
			Fflg++;
			continue;

		case 'R':
			Rflg++;
			continue;

		case 'D':
			Dflg++;
			continue;

		default:
			fprintf (stderr, "usage: ls [-1ACDFRacdfgilmnqrstux] [files]\n");
			exit(1);
		}
	}
	if (Fflg)
		fixedwidth++;
	if (fflg) {
		aflg++;
		lflg = 0;
		sflg = 0;
		tflg = 0;
		statreq = 0;
		xtraent = 0;
	}
	if(lflg) {
		Cflg = 0;
		t = "/etc/passwd";
		if (gflg)
			t = "/etc/group";
		nopad = 0;
		fixedwidth = 70;
		pwdf = fopen(t, "r");
	}
	if (argc==0) {
		argc++;
		argv = &dotp - 1;
	}
	for (i=0; i < argc; i++) {
		argv++;
		if (Cflg) {
			width = strlen (*argv);
			if (width > filewidth)
				filewidth = width;
		}
		if ((ep = gstat(*argv, 1))==NULL)
			continue;
		ep->ln.namep = *argv;
		ep->lflags |= ISARG;
	}
	if (!Cflg)
		filewidth = MAXFILEWIDTH;
	else
	colwidth = fixedwidth + filewidth;
	qsort(firstp, lastp - firstp, sizeof *lastp, compar);
	slastp = lastp;
	/* For each argument user typed */
	for (epp=firstp; epp<slastp; epp++) {
		ep = *epp;
		if (ep->ltype=='d' && dflg==0 || fflg)
			pdirectory(ep->ln.namep, (argc>1), slastp);
		else
			pentry(ep);
	}
	/* -R: print subdirectories found */
	while (dfirst) {
		pdirectory (dfirst->dc_name, 1, firstp);
		dtemp = dfirst;
		dfirst = dfirst->dc_next;
		cfree (dtemp->dc_name);
		cfree (dtemp);
	}
	if (outcol)
		putc('\n', stdout);
	fflush(stdout);
}

/*
 * pdirectory: print the directory name, labelling it if title is
 * nonzero, using lp as the place to start reading in the dir.
 */
pdirectory (name, title, lp)
char *name;
int title;
struct lbuf **lp;
{

	filewidth = 0;
	curdir = name;
	if (title)
		printf("\n%s:\n", name);
	lastp = lp;
	readdir(name);
	if (!Cflg)
		filewidth = MAXFILEWIDTH;
	colwidth = fixedwidth + filewidth;
	if (tabflg) {
		if (colwidth <= 8)
			colwidth = 8;
		else
			if (colwidth <= 16)
				colwidth = 16;
	}
	if (fflg==0)
		qsort(lp,lastp - lp,sizeof *lastp,compar);
	if (lflg || sflg)
		printf("total %D", tblocks);
	pem(lp, lastp);
	newline();
}

/*
 * pem: print 'em.  Print a list of files (e.g. a directory) bounded
 * by slp and lp.
 */
pem(slp, lp)
	register struct lbuf **slp, **lp;
{
	int ncols, nrows, row, col;
	register struct lbuf **ep;

	if (tabflg) {
		if (colwidth <= 8)
			colwidth = 8;
		else
			if (colwidth <= 16)
				colwidth = 16;
	}
	ncols = 80 / colwidth;
	if (ncols == 1 || Cflg == 0) {
		for (ep = slp; ep < lp; ep++)
			pentry(*ep);
		return;
	}
	if (across) {
		for (ep = slp; ep < lp; ep++)
			pentry(*ep);
		return;
	}
	if (xtraent)
		slp--;
	nrows = (lp - slp - 1) / ncols + 1;
	for (row = 0; row < nrows; row++) {
		col = row == 0 && xtraent;
		for (; col < ncols; col++) {
			ep = slp + (nrows * col) + row;
			if (ep < lp)
				pentry(*ep);
		}
		if (outcol)
			printf("\n");
	}
}

/*
 * pputchar: like putchar but knows how to handle control chars.
 */
pputchar(c)
	char c;
{

	switch (c) {
		case '\t':
			outcol = (outcol + 8) &~ 7;
			break;
		case '\n':
			outcol = 0;
			break;
		default:
			if (qflg)
				if (c < ' ' || c >= 0177)
					c = '?';
			outcol++;
			break;
	}
	putc(c, stdout);
}

newline()
{
	if (outcol)
		putc('\n', stdout);
	outcol = 0;
}

/*
 * column: get to the beginning of the next column.
 */
column()
{

	if (outcol == 0)
		return;
	if (nopad) {
		putc(',', stdout);
		outcol++;
		if (outcol + colwidth + 2 > 80) {
			putc('\n', stdout);
			outcol = 0;
			return;
		}
		putc(' ', stdout);
		outcol++;
		return;
	}
	if (Cflg == 0) {
		putc('\n', stdout);
		return;
	}
	if ((outcol / colwidth + 2) * colwidth > 80) {
		putc('\n', stdout);
		outcol = 0;
		return;
	}
	if (tabflg && (colwidth <= 16)) {
		if (colwidth > 8)
			if ((outcol % 16) < 8) {
				outcol += 8 - (outcol % 8);
				putc ('\t', stdout);
			}
		outcol += 8 - (outcol % 8);
		putc ('\t', stdout);
		return;
	}
	do {
		outcol++;
		putc(' ', stdout);
	} while (outcol % colwidth);
}

long
nblock(size)
long size;
{
	return((size+511)>>9);
}

/*
 * This code handles the rwx- business.
 * You figure it out.
 */
int	m1[] = { 1, S_IREAD>>0, 'r', '-' };
int	m2[] = { 1, S_IWRITE>>0, 'w', '-' };
int	m3[] = { 2, S_ISUID, 's', S_IEXEC>>0, 'x', '-' };
int	m4[] = { 1, S_IREAD>>3, 'r', '-' };
int	m5[] = { 1, S_IWRITE>>3, 'w', '-' };
int	m6[] = { 2, S_ISGID, 's', S_IEXEC>>3, 'x', '-' };
int	m7[] = { 1, S_IREAD>>6, 'r', '-' };
int	m8[] = { 1, S_IWRITE>>6, 'w', '-' };
int	m9[] = { 2, S_ISVTX, 't', S_IEXEC>>6, 'x', '-' };

int	*m[] = { m1, m2, m3, m4, m5, m6, m7, m8, m9};

pmode(aflag)
{
	register int **mp;

	flags = aflag;
	for (mp = &m[0]; mp < &m[sizeof(m)/sizeof(m[0])];)
		select(*mp++);
}

select(pairp)
register int *pairp;
{
	register int n;

	n = *pairp++;
	while (--n>=0 && (flags&*pairp++)==0)
		pairp++;
	pputchar(*pairp);
}

/*
 * returns cat(dir, "/", file), unless dir ends in /, when it doesn't //
 */
char *
makename(dir, file)
char *dir, *file;
{
	static char dfile[100];
	register char *dp, *fp;
	register int i;

	dp = dfile;
	fp = dir;
	while (*fp)
		*dp++ = *fp++;
	if (*(dp-1) != '/')
	*dp++ = '/';
	fp = file;
	for (i=0; i<DIRSIZ; i++)
		*dp++ = *fp++;
	*dp = 0;
	return(dfile);
}

/*
 * readdir: read in the directory whose name is dir,
 * starting at lastp.
 */
readdir(dir)
char *dir;
{
	static struct direct dentry;
	register int j, width;
	register struct lbuf *ep;

	if ((dirf = fopen(dir, "r")) == NULL) {
		printf("%s unreadable\n", dir);
		return;
	}
	tblocks = 0;
	for(;;) {
		if (fread(&dentry, sizeof(dentry), 1, dirf) != 1)
			break;
		if (dentry.d_ino==0 ||
			aflg==0 && dentry.d_name[0]=='.' && (
			!Aflg ||
			dentry.d_name[1]=='\0'
			|| dentry.d_name[1]=='.' && dentry.d_name[2]=='\0'))
			continue;
		if (Cflg) {
			width = strlen (dentry.d_name);
			if (width > filewidth)
				filewidth = width;
		}
		ep = gstat(makename(dir, dentry.d_name), Dflg || Fflg || Rflg);
		if (ep==NULL)
			continue;
		if (ep->lnum != -1)
			ep->lnum = dentry.d_ino;
		for (j=0; j<DIRSIZ; j++)
			ep->ln.lname[j] = dentry.d_name[j];
	}
	fclose(dirf);
}

/*
 * stat the given file and return an lbuf containing it.
 * argfl is nonzero if a stat is required because the file is
 * an argument, rather than having been found in a directory.
 */
struct lbuf *
gstat(file, argfl)
char *file;
{
	struct stat statb;
	register struct lbuf *rep;
	static int nomocore;

	if (nomocore)
		return(NULL);
	rep = (struct lbuf *)malloc(sizeof(struct lbuf));
	if (rep==NULL) {
		fprintf(stderr, "ls: out of memory\n");
		nomocore = 1;
		return(NULL);
	}
	if (lastp >= &flist[NFILES]) {
		static int msg;
		lastp--;
		if (msg==0) {
			fprintf(stderr, "ls: too many files\n");
			msg++;
		}
	}
	*lastp++ = rep;
	rep->lflags = 0;
	rep->lnum = 0;
	rep->ltype = '-';
	if (argfl || statreq) {
		if (stat(file, &statb)<0) {
			printf("%s not found\n", file);
			statb.st_ino = -1;
			statb.st_size = 0;
			statb.st_mode = 0;
			if (argfl) {
				lastp--;
				return(0);
			}
		}
		rep->lnum = statb.st_ino;
		rep->lsize = statb.st_size;
		if (Dflg && (statb.st_mode&S_IFMT) != S_IFDIR) {
			lastp--;
			free(rep);
			return(0);
		}
		switch(statb.st_mode&S_IFMT) {

		case S_IFDIR:
			rep->ltype = 'd';
			break;

		case S_IFBLK:
			rep->ltype = 'b';
			rep->lsize = statb.st_rdev;
			break;

		case S_IFIFO:
			rep->ltype = 'p';
			break;

		case S_IFCHR:
			rep->ltype = 'c';
			rep->lsize = statb.st_rdev;
			break;
		}
		rep->lflags = statb.st_mode & ~S_IFMT;
		rep->luid = statb.st_uid;
		rep->lgid = statb.st_gid;
		rep->lnl = statb.st_nlink;
		if(uflg)
			rep->lmtime = statb.st_atime;
		else if (cflg)
			rep->lmtime = statb.st_ctime;
		else
			rep->lmtime = statb.st_mtime;
		tblocks += nblock(statb.st_size);
	}
	return(rep);
}

/*
 * decide whether to print pp1 before or after pp2, based on their
 * names, various times, and the r flag.
 */
compar(pp1, pp2)
struct lbuf **pp1, **pp2;
{
	register struct lbuf *p1, *p2;

	p1 = *pp1;
	p2 = *pp2;
	if (dflg==0) {
		if (p1->lflags&ISARG && p1->ltype=='d') {
			if (!(p2->lflags&ISARG && p2->ltype=='d'))
				return(1);
		} else {
			if (p2->lflags&ISARG && p2->ltype=='d')
				return(-1);
		}
	}
	if (tflg) {
		if(p2->lmtime == p1->lmtime)
			return(0);
		if(p2->lmtime > p1->lmtime)
			return(rflg);
		return(-rflg);
	}
	return(rflg * strcmp(p1->lflags&ISARG? p1->ln.namep: p1->ln.lname,
				p2->lflags&ISARG? p2->ln.namep: p2->ln.lname));
}

/*
 * print the entry pointed at by ap
 */
pentry(ap)
struct lbuf *ap;
{
	register struct lbuf *p;
	register struct dchain *dp;
	register char *cp;
	char fname[100];
	char *pname;
	struct passwd *getpwuid();
	struct passwd *pwptr;
	struct group *getgrgid();
	struct group *grptr;

	fname[0] = 0;
	p = ap;
	if (p->lnum == -1)
		return;
	column();
	if (Rflg && ap->ltype == 'd' && strcmp(ap->ln.lname, ".") &&
			strcmp(ap->ln.lname, "..")) {
		dp = (struct dchain *) calloc(1, sizeof(struct dchain));
		pname = makename (curdir, ap->ln.lname);
		dp->dc_name = (char *) calloc(1, strlen(pname)+1);
		strcpy(dp->dc_name, pname);
		if (dfirst == 0)
			dfirst = dp;
		else
			cdfirst->dc_next = dp;
		cdfirst = dp;
	}
	if (iflg)
		if (nopad && !lflg)
			printf("%d ", p->lnum);
		else
			printf("%5d ", p->lnum);
	if (sflg)
		if (nopad && !lflg)
			printf("%D ", nblock(p->lsize));
		else
			printf("%4D ", nblock(p->lsize));
	if (lflg || oflg) {
		pputchar(p->ltype);
		pmode(p->lflags);
		printf("%2d ", p->lnl);
		pwptr = getpwuid(p->luid);
		if (ogflg == 0)
			if (nflg == 0 && pwptr != 0)
				printf("%-9.9s", pwptr->pw_name);
			else
				printf("%-9d", p->luid);
		if (gflg && !oflg) {
			grptr = getgrgid(p->lgid);
			if (nflg == 0 && grptr != 0)
				printf("%-8.8s", grptr->gr_name);
			else
				printf("%-8d", p->lgid);
		}
		if (p->ltype == 'b' || p->ltype == 'c')
			printf("%3d,%3d", major((int)p->lsize), minor((int)p->lsize));
		else
			printf("%7ld", p->lsize);
		cp = ctime(&p->lmtime);
		if(p->lmtime < year)
			printf(" %-7.7s %-4.4s ", cp+4, cp+20); else
			printf(" %-12.12s ", cp+4);
	}
	if (p->lflags & ISARG)
	    strncat (fname, p->ln.namep, 98);
	else
	    strncat (fname, p->ln.lname, 14);
	if (Fflg) {
	    if (p->ltype == 'd')
		strcat (fname, "/");
	    else if (p->lflags & 0111)
		strcat (fname, "*");
	    else
		strcat (fname, " ");
	}
	printf ("%s", fname);
	free(ap);
}

/*
 * printf and _doprnt of ls: integers and strings only, the output through pputchar
 */
union lw {
	long	l;
	unsigned w[2];
};

printf(fmt, a1, a2, a3, a4, a5, args)
char *fmt;
{
	int regs[5];

	regs[0] = a1;
	regs[1] = a2;
	regs[2] = a3;
	regs[3] = a4;
	regs[4] = a5;
	_doprnt(fmt, regs, &args, 0);
	return(0);
}

/*
 * a long argument needs an even register pair
 */
_align(np, app, stkap)
int *np;
int **app;
int *stkap;
{
	int tmp;

	if (*np == 0 || *np == 2) {
		*np += 1;
		*app += 1;
	} else if (*np == 4) {
		*np += 2;
		*app = stkap;
		return;
	}
	tmp = **app;
	**app = (*app)[1];
	(*app)[1] = tmp;
}

_doprnt(fmt, argp, stkap, nreg)
register unsigned char *fmt;
int *argp;
int *stkap;
int nreg;
{
	int *ap;
	register c, fmtlen, len;
	int width, prec, base;
	union lw val;
	char *str;
	char buf[26];
	int left, pad;
	char zero, fill;
	int conv;

	ap = argp;
	fmtlen = _length(fmt);
	while (fmtlen--) {
		if ((c = *fmt++) != '%') {
			pputchar(c);
			continue;
		}
		if (fmtlen == 0)
			break;
		fmtlen--;
		c = *fmt++;
		prec = 0;
		width = 0;
		fill = ' ';
		left = 0;
		zero = 0;
		if (c == '-') {
			left++;
			if (fmtlen-- == 0)
				break;
			c = *fmt++;
		}
		if (c < 'a') {
			if (c == '*') {
				width = *ap++;
				if (++nreg == 5)
					ap = stkap;
			} else {
				if (c == '0')
					fill = '0';
				len = _atoip(--fmt, ++fmtlen, &width);
				fmt += len;
				fmtlen -= len;
			}
			if (fmtlen == 0)
				break;
			if (*fmt == '.') {
				fmt++;
				fmtlen--;
				if (fmtlen == 0)
					break;
				if (*fmt == '*') {
					prec = *ap++;
					len = 1;
					if (++nreg == 5)
						ap = stkap;
				} else
					len = _atoip(fmt, fmtlen, &prec);
				if (len > 0)
					zero++;
				fmtlen -= len;
				fmt += len;
			}
			if (fmtlen == 0)
				break;
			fmtlen--;
			c = *fmt++;
		}
		base = 10;
		conv = c;
		switch (c) {
		case 'o':
			base = 8;
			goto uns;
		case 'x':
			base = 16;
			goto uns;
		case 'u':
		uns:
			val.l = *ap++;
			val.w[0] = 0;
			if (++nreg == 5)
				ap = stkap;
			goto conv_int;
		case 'O':
		lo:
			base = 8;
			goto lng;
		case 'X':
		lx:
			base = 16;
			goto lng;
		case 'l':
			if (fmtlen == 0)
				goto fin;
			fmtlen--;
			if ((c = *fmt++) == 'o')
				goto lo;
			if (c == 'x')
				goto lx;
			if (c == 'd')
		case 'D':
				base = 10;
		lng:
			if (nreg < 5)
				_align(&nreg, &ap, stkap);
			val.l = *(long *)ap;
			ap += 2;
			nreg += 2;
			if (nreg == 5)
				ap = stkap;
			goto conv_int;
		case 'd':
			val.l = *ap++;
			if (++nreg == 5)
				ap = stkap;
		conv_int:
			str = buf;
			len = _cnvt(str, base, val.l);
			break;
		case 's':
			fill = ' ';
			zero = 0;
			str = (char *)*ap++;
			len = _length(str);
			if (++nreg == 5)
				ap = stkap;
			break;
		case 'c':
			fill = ' ';
			zero = 0;
			len = 1;
			str = (char *)ap++;
			str++;
			if (++nreg == 5)
				ap = stkap;
			break;
		default:
			pputchar(c);
			continue;
		}
	fin:
		if (prec && conv == 's' && len > prec)
			len = prec;
		if (zero)
			if (len + 1 < width)
				pad = width - len - 1;
			else
				pad = 0;
		else if (len < width)
			pad = width - len;
		else
			pad = 0;
		if (!left)
			while (pad--)
				pputchar(fill);
		if (zero)
			pputchar('0');
		while (len--)
			pputchar(*str++);
		if (left)
			while (pad--)
				pputchar(fill);
	}
}

_atoip(str, len, result)
register char *str;
int len;
int *result;
{
	register left, n;
	int neg;
	char c;

	neg = 0;
	left = len;
	if (len && *str == '-') {
		len--;
		str++;
		neg = 1;
	}
	n = 0;
	while (left) {
		if ((c = *str++) < '0' || c > '9')
			break;
		n = n*10 + (c - '0');
		left--;
	}
	*result = neg ? -n : n;
	return(len - left);
}
