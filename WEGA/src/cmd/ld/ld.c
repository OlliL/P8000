struct _iobuf {
	int _fd;
	int _cnt;
	char *_ptr;
	char _buf[256];
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
register struct _iobuf *p;
{
	*p->_ptr++ = c;
	p->_cnt--;
	if (p->_cnt <= 0)
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
register struct _iobuf *p;
{
	int spare1;

	register n;

	n = p->_ptr - p->_buf;
	if (n > 0) {
		if (write(p->_fd, p->_buf, n) != n)
			f1e86(2, "x");
	}
	p->_ptr = p->_buf;
	p->_cnt = 256;
}

