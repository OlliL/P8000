/*
 * ar - archive and library maintainer, host version of the WEGA /bin/ar
 *
 * Derived from WEGA/src/cmd/ar.c (reconstructed from the original binary,
 * ar.c 2.2 11/18/83): same commands, options, messages, exit codes and
 * archives.  Changes against that source, all for the host:
 *   - the archive header is packed and unpacked by hand (26 bytes, big endian,
 *     as on the Z8000) instead of reading and writing a struct;
 *   - the integer widths of the Z8000 are kept where they are visible: dates and
 *     sizes are 32 bit, uid and gid a signed char, the mode 16 bit;
 *   - temporary files are made with mkstemp (the old mktemp is not safe and
 *     needs six X on Linux); the names are not visible to the user;
 *   - ANSI prototypes.
 * Archive format: ar(5), WEGA/src/head/ar.h.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "wctime.h"

#define	ARMAG0	0xFF			/* ARMAG 0177545 as two bytes, big endian */
#define	ARMAG1	0x65
#define	ARHDR	26			/* size of the header on disk */

#define	SKIP	1
#define	IODD	2
#define	OODD	4
#define	HEAD	8

/* struct ar_hdr of ar.h with the widths of the Z8000 */
struct	arhdr {
	char		ar_name[14];
	int32_t		ar_date;
	signed char	ar_uid;
	signed char	ar_gid;
	int16_t		ar_mode;
	int32_t		ar_size;
};

struct	stat	stbuf;
struct	arhdr	arbuf;

char	*man	= "mrxtdpq";
char	*opt	= "cuvnbail";
int	signum[] = {SIGHUP, SIGINT, SIGQUIT, 0};
char	tmp0[]	= "/tmp/vXXXXXX";
char	tmp1[]	= "/tmp/v1XXXXXX";
char	tmp2[]	= "/tmp/v2XXXXXX";

void	(*comfun)(void);
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

static void	rcmd(void), dcmd(void), xcmd(void), pcmd(void), mcmd(void);
static void	tcmd(void), qcmd(void);
static void	setcom(void (*fun)(void)), init(void), usage(void), noar(void);
static void	sigdone(int), done(int), cleanup(void), install(void), movefil(int);
static void	copyfil(int, int, int), phserr(void), mesg(int), longt(void);
static void	pmode(void), wrerr(void), bamatch(void);
static int	getaf(void), getdir(void), match(void), notfound(void), morefil(void);
static int	stats(void);
static void	getqf(void);
static char	*trim(char *);
static char	*xmktemp(char *, int *);

static void
put32(unsigned char *p, int32_t v)
{
	p[0] = (uint32_t)v >> 24;
	p[1] = (uint32_t)v >> 16;
	p[2] = (uint32_t)v >> 8;
	p[3] = (uint32_t)v;
}

static int32_t
get32(const unsigned char *p)
{
	return (int32_t)((uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3]);
}

/* write the header of arbuf */
static int
puthdr(int fd)
{
	unsigned char h[ARHDR];

	memcpy(h, arbuf.ar_name, 14);
	put32(h + 14, arbuf.ar_date);
	h[18] = arbuf.ar_uid;
	h[19] = arbuf.ar_gid;
	h[20] = (uint16_t)arbuf.ar_mode >> 8;
	h[21] = (uint16_t)arbuf.ar_mode;
	put32(h + 22, arbuf.ar_size);
	return write(fd, h, ARHDR) == ARHDR;
}

/* read a header into arbuf; 0 when there is no complete one */
static int
gethdr(int fd)
{
	unsigned char h[ARHDR];

	if (read(fd, h, ARHDR) != ARHDR)
		return 0;
	memcpy(arbuf.ar_name, h, 14);
	arbuf.ar_date = get32(h + 14);
	arbuf.ar_uid = h[18];
	arbuf.ar_gid = h[19];
	arbuf.ar_mode = (int16_t)(h[20] << 8 | h[21]);
	arbuf.ar_size = get32(h + 22);
	return 1;
}

