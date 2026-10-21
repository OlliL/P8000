/*
 * _doprnt, Jan-1983 revision (as embedded in size/strip/ranlib).
 * Reconstructed from the object code: the argument list arrives split
 * in register arguments, floating point register arguments and stack
 * arguments (see printf.c).  The float conversion is a stub in this
 * revision.
 */
#include	<stdio.h>

union lw {
	long	l;
	unsigned w[2];
};

static getnum(), fix(), ecv(), fcv(), gcv();

/*
 * a long argument needs an even register pair
 */
static
fix(np, app, stkap)
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
	} else {
		tmp = **app;
		**app = (*app)[1];
		(*app)[1] = tmp;
	}
}

_doprnt(fmt, argp, fpap, stkap, nreg, file)
register char *fmt;
int *argp;
char *fpap;
int *stkap;
int nreg;
FILE *file;
{
	register c, fmtlen, len;
	int x1, x2, x3;
	int fpn;
	int *ap, *sap;
	int width, prec, base;
	union lw val;
	char *str;
	char buf[64];
	int left, pad;
	char fill;
	int conv;
	char *_ltostr();
	
	fpn = 0;
	ap = argp;
	fmtlen = strlen(fmt);
	while (fmtlen--) {
		c = *fmt++ & 0377;
		if (c != '%') {
			putc(c, file);
			continue;
		}
		if (fmtlen == 0)
			break;
		fmtlen--;
		c = *fmt++ & 0377;
		width = 0;
		prec = -1;
		fill = ' ';
		left = 0;
		if (c == '-') {
			left++;
			if (fmtlen-- == 0)
				break;
			c = *fmt++ & 0377;
		}
		if (c < 'a') {
			if (c == '*') {
				width = *ap++;
				if (++nreg == 5)
					ap = stkap;
			} else {
				if (c == '0')
					fill = '0';
				len = getnum(--fmt, ++fmtlen, &width);
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
					len = getnum(fmt, fmtlen, &prec);
				fmtlen -= len;
				fmt += len;
			}
			if (fmtlen == 0)
				break;
			fmtlen--;
			c = *fmt++ & 0377;
		}
		if (prec < 0) {
			switch (c) {
			case 'e': case 'f': case 'g':
				prec = 6;
				break;
			case 's':
				prec = 0;
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
		case 'd':
			base = -10;
			val.l = *ap++;
			if (++nreg == 5)
				ap = stkap;
			goto conv_int;
		case 'l':
			if (fmtlen == 0)
				goto fin;
			fmtlen--;
			c = *fmt++ & 0377;
			if (c == 'o')
				base = 8;
			else if (c == 'x')
				base = 16;
			else if (c == 'd')
				base = -10;
			else if (c == 'u')
				base = 10;
			else {
				putc('l', file);
				putc(c, file);
				goto fin;
			}
			goto lng;
		case 'O':
			base = 8;
			goto lng;
		case 'X':
			base = 16;
			goto lng;
		case 'D':
			base = -10;
			goto lng;
		case 'U':
			base = 10;
		lng:
			if (nreg < 5)
				fix(&nreg, &ap, stkap);
			val.l = *(long *)ap;
			ap += 2;
			nreg += 2;
			if (nreg == 5)
				ap = stkap;
		conv_int:
			str = buf;
			len = _ltostr(str, base, val.l);
			break;
		case 's':
			fill = ' ';
			str = (char *)*ap++;
			if (str == 0)
				str = "(null)";
			len = strlen(str);
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
			if (nreg < 5) {
				if (fpn < 4) {
					sap = ap;
					ap = (int *)fpap;
					fpap += 8;
					fpn++;
				} else if (fpn == 4) {
					sap = 0;
					ap = stkap;
					nreg += 5;
				}
			} else
				sap = 0;
			str = fcv(*(double *)ap, prec, conv, buf);
			if (sap)
				ap = sap;
			else
				ap += 4;
			len = strlen(str);
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


static
getnum(str, len, result)
register char *str;
int len;
int *result;
{
	register left, n;
	int neg;
	char c;

	left = len;
	neg = 0;
	if (len && *str == '-') {
		len--;
		str++;
		neg = 1;
	}
	n = 0;
	while (left) {
		c = *str++;
		if (c < '0' || c > '9')
			break;
		n = n*10 + (c - '0');
		left--;
	}
	*result = neg ? -n : n;
	return(len - left);
}

static ecv() {}
static fcv() {}
static gcv() {}
