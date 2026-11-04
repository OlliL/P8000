#include	<stdio.h>

#undef	putc
#define	putc(x,p) (--(p)->_cnt>=0? (int)(*(char *)(p)->_ptr++=(x)): _flsbuf((x),p))

static	char nullstr[] = "(null)";

union lw {
	long	l;
	unsigned w[2];
};

char	*_cnvt();
char	*_fegcvt();

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

_doprnt(fmt, argp, stkap, nreg, file)
register unsigned char *fmt;
int *argp;
int *stkap;
int nreg;
FILE *file;
{
	register c, fmtlen, len;
	int *ap;
	int width, prec, base;
	union lw val;
	char *str;
	char buf[64];
	int left, pad;
	char fill;
	int conv;

	ap = argp;
	fmtlen = _length(fmt);
	while (fmtlen--) {
		if ((c = *fmt++) != '%') {
			putc(c, file);
			continue;
		}
		if (fmtlen == 0)
			break;
		fmtlen--;
		c = *fmt++;
		width = 0;
		prec = -1;
		fill = ' ';
		left = 0;
		if (c == '-') {
			left++;
			if (fmtlen-- == 0)
				break;
			c = *fmt++;
		}
		if (c < 'a')
		while (c == '*' || (c >= '0' && c <= '9') || c == '.') {
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
				fmtlen -= len;
				fmt += len;
			}
			if (fmtlen == 0)
				break;
			fmtlen--;
			c = *fmt++;
		}
		if (prec < 0) {
			switch (c) {
			case 's':
				prec = 0;
				break;
			case 'e': case 'f': case 'g':
				prec = 6;
			}
		}
		conv = c;
		switch (c) {
		case 'o':
			base = 8;
			goto uns;
		case 'x':
			base = 16;
			goto uns;
		case 'u':
			base = 10;
		uns:
			val.l = *ap++;
			val.w[0] = 0;
			if (++nreg == 5)
				ap = stkap;
			goto conv_int;
		case 'l':
			if (fmtlen == 0)
				goto fin;
			fmtlen--;
			if ((c = *fmt++) == 'o') {
		case 'O':
				base = 8;
			} else if (c == 'x') {
		case 'X':
				base = 16;
			} else if (c == 'd') {
		case 'D':
				base = -10;
			} else if (c == 'u') {
		case 'U':
				base = 10;
			} else {
				putc('l', file);
				putc(c, file);
				goto fin;
			}
			if (nreg < 5)
				_align(&nreg, &ap, stkap);
			val.l = *(long *)ap;
			ap += 2;
			nreg += 2;
			if (nreg == 5)
				ap = stkap;
			goto conv_int;
		case 'd':
			base = -10;
			val.l = *ap++;
			if (++nreg == 5)
				ap = stkap;
		conv_int:
			str = buf;
			if (base == -10 && val.l < 0) {
				if (val.l == 0x80000000)
					base = 10;
				val.l *= -1;
				putc('-', file);
			}
			len = _cnvt(str, base, val.l);
			break;
		case 's':
			fill = ' ';
			str = (char *)*ap++;
			if (str == 0)
				str = nullstr;
			len = _length(str);
			if (++nreg == 5)
				ap = stkap;
			break;
		case 'c':
			fill = ' ';
			len = 1;
			str = (char *)ap++;
			str++;
			if (++nreg == 5)
				ap = stkap;
			break;
		case 'e': case 'f': case 'g':
			if (nreg < 5)
				ap = stkap;
			str = _fegcvt(*(double *)ap, prec, buf, c);
			ap += 4;
			nreg += 5;
			len = _length(str);
			break;
		default:
			putc(c, file);
			continue;
		}
	fin:
		if (prec && conv == 's' && len > prec)
			len = prec;
		if (len < width)
			pad = width - len;
		else
			pad = 0;
		if (!left)
			while (pad--)
				putc(fill, file);
		while (len--)
			putc(*str++, file);
		if (left)
			while (pad--)
				putc(fill, file);
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
