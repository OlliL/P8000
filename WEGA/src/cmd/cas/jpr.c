static char sccsid[] = "@[$]jpr.c\t1.2 03/24/83 15:48:01 - 87wega3.2";

/*
 * jpr - jpr and callr: a jump or call that becomes jr/calr or jp/call
 *
 * The length of the instruction is not known in the first pass, because the distance to
 * the label may change by the other jpr instructions in between.  jpr looks at the
 * operands: with a distance that is surely short or surely long the instruction is
 * written at once as jr/calr or as jp/call.  Otherwise it gets an entry of the itable
 * (struct jent, chained per location counter by lcaux[][].la_1) and a record 38 or 39 for
 * the second pass (jpr2.c).  lcaux[][].la_0 counts these entries.
 */
#include "cas.h"

#define ITAB	0x88000030L		/* the information table (ssymtab.c) */

jpr(tok, list)
int tok;
NODE *list;
{
	register NODE *p;
	register NODE *e;
	register int ncc;
	register int type;
	int sym;
	int idx;
	int res;
	long off;

	ncc = 0;
	for (p = list; p != 0; p = p->n_next) {
		switch (p->n_type) {
		case 0x36:
			if (tok == 0x51)
				error(2, "Invalid operand combination\n");
			ncc++;
			continue;
		case 0x56:
			break;
		default:
			goto plain;
		}
		e = (NODE *)p->n_u.l;
		type = exprtype(e);
		switch (e->n_type) {
		case 3:
			goto plain;
		case 7:
			sym = e->n_u.pw.w;
			off = 0;
			break;
		case 0x19:
			if (type != 7 && type != 1) {
				goto plain;
			}
			sym = ((NODE *)e->n_u.cc.l1)->n_u.pw.w;
			off = ((NODE *)e->n_u.cc.l2)->n_u.l;
			if (e->n_u.cc.c2 == 0x1b)
				off = -off;
			break;
		default:
			error(2, "Expressions used in jpr must be constant\n");
		}
	}
	if (type == 1) {
		res = jprange(tok, sym, off);
		switch (res) {
		case 0:
			jrput(tok, list);
			return;
		case 1:
			jpput(tok, list);
			return;
		case 2:
			break;
		default:
			return;
		}
	}
	idx = jprnew(tok, sym, off);
	jprput(idx, tok, ncc, list);
	return;
plain:
	jpput(tok, list);
	return (0);
}

/* a new entry of the itable for a jpr that is not decided yet; returns its index */
jprnew(tok, sym, off)
int tok;
int sym;
long off;
{
	register struct jent *p;

#ifdef OLD_CAS
	if (lcaux[cursec][cursub].la_0 >= OLD_ENTMAX)
		earlyopt();
#endif
	p = (struct jent *)italloc(14);
	p->j_0 = (cursec << 8) & cursub;
	p->j_2 = (int)lctab[cursec][cursub];
	p->j_4 = sym;
	p->j_6 = off;
	p->j_12 = 0;
	p->j_13 = tok;
	++lcaux[cursec][cursub].la_0;
	p->j_10 = lcaux[cursec][cursub].la_1;
	lcaux[cursec][cursub].la_1 = (char *)p - (char *)ITAB;
	return ((char *)p - (char *)ITAB);
}

/*
 * Is the jump surely short (0), surely long (1) or not decided yet (2)?  d1 is the distance
 * to the symbol now; d2 the distance when all the jpr entries since its definition become
 * long (2 bytes longer, 4 with segments).  jr reaches -254 ... 256, calr -4092 ... 4098.
 */
jprange(tok, sym, off)
int tok;
int sym;
long off;
{
	register struct sym *sp;
	register struct sym *ip;
	register int n0;
	register int n1;
	long here;
	long target;
	long grow;
	long d1;
	long d2;

	sp = stabp(sym);
	n0 = sp->s_0;
	ip = itabp(sp->s_8);
	target = ip->s_2;
	here = lctab[cursec][cursub];
	n1 = lcaux[cursec][cursub].la_0;
	d1 = target - here + off;
	if (segmode)
		grow = (unsigned)((n1 - n0) << 2);
	else
		grow = (unsigned)((n1 - n0) << 1);
	d2 = d1 - grow;
	switch (tok) {
	case 0x50:
		if (d1 <= 256L && d1 >= -254L && d2 <= 256L && d2 >= -254L)
			return (0);
		if ((d1 > 256L || d1 < -254L) && (d2 > 256L || d2 < -254L))
			return (1);
		return (2);
	case 0x51:
		if (d1 <= 4098L && d1 >= -4092L && d2 <= 4098L && d2 >= -4092L)
			return (0);
		if ((d1 > 4098L || d1 < -4092L) && (d2 > 4098L || d2 < -4092L))
			return (1);
		return (2);
	}
	/* no return: r2 still holds tok (sic) */
}

/* the record of the jpr for the second pass: tag 38 (jpr) or 39 (callr), entry, number of conditions, operands */
jprput(idx, tok, ncc, list)
int idx;
int tok;
int ncc;
NODE *list;
{
	register NODE *p;

	locadv(2);
	if (tok == 0x50)
		iputb(0x26);
	else
		iputb(0x27);
	iputw(idx);
	if (ncc)
		iputb(2);
	else
		iputb(1);
	for (p = list; p != 0; p = p->n_next) {
		iputnode(p->n_u.ll.l1);
		iputnode(p->n_u.ll.l2);
	}
	for (p = list; p != 0; p = p->n_next)
		nfreed++;
}

/* the instruction as jp or call */
jpput(tok, list)
int tok;
NODE *list;
{
	struct pst *op;

	switch (tok) {
	case 0x50:
		op = pstlook("jp");
		putins(op, list);
		break;
	case 0x51:
		op = pstlook("call");
		putins(op, list);
		break;
	}
}

/* the instruction as jr or calr */
jrput(tok, list)
int tok;
NODE *list;
{
	struct pst *op;

	switch (tok) {
	case 0x50:
		op = pstlook("jr");
		putins(op, list);
		break;
	case 0x51:
		op = pstlook("calr");
		putins(op, list);
		break;
	}
}
