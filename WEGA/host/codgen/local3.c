/*
 * local3.c - machine dependent helpers of the code generator that the matcher uses (codgen / scodgen)
 *
 * Reconstructed from /lib/codgen: special() 0x471a, shtemp() 0x3e8e, shumul() 0x3f4e, shincr() 0x3ed6,
 * ltypeok() 0x3e1c, ltypeop() 0x3e66, ispow2() 0x8af6, bsize() 0x8878, szty() 0x88c4, rcount() 0x15ba.
 * The values are those of the non-segmented codgen; scodgen differs where a pointer is involved (to do).
 */
#include "cg.h"

extern int cerror(const char *, ...);
extern int rstatus[];		/* regs3.c: status bits of the registers (SAREG, STAREG, ... by register number) */

#define MAXRCOUNT 5000

/* a loop guard: the matcher is called more than 5000 times for one expression */
static int nrecur;
void
rcount(void)
{
	if (++nrecur > MAXRCOUNT)
		cerror("expression causes compiler loop: try simplifying");
}
void rcount_reset(void) { nrecur = 0; }

/* the operator is a leaf that can be addressed: NAME, REG, ICON, OREG */
int
ltypeop(struct node *p)
{
	return p->op == NAME || p->op == REG || p->op == ICON || p->op == OREG;
}

/* log2 of a power of two, else -1 */
int
ispow2(long c)
{
	int n;

	if (c <= 0 || (c & (c - 1)))
		return -1;
	for (n = 0; c > 1; c >>= 1)
		n++;
	return n;
}

/* size of a type in bits (non-segmented: pointers 16); SHORT and USHORT do not occur (cparse maps them to int) */
int
bsize(int t)
{
	switch (t) {
	case 2: case 12: return 8;			/* CHAR UCHAR */
	case 4: case 14: return 16;			/* INT UNSIGNED */
	case 5: case 6: case 15: return 32;		/* LONG FLOAT ULONG */
	case 7: return 64;				/* DOUBLE */
	}
	if ((t & 0x30) == 0x10)
		return 16;				/* a pointer */
	cerror("illegal bsize type: %%%0.2x\n", t);
	return 0;
}

/* size of a type in words: long, float and unsigned long 2, double 4, everything else 1 */
int
szty(int t)
{
	switch (t) {
	case 5: case 6: case 15: return 2;
	case 7: return 4;
	}
	return 1;
}

/* is p->lval an operand that a one word instruction takes: an unnamed constant */
static int
plainicon(struct node *p)
{
	return p->op == ICON && p->sname == 0 && p->rval >= 0;
}

/* the SPECIAL shapes: small constants and powers of two */
int
special(struct node *p, int shape)
{
	long v;

	if (p->op != ICON)
		return 0;
	if (p->sname) {
		if (shape != SPECIAL + 8)
			return 0;
		return 1;
	}
	if (p->rval < 0)
		return 0;
	v = p->lval;
	switch (shape) {
	case SPECIAL + 1: return v >= 0 && v <= 15;
	case SPECIAL + 2: return v >= -16 && v <= 16;
	case SPECIAL + 3: return v >= -256 && v <= 255;
	case SPECIAL + 5:
		if (!plainicon(p))
			return 0;
		/* FALLTHROUGH */
	case SPECIAL + 4: return v >= -32768 && v <= 32767;
	case SPECIAL + 6: return ispow2(v) >= 0;
	case SPECIAL + 7: return ispow2(~v) >= 0;
	case SPECIAL + 8: return 0;
	}
	cerror("bad special shape: SPECIAL | %%%0.2x", shape - SPECIAL);
	return 0;
}

/* STAR over p: a register variable or a constant (operand that needs no register) */
int
shtemp(struct node *p)
{
	if (p->op == STAR)
		p = p->left;
	if (p->op == REG || p->op == OREG)
		return (rstatus[p->rval] & 0x14) == 0;
	return p->op == NAME || p->op == ICON;
}

/* does an increment of incr bytes on a pointer of type t step by the size of the object it points to? */
int
shincr(int t, long incr)
{
	if ((t & 0x30) != 0x10)
		return 0;
	t = (t >> 2 & ~0xF) | (t & 0xF);
	if ((t & 0x30) == 0x10)
		return incr == 2;		/* a pointer to a pointer (16 bit) */
	switch (t) {
	case 2: case 12: return incr == 1;
	case 3: case 4: case 13: case 14: return incr == 2;
	case 6: return incr == 4;
	case 7: return incr == 8;
	}
	return 0;
}

/* STAR over p: which address shapes does it fit? STARNM for a name, STARREG for a post increment through a register */
int
shumul(struct node *p)
{
	int o = p->op;

	if (o == NAME || o == OREG || o == ICON)
		return STARNM;
	if ((o == 0x4E || o == 9) && p->left->op == REG && p->right->op == ICON && p->right->sname == 0 &&
	    shincr(p->left->type, p->right->lval))
		return STARREG;
	return 0;
}

/* an operator that is a leaf, or (with p) a STAR over a leaf that shumul accepts */
int
ltypeok(int op, struct node *p)
{
	if (op == NAME || op == REG || op == ICON || op == OREG)
		return 1;
	if (op == STAR && p != 0 && shumul(p->left))
		return 1;
	return 0;
}
