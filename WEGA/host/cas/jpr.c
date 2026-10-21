/*
 * jpr - jpr and callr: a jump or call that becomes jr/calr or jp/call
 *
 * The length of the instruction is not known in the first pass, because the distance to
 * the label may change by the other jpr instructions in between. jpr looks at the
 * operands: with a distance that is surely short or surely long the instruction is
 * written at once as jr/calr or as jp/call. Otherwise it gets an entry of the information
 * table (struct jent, chained per location counter by lcaux[][].la_1) and a record 38 or
 * 39 for the second pass (jpr2.c). lcaux[][].la_0 counts these entries.
 */
#include "cas.h"

static int jprnew(int tok, int sym, int32_t off);
static int jprange(int tok, int sym, int32_t off);
static void jprput(int idx, int tok, int ncc, NODE *list);
static void jpput(int tok, NODE *list);
static void jrput(int tok, NODE *list);

void
jpr(int tok, NODE *list)
{
	NODE *p, *e;
	int ncc, type, sym, idx, res;
	int32_t off;

	ncc = 0;
	type = 0;
	sym = 0;
	off = 0;
	for (p = list; p != 0; p = p->n_next) {
		if (p->n_type == 0x36) {
			if (tok == 0x51)
				error(2, "Invalid operand combination\n");
			ncc++;
			continue;
		}
		if (p->n_type != 0x56) {
			jpput(tok, list);
			return;
		}
		e = p->n_a;
		type = exprtype(e);
		if (e->n_type == 3) {
			jpput(tok, list);
			return;
		}
		if (e->n_type == 7) {
			sym = e->n_w;
			off = 0;
			continue;
		}
		if (e->n_type != 0x19) {
			error(2, "Expressions used in jpr must be constant\n");
			continue;
		}
		if (type != 7 && type != 1) {
			jpput(tok, list);
			return;
		}
		sym = e->n_a->n_w;
		off = e->n_b->n_val;
		if (e->n_c2 == 0x1b)
			off = -off;
	}
	if (type == 1) {
		res = jprange(tok, sym, off);
		if (res == 0) {
			jrput(tok, list);
			return;
		}
		if (res == 1) {
			jpput(tok, list);
			return;
		}
		if (res != 2)
			return;
	}
	idx = jprnew(tok, sym, off);
	jprput(idx, tok, ncc, list);
}

/* a new entry of the information table for a jpr that is not decided yet; returns its index */
static int
jprnew(int tok, int sym, int32_t off)
{
	struct jent *p;

	p = (struct jent *)italloc(14);
	p->j_0 = (cursec << 8) & cursub;
	p->j_2 = (int16_t)lctab[cursec][cursub];
	p->j_4 = sym;
	p->j_6 = off;
	p->j_12 = 0;
	p->j_13 = tok;
	lcaux[cursec][cursub].la_0++;
	p->j_10 = lcaux[cursec][cursub].la_1;
	lcaux[cursec][cursub].la_1 = itidx(p);
	return (itidx(p));
}

/*
 * Is the jump surely short (0), surely long (1) or not decided yet (2)? d1 is the distance
 * to the symbol now; d2 the distance when all the jpr entries since its definition become
 * long (2 bytes longer, 4 with segments). jr reaches -254 ... 256, calr -4092 ... 4098.
 */
static int
jprange(int tok, int sym, int32_t off)
{
	struct sym *sp, *ip;
	int n0, n1;
	int32_t here, target, grow, d1, d2;

	sp = stabp(sym);
	n0 = sp->s_0;
	ip = itabp(sp->s_8);
	target = ip->s_2;
	here = lctab[cursec][cursub];
	n1 = lcaux[cursec][cursub].la_0;
	d1 = target - here + off;
	if (segmode == 0)
		grow = (uint16_t)((n1 - n0) << 1);
	else
		grow = (uint16_t)((n1 - n0) << 2);
	d2 = d1 - grow;
	switch (tok) {
	case 0x50:
		if (d1 <= 256 && d1 >= -254 && d2 <= 256 && d2 >= -254)
			return (0);
		if ((d1 <= 256 && d1 >= -254) || (d2 <= 256 && d2 >= -254))
			return (2);
		return (1);
	case 0x51:
		if (d1 <= 4098 && d1 >= -4092 && d2 <= 4098 && d2 >= -4092)
			return (0);
		if ((d1 <= 4098 && d1 >= -4092) || (d2 <= 4098 && d2 >= -4092))
			return (2);
		return (1);
	}
	return (tok);
}

/* the record of the jpr for the second pass: tag 38 (jpr) or 39 (callr), entry, number of conditions, operands */
static void
jprput(int idx, int tok, int ncc, NODE *list)
{
	NODE *p;

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
		iputnode(p->n_a);
		iputnode(p->n_b);
	}
	for (p = list; p != 0; p = p->n_next)
		nfreed++;
}

/* the instruction as jp or call */
static void
jpput(int tok, NODE *list)
{
	switch (tok) {
	case 0x50:
		putins(pstlook("jp"), list);
		break;
	case 0x51:
		putins(pstlook("call"), list);
		break;
	}
}

/* the instruction as jr or calr */
static void
jrput(int tok, NODE *list)
{
	switch (tok) {
	case 0x50:
		putins(pstlook("jr"), list);
		break;
	case 0x51:
		putins(pstlook("calr"), list);
		break;
	}
}
