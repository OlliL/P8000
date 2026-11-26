static char sccsid[] = "@[$]expr.c\t3.2  08/25/83 11:53:38 - 87wega3.2";

#include "defs.h"
#include <ctype.h>

CHAR		isymbol[128];
CHAR		symname[128];


MSG		BADSYM;
MSG		BADVAR;
MSG		BADKET;
MSG		BADSYN;
MSG		NOCFN;
MSG		NOADR;
MSG		BADLOC;
MSG		BADSEG;



expr(a)
{	/* term | term dyadic expr |  */
	INT		rc;
	L_INT		lhs;

	rdc(); lp--; rc=term(a);

	WHILE rc
	DO  lhs = expv.a_offset;

	    switch (readchar()) {

		    case '+':
			term(a|1); expv.a_offset += lhs; break;

		    case '-':
			term(a|1); expv.a_offset = lhs - expv.a_offset; break;

		    case '#':
			term(a|1); expv.a_offset = round(lhs,expv.a_offset); break;

		    case '*':
			term(a|1); expv.a_offset *= lhs; break;

		    case '%':
			term(a|1); expv.a_offset = lhs/expv.a_offset; break;

		    case '&':
			term(a|1); expv.a_offset &= lhs; break;

		    case '|':
			term(a|1); expv.a_offset |= lhs; break;

		    case ')':
			IF (a&2)==0 THEN error(BADKET); FI

		    default:
			lp--;
			return(rc);
	    }
	OD
	return(rc);
}

term(a)
{	/* item | monadic item | (expr) | */
	ADDR		tmp;

	switch (readchar()) {

		    case '*':
			term(a|1);
			tmp.a_segno = expv.a_segno; tmp.a_offset = expv.a_offset+2;
			expv.a_offset=leng(chkget(&expv,DSP));
			IF segflg==1
			THEN expv.a_segno = (shorten(expv.a_offset)>>8)&0177;
			     expv.a_offset=leng(chkget(&tmp,DSP));
			FI
			return(1);

		    case '@':
			term(a|1);
			tmp.a_segno = expv.a_segno; tmp.a_offset = expv.a_offset+2;
			expv.a_offset=leng(chkget(&expv,ISP));
			IF segflg==1
			THEN expv.a_segno = (chkget(&tmp,ISP)>>8)&0177;
			FI
			return(1);

		    case '-':
			term(a|1); expv.a_offset = -expv.a_offset; return(1);

		    case '~':
			term(a|1); expv.a_offset = ~expv.a_offset; return(1);

		    case '(':
			expr(2);
			IF *lp!=')'
			THEN	error(BADSYN);
			ELSE	lp++; return(1);
			FI

		    default:
			lp--;
			return(item(a));
	}
}

