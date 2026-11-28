static char sccsid[] = "@[$]sym.c\t3.2  08/25/83 09:49:08 - 87wega3.2";

#include "defs.h"


MSG		BADFIL;


/* symbol management */
ADDR		*findsym();


/* symbol table and file handling service routines */

longseek(f, a)
L_INT a;
{
	return(lseek(f,a,0) != -1);
}

valpr(ap,type)
ADDR	*ap;
{
	ADDR		*dp;

	dp = findsym(ap,type);
	IF leng(maxoff) > dp->a_offset
	THEN	printf("%s", symname);
		IF dp->a_offset
		THEN	printf("+%%%x", (INT)dp->a_offset);
		FI
	FI
}

localsym()
{
	return(0);
}

psymoff(ap,type,s)
ADDR	*ap; int type; char *s;
{
	ADDR		*dp;

	dp = findsym(ap,type);
	IF leng(maxoff) <= dp->a_offset
	THEN	IF segflg
		THEN printf("<<%%%x>>%%%04X", ap->a_segno&0xff, ap->a_offset);
		ELSE printf("%%%04X", ap->a_offset);
		FI
	ELSE	printf("%s", symname);
		IF dp->a_offset THEN printf("+%%%x", (INT)dp->a_offset); FI
	FI
	printf(s);
}

ADDR	*findsym(ap, space)
ADDR	*ap;
{
	REG struct segsym	*p;
	L_INT		off, base, cur, best;
	INT		type;
	struct segsym	*bestp;
	static ADDR	fnd = { 0, 0L };

	off = ap->a_offset;
	best = 0;
	cur = 0;
	bestp = 0;
	fnd.a_segno = ap->a_segno;
	fnd.a_offset = 0x1ffffL;
	IF space
	THEN IF p = segsyms
	     THEN WHILE fnd.a_offset
		  DO IF (type = p->ss_type) == -1 THEN break; FI
		     IF type==space ANDF p->ss_seg==fnd.a_segno ANDF (off-(base=p->ss_base)<fnd.a_offset ANDF off>=base)
		     THEN fnd.a_offset = off-base;
			  best = cur;
			  bestp = p;
		     FI
		     cur += p->ss_count;
		     p++;
		  OD
		  IF bestp
		  THEN lseek(fsym, symoff, 0);
		       lseek(fsym, best, 1);
		       read(fsym, &fsymbuf, sizeof fsymbuf);
		       symcopy(&fsymbuf, -1);
		  FI
	     FI
	FI
	return(&fnd);
}

nextsym()
{
}

/* sequential search through file */
symset()
{
	symnxt = symbuf;
	IF symrqd
	THEN	longseek(fsym, symoff);
		symread(); symrqd=FALSE;
	ELSE	longseek(fsym, symoff+(symend-symbuf)*SYMTABSIZ);
	FI
}

SYMPTR	symget()
{
	INT	n;
	REG INT	rc;

	IF symnxt >= symend
	THEN	rc=symread(); symrqd=TRUE;
	ELSE	rc=TRUE;
	FI
	IF symnxt->symc[0]&0200
	THEN	n = (symnxt->symc[0]&0177)-8;
		n = (n+13)/14+1;
	ELSE	n = 1;
	FI
	IF nsyms>0 ANDF rc THEN symcopy(symnxt, 0); FI
	symnxt += n;
	return((nsyms>0 ANDF rc) ? symnxt-n : 0);
}

symread()
{
	INT		symlen;
	INT		i, n;
	L_INT		off;
	SYMPTR		last, p;

	IF (symlen=read(fsym,symbuf,sizeof symbuf))>=SYMTABSIZ
	THEN	symnxt = symbuf;
		FOR p = symbuf; p < &symbuf[symlen/SYMTABSIZ];
		DO	last = p;
			IF p->symc[0]&0200
			THEN	i = (p->symc[0]&0177)-8;
				n = (i+13)/14+1;
				p += n;
			ELSE	p++;
			FI
		OD
		IF p > &symbuf[SYMSIZ]
		THEN	off = ((long)((&symbuf[SYMSIZ]-last)*SYMTABSIZ))&0xffff;
			off = -off;
			lseek(fsym, off, 1);
			p = last;
		FI
		symend = p;
		return(TRUE);
	ELSE	return(FALSE);
	FI
}

symcopy(sp, flag)
SYMPTR	sp;
{
	INT		i, j, len, k, l;

	IF sp->symc[0]&0200
	THEN	len = sp->symc[0]&0177;
		strncpy(symname, &sp->symc[1], 7);
		IF flag
		THEN read(fsym, &symname[7], len-7);
		ELSE strncpy(&symname[7], sp+1, 120);
		FI
		symname[len-1] = 0;
	ELSE	strncpy(symname, sp->symc, 8);
		symname[8] = 0;
	FI
}
