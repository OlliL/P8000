/*
 * _cnvt of /lib/cnvt.o and /lib/scnvt.o (not the member of libc.a: _hexchar, set by doprnt.c, gives the hex digits in
 * upper case for %X), written from the objects; the base is src/lib/libc/common/gen/cnvt.c. v1 without -O (ccv1, sccv1).
 */

char _hexchar;

_cnvt(buf, base, val)
char *buf;
long val;
{
	register char *p;
	register char *r;
	unsigned long u;
	char tmp[14];
	char c;
	char *_deciml();

	r = buf;
	if (base == -10) {
		base = 10;
		if (val < 0) {
			*r++ = '-';
			val = -val;
		}
	}
	p = tmp;
	u = val;
	switch (base) {
	case 8:
		do {
			*p++ = (u&7) + '0';
		} while (u >>= 3);
		goto rev;
	case 16:
		if (_hexchar == 'X')
			c = 'A';
		else
			c = 'a';
		do {
			*p = (u&15) + '0';
			if (*p > '9')
				*p += c - ':';
			p++;
		} while (u >>= 4);
	rev:
		do {
			*r++ = *--p;
		} while (p != tmp);
		return(r-buf);
	case 10:
		p = _deciml(&tmp[9], val);
		do {
			*r++ = *p++;
		} while (p != &tmp[10]);
		return(r-buf);
	}
}
