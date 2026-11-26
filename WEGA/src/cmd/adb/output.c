static char sccsid[] = "@[$]output.c\t3.2  08/25/83 09:48:56 - 87wega3.2";

#include "defs.h"




char	*_fegcvt();

union lw {
	long	l;
	unsigned w[2];
};

eqstr(s1, s2)
	REG STRING	s1, s2;
{
	WHILE *s1++ == *s2
	DO IF *s2++ == 0
	   THEN return(1);
	   FI
	OD
	return(0);
}

length(s)
	STRING		s;
{
	INT		n = 0;

	WHILE *s++ DO n++; OD
	return(n);
}

printc2(c, sp)
	CHAR		c;
	STRING		*sp;
{
	CHAR		d;
	STRING		q;
	INT		posn, tabs, p;

	IF mkfault
	THEN	return;
	ELIF sp==0
	THEN IF (*printptr=c)==EOR
	     THEN tabs=0; posn=0; q=printbuf;
		  FOR p=0; p<printptr-printbuf; p++
		  DO d=printbuf[p];
		     IF (p&7)==0 ANDF posn
		     THEN tabs++; posn=0;
		     FI
		     IF d==SP
		     THEN posn++;
		     ELSE WHILE tabs>0 DO *q++=TB; tabs--; OD
			  WHILE posn>0 DO *q++=SP; posn--; OD
			  *q++=d;
		     FI
		  OD
		  *q++=EOR;
		  write(outfile,printbuf,q-printbuf);
		  printptr=printbuf;
	     ELIF c==TB
	     THEN REP *printptr++=SP; PER (printptr-printbuf)&7 DONE
	     ELIF c
	     THEN printptr++;
	     FI
	ELSE	**sp = c;
		(*sp)++;
	FI
}

sprintf(buf, fmt, a1, a2, a3, a4, d1, d2, d3, d4, args)
STRING	buf, fmt;
double	d1, d2, d3, d4;
{
	int	regs[4];
	STRING	bp;
	double	fregs[4];

	bp = buf;
	regs[0] = a1;
	regs[1] = a2;
	regs[2] = a3;
	regs[3] = a4;
	fregs[0] = d1;
	fregs[1] = d2;
	fregs[2] = d3;
	fregs[3] = d4;
	_doprnt(fmt, regs, fregs, &args, 1, &bp);
	*bp = 0;
	return(buf);
}

printf(fmt, a1, a2, a3, a4, a5, d1, d2, d3, d4, args)
STRING	fmt;
double	d1, d2, d3, d4;
{
	int	regs[5];
	double	fregs[4];

	regs[0] = a1;
	regs[1] = a2;
	regs[2] = a3;
	regs[3] = a4;
	regs[4] = a5;
	fregs[0] = d1;
	fregs[1] = d2;
	fregs[2] = d3;
	fregs[3] = d4;
	_doprnt(fmt, regs, fregs, &args, 0, 0);
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

_doprnt(fmt, argp, fpap, stkap, nreg, sp)
register unsigned char *fmt;
int *argp;
char *fpap;
int *stkap;
int nreg;
{
	int fpn;
	int *ap, *sap;
	register c, fmtlen, len;
	int width, prec, base;
	union lw val;
	char *str;
	char buf[64];
	int left, pad;
	char fill;
	int conv;

	fpn = 0;
	ap = argp;
	fmtlen = length(fmt);
	while (fmtlen--) {
		if ((c = *fmt++) != '%') {
			printc2(c, sp);
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
		if (c == '-') {
			left++;
			if (fmtlen-- == 0)
				break;
			c = *fmt++;
		}
		if (c < 'a') {
			if (c == '0')
				fill = '0';
			len = _atoip(--fmt, ++fmtlen, &width);
			fmt += len;
			fmtlen -= len;
			if (fmtlen == 0)
				break;
			if (*fmt == '.') {
				fmt++;
				fmtlen--;
				if (fmtlen == 0)
					break;
				len = _atoip(fmt, fmtlen, &prec);
				fmtlen -= len;
				fmt += len;
			}
			if (fmtlen == 0)
				break;
			fmtlen--;
			c = *fmt++;
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
		conv_int:
			str = buf;
			len = _acnvt(str, base, val.l);
			break;
		case 'O':
			base = 8;
		lng:
			if (nreg < 5)
				_align(&nreg, &ap, stkap);
			val.l = *(long *)ap;
			ap += 2;
			nreg += 2;
			if (nreg == 5)
				ap = stkap;
			goto conv_int;
		case 'X':
			base = 16;
			goto lng;
		case 'U':
			base = 10;
			goto lng;
		case 'D':
			base = -10;
			goto lng;
		case 'd':
			base = -10;
			val.l = *ap++;
			if (++nreg == 5)
				ap = stkap;
			goto conv_int;
		case 'Y':
			if (nreg < 5)
				_align(&nreg, &ap, stkap);
			val.l = *(long *)ap;
			ap += 2;
			nreg += 2;
			if (nreg == 5)
				ap = stkap;
			str = buf;
			len = _date(str, val.l);
			break;
		case 's':
			fill = ' ';
			str = (char *)*ap++;
			len = length(str);
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
		case 'T': case 't':
			if (c == 'T')
				width = *ap++;
			if (width)
				width -= charpos()%width;
			len = 0;
			break;
		case 'M': case 'm':
			if (c == 'M')
				width = *ap++;
			len = 0;
			break;
		case 'F': case 'f':
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
			str = _fegcvt(*(double *)ap, prec, buf, c);
			if (sap)
				ap = sap;
			else
				ap += 4;
			len = length(str);
			break;
		default:
			printc2(c, sp);
			continue;
		}
		if (prec && conv == 's' && len > prec)
			len = prec;
		if (len < width)
			pad = width - len;
		else
			pad = 0;
		if (!left)
			while (pad--)
				printc2(fill, sp);
		while (len--)
			printc2(*str++, sp);
		if (left)
			while (pad--)
				printc2(fill, sp);
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

_acnvt(buf, base, val)
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
		} while (u >>= 3);
		goto rev;
	case 16:
		do {
			*p = (u&15) + '0';
			if (*p > '9')
				*p += 'a' - ':';
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

_date(buf, tvec)
	STRING		buf;
	L_INT		tvec;
{
	REG INT		i;
	REG STRING	timeptr;
	REG STRING	p;
	INT		n;

	p = buf;
	timeptr = ctime(&tvec);
	FOR i=20; i<24; i++ DO *p++ = *(timeptr+i); OD
	FOR i=3; i<19; i++ DO *p++ = *(timeptr+i); OD
	return(20);
}

charpos()
{	return(printptr-printbuf);
}

flushbuf()
{	IF printptr!=printbuf
	THEN printc2(EOR, 0);
	FI
}

printdate(tvec)
	L_INT		tvec;
{
	REG INT		i;
	REG STRING	timeptr;

	timeptr = ctime(&tvec);
	FOR i=20; i<24; i++ DO *digitptr++ = *(timeptr+i); OD
	FOR i=3; i<19; i++ DO *digitptr++ = *(timeptr+i); OD
} /*printdate*/

prints(s)
char *s;
{	printf("%s",s);
}

newline()
{
	printc2(EOR, 0);
}

iclose()
{
	IF infile
	THEN	close(infile); infile=0;
	FI
}

oclose()
{
	IF outfile!=1
	THEN	flushbuf(); close(outfile); outfile=1;
	FI
}

endline()
{
	IF charpos()>=maxpos
	THEN printf("\n");
	FI
}