item(a)
{	/* name [ . local ] | number | . | ^ | <var | <register | 'x | | */
	INT		base, d, frpt, regptr;
	CHAR		savc;
	BOOL		hex;
	UNION{REAL r; L_INT i;} real;
	SYMPTR		symp;

	hex=FALSE;

	readchar();
	IF symchar(0)
	THEN	readsym();
		IF lastc=='.'
		THEN	error("Sorry, local variable names not implemented");
		ELIF (symp=lookupsym(isymbol))==0
		THEN	IF strlen(isymbol)>7
			THEN	printf("'%s': %s.  Truncated, searching....\n", isymbol, BADSYM);
				isymbol[7]=0;
				IF (symp=lookupsym(isymbol))==0
				THEN error(BADSYM);
				FI
			ELSE	error(BADSYM);
			FI
		FI
		symtype = SYMTYPE(symp->symf);
		itolws.L = symp->symv;
		expv.a_segno = (itolws.I[0]>>8)&0177;
		expv.a_offset = leng(symp->symv&0xffff);
		lp--;

	ELIF isdigit(lastc) ORF (hex=TRUE, lastc=='%' ANDF isxdigit(readchar()))
	THEN	expv.a_offset = 0;
		base = ((ibase&4) ORF hex ? 16 : (lastc == '0' ORF (ibase&1) ? 8 : 10));
		WHILE ((ibase&4) ORF hex ? isxdigit(lastc) : isdigit(lastc))
		DO  expv.a_offset *= base;
		    IF (d=convdig(lastc))>=base THEN error(BADSYN); FI
		    expv.a_offset += d; readchar();
		    IF expv.a_offset==0 ANDF (lastc=='x' ORF lastc=='X')
		    THEN hex=TRUE; base=16; readchar();
		    FI
		OD
		IF lastc=='.' ANDF (base==10 ORF expv.a_offset==0) ANDF !hex
		THEN
			real.r=expv.a_offset; frpt=0; base=10;
			WHILE isdigit(readchar())
			DO	real.r *= base; frpt++;
				real.r += lastc-'0';
			OD
			WHILE frpt--
			DO	real.r /= base; OD
			expv.a_offset = real.i;
		FI
		lp--;

	ELIF lastc=='.'
	THEN	readchar();
		IF symchar(0)
		THEN	error("Sorry, no local symbols");
		ELSE	expv=dot;
		FI
		lp--;

	ELIF lastc=='"'
	THEN	expv=ditto;

	ELIF lastc=='+'
	THEN	expv.a_offset=inkdot(dotinc)->a_offset;

	ELIF lastc=='^'
	THEN	expv.a_offset=inkdot(-dotinc)->a_offset;

	ELIF lastc=='<'
	THEN	savc=rdc();
		IF (regptr=getreg(savc)) != -1
		THEN	IF segflg==1
			THEN IF regptr==19
			     THEN expv.a_segno = (regs[18]>>8)&0177;
			     ELIF regptr==15
			     THEN expv.a_segno = curseg;
			     FI
			FI
			expv.a_offset=regs[regptr];
		ELIF (base=varchk(savc)) != -1
		THEN	IF base&0x8000
			THEN expv=svar[base&0x7fff];
			ELSE expv.a_offset=var[base];
			FI
		ELIF savc=='<'
		THEN	IF segflg!=1
			THEN error("Segmented address given, program is not segmented");
			FI
			item(1);
			IF rdc()!='>' ORF rdc()!='>'
			THEN error(BADVAR);
			FI
			IF expv.a_offset<0 ORF expv.a_offset>=128
			THEN error(BADSEG);
			FI
			expv.a_segno = expv.a_offset;
			rdc(); lp--;
			expr(1);
		ELSE	error(BADVAR);
		FI

	ELIF lastc=='\''
	THEN	d=4; expv.a_offset=0;
		WHILE quotchar()
		DO  IF d--
		    THEN IF d==1 THEN expv.a_offset <<= 16; FI
			 expv.a_offset |= ((d&1)?lastc<<8:lastc);
		    ELSE error(BADSYN);
		    FI
		OD

	ELIF a
	THEN	error(NOADR);
	ELSE	lp--; return(0);
	FI
	return(1);
}

/* service routines for expression reading */
readsym()
{
	REG char	*p;

	p = isymbol;
	REP IF p < &isymbol[127]
	    THEN *p++ = lastc;
		 readchar();
	    FI
	PER symchar(1) DONE
	*p = 0;
}

SYMPTR	lookupsym(symstr)
STRING	symstr;
{
	SYMPTR		symp;

	symset();
	WHILE (symp=symget())
	DO
	    IF (symp->symf&SYMCHK)==symp->symf
	       ANDF eqsym(symname, symstr,'_')
	    THEN return(symp);
	    FI
	OD
	return(0);
}

convdig(c)
CHAR c;
{
	IF isdigit(c)
	THEN	return(c-'0');
	ELIF isxdigit(c)
	THEN	IF isupper(c)
		THEN return(tolower(c)-'a'+10);
		ELSE return(c-'a'+10);
		FI
	ELSE	return(17);
	FI
}

symchar(dig)
{
	IF lastc=='\\' THEN readchar(); return(TRUE); FI
	return( isalpha(lastc) ORF lastc=='_' ORF dig ANDF isdigit(lastc) );
}

varchk(name)
CHAR	name;
{
	IF isdigit(name)
	THEN	IF name>='0' ANDF name<='2'
		THEN return((name-'0')|0x8000);
		FI
		return(name-'0');
	FI
	IF isalpha(name)
	THEN	IF name=='e' THEN return(0x8003); FI
		return((name&037)-1+10);
	FI
	return(-1);
}

chkloc()
{
	error("in chkloc");
}

eqsym(s1, s2, c)
REG STRING	s1, s2;
CHAR		c;
{
	IF eqstr(s1,s2)
	THEN	return(TRUE);
	ELIF *s1==c
	THEN	CHAR		s3[128];
		REG INT		i;

		s3[0]=c;
		FOR i=1; s3[i] = *s2++; i++
		DO OD
		return(eqstr(s1,s3));
	FI
	return(FALSE);
}
