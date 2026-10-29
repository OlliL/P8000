#include <stdio.h>
#include "ld.h"

struct page page[2];
struct pghdr fpage;
int infil;

int ntab;
struct tent tab[1];
int symindex;
long *libp;
int v66c2, v66c6, v66cc, v66ce;
int fctrel, fcdrel, fcbrel;

char *ofilnam = "l.out";
int v007c, v0080;
char tmpname[] = "/tmp/ldaXXXXX";
int v669a, v66a4;

struct stream text;
struct stream reloc;
char libname[] = "/lib/libxxxxxxxxxxx";
char libpath[20];
int v66b4;

struct buf toutb;
unsigned char v67e4[256];

struct archdr archdr;
char *pname;
char *filname;
int exstat;

sig_handler()
{
	int mode;

	if (v669a)
		limit_msg();
	unlink("l.out");
	if (!v007c) {
		if (!v66a4 || !v0080) {
			mode = ~umask(0) & 0x1ff;
			chmod(ofilnam, mode);
		}
	}
	if (v007c)
		unlink(ofilnam);
	_exit(v007c);
}

f050c(x, y)
int x, y;
{
	int spare;

	register struct tent *e;

	e = &tab[ntab++];
	e->t0 = x;
	e->t1 = symindex;
	e->t2 = (int)libp;
	e->t3 = y;
	e->t4 = fctrel;
	e->t5 = fcdrel;
	e->t6 = fcbrel;
	e->t7 = v66c2;
	e->t8 = v66c6;
	e->t9 = v66cc;
	e->t10 = v66ce;
}

add16_checked(x, y, a)
unsigned x, y;
char *a;
{
	long s;

	s = (long)x + (long)y;
	if (s >= 0x10000)
		report_error(1, a);
	return(s);
}

/* open_tmp: create the temporary file (flag) or the output file and open it for the buffer out (0x120e..0x12a8 of /bin/ld) */
open_tmp(out, flag)
struct buf *out;
int flag;
{
	int spare;
	register int fd;
	char *name;

	name = flag ? tmpname : ofilnam;
	fd = creat(name, 0x1b6);
	if (fd < 0) {
		if (flag)
			report_error(2, "cannot create temp");
		else {
			fprintf(stderr, "ld: cannot create output\n");
			unlink("l.out");
			exit(2);
		}
	}
	close(fd);
	out->fildes = open(name, 2);
	if (flag)
		unlink(tmpname);
	out->nleft = 0x100;
	out->xnext = out->iobuf;
}
/* add16_checked: sum of two words, report_error(1) with the message a when the sum is 0x10000 or more (sld add16_checked;
   0x089a..0x08cc of /bin/ld) */

/* load2arg: pass two for one argument (object file or archive member; sld load2arg; 0x13ca..0x1452 of /bin/ld) */
load2arg(p)
char *p;
{
	register char *q;
	register long *lp;
	int spare[2];

	q = p;
	if (getfile(q) == 0) {
		goto test0;
body0:
		q++;
test0:
		if (*q != 0)
			goto body0;
		for (;;) {
			if (q < p)
				break;
			if (*--q == 0x2f)
				break;
		}
		q++;
		mkfsym(q);
		load2(0L);
	} else {
		lp = libp;
		goto test1;
body1:
		dseek(&text, *lp, 26);
		mget(&archdr, 26);
		mkfsym(archdr.aname);
		load2(*lp + 13);
		lp++;
test1:
		if (*lp != -1)
			goto body1;
		lp++;
		libp = lp;
	}
	close(infil);
}

copy(p)
struct buf *p;
{
	int spare1, spare2;
	unsigned char *q;
	register fd;
	register n;

	flush_buffer(p);
	fd = p->fildes;
	lseek(fd, 0L, 0);
	goto test;
body:
	q = v67e4;
	do
		put_int_buf(*q++, &toutb);
	while (--n);
test:
	n = read(fd, v67e4, 256);
	if (n > 1)
		goto body;
	close(fd);
}

high_byte(x)
unsigned x;
{
	if ((x & 0xff00) != x)
		report_error(2, "internal error - mkchar");
	return((x >> 8) & 0xff);
}

struct local local[1];

/* mget: read n bytes of the text stream into the buffer p (0x1a9a..0x1b0a of /bin/ld) */
mget(p, n)
int *p;
int n;
{
	int spare1, spare2, spare3;

	register int *r;
	register i;
	register int *q;

	i = n;
	i >>= 1;
	r = p;
	text.nibuf -= i;
	if (text.nibuf >= 0) {
		text.size -= i;
		if (text.size > 0) {
			q = text.ptr;
			do
				*r++ = *q++;
			while (--i);
			text.ptr = q;
			return;
		}
		text.size += i;
	}
	text.nibuf += i;
	do
		*r++ = get(&text);
	while (--i);
}

/* mput: write n bytes (words) of the array a to the output buffer q (0x1b0a..0x1b38 of /bin/ld) */
mput(q, p, n)
struct buf *q;
int *p;
int n;
{
	int spare1, spare2;

	register int *r;
	register i;

	r = p;
	i = n >> 1;
	do {
		f221e(*r++, q);
	} while (--i);
}

