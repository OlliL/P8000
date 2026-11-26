static char sccsid[] = "@[$]printf.c\t2.1  08/10/83 15:38:40 - 87wega3.2";

/*
 * printf of csh (WEGA): the printf of ls, through putchar (integers and strings only)
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
			putchar(c);
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
			putchar(c);
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
				putchar(fill);
		if (zero)
			putchar('0');
		while (len--)
			putchar(*str++);
		if (left)
			while (pad--)
				putchar(fill);
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
