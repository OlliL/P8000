struct pghdr {
	int nuser;
	int bno;
};

struct stream {
	int *ptr;
	int bno;
	int nibuf;
	int size;
	struct pghdr *pno;
};

struct page {
	int nuser;
	int bno;
	int nibuf;
	int buff[256];
};

struct page page[2];
struct pghdr fpage;
int infil;

struct tent {
	int t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10;
};

int ntab;
struct tent tab[1];
int v44a6, v0070;
int v66c2, v66c6, v66cc, v66ce, v66d0, v66d2, v66d4;

char *v007a;
int v007c, v0080;
int v669a, v66a4;

struct stream text;

struct buf {
	int fildes;
	int nleft;
	char *xnext;
	char iobuf[256];
};

struct buf v66d8;
unsigned char v67e4[256];

half(x)
int x;
{
	return((x >> 1) & 0x7fff);
}

high_byte(x)
unsigned x;
{
	if ((x & 0xff00) != x)
		report_error(2, "x");
	return((x >> 8) & 0xff);
}

/* local symbol table: index -> value, 4 bytes per entry (sld: struct local with a symbol pointer, 6 bytes) */
struct local {
	int locindex;
	int locsymbol;
};

struct local local[1];

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
	report_error(2, "x");
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

put_int_buf(v, p)
int v;
register struct buf *p;
{
	*p->xnext++ = v;
	if (--p->nleft <= 0)
		flush_buffer(p);
}

flush_buffer(p)
register struct buf *p;
{
	int spare1;

	register n;

	n = p->xnext - p->iobuf;
	if (n > 0) {
		if (write(p->fildes, p->iobuf, n) != n)
			report_error(2, "x");
	}
	p->xnext = p->iobuf;
	p->nleft = 256;
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

sig_handler()
{
	int mode;

	if (v669a)
		f2c02();
	unlink("l.out");
	if (!v007c) {
		if (!v66a4 || !v0080) {
			mode = ~umask(0) & 0x1ff;
			chmod(v007a, mode);
		}
	}
	if (v007c)
		unlink(v007a);
	_exit(v007c);
}

f050c(x, y)
int x, y;
{
	int spare;

	register struct tent *e;

	e = &tab[ntab++];
	e->t0 = x;
	e->t1 = v44a6;
	e->t2 = v0070;
	e->t3 = y;
	e->t4 = v66d0;
	e->t5 = v66d2;
	e->t6 = v66d4;
	e->t7 = v66c2;
	e->t8 = v66c6;
	e->t9 = v66cc;
	e->t10 = v66ce;
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
			report_error(2, "x");
		fpage.nuser++;
		p->pno->nuser--;
		p->pno = &fpage;
	}
	return(*p->ptr++);
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
		put_int_buf(*q++, &v66d8);
	while (--n);
test:
	n = read(fd, v67e4, 256);
	if (n > 1)
		goto body;
	close(fd);
}

