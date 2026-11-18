#define SSIZE	50
#define TGRP	48
#define A256	02
#define ZERO	01
#define	NBRA	10
#define CIRCFL	32;
#define SLOP	5
#define	EOF	0

#define	CBRA	60
#define GRP	40
#define SGRP	56
#define PGRP	68
#define EGRP	44
#define RNGE	03
#define	CCHR	20
#define	CDOT	64
#define	CCL	24
#define	NCCL	8
#define	CDOL	28
#define	CEOF	52
#define	CKET	12

#define	STAR	01
#define PLUS	02
#define MINUS	16

#ifndef SEG
#define	PINT	int
#define	NREGS	6
#else
#define	PINT	long
#define	NREGS	3
#endif

PINT	*__sp_;
PINT	__rpop();
char	*malloc();
char	*__stmax;
int	__i_size;
char *
regcmp(cs1,cs2,cs3,cs4,cs5,cs6,cs7,cs8,cs9,cs10) 
char *cs1,*cs2,*cs3,*cs4,*cs5,*cs6,*cs7,*cs8,*cs9,*cs10;
{
#ifndef SEG
	char *regs[6];
#else
	struct { char *r0, *r1, *r2; } regs;
#endif
#ifndef SEG
#define	REGS	(PINT *)regs
#else
#define	REGS	(PINT *)&regs
#endif
	register int n, f;
	register c;
	register char *ep, *sp;
	PINT *adx;
	int i,cflg;
	char *lastep, *sep, *eptr;
	char bracket[NBRA], *bracketp;
	int nbra,ngrp;
	int cclcnt;
	char stack[SSIZE];

	__sp_ = (PINT *)stack;
	*__sp_ = -1;
	__stmax = &stack[SSIZE];

#ifndef SEG
	regs[0] = cs1;
	regs[1] = cs2;
	regs[2] = cs3;
	regs[3] = cs4;
	regs[4] = cs5;
	regs[5] = cs6;
#else
	regs.r0 = cs1;
	regs.r1 = cs2;
	regs.r2 = cs3;
#endif
	adx = REGS;
	i = nbra = ngrp = 0;
	n = f = 0;
	while(*adx) {
		i += __size(*adx++);
		if (++n > NREGS && !f) {
			f = 1;
#ifndef SEG
			adx = (PINT *)&regs[5];
#else
			adx = (PINT *)&cs4;
#endif
		}
	}
	n = f = 0;
	adx = REGS;
	sp = (char *)*adx++;
	n++;
	if((sep = ep = (char *)malloc(2*i+SLOP)) == (char *)0)
		return(0);
	bracketp = bracket;
	if ((c = *sp++) == EOF) goto cerror;
	if (c=='^') {
		c = *sp++;
		*ep++ = CIRCFL;
	}
	if ((c=='*') || (c=='+') || (c=='{'))
		goto cerror;
	sp--;
	for (;;) {
		if ((c = *sp++) == EOF) {
			if (*adx) {
				sp = (char *)*adx++;
				if (++n > NREGS && !f) {
					f = 1;
#ifndef SEG
					adx = (PINT *)&regs[5];
#else
					adx = (PINT *)&cs4;
#endif
				}
				continue;
			}
			*ep++ = CEOF;
			if (--nbra > NBRA || *__sp_ != -1)
				goto cerror;
			__i_size = ep - sep;
			return(sep);
		}
		if ((c!='*') && (c!='{')  && (c!='+'))
			lastep = ep;
		switch (c) {

		case '(':
			if (!__rpush(ep)) goto cerror;
			*ep++ = CBRA;
			*ep++ = -1;
			continue;
		case ')':
			if (!(eptr=(char *)__rpop())) goto cerror;
			if ((c = *sp++) == '$') {
				if ('0' > (c = *sp++) || c > '9')
					goto cerror;
				*ep++ = CKET;
				*ep++ = *++eptr = nbra++;
				*ep++ = (c-'0');
				continue;
			}
			*ep++ = EGRP;
			*ep++ = ngrp++;
			sp--;
			switch (c) {
			case '+':
				*eptr = PGRP;
				break;
			case '*':
				*eptr = SGRP;
				break;
			case '{':
				*eptr = TGRP;
				break;
			default:
				*eptr = GRP;
				continue;
			}
			i = ep - eptr - 2;
			for (cclcnt = 0; i >= 256; cclcnt++)
				i -= 256;
			if (cclcnt > 3) goto cerror;
			*eptr |= cclcnt;
			*++eptr = i;
			continue;

		case '\\':
			*ep++ = CCHR;
			if ((c = *sp++) == EOF)
				goto cerror;
			*ep++ = c;
			continue;

		case '{':
			*lastep |= RNGE;
			cflg = 0;
		nlim:
			if ((c = *sp++) == '}') goto cerror;
			i = 0;
			do {
				if ('0' <= c && c <= '9')
					i = (i*10+(c-'0'));
				else goto cerror;
			} while (((c = *sp++) != '}') && (c != ','));
			if (i>255) goto cerror;
			*ep++ = i;
			if (c==',') {
				if (cflg++) goto cerror;
				if((c = *sp++) == '}') {
					*ep++ = -1;
					continue;
				}
				else {
					sp--;
					goto nlim;
				}
			}
			if (!cflg) *ep++ = i;
			else if ((ep[-1]&0377) < (ep[-2]&0377)) goto cerror;
			continue;

		case '.':
			*ep++ = CDOT;
			continue;

		case '+':
			if (*lastep==CBRA || *lastep==CKET)
				goto cerror;
			*lastep |= PLUS;
			continue;

		case '*':
			if (*lastep==CBRA || *lastep==CKET)
			goto cerror;
			*lastep |= STAR;
			continue;

		case '$':
			if ((*sp != EOF) || (*adx))
				goto defchar;
			*ep++ = CDOL;
			continue;

		case '[':
			*ep++ = CCL;
			*ep++ = 0;
			cclcnt = 1;
			if ((c = *sp++) == '^') {
				c = *sp++;
				ep[-2] = NCCL;
			}
			do {
				if (c==EOF)
					goto cerror;
				if ((c=='-') && (cclcnt>1) && (*sp!=']')) {
					*ep++ = ep[-1];
					ep[-2] = MINUS;
					cclcnt++;
					continue;
				}
				*ep++ = c;
				cclcnt++;
			} while ((c = *sp++) != ']');
			lastep[1] = cclcnt;
			continue;

		defchar:
		default:
			*ep++ = CCHR;
			*ep++ = c;
		}
	}
   cerror:
	free(sep);
	return(0);
}
__size(strg) char *strg;
{
	int	i;

	i = 1;
	while(*strg++) i++;
	return(i);
}
PINT __rpop() {
	return (*__sp_ == -1)?0:*__sp_--;
}
__rpush(ptr) char *ptr;
{
	if (++__sp_ > (PINT *)__stmax) return(0);
	*__sp_ = (PINT)ptr;
	return(1);
}
