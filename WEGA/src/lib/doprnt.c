/*
 * _doprnt of /lib/doprnt.o and /lib/sdoprnt.o (not the member of libc.a: %X in upper case through _hexchar of cnvt.c,
 * no %O %D %U), written from the objects; the base is src/lib/oldlibc/doprnt/0_6.c. v1 without -O (ccv1, sccv1 -DSEG),
 * the stdio.h of src/head/oldv7. The segmented object comes from another revision: SEG decides the frame order of count,
 * the long pointer of %s, the last float argument and the test of _pfile (as in the doprnt.c of libc).
 */

#include	<stdio.h>

#undef	putc
#define	putc(x,p) (--(p)->_cnt>=0? (int)(*(char *)(p)->_ptr++=(x)): _flsbuf((x),p))

static	char nullstr[] = "(null)";

union lw {
	long	l;
	unsigned w[2];
};

#ifndef SEG
char	*_cnvt();
#endif /* ! SEG */
char	*_fegcvt();
extern char _hexchar;

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

FILE	*_pfile;

_doprnt(fmt, argp, fpap, stkap, nreg, file)
register unsigned char *fmt;
int *argp;
char *fpap;
int *stkap;
int nreg;
FILE *file;
{
	int fpn;
	int *ap, *sap;
	register c, fmtlen, len;
	int width, prec, base;
	union lw val;
	char *str;
	char buf[64];
	char *p;
	char c1, c2, sign;
	char *p2;
#ifndef SEG
	int left, pad, count, alt, space, plus, n;
#else
	int left, pad, alt, space, plus, n;
	int count;
#endif
	char fill;
	int conv;

	fpn = 0;
	count = 0;
	ap = argp;
	fmtlen = _length(fmt);
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
		_hexchar = 'x';
	loop:
		switch (c) {
		case '-':
			if (left == 0)
				left = 1;
			else
				break;
			goto next;
		case '+':
			if (plus == 0)
				plus = 1;
			else
				break;
			goto next;
		case ' ':
			if (space == 0)
				space = 1;
			else
				break;
			goto next;
		case '#':
			if (alt == 0)
				alt = 1;
			else
				break;
			goto next;
		next:
			if (fmtlen-- == 0)
				break;
			c = *fmt++;
			goto loop;
		}
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
		case 'X':
			base = 16;
			_hexchar = 'X';
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
			if ((c = *fmt++) == 'o')
				base = 8;
			else if (c == 'x')
				base = 16;
			else if (c == 'd')
				base = -10;
			else if (c == 'u')
				base = 10;
			else if (c == 'X') {
				_hexchar = 'X';
				base = 16;
			} else {
				putc('l', file);
				putc(c, file);
				count += 2;
				goto fin;
			}
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
			base = -10;
			val.l = *ap++;
			if (++nreg == 5)
				ap = stkap;
		conv_int:
			if (plus && base == -10 && val.l >= 0) {
				buf[0] = sign;
				p = buf + (n = 1);
			} else if (base == 8 && alt && val.l != 0) {
				buf[0] = '0';
				p = buf + (n = 1);
			} else if (base == 16 && alt && val.l != 0) {
				buf[0] = '0';
				buf[1] = _hexchar;
				p = buf + (n = 2);
			} else
				p = buf;
			if (base == -10 && val.l < 0) {
				if (val.l == 0x80000000)
					base = 10;
				val.l *= -1;
				putc('-', file);
				count++;
			}
			str = buf;
			len = (int) _cnvt(p, base, val.l) + n;
			break;
		case 's':
			fill = ' ';
#ifndef SEG
			str = (char *)*ap++;
#else
			if (nreg < 5)
				_align(&nreg, &ap, stkap);
			str = *(char **)ap;
			ap += 2;
#endif
			if (str == 0)
				str = nullstr;
			len = _length(str);
#ifndef SEG
			if (++nreg == 5)
#else
			nreg += 2;
			if (nreg == 5)
#endif
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
			if (fpn < 4) {
				sap = ap;
				ap = (int *)fpap;
				fpap += 8;
				fpn++;
#ifndef SEG
			} else if (fpn == 4) {
#else
			} else {
#endif
				sap = 0;
				ap = stkap;
				nreg += 5;
			}
			str = _fegcvt(*(double *)ap, prec, buf, c);
			if (sap)
				ap = sap;
			else
				ap += 4;
			if (plus && *str != '-' && *str) {
				p = str;
				c1 = *p;
				*p++ = sign;
				while (c1) {
					c2 = *p;
					*p++ = c1;
					c1 = c2;
				}
				*p = 0;
			}
			if (!alt) {
				if (c == 'e' || c == 'E') {
					for (p = str; *p; p++)
						if (*p == '.' && *++p == 'e') {
							p--;
							while (*p) {
								*p = p[1];
								p++;
							}
							break;
						}
				} else if (c == 'g' || c == 'G') {
					for (p = str; *p; p++)
						if (*p == '.') {
							p2 = p++;
							while (*p == '0')
								p++;
							if (*p == 'e')
								strcpy(p2, p);
							break;
						}
				}
			} else {
				if (c == 'f') {
					p = str;
					while (*p && *p != '.')
						p++;
					if (*p == 0) {
						*p++ = '.';
						*p = 0;
					}
				} else if (c == 'g' || c == 'G') {
					p = str;
					while (*p && *p != '.')
						p++;
					if (*p == 0) {
						*p++ = '.';
						n = 0;
						while (n < prec) {
							*p++ = '0';
							n++;
						}
						*p = 0;
					}
				}
			}
			if (c == 'E' || c == 'G') {
				p = str;
				while (*p) {
					if (*p == 'e') {
						*p = 'E';
						break;
					}
					p++;
				}
			}
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
			putc((int)*str++, file);
			count++;
		}
		if (left)
			while (pad--) {
				putc(fill, file);
				count++;
			}
	}
#ifndef SEG
	return(_pfile && ferror(_pfile) ? EOF : count);
#else
	return(_pfile != NULL && ferror(_pfile) ? EOF : count);
#endif
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