dseek(sp, v, n)
struct stream *sp;
long v;
int n;
{
	int spare1, spare2;
	int nw;
	long hi, lo;
	register struct stream *obj;
	register struct page *cur;

	hi = (v >> 8) & 0xffffff;
	lo = v & 0xff;
	obj = sp;
	obj->pno->nuser -= 1;
	cur = &page[0];
	if ((long)cur->bno == hi)
		goto common;
	cur = &page[1];
	if ((long)cur->bno == hi)
		goto common;
	if (cur->nuser) {
		cur = &page[0];
		if (cur->nuser) {
			report_error(2, "No pages");
			goto common;
		}
	}
	if (page[0].nuser == 0 && page[1].nuser == 0 && page[0].bno < page[1].bno)
		cur = &page[0];
	cur->bno = hi;
	lseek(infil, (v & 0xffffff00L) << 1, 0);
	nw = read(infil, cur->buff, 0x200) >> 1;
	if (nw < 0)
		nw = 0;
	cur->nibuf = nw;
common:
	cur->nuser += 1;
	obj->bno = hi;
	obj->pno = (struct pghdr *)cur;
	obj->ptr = (int *)((char *)cur->buff + (lo << 1));
	if (n != -1)
		obj->size = half(n);
	obj->nibuf = cur->nibuf - (int)lo;
	if (obj->nibuf <= 0)
		obj->size = 0;
}

half(x)
int x;
{
	return((x >> 1) & 0x7fff);
}

get(p)
register struct stream *p;
{
	int spare;

	if (--p->nibuf < 0) {
		dseek(p, -1, (long)(p->bno + 1) << 8);
		--p->nibuf;
	}
	if (--p->size <= 0) {
		if (p->size < 0)
			report_error(2, "Premature EOF");
		fpage.nuser++;
		p->pno->nuser--;
		p->pno = &fpage;
	}
	return(*p->ptr++);
}

getfile(name)
char *name;
{
	register char *s;
	register char *p;
	struct {
		char pad[0x16];
		long mtime;
		long ctime;
	} st;

	s = name;
	infil = -1;
	archdr.aname[0] = 0;
	filname = name;
	if (s[0] == '-' && s[1] == 'l') {
		filname = libpath;
		if (s[2] == 0)
			s = "-lc";
		p = rindex(libname, '/') + 4;
		strncpy(p, s + 2, 0x168 - (int)p);
		strcat(p, v66b4 ? ".a" : ".8");
		infil = open(libname, 0);
		if (infil == -1) {
			strcpy(libpath, "/usr");
			strcat(libpath, libname);
			infil = open(libpath, 0);
			if (infil == -1) {
				strcpy(libpath, "/z/bin");
				strcat(libpath, libname);
			}
		}
	}
	if (infil == -1) {
		infil = open(filname, 0);
		if (infil < 0)
			report_error(2, "cannot open");
	}
	page[1].bno = -1;
	page[0].bno = -1;
	page[1].nuser = 0;
	page[0].nuser = 0;
	reloc.pno = (struct pghdr *)&fpage;
	text.pno = (struct pghdr *)&fpage;
	fpage.nuser = 2;
	dseek(&text, 0L, 2);
	if (text.size <= 0)
		report_error(2, "Premature EOF");
	if (get(&text) != -155)
		return 0;
	dseek(&text, 1L, 0x1a);
	if (text.size <= 0)
		return 1;
	mget(&archdr, 0x1a);
	if (strncmp(archdr.aname, "__.SYMDEF", 14))
		return 1;
	fstat(infil, &st);
	if (st.mtime > archdr.atime)
		return 3;
	return 2;
}

/* report_error: print the message with the file name and stop when the level is above 1 (0x1e86..0x1f22 of /bin/ld) */
report_error(level, msg, a, b, c, d, e)
int level;
char *msg;
int a;
int b;
int c;
int d;
int e;
{
	if (!exstat)
		fprintf(stderr, "%s: ", pname);
	if (filname)
		fprintf(stderr, "%s", filname);
	if (archdr.aname[0])
		fprintf(stderr, "(%.14s)", archdr.aname);
	fprintf(stderr, ": ");
	fprintf(stderr, msg, a, b, c, d, e);
	fprintf(stderr, "\n");
	if (level > 1)
		sig_handler();
	exstat = level;
}

/* table_lookup: find an entry of the local symbol table (0x1f22..0x1f64 of /bin/ld) */
table_lookup(end, a)
struct local *end;
int a;
{
	int spare1, spare2, spare3;

	register struct local *p;
	register struct local *e;
	register k;

	e = end;
	k = (a >> 4) & 0x0fff;
	p = local;
	while (p < e) {
		if (p->locindex == k)
			return(p->locsymbol);
		p++;
	}
	report_error(2, "Local symbol botch");
}

/* str_compare: 1 when the strings are equal, else 0 (sld str_compare without cmpmode; 0x21f2..0x221e of /bin/ld) */
str_compare(p, q)
char *p, *q;
{
	for (;;) {
		if (*p == *q++) {
			if (*p++)
				continue;
			return(1);
		}
		return(0);
	}
}

f221e(x, p)
int x;
register struct buf *p;
{
	*p->xnext++ = x >> 8;
	p->nleft--;
	if (p->nleft <= 0)
		flush_buffer(p);
	*p->xnext++ = x;
	p->nleft--;
	if (p->nleft <= 0)
		flush_buffer(p);
}

put_int_buf(v, p)
int v;
register struct buf *p;
{
	*p->xnext++ = v;
	if (--p->nleft <= 0)
		flush_buffer(p);
}

/* flush_buffer: write the buffer to its file and reset it (sld flush_buffer; 0x229c..0x22e8 of /bin/ld) */
flush_buffer(a)
register struct buf *a;
{
	int spare1;

	register n;

	n = (char *)a->xnext - (char *)a->iobuf;
	if (n > 0) {
		if (write(a->fildes, a->iobuf, n) != n)
			report_error(2, "output error");
	}
	a->xnext = a->iobuf;
	a->nleft = 0x100;
}

/* getfile: open an input file, searching the library directories (sld getfile; 0x1ce2..0x1e86 of /bin/ld; finding:
   three registers in the original, the count of strncpy and the last branch differ) */
