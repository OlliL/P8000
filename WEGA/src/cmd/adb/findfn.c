static char sccsid[] = "@[$]findfn.c\t3.2 08/25/83 09:48:43 -           ";

#include "defs.h"

struct segsym	*segsyms;
L_INT		symoff;
SYMTAB		fsymbuf;



CHAR		fndseg = 0;
L_INT		fnddiff = 0;

stfind(ap, space)
ADDR	*ap;
{
	REG struct segsym	*p;
	L_INT		off, base, cur, best;
	INT		type;
	struct segsym	*bestp;

	off = ap->a_offset;
	fndseg = ap->a_segno;
	fnddiff = 0x1ffffL;
	best = 0;
	cur = 0;
	bestp = 0;
	IF space
	THEN IF p = segsyms
	     THEN WHILE fnddiff
		  DO IF (type = p->ss_type) == -1 THEN break; FI
		     IF type==space ANDF p->ss_seg==fndseg ANDF (off-(base=p->ss_base)<fnddiff ANDF off>=base)
		     THEN fnddiff = off-base;
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
		       return(fsymbuf.symv);
		  FI
	     FI
	FI
}

frsize(ap)
ADDR	*ap;
{
	POS	inst;
	ADDR	a;

	inst = get(ap, ISP);
	IF (inst&0xff00) == 0xe800		/* jr */
	THEN	a.a_segno = (segflg==1 ? ap->a_segno : 0);
		a.a_offset = ap->a_offset + (inst&0xff)*2 + 2;
		return(frsize(&a));
	ELIF (inst&0xfff0) == 0xabf0		/* dec r15,#n */
	THEN	return((inst&0xf)+1);
	ELIF inst == 0x030f			/* sub r15,#n */
	THEN	a.a_segno = (segflg==1 ? ap->a_segno : 0);
		a.a_offset = ap->a_offset+2;
		return(get(&a, ISP));
	ELIF inst == 0x5e08			/* jp addr */
	THEN	IF segflg==1
		THEN	ADDR	b;

			a.a_segno = ap->a_segno;
			a.a_offset = ap->a_offset+2;
			b.a_segno = (get(&a, ISP)>>8)&0177;
			a.a_offset = ap->a_offset+4;
			b.a_offset = (POS)get(&a, ISP);
			frsize(&b);
			return;
		ELSE	a.a_segno = 0;
			a.a_offset = ap->a_offset+2;
			a.a_offset = (POS)get(&a, ISP);
			a.a_segno = 0;
			return(frsize(&a));
		FI
	FI
	return(0);
}

stkoff(p1, p2)
ADDR	*p1, *p2;
{
	ADDR	a, b, c;
	INT	n, m;

	c.a_segno = (segflg==1 ? curseg : 0);
	c.a_offset = (POS)regs[15];
	a.a_segno = (segflg==1 ? p1->a_segno : 0);
	a.a_offset = (POS)stfind(p1, ISYM);
	n = 0;
	LOOP
		b.a_offset = (POS)stfind(p2, ISYM);
		b.a_segno = (segflg==1 ? p2->a_segno : 0);
		IF b.a_offset == 0
		THEN	printf("routine not found on stack\n");
			return(-1);
		FI
		m = frsize(&b);
		IF a.a_segno==b.a_segno ANDF a.a_offset==b.a_offset
		THEN	IF p1->a_offset == p2->a_offset
			THEN return(0);
			ELIF p1->a_offset == a.a_offset
			THEN return(n+m);
			ELIF p2->a_offset == a.a_offset
			THEN return(n-m);
			ELSE return(n);
			FI
		FI
		IF p2->a_segno==b.a_segno ANDF b.a_offset!=p2->a_offset ANDF get(p2, ISP)!=0x9e08
		THEN	c.a_offset += (POS)m;
			n += m;
		FI
		IF segflg==1
		THEN	p2->a_segno = (get(&c, DSP)>>8)&0177;
			c.a_offset += 2;
			n += 2;
		ELSE	p2->a_segno = 0;
		FI
		p2->a_offset = (POS)get(&c, DSP);
		c.a_offset += 2;
		n += 2;
	POOL
}

findroutine(ap, verbose)
ADDR	*ap;
{
	ADDR	a, b, c, d, e;
	INT	n;
	L_INT	l1, l2;

	e.a_segno = (segflg==1 ? curseg : 0);
	e.a_offset = (POS)regs[15];
	d.a_segno = (segflg==1 ? (regs[18]>>8)&0177 : 0);
	d.a_offset = (POS)regs[19];
	IF (ap->a_segno==d.a_segno ANDF d.a_offset!=ap->a_offset) ORF adrflg
	THEN	IF (n = stkoff(ap, &d)) == -1 THEN return; FI
		e.a_offset += n;
		e.a_segno = (segflg==1 ? curseg : 0);
	FI
	c.a_segno = ap->a_segno;
	c.a_offset = ap->a_offset;
	WHILE cntval--
	DO	chkerr();
		IF (b.a_offset = (POS)stfind(&c, ISYM)) == 0 THEN return; FI
		b.a_segno = (segflg==1 ? c.a_segno : 0);
		printf("%s()\n", symname);
		n = frsize(&b);
		IF verbose THEN printf("\tstack frame:"); FI
		IF b.a_segno==c.a_segno ANDF b.a_offset!=c.a_offset ANDF get(&c, ISP)!=0x9e08
		THEN	IF verbose
			THEN	l2 = n + e.a_offset;
				FOR l1 = e.a_offset; l1 <= l2; l1 += 2
				DO	a.a_segno = (segflg==1 ? curseg : 0);
					a.a_offset = l1;
					IF l1 == l2 ANDF segflg==1
					THEN	printf("\n\t\t%%%04x: <<%%%02x>>", shorten(l1), (get(&a, DSP)>>8)&0177);
						a.a_offset = l1 += 2;
						printf("%%%04x", get(&a, DSP));
					ELSE	printf("\n\t\t%%%04x: %%%04x", shorten(l1), get(&a, DSP));
					FI
				OD
			FI
			e.a_segno = (segflg==1 ? curseg : 0);
			e.a_offset = (POS)n + e.a_offset;
		ELSE	IF verbose
			THEN	l2 = e.a_offset;
				FOR l1 = e.a_offset - n; l1 <= l2; l1 += 2
				DO	a.a_segno = (segflg==1 ? curseg : 0);
					a.a_offset = l1;
					IF l1 == l2 ANDF segflg==1
					THEN	printf("\n\t\t%%%04x: <<%%%02x>>", shorten(l1), (get(&a, DSP)>>8)&0177);
						a.a_offset = l1 += 2;
						printf("%%%04x", get(&a, DSP));
					ELSE	printf("\n\t\t%%%04x: %%%04x", shorten(l1), get(&a, DSP));
					FI
				OD
			FI
		FI
		IF verbose THEN printf(" (return address)\n"); FI
		IF segflg==1
		THEN	c.a_segno = (get(&e, DSP)>>8)&0177;
			e.a_offset += 2;
		ELSE	c.a_segno = 0;
		FI
		c.a_offset = (POS)get(&e, DSP);
		e.a_offset += 2;
	OD
}
