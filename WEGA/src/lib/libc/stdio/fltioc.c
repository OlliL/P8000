/* _inflt, gcvt, fegcvt, _fegcvt: reconstructed from the member of /lib/libc.a */

#include <stdio.h>
#include <ctype.h>

extern double atof();
extern char *ecvt();
char *_fegcvt();

_fltuse() {}

_inflt(ptr, len, size, iop, eofptr)
int **ptr;
int *eofptr;
FILE *iop;
{
	register char *np, *q;
	char c;
	register ndigit, expseen, dotseen;
	char numbuf[128];

	np = numbuf;
	ndigit = expseen = dotseen = 0;
	if (len > 126)
		len = 126;
	while ((c = getc(iop)) == ' ' || c == '\t' || c == '\n')
		;
	if (c == '-') {
		*np++ = c;
		c = getc(iop);
		len--;
	} else if (c == '+') {
		len--;
		c = getc(iop);
	}
	if (c == 'I' || c == 'N') {
		q = c == 'I' ? "INFINITY" : "NAN(~~~~~~)";
		while (--len >= 0) {
			if (np[-1] == ')')
				break;
			if (*q == 0)
				break;
			if (*q != c && *q != '~')
				break;
			*np++ = c;
			q++;
			c = getc(iop);
		}
	} else
		for ( ; --len >= 0; *np++ = c, c = getc(iop)) {
			if (isdigit(c))
				ndigit++;
			else if (c == '.' && !dotseen && !expseen)
				dotseen++;
			else if ((c == 'e' || c == 'E') && !expseen && ndigit) {
				expseen++;
				*np++ = c;
				c = getc(iop);
				if (c != '+' && c != '-' && (c < '0' || '9' < c))
					break;
			} else
				break;
		}
	if (c != EOF) {
		ungetc(c, iop);
		*eofptr = 0;
	} else
		*eofptr = 1;
	if (ptr == NULL || np == numbuf)
		return(0);
	*np++ = 0;
	switch (size) {
	case 0:
	case 1:
		**(float **)ptr = atof(numbuf);
		break;
	case 2:
		**(double **)ptr = atof(numbuf);
		break;
	}
	return(1);
}

char *
gcvt(number, ndigit, buf)
double number;
char *buf;
{
	return(_fegcvt(number, ndigit, buf, 'g'));
}

char *
fegcvt(number, ndigit, buf, c)
double number;
char *buf;
char c;
{
	return(_fegcvt(number, ndigit, buf, c));
}

char *
_fegcvt(number, ndigit, buf, c)
double number;
char *buf;
char c;
{
	register char *b, *p;
	register i, mode;
	int decpt, sign;

	b = buf;
	if (ndigit < 0)
		ndigit = 6;
	mode = c == 'f' ? -ndigit : ndigit + 1;
	p = ecvt(number, mode, &decpt, &sign);
	if (sign)
		*b++ = '-';
	if (*p > '9') {
		strcpy(b, p);
		return(buf);
	}
	if (c == 'g' && -ndigit / 2 <= decpt && (ndigit < 6 ? 6 : ndigit) >= decpt && decpt + ndigit <= 19) {
		mode = -ndigit;
		p = ecvt(number, mode, &decpt, &sign);
	}
	if (*p == '0' && p[1] == 0) {
		if (c == 'g') {
			mode = 0;
one:
			i = 1;
digits:
			while (i > 0 && *p) {
				*b++ = *p++;
				i--;
			}
			goto zeros;
		}
		if (c == 'f')
			i = 1 - ndigit;
		else {
			i = 1 - ndigit;
			decpt = 1;
		}
	} else if (mode > 0)
		goto one;
	else
		i = decpt;
	if (i > 0)
		goto digits;
	*b++ = '0';
	*b++ = '.';
	while (i < 0) {
		*b++ = '0';
		i++;
	}
	goto tail;
zeros:
	while (i > 0) {
		*b++ = '0';
		i--;
	}
	*b++ = '.';
tail:
	while (*p) {
		*b++ = *p++;
		i++;
	}
	if (mode > 0) {
		*b++ = 'e';
		decpt--;
		i = _cnvt(b, -10, (long)decpt);
		b[i] = 0;
	} else if (mode < 0) {
		*b = 0;
		if (c == 'g') {
			while (*--b == '0')
				*b = 0;
			if (*b == '.')
				*b = 0;
		}
	} else {
		--b;
		*b = 0;
	}
	return(buf);
}
