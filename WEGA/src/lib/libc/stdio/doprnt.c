/* @[$] doprnt.c	2.3  11/17/83 10:26:03 - 87wega3.2 */

#include	<stdio.h>

#undef	putc
#define	putc(x,p) (--(p)->_cnt>=0? (int)(*(char *)(p)->_ptr++=(x)): _flsbuf((x),p))

FILE	*_pfile;

static	int fldone;

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
	} else {
		tmp = **app;
		**app = (*app)[1];
		(*app)[1] = tmp;
	}
}

_doprnt(fmt, argp, stkap, nreg, file)
register unsigned char *fmt;
int *argp;
int *stkap;
int nreg;
FILE *file;
{
	register c, fmtlen, len;
	int fpn;
	int *ap, *sap;
	int width, prec, base;
	union lw val;
	char *str;
	char buf[64];
	char *p;
	char c1, c2, sign;
	char *p2;
	int left, pad, alt, space, plus, n;
	int count;
	char fbuf[32];
	char *fpap;
	char fill;
	int conv;

	fpn = 0;
	count = 0;
	fpap = fbuf;
	ap = argp;
	fmtlen = _length(fmt);
	fldone = 0;
	while (fmtlen--) {
		if ((c = *fmt++) != '%') {
			putc(c, file);
			count++;
			continue;
		}
		if (fmtlen == 0)
			break;
		fmtlen--;
		c = *fmt++;
		width = 0;
		prec = -1;
		fill = ' ';
		n = 0;
		space = 0;
		alt = 0;
		plus = 0;
		left = 0;
		for (;;) {
			switch (c) {
			case '-':
				if (left)
					goto flagsdone;
				left = 1;
				goto next;
			case '+':
				if (plus)
					goto flagsdone;
				plus = 1;
				goto next;
			case ' ':
				if (space)
					goto flagsdone;
				space = 1;
				goto next;
			case '#':
				if (alt)
					goto flagsdone;
				alt = 1;
				goto next;
			default:
				goto flagsdone;
			next:
				if (fmtlen-- == 0)
					goto flagsdone;
				c = *fmt++;
			}
		}
	flagsdone:
		if (!plus && space) {
			sign = ' ';
			plus = 1;
		} else
			sign = '+';
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
			case 'E': case 'G': case 'e': case 'f': case 'g':
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
			c = *fmt++;
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
				count += 2;
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
				_align(&nreg, &ap, stkap);
			val.l = *(long *)ap;
			ap += 2;
			nreg += 2;
			if (nreg == 5)
				ap = stkap;
		conv_int:
			if (plus && base == -10 && val.l >= 0) {
				buf[0] = sign;
				n = 1;
				p = buf + 1;
			} else if (base == 8 && alt && val.l != 0) {
				buf[0] = '0';
				n = 1;
				p = buf + 1;
			} else if (base == 16 && alt && val.l != 0) {
				buf[0] = '0';
				buf[1] = 'x';
				n = 2;
				p = buf + 2;
			} else
				p = buf;
			str = buf;
			len = _cnvt(p, base, val.l) + n;
			break;
		case 's':
			fill = ' ';
			str = (char *)*ap++;
			if (str == 0)
				str = "(null)";
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
		case 'E': case 'G': case 'e': case 'f': case 'g':
			if (!fldone++)
				_stfl(fbuf);
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
			str = _fegcvt(*(double *)ap, prec, conv, buf);
			if (sap)
				ap = sap;
			else
				ap += 4;
			len = _length(str);
			break;
		default:
			putc(c, file);
			count++;
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
			while (pad--) {
				putc(fill, file);
				count++;
			}
		while (len--) {
			putc(*str++, file);
			count++;
		}
		if (left)
			while (pad--) {
				putc(fill, file);
				count++;
			}
	}
	if (_pfile && ferror(_pfile))
		return(EOF);
	return(count);
}

_atoip(str, len, result)
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
