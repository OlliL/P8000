#define	ERRMSG	"\r\nFatal system error: "

extern int	iserrno;

stcat(src, dst)
register char	*src, *dst;
{
	while (*dst)
		dst++;
	while (*dst++ = *src++)
		;
}

stcmpr(s1, s2)
register char	*s1, *s2;
{
	while (*s1 == *s2) {
		if (*s1 == 0)
			return(0);
		s1++;
		s2++;
	}
	return((*s1 & 0377) < (*s2 & 0377) ? -1 : 1);
}

stcopy(src, dst)
register char	*src, *dst;
{
	while (*dst++ = *src++)
		;
}

stleng(s)
register char	*s;
{
	register char	*p;

	p = s;
	while (*p)
		p++;
	return(p - s);
}

wdfill(p, n, v)
register int	*p;
register int	n, v;
{
	while (n > 0) {
		*p++ = v;
		n -= 2;
	}
}

wdcopy(src, dst, n)
register int	*src, *dst;
register int	n;
{
	while (n > 0) {
		*dst++ = *src++;
		n -= 2;
	}
}

byfill(p, n, c)
register char	*p;
register int	n, c;
{
	while (n-- > 0)
		*p++ = c;
}

bycopy(src, dst, n)
register char	*src, *dst;
register int	n;
{
	while (n-- > 0)
		*dst++ = *src++;
}

bycmpr(s1, s2, n)
register char	*s1, *s2;
register int	n;
{
	if (n <= 0)
		return(0);
	while (*s1 == *s2) {
		if (--n == 0)
			return(0);
		s1++;
		s2++;
	}
	return((*s1 & 0377) < (*s2 & 0377) ? -1 : 1);
}

error(s)
char	*s;
{
	int	n;

	write(2, ERRMSG, sizeof(ERRMSG));
	n = stleng(s);
	write(2, s, n);
	write(2, "\r\n", 2);
	abort();
}

long
ldlong(p)
register char	*p;
{
	long	l;

	l = ((((((long)p[0] << 8) + (p[1] & 0377)) << 8) + (p[2] & 0377)) << 8) + (p[3] & 0377);
	if (l == 0)
		l = l;
	return(l);
}

stlong(l, p)
register long	l;
register char	*p;
{
	p[0] = l >> 24;
	p[1] = l >> 16;
	p[2] = l >> 8;
	p[3] = l;
}

float
ldfloat(p)
register char	*p;
{
	float	f;
	register char	*q;
	register int	i;

	q = (char *)&f;
	i = 4;
	do
		*q++ = *p++;
	while (--i);
	return(f);
}

stfloat(f, p)
float	f;
register char	*p;
{
	register char	*q;
	register int	i;
	float	g;

	g = f;
	q = (char *)&g;
	i = 4;
	do
		*p++ = *q++;
	while (--i);
}

double
lddbl(p)
register char	*p;
{
	double	d;
	register char	*q;
	register int	i;

	q = (char *)&d;
	i = 8;
	do
		*q++ = *p++;
	while (--i);
	return(d);
}

stdbl(d, p)
double	d;
register char	*p;
{
	register char	*q;
	register int	i;
	double	e;

	e = d;
	q = (char *)&e;
	i = 8;
	do
		*p++ = *q++;
	while (--i);
}

long
longmult(a, b)
long	a, b;
{
	long	r;

	r = 0;
	while (b != 0 && a != 0) {
		if ((b & 1) != 0)
			r += a;
		a <<= 1;
		b >>= 1;
	}
	return(r);
}

ckfilname(name)
char	*name;
{
	register char	*p, *q;

	p = q = name;
	while (*p)
		if (*p++ == '/')
			q = p;
	if (stleng(name) >= 59 || stleng(q) > 10 || *q == 0) {
		iserrno = 114;
		return(-1);
	}
	return(0);
}

mkidxname(name, buf)
register char	*name, *buf;
{
	stcopy(name, buf);
	stcat(".idx", buf);
}

mkdatname(name, buf)
register char	*name, *buf;
{
	stcopy(name, buf);
	stcat(".dat", buf);
}
