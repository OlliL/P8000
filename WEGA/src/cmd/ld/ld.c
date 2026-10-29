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

int *v1030;
int v1034, v1036;

struct buf {
	int fildes;
	int nleft;
	char *xnext;
	char iobuf[256];
};

f1c5a(x)
int x;
{
	return((x >> 1) & 0x7fff);
}

f1a74(x)
unsigned x;
{
	if ((x & 0xff00) != x)
		f1e86(2, "x");
	return((x >> 8) & 0xff);
}

struct ent {
	int key;
	int val;
};

struct ent vtab[1];

f1f22(end, x)
struct ent *end;
int x;
{
	int spare1, spare2, spare3;

	register struct ent *p;
	register struct ent *e;
	register key;

	e = end;
	key = (x >> 4) & 0x0fff;
	p = vtab;
	while (p < e) {
		if (p->key == key)
			return(p->val);
		p++;
	}
	f1e86(2, "x");
}

f21f2(t, s)
char *t, *s;
{
	for (;;) {
		if (*t == *s++) {
			if (*t++)
				continue;
			return(1);
		}
		return(0);
	}
}

f226e(c, p)
char c;
register struct buf *p;
{
	*p->xnext++ = c;
	p->nleft--;
	if (p->nleft <= 0)
		f229c(p);
}

f1b0a(q, p, n)
int q, *p, n;
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

f229c(p)
register struct buf *p;
{
	int spare1;

	register n;

	n = p->xnext - p->iobuf;
	if (n > 0) {
		if (write(p->fildes, p->iobuf, n) != n)
			f1e86(2, "x");
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
		f229c(p);
	*p->xnext++ = x;
	p->nleft--;
	if (p->nleft <= 0)
		f229c(p);
}

f1a9a(p, n)
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
	v1034 -= i;
	if (v1034 < 0)
		goto restore34;
	v1036 -= i;
	if (v1036 > 0) {
		q = v1030;
		do {
			*r++ = *q++;
		} while (--i);
		v1030 = q;
		return;
	}
	v1036 += i;
restore34:
	v1034 += i;
	do {
		*r++ = f1c6c(&v1030);
	} while (--i);
}

f0042()
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

f1b38(sp, v, n)
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
			f1e86(2, "No pages");
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
		obj->size = f1c5a(n);
	obj->nibuf = cur->nibuf - (int)lo;
	if (obj->nibuf <= 0)
		obj->size = 0;
}

f1c6c(p)
register struct stream *p;
{
	int spare;

	if (--p->nibuf < 0) {
		f1b38(p, -1, (long)(p->bno + 1) << 8);
		--p->nibuf;
	}
	if (--p->size <= 0) {
		if (p->size < 0)
			f1e86(2, "x");
		fpage.nuser++;
		p->pno->nuser--;
		p->pno = &fpage;
	}
	return(*p->ptr++);
}

