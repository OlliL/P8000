/*
 * ar - archive and library maintainer (WEGA, System III derived)
 *
 * Reconstructed from the original WEGA binary (/bin/ar, ar.c 2.2 11/18/83).
 * The object code produced by the original cc from this file must match
 * the original binary byte for byte, so statement structure, declaration
 * order and register usage follow the original.
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <ar.h>

#define	SKIP	1
#define	IODD	2
#define	OODD	4
#define	HEAD	8

static	char	sccsid[] = "@[$]ar.c\t2.2  11/18/83 12:48:00 - 87wega3.2";

struct	stat	stbuf;
struct	ar_hdr	arbuf;

char	*man	= { "mrxtdpq" };
char	*opt	= { "cuvnbail" };
int	signum[] = {SIGHUP, SIGINT, SIGQUIT, 0};
char	*tmp0	= "/tmp/vXXXXX";
char	*tmp1	= "/tmp/v1XXXXX";
char	*tmp2	= "/tmp/v2XXXXX";

int	sigdone();
int	rcmd();
int	dcmd();
int	xcmd();
int	pcmd();
int	mcmd();
int	tcmd();
int	qcmd();
int	(*comfun)();
char	flg[26];
char	**namv;
int	namc;
char	*arnam;
char	*ponam;
char	*tfnam;
char	*tf1nam;
char	*tf2nam;
char	*file;
char	name[16];
int	af;
int	tf;
int	tf1;
int	tf2;
int	qf;
int	bastate;
char	buf[512];
char	*av0;

char	*trim();
char	*mktemp();
char	*ctime();

main(argc, argv)
char *argv[];
{
	register i;
	register char *cp;

	for(i=0; signum[i]; i++)
		if(signal(signum[i], SIG_IGN) != SIG_IGN)
			signal(signum[i], sigdone);
	av0 = trim(argv[0]);
	if(argc < 3)
		usage();
	cp = argv[1];
	for(cp = argv[1]; *cp; cp++)
	switch(*cp) {
	case 'a':
	case 'b':
	case 'c':
	case 'i':
	case 'l':
	case 'n':
	case 'u':
	case 'v':
		flg[*cp-'a']++;
		continue;

	case 'r':
		setcom(rcmd);
		continue;

	case 'd':
		setcom(dcmd);
		continue;

	case 'x':
		setcom(xcmd);
		continue;

	case 't':
		setcom(tcmd);
		continue;

	case 'p':
		setcom(pcmd);
		continue;

	case 'm':
		setcom(mcmd);
		continue;

	case 'q':
		setcom(qcmd);
		continue;

	default:
		fprintf(stderr, "%s: bad option `%c'\n", av0, *cp);
		done(1);
	}
	if(flg['l'-'a']) {
		tmp0 = "vXXXXX";
		tmp1 = "v1XXXXX";
		tmp2 = "v2XXXXX";
	}
	if(flg['i'-'a'])
		flg['b'-'a']++;
	if(flg['a'-'a'] || flg['b'-'a']) {
		bastate = 1;
		ponam = trim(argv[2]);
		argv++;
		argc--;
		if(argc < 3)
			usage();
	}
	arnam = argv[2];
	namv = argv+3;
	namc = argc-3;
	if(comfun == 0) {
		if(flg['u'-'a'] == 0) {
			fprintf(stderr, "%s: one of [%s] must be specified\n", av0, man);
			done(1);
		}
		setcom(rcmd);
	}
	(*comfun)();
	done(notfound());
}

setcom(fun)
int (*fun)();
{

	if(comfun != 0) {
		fprintf(stderr, "%s: only one of [%s] allowed\n", av0, man);
		done(1);
	}
	comfun = fun;
}

rcmd()
{
	register f;
	int u;

	init();
	getaf();
	while(!getdir()) {
		bamatch();
		if(namc == 0 || match()) {
			f = stats();
			if(f < 0) {
				if(namc)
					fprintf(stderr, "%s: cannot open %s\n", av0, file);
				goto cp;
			}
			if(flg['u'-'a'])
				if(stbuf.st_mtime <= arbuf.ar_date) {
					close(f);
					goto cp;
				}
			mesg('r');
			copyfil(af, -1, IODD+SKIP);
			movefil(f);
			continue;
		}
	cp:
		mesg('c');
		copyfil(af, tf, IODD+OODD+HEAD);
	}
	cleanup();
}

dcmd()
{

	init();
	if(getaf())
		noar();
	while(!getdir()) {
		if(match()) {
			mesg('d');
			copyfil(af, -1, IODD+SKIP);
			continue;
		}
		mesg('c');
		copyfil(af, tf, IODD+OODD+HEAD);
	}
	install();
}

xcmd()
{
	register f;

	if(getaf())
		noar();
	while(!getdir()) {
		if(namc == 0 || match()) {
			f = creat(file, arbuf.ar_mode & 0777);
			if(f < 0) {
				fprintf(stderr, "%s: %s cannot create\n", av0, file);
				goto sk;
			}
			mesg('x');
			copyfil(af, f, IODD);
			close(f);
			continue;
		}
	sk:
		mesg('c');
		copyfil(af, -1, IODD+SKIP);
		if (namc > 0 && !morefil())
			done(0);
	}
}

pcmd()
{

	if(getaf())
		noar();
	while(!getdir()) {
		if(namc == 0 || match()) {
			if(flg['v'-'a']) {
				printf("\n<%s>\n\n", file);
				fflush(stdout);
			}
			copyfil(af, 1, IODD);
			continue;
		}
		copyfil(af, -1, IODD+SKIP);
	}
}

mcmd()
{

	init();
	if(getaf())
		noar();
	tf2nam = mktemp(tmp2);
	close(creat(tf2nam, 0600));
	tf2 = open(tf2nam, 2);
	if(tf2 < 0) {
		fprintf(stderr, "%s: cannot create third temp\n", av0);
		done(1);
	}
	while(!getdir()) {
		bamatch();
		if(match()) {
			mesg('m');
			copyfil(af, tf2, IODD+OODD+HEAD);
			continue;
		}
		mesg('c');
		copyfil(af, tf, IODD+OODD+HEAD);
	}
	install();
}

tcmd()
{

	if(getaf())
		noar();
	while(!getdir()) {
		if(namc == 0 || match()) {
			if(flg['v'-'a'])
				longt();
			printf("%s\n", trim(file));
		}
		copyfil(af, -1, IODD+SKIP);
	}
}

qcmd()
{
	register i, f;

	if(flg['a'-'a'] || flg['b'-'a']) {
		fprintf(stderr, "%s: abi not allowed with q\n", av0);
		done(1);
	}
	getqf();
	for(i=0; signum[i]; i++)
		signal(signum[i], SIG_IGN);
	lseek(qf, 0l, 2);
	for(i=0; i<namc; i++) {
		file = namv[i];
		if(file == 0)
			continue;
		namv[i] = 0;
		mesg('q');
		f = stats();
		if(f < 0) {
			fprintf(stderr, "%s: %s cannot open\n", av0, file);
			continue;
		}
		tf = qf;
		movefil(f);
		qf = tf;
	}
}

init()
{
	static short mbuf = ARMAG;

	tfnam = mktemp(tmp0);
	close(creat(tfnam, 0600));
	tf = open(tfnam, 2);
	if(tf < 0) {
		fprintf(stderr, "%s: cannot create temp file\n", av0);
		done(1);
	}
	if (write(tf, (char *)&mbuf, sizeof mbuf) != sizeof mbuf)
		wrerr();
}

getaf()
{
	int mbuf;

	af = open(arnam, 0);
	if(af < 0)
		return(1);
	if (read(af, (char *)&mbuf, sizeof mbuf) != sizeof mbuf || mbuf != ARMAG) {
		fprintf(stderr, "%s: %s not in archive format\n", av0, arnam);
		done(1);
	}
	return(0);
}

getqf()
{
	int mbuf;

	if ((qf = open(arnam, 2)) < 0) {
		if(!flg['c'-'a'])
			fprintf(stderr, "%s: creating %s\n", av0, arnam);
		close(creat(arnam, 0666));
		if ((qf = open(arnam, 2)) < 0) {
			fprintf(stderr, "%s: cannot create %s\n", av0, arnam);
			done(1);
		}
		mbuf = ARMAG;
		if (write(qf, (char *)&mbuf, sizeof mbuf) != sizeof mbuf)
			wrerr();
	} else
	if (read(qf, (char *)&mbuf, sizeof mbuf) != sizeof mbuf || mbuf != ARMAG) {
		fprintf(stderr, "%s: %s not in archive format\n", av0, arnam);
		done(1);
	}
}

usage()
{
	printf("usage: %s [%s][%s] archive files ...\n", av0, opt, man);
	done(1);
}

noar()
{
	fprintf(stderr, "%s: %s does not exist\n", av0, arnam);
	done(1);
}

sigdone()
{
	done(100);
}

done(c)
{
	if(tfnam)
		unlink(tfnam);
	if(tf1nam)
		unlink(tf1nam);
	if(tf2nam)
		unlink(tf2nam);
	exit(c);
}

notfound()
{
	register i, n;
	int u;

	n = 0;
	for(i=0; i<namc; i++)
		if(namv[i]) {
			fprintf(stderr, "%s: %s not found\n", av0, namv[i]);
			n++;
		}
	return(n);
}

morefil()
{
	register i, n;

	n = 0;
	for(i=0; i<namc; i++)
		if(namv[i])
			n++;
	return(n);
}

cleanup()
{
	register i, f;

	for(i=0; i<namc; i++) {
		file = namv[i];
		if(file == 0)
			continue;
		namv[i] = 0;
		mesg('a');
		f = stats();
		if(f < 0) {
			fprintf(stderr, "%s: %s cannot open\n", av0, file);
			continue;
		}
		movefil(f);
	}
	install();
}

install()
{
	register i;

	for(i=0; signum[i]; i++)
		signal(signum[i], SIG_IGN);
	if(af < 0)
		if(!flg['c'-'a'])
			fprintf(stderr, "%s: creating %s\n", av0, arnam);
	close(af);
	af = creat(arnam, 0666);
	if(af < 0) {
		fprintf(stderr, "%s: cannot create %s\n", av0, arnam);
		done(1);
	}
	if(tfnam) {
		lseek(tf, 0l, 0);
		while((i = read(tf, buf, 512)) > 0)
			if(write(af, buf, i) != i)
				wrerr();
	}
	if(tf2nam) {
		lseek(tf2, 0l, 0);
		while((i = read(tf2, buf, 512)) > 0)
			if(write(af, buf, i) != i)
				wrerr();
	}
	if(tf1nam) {
		lseek(tf1, 0l, 0);
		while((i = read(tf1, buf, 512)) > 0)
			if(write(af, buf, i) != i)
				wrerr();
	}
}

movefil(f)
{
	register char *cp;
	register i;

	cp = trim(file);
	for(i=0; i<13; i++) {
		if((arbuf.ar_name[i] = *cp) != 0)
			cp++;
	}
	if(i == 13)
		arbuf.ar_name[i] = 0;
	arbuf.ar_size = stbuf.st_size;
	arbuf.ar_date = stbuf.st_mtime;
	arbuf.ar_uid = stbuf.st_uid;
	arbuf.ar_gid = stbuf.st_gid;
	arbuf.ar_mode = stbuf.st_mode;
	copyfil(f, tf, OODD+HEAD);
	close(f);
}

stats()
{
	register f;

	f = open(file, 0);
	if(f < 0)
		return(f);
	if(fstat(f, &stbuf) < 0) {
		close(f);
		return(-1);
	}
	return(f);
}

copyfil(fi, fo, flag)
{
	register i, o;
	int pe;

	if(flag & HEAD)
		if (write(fo, (char *)&arbuf, sizeof arbuf) != sizeof arbuf)
			wrerr();
	pe = 0;
	while(arbuf.ar_size > 0) {
		i = o = 512;
		if(arbuf.ar_size < i) {
			i = o = arbuf.ar_size;
			if(i & 1) {
				if(flag & IODD)
					i++;
				if(flag & OODD)
					o++;
			}
		}
		if(read(fi, buf, i) != i)
			pe++;
		if((flag & SKIP) == 0)
			if(write(fo, buf, o) != o)
				wrerr();
		arbuf.ar_size -= 512;
	}
	if(pe)
		phserr();
}

getdir()
{
	register i;

	i = read(af, (char *)&arbuf, sizeof arbuf);
	if(i != sizeof arbuf) {
		if(tf1nam) {
			i = tf;
			tf = tf1;
			tf1 = i;
		}
		return(1);
	}
	for(i=0; i<14; i++)
		name[i] = arbuf.ar_name[i];
	file = name;
	return(0);
}

match()
{
	register i;

	for(i=0; i<namc; i++) {
		if(namv[i] == 0)
			continue;
		if(strcmp(trim(namv[i]), file) == 0) {
			file = namv[i];
			namv[i] = 0;
			return(1);
		}
	}
	return(0);
}

bamatch()
{
	register f;

	switch(bastate) {

	case 1:
		if(strcmp(file, ponam) != 0)
			return;
		bastate = 2;
		if(flg['a'-'a'])
			return;

	case 2:
		bastate = 0;
		tf1nam = mktemp(tmp1);
		close(creat(tf1nam, 0600));
		f = open(tf1nam, 2);
		if(f < 0) {
			fprintf(stderr, "%s: cannot create second temp\n", av0);
			return;
		}
		tf1 = tf;
		tf = f;
	}
}

phserr()
{

	fprintf(stderr, "%s: phase error on %s\n", av0, file);
}

mesg(c)
{

	if(flg['v'-'a'])
		if(c != 'c' || flg['v'-'a'] > 1)
			printf("%c - %s\n", c, file);
}

char *
trim(s)
char *s;
{
	register char *p1, *p2;

	for(p1 = s; *p1; p1++)
		;
	while(p1 > s) {
		if(*--p1 != '/')
			break;
		*p1 = 0;
	}
	p2 = s;
	for(p1 = s; *p1; p1++)
		if(*p1 == '/')
			p2 = p1+1;
	return(p2);
}

longt()
{
	register char *cp;
	int u;

	pmode();
	printf("%3d/%1d", arbuf.ar_uid, arbuf.ar_gid);
	printf("%7ld", arbuf.ar_size);
	cp = ctime(&arbuf.ar_date);
	printf(" %-12.12s %-4.4s ", cp+4, cp+20);
}

#define	ROWN	0400
#define	WOWN	0200
#define	XOWN	0100
#define	RGRP	040
#define	WGRP	020
#define	XGRP	010
#define	ROTH	04
#define	WOTH	02
#define	XOTH	01
#define	SUID	04000
#define	SGID	02000
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

pmode()
{
	register int **mp;

	for(mp = &m[0]; mp < &m[9];)
		select(*mp++);
}

select(pairp)
int *pairp;
{
	register int n, *ap;

	ap = pairp;
	n = *ap++;
	while(--n>=0 && (*ap++ & arbuf.ar_mode)==0)
		ap++;
	putchar(*ap);
}

wrerr()
{
	perror("ar write error");
	done(1);
}