int
main(int argc, char **argv)
{
	int i;
	char *cp;

	for (i = 0; signum[i]; i++)
		if (signal(signum[i], SIG_IGN) != SIG_IGN)
			signal(signum[i], sigdone);
	av0 = trim(argv[0]);
	if (argc < 3)
		usage();
	cp = argv[1];
	for (cp = argv[1]; *cp; cp++)
	switch (*cp) {
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
	if (flg['l'-'a']) {
		/* local temporary files in the current directory */
		memcpy(tmp0, "vXXXXXX\0\0\0\0", 12);
		memcpy(tmp1, "v1XXXXXX\0\0\0", 12);
		memcpy(tmp2, "v2XXXXXX\0\0\0", 12);
	}
	if (flg['i'-'a'])
		flg['b'-'a']++;
	if (flg['a'-'a'] || flg['b'-'a']) {
		bastate = 1;
		ponam = trim(argv[2]);
		argv++;
		argc--;
		if (argc < 3)
			usage();
	}
	arnam = argv[2];
	namv = argv+3;
	namc = argc-3;
	if (comfun == 0) {
		if (flg['u'-'a'] == 0) {
			fprintf(stderr, "%s: one of [%s] must be specified\n", av0, man);
			done(1);
		}
		setcom(rcmd);
	}
	(*comfun)();
	done(notfound());
	return 0;
}

static void
setcom(void (*fun)(void))
{
	if (comfun != 0) {
		fprintf(stderr, "%s: only one of [%s] allowed\n", av0, man);
		done(1);
	}
	comfun = fun;
}

static void
rcmd(void)
{
	int f;

	init();
	getaf();
	while (!getdir()) {
		bamatch();
		if (namc == 0 || match()) {
			f = stats();
			if (f < 0) {
				if (namc)
					fprintf(stderr, "%s: cannot open %s\n", av0, file);
				goto cp;
			}
			if (flg['u'-'a'])
				if ((int32_t)stbuf.st_mtime <= arbuf.ar_date) {
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

static void
dcmd(void)
{
	init();
	if (getaf())
		noar();
	while (!getdir()) {
		if (match()) {
			mesg('d');
			copyfil(af, -1, IODD+SKIP);
			continue;
		}
		mesg('c');
		copyfil(af, tf, IODD+OODD+HEAD);
	}
	install();
}

static void
xcmd(void)
{
	int f;

	if (getaf())
		noar();
	while (!getdir()) {
		if (namc == 0 || match()) {
			f = creat(file, (arbuf.ar_mode & 0777));
			if (f < 0) {
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

static void
pcmd(void)
{
	if (getaf())
		noar();
	while (!getdir()) {
		if (namc == 0 || match()) {
			if (flg['v'-'a']) {
				printf("\n<%s>\n\n", file);
				fflush(stdout);
			}
			copyfil(af, 1, IODD);
			continue;
		}
		copyfil(af, -1, IODD+SKIP);
	}
}

static void
mcmd(void)
{
	init();
	if (getaf())
		noar();
	tf2nam = xmktemp(tmp2, &tf2);
	if (tf2 < 0) {
		fprintf(stderr, "%s: cannot create third temp\n", av0);
		done(1);
	}
	while (!getdir()) {
		bamatch();
		if (match()) {
			mesg('m');
			copyfil(af, tf2, IODD+OODD+HEAD);
			continue;
		}
		mesg('c');
		copyfil(af, tf, IODD+OODD+HEAD);
	}
	install();
}

static void
tcmd(void)
{
	if (getaf())
		noar();
	while (!getdir()) {
		if (namc == 0 || match()) {
			if (flg['v'-'a'])
				longt();
			printf("%s\n", trim(file));
		}
		copyfil(af, -1, IODD+SKIP);
	}
}

static void
qcmd(void)
{
	int i, f;

	if (flg['a'-'a'] || flg['b'-'a']) {
		fprintf(stderr, "%s: abi not allowed with q\n", av0);
		done(1);
	}
	getqf();
	for (i = 0; signum[i]; i++)
		signal(signum[i], SIG_IGN);
	lseek(qf, 0l, 2);
	for (i = 0; i < namc; i++) {
		file = namv[i];
		if (file == 0)
			continue;
		namv[i] = 0;
		mesg('q');
		f = stats();
		if (f < 0) {
			fprintf(stderr, "%s: %s cannot open\n", av0, file);
			continue;
		}
		tf = qf;
		movefil(f);
		qf = tf;
	}
}

/* a temporary file: returns its name, *fd is -1 when it cannot be made */
static char *
xmktemp(char *tmpl, int *fd)
{
	*fd = mkstemp(tmpl);
	return tmpl;
}

static void
init(void)
{
	static unsigned char mbuf[2] = { ARMAG0, ARMAG1 };

	tfnam = xmktemp(tmp0, &tf);
	if (tf < 0) {
		fprintf(stderr, "%s: cannot create temp file\n", av0);
		done(1);
	}
	if (write(tf, mbuf, sizeof mbuf) != sizeof mbuf)
		wrerr();
}

static int
getaf(void)
{
	unsigned char mbuf[2];

	af = open(arnam, 0);
	if (af < 0)
		return 1;
	if (read(af, mbuf, 2) != 2 || mbuf[0] != ARMAG0 || mbuf[1] != ARMAG1) {
		fprintf(stderr, "%s: %s not in archive format\n", av0, arnam);
		done(1);
	}
	return 0;
}

static void
getqf(void)
{
	unsigned char mbuf[2];

	if ((qf = open(arnam, 2)) < 0) {
		if (!flg['c'-'a'])
			fprintf(stderr, "%s: creating %s\n", av0, arnam);
		close(creat(arnam, 0666));
		if ((qf = open(arnam, 2)) < 0) {
			fprintf(stderr, "%s: cannot create %s\n", av0, arnam);
			done(1);
		}
		mbuf[0] = ARMAG0;
		mbuf[1] = ARMAG1;
		if (write(qf, mbuf, 2) != 2)
			wrerr();
	} else
	if (read(qf, mbuf, 2) != 2 || mbuf[0] != ARMAG0 || mbuf[1] != ARMAG1) {
		fprintf(stderr, "%s: %s not in archive format\n", av0, arnam);
		done(1);
	}
}

static void
usage(void)
{
	printf("usage: %s [%s][%s] archive files ...\n", av0, opt, man);
	done(1);
}

static void
noar(void)
{
	fprintf(stderr, "%s: %s does not exist\n", av0, arnam);
	done(1);
}

static void
sigdone(int sig)
{
	done(100);
}

static void
done(int c)
{
	if (tfnam)
		unlink(tfnam);
	if (tf1nam)
		unlink(tf1nam);
	if (tf2nam)
		unlink(tf2nam);
	exit(c);
}

static int
notfound(void)
{
	int i, n;

	n = 0;
	for (i = 0; i < namc; i++)
		if (namv[i]) {
			fprintf(stderr, "%s: %s not found\n", av0, namv[i]);
			n++;
		}
	return n;
}

static int
morefil(void)
{
	int i, n;

	n = 0;
	for (i = 0; i < namc; i++)
		if (namv[i])
			n++;
	return n;
}

static void
cleanup(void)
{
	int i, f;

	for (i = 0; i < namc; i++) {
		file = namv[i];
		if (file == 0)
			continue;
		namv[i] = 0;
		mesg('a');
		f = stats();
		if (f < 0) {
			fprintf(stderr, "%s: %s cannot open\n", av0, file);
			continue;
		}
		movefil(f);
	}
	install();
}

static void
install(void)
{
	int i;

	for (i = 0; signum[i]; i++)
		signal(signum[i], SIG_IGN);
	if (af < 0)
		if (!flg['c'-'a'])
			fprintf(stderr, "%s: creating %s\n", av0, arnam);
	close(af);
	af = creat(arnam, 0666);
	if (af < 0) {
		fprintf(stderr, "%s: cannot create %s\n", av0, arnam);
		done(1);
	}
	if (tfnam) {
		lseek(tf, 0l, 0);
		while ((i = read(tf, buf, 512)) > 0)
			if (write(af, buf, i) != i)
				wrerr();
	}
	if (tf2nam) {
		lseek(tf2, 0l, 0);
		while ((i = read(tf2, buf, 512)) > 0)
			if (write(af, buf, i) != i)
				wrerr();
	}
	if (tf1nam) {
		lseek(tf1, 0l, 0);
		while ((i = read(tf1, buf, 512)) > 0)
			if (write(af, buf, i) != i)
				wrerr();
	}
}

static void
movefil(int f)
{
	char *cp;
	int i;

	cp = trim(file);
	for (i = 0; i < 13; i++) {
		if ((arbuf.ar_name[i] = *cp) != 0)
			cp++;
	}
	if (i == 13)
		arbuf.ar_name[i] = 0;
	arbuf.ar_size = (int32_t)stbuf.st_size;
	arbuf.ar_date = (int32_t)stbuf.st_mtime;
	arbuf.ar_uid = (signed char)stbuf.st_uid;
	arbuf.ar_gid = (signed char)stbuf.st_gid;
	arbuf.ar_mode = (int16_t)stbuf.st_mode;
	copyfil(f, tf, OODD+HEAD);
	close(f);
}

static int
stats(void)
{
	int f;

	f = open(file, 0);
	if (f < 0)
		return f;
	if (fstat(f, &stbuf) < 0) {
		close(f);
		return -1;
	}
	return f;
}

static void
copyfil(int fi, int fo, int flag)
{
	int i, o;
	int pe;

	if (flag & HEAD)
		if (!puthdr(fo))
			wrerr();
	pe = 0;
	while (arbuf.ar_size > 0) {
		i = o = 512;
		if (arbuf.ar_size < i) {
			i = o = arbuf.ar_size;
			if (i & 1) {
				if (flag & IODD)
					i++;
				if (flag & OODD)
					o++;
			}
		}
		if (read(fi, buf, i) != i)
			pe++;
		if ((flag & SKIP) == 0)
			if (write(fo, buf, o) != o)
				wrerr();
		arbuf.ar_size -= 512;
	}
	if (pe)
		phserr();
}

static int
getdir(void)
{
	int i;

	if (!gethdr(af)) {
		if (tf1nam) {
			i = tf;
			tf = tf1;
			tf1 = i;
		}
		return 1;
	}
	for (i = 0; i < 14; i++)
		name[i] = arbuf.ar_name[i];
	file = name;
	return 0;
}

static int
match(void)
{
	int i;

	for (i = 0; i < namc; i++) {
		if (namv[i] == 0)
			continue;
		if (strcmp(trim(namv[i]), file) == 0) {
			file = namv[i];
			namv[i] = 0;
			return 1;
		}
	}
	return 0;
}

static void
bamatch(void)
{
	int f;

	switch (bastate) {

	case 1:
		if (strcmp(file, ponam) != 0)
			return;
		bastate = 2;
		if (flg['a'-'a'])
			return;

	case 2:
		bastate = 0;
		tf1nam = xmktemp(tmp1, &f);
		if (f < 0) {
			fprintf(stderr, "%s: cannot create second temp\n", av0);
			return;
		}
		tf1 = tf;
		tf = f;
	}
}

static void
phserr(void)
{
	fprintf(stderr, "%s: phase error on %s\n", av0, file);
}

static void
mesg(int c)
{
	if (flg['v'-'a'])
		if (c != 'c' || flg['v'-'a'] > 1)
			printf("%c - %s\n", c, file);
}

static char *
trim(char *s)
{
	char *p1, *p2;

	for (p1 = s; *p1; p1++)
		;
	while (p1 > s) {
		if (*--p1 != '/')
			break;
		*p1 = 0;
	}
	p2 = s;
	for (p1 = s; *p1; p1++)
		if (*p1 == '/')
			p2 = p1+1;
	return p2;
}

static void
longt(void)
{
	char *cp;

	pmode();
	printf("%3d/%1d", arbuf.ar_uid, arbuf.ar_gid);
	printf("%7ld", (long)arbuf.ar_size);
	cp = wega_ctime(arbuf.ar_date);	/* WEGA ctime: TZ as there, default EST5EDT */
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

static int	m1[] = { 1, ROWN, 'r', '-' };
static int	m2[] = { 1, WOWN, 'w', '-' };
static int	m3[] = { 2, SUID, 's', XOWN, 'x', '-' };
static int	m4[] = { 1, RGRP, 'r', '-' };
static int	m5[] = { 1, WGRP, 'w', '-' };
static int	m6[] = { 2, SGID, 's', XGRP, 'x', '-' };
static int	m7[] = { 1, ROTH, 'r', '-' };
static int	m8[] = { 1, WOTH, 'w', '-' };
static int	m9[] = { 2, STXT, 't', XOTH, 'x', '-' };

static int	*m[] = { m1, m2, m3, m4, m5, m6, m7, m8, m9};

static void
select_(int *pairp)
{
	int n, *ap;

	ap = pairp;
	n = *ap++;
	while (--n >= 0 && (*ap++ & arbuf.ar_mode) == 0)
		ap++;
	putchar(*ap);
}

static void
pmode(void)
{
	int **mp;

	for (mp = &m[0]; mp < &m[9];)
		select_(*mp++);
}

static void
wrerr(void)
{
	perror("ar write error");
	done(1);
}
