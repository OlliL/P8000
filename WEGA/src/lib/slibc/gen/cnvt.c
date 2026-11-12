/* _cnvt: a long to text in base 8, 10 (-10: signed) or 16; reconstructed from the member of /lib/libc.a */

_cnvt(buf, base, val)
char *buf;
long val;
{
	register char *p;
	register char *r;
	unsigned long u;
	char tmp[14];
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
			u >>= 3;
		} while (u);
		goto rev;
	case 16:
		do {
			*p = (u&15) + '0';
			if (*p > '9')
				*p += 39;
			p++;
			u >>= 4;
		} while (u);
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
