/* @[$] doscan.c	2.1  09/12/83 11:06:06 - 87wega3.2 */

#include <stdio.h>
#include	<ctype.h>

#define	SPC	01
#define	STP	02

#define	SHORT	0
#define	REGULAR	1
#define	LONG	2
#define	INT	0
#define	FLOAT	1

char	*getccl();

char	_sctab[128] = {
	0,0,0,0,0,0,0,0,
	0,SPC,SPC,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	SPC,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
};

_doscan(iop, fmt, argp, stackp)
FILE *iop;
register char *fmt;
register int **argp;
int **stackp;
{
	register int ch;
	int nmatch, len, ch1;
	int **ptr, fileended, size, nreg;

	nmatch = 0;
	fileended = 0;
	nreg = 0;
	for (;;) switch (ch = *fmt++) {
	case '\0': 
		return (nmatch);
	case '%': 
		if ((ch = *fmt++) == '%')
			goto def;
		ptr = 0;
		if (ch != '*') {
			ptr = argp++;
			if (++nreg == 5)
				argp = stackp;
		} else
			ch = *fmt++;
		len = 0;
		size = REGULAR;
		while (isdigit(ch)) {
			len = len*10 + ch - '0';
			ch = *fmt++;
		}
		if (len == 0)
			len = 30000;
		if (ch=='l') {
			ch = *fmt++;
			size = LONG;
		} else if (ch=='h') {
			size = SHORT;
			ch = *fmt++;
		} else if (ch=='[')
			fmt = getccl(fmt);
		if (isupper(ch)) {
			ch = tolower(ch);
			size = LONG;
		}
		if (ch == '\0')
			return(-1);
		if (innum(ptr, ch, len, size, iop, &fileended) && ptr)
			nmatch++;
		if (fileended)
			return(nmatch? nmatch: -1);
		break;

	case ' ':
	case '\n':
	case '\t': 
		while ((ch1 = getc(iop))==' ' || ch1=='\t' || ch1=='\n')
			;
		if (ch1 != EOF)
			ungetc(ch1, iop);
		break;

	default: 
	def:
		ch1 = getc(iop);
		if (ch1 != ch) {
			if (ch1==EOF)
				return(-1);
			ungetc(ch1, iop);
			return(nmatch);
		}
	}
}

innum(ptr, type, len, size, iop, eofptr)
int **ptr, *eofptr;
struct _iobuf *iop;
{
	register c, base;
	int negflg, c1, ndigit;
	long lcval;

	ndigit = 0;
	if (type=='c' || type=='s' || type=='[')
		return(instr(ptr? *(char **)ptr: (char *)NULL, type, len, iop, eofptr));
	if (type=='e'||type=='f')
		return(_inflt(ptr, len, size, iop, eofptr));
	lcval = 0;
	base = 10;
	if (type=='o')
		base = 8;
	else if (type=='x')
		base = 16;
	negflg = 0;
	while ((c = getc(iop))==' ' || c=='\t' || c=='\n');
	if (c=='-') {
		negflg++;
		c = getc(iop);
		len--;
	} else if (c=='+') {
		len--;
		c = getc(iop);
	}
	for ( ; --len>=0; c = getc(iop)) {
		if (isdigit(c)
		 || base==16 && ('a'<=c && c<='f' || 'A'<=c && c<='F')) {
			ndigit = 1;
			if (base==8)
				lcval <<=3;
			else if (base==10)
				lcval = ((lcval<<2) + lcval)<<1;
			else
				lcval <<= 4;
			c1 = c;
			if ('0'<=c && c<='9')
				c -= '0';
			else if ('a'<=c && c<='f')
				c -= 'a'-10;
			else
				c -= 'A'-10;
			lcval += c;
			c = c1;
			continue;
		} else
			break;
	}
	if (negflg)
		lcval = -lcval;
	if (c != EOF) {
		ungetc(c, iop);
		*eofptr = 0;
	} else
		*eofptr = 1;
	if (ptr==NULL)
		return(0);
	switch(size) {

	case SHORT:
		**(short **)ptr = lcval;
		break;

	case REGULAR:
		**(int **)ptr = lcval;
		break;

	case LONG:
		**(long **)ptr = lcval;
		break;
	}
	if (ndigit)
		return(1);
	return(0);
}

instr(ptr, type, len, iop, eofptr)
register char *ptr;
register struct _iobuf *iop;
int *eofptr;
{
	register ch;
	register char *optr;
	int ignstp;

	*eofptr = 0;
	optr = ptr;
	if (type=='c' && len==30000)
		len = 1;
	ignstp = 0;
	if (type=='s')
		ignstp = SPC;
	while (_sctab[ch = getc(iop)] & ignstp)
		if (ch==EOF)
			break;
	ignstp = SPC;
	if (type=='c')
		ignstp = 0;
	else if (type=='[')
		ignstp = STP;
	while (ch!=EOF && (_sctab[ch]&ignstp)==0) {
		if (ptr)
			*ptr++ = ch;
		if (--len <= 0)
			break;
		ch = getc(iop);
	}
	if (ch != EOF) {
		if (len > 0)
			ungetc(ch, iop);
		*eofptr = 0;
	} else
		*eofptr = 1;
	if (ptr && ptr!=optr) {
		if (type!='c')
			*ptr++ = '\0';
		return(1);
	}
	return(0);
}

char *
getccl(s)
register char *s;
{
	register c, t;


	t = 0;
	if (*s == '^') {
		t++;
		s++;
	}
	for (c = 0; c < 128; c++)
		if (t)
			_sctab[c] &= ~STP;
		else
			_sctab[c] |= STP;
	while (((c = *s++)&0177) != ']') {
		if (t)
			_sctab[c++] |= STP;
		else
			_sctab[c++] &= ~STP;
		if (c==0)
			return(--s);
	}
	return(s);
}
