/*
 * match3.c - shapes, types and the operator index of the table (codgen / scodgen)
 *
 * Reconstructed from the functions at 0x5e92 (tshape), 0x5ff6 (ttype) and 0x60e2 (setrew) of /lib/codgen.
 * Work in progress: match() and the rest of the module follow, see ../KNOWLEDGE.md.
 */
#include "cg.h"

/* register data (regs3): flag bits of a register, by register number */
#define RBFLAG	0x0008		/* a register of the B class (long registers) */
#define RTEMPFLAGS 0x0014	/* the register is a scratch (STAREG or STBREG) register */
extern int rstatus[];

int fldsz, fldoff, fldshf;	/* the field that tshape found (size, offset, shift) */

/* local3.c helpers used here */
extern int special();	/* SPECIAL | n shapes of the machine */
extern int shltype();	/* a leaf, or a star over something addressable */
extern int shtemp();		/* the operand lives in a register variable or is a constant */
extern int shumul();		/* STARNM / STARREG / SOREG shapes that p fits */
extern int ltypeop();		/* the operator of p is NAME, REG, ICON or OREG */

/* does the node p fit the shape? */
int
tshape(p, shape)
	register struct node *p;
	register int shape;
{
	register int o = p->op;
	int r;

	if (shape == SANY)
		return 1;
	if (shape & SPECIAL) {
		if (shape == SZERO)	/* the constant 0 without a name */
			return o == ICON && p->sname == 0 && p->rval >= 0 && p->lval == 0;
		return special(p, shape);
	}
	if ((shape & INTEMP) && shtemp(p))
		return 1;
	if ((shape & SWADD) && (o == NAME || o == OREG) && (p->lval & 1))
		return 0;			/* a word address must be even */
	switch (o) {
	case NAME:
		return shape & SNAME;
	case ICON:
		return shape & SCON;
	case FLD:
		if (!(shape & SFLD) || !ltypeop(p->left))
			return 0;
		fldsz = p->rval & 0x3F;
		fldoff = p->rval >> 6;
		fldshf = 16 - fldsz - (p->rval >> 6);
		return 1;
	case STAR:
		return shumul(p->left) & shape;
	case REG:
		r = rstatus[p->rval] & RBFLAG ? SBREG : SAREG;
#ifdef OLD_D3K
		/* v0: the register of a parameter is not a scratch one for the tree (it is copied first) */
		if ((rstatus[p->rval] & RTEMPFLAGS) && !(p->sname && p->sname->sclass == 9))
#else
		if (rstatus[p->rval] & RTEMPFLAGS)
#endif
			r = r == SBREG ? (SBREG | STBREG) : (SAREG | STAREG);
		return shape & r;
	case OREG:
		return shape & SOREG;
	case CCODES:
		return shape & SCC;
	}
	return 0;
}

/* does the type fit the type word of a table entry? */
int
ttype(t, tword)
	register int t;
	register int tword;
{
	if ((tword & TANY) || tword == 0x4000 || tword == 0x8000)
		return 1;
	if (t == 0)
		t = TINT;		/* FARG / undefined: an int */
	if ((t & 0x30) == 0x10 && (tword & TPOINT2)) {
		do
			t = ((unsigned)t >> 2 & ~0xF) | (t & 0xF);	/* strip one level of pointer */
		while ((t & 0x30) == 0x30);
		return ttype(t, tword & ~TPOINT2);
	}
	if (t != (t & 0xF))		/* a pointer, array or function type */
		return tword & TPOINT;
	if (tword & TPOINT2)
		return 0;
	switch (t) {
	case 2:  return tword & TCHAR;
	case 3:  return tword & TSHORT;
	case 4:  return tword & TINT;
	case 5:  return tword & TLONG;
	case 6:  return tword & TFLOAT;
	case 7:  return tword & TDOUBLE;
	case 8: case 9: return tword & TSTRUCT;
	case 12: return tword & TUCHAR;
	case 13: return tword & TUSHORT;
	case 14: return tword & TUNSIGNED;
	case 15: return tword & TULONG;
	}
	return 0;			/* ENUMTY, MOETY, FARG */
}

/*
 * The operator classes of the table: an entry whose op is 0x1000 + k stands for every operator whose dope flags
 * match opclass[k] (OPSIMP, ASG OPSIMP, OPCOMM, ... in pairs). Data at 0x3530 of /lib/codgen.
 */
int opclass[] = {
	0x20, 0x21, 0x40, 0x41, 0x800, 0x801, 0x80, 0x81, 0x4, 0xe, 0x2, 0xe, 0x0, 0x2001, 0x10, 0xe,
	0x100, 0x101, 0x1000, 0x1001, 0x8000, 0x8001, 0x200, 0x201,
};

struct optab *opptr[128];	/* the first table entry that can serve an operator */
struct optab *rwdefault;	/* the entry with needs == 0x1000 (the default rewrite entry) */
extern int cerror();
extern int ltypeok();	/* local3.c 0x3e1c */

/* build opptr[] */
void
setrew()
{
	register struct optab *q;
	register int op, mask;

	for (q = table; q->op != FREE; q++)
		if (q->needs == 0x1000) {
			rwdefault = q;
			break;
		}
	if (q->op == FREE)
		cerror("bad setrew");
	for (op = 0; op < 129; op++) {
		if (dope[op] == 0)
			continue;
		for (q = table;; q++) {
			if (q->op == FREE) {
				opptr[op] = q;
				break;
			}
			if (q->op < 0x1000) {
				if (q->op == op) {
					opptr[op] = q;
					break;
				}
				continue;
			}
			mask = opclass[q->op - 0x1000];
			if (mask & 0x4000) {
				if (op == NAME || op == ICON || op == OREG || ltypeok(op, (struct node *)0)) {
					opptr[op] = q;
					break;
				}
			} else if (mask == ((mask | 1) & dope[op])) {
				opptr[op] = q;
				break;
			}
		}
	}
}

/* ---- match() ----------------------------------------------------------------------------------------- */

#define NOMATCH	0x1000
#define MDONE	0x1001
#define FORREW	0x4000
#define SCONV	110
#define REWRITE	0x1000		/* in q->needs: the entry only rewrites the tree, q->rewrite is the answer */

extern void rcount();
extern int ltypeok();
extern void fixname();		/* 0xa0c6: a NAME that lives in a register becomes REG, an auto becomes OREG */
extern void oregfix();		/* 0xa12a: a NAME of an auto or a parameter becomes OREG off the frame pointer */
#ifdef OLD_D2C
extern void symdrop();
#endif
extern void symuse();			/* 0x964c: note that the symbol of an assignment target is used */
extern int allo();	/* 0x67e8: get the registers the entry needs */
extern void expand();	/* 0x6444 */
extern void reclaim();	/* 0x7058 */
#ifdef OLD_D2A
extern int getreg();
extern void rmove(), rfree();
#endif
extern int bsize();
extern struct node resc[];			/* the work registers RESC1 .. RESC3 (0x12 byte nodes at 0x8f24) */

/* the node an operand letter of a template stands for */
struct node *
getlr(p, c)
	struct node *p;
	char c;
{
	switch (c) {
	case '1': case '2': case '3':
		return &resc[c - '1'];
	case 'L': case 'l':
		return (dope[p->op] & 0xE) == LTYPE ? p : p->left;
	case 'R': case 'r':
		return (dope[p->op] & 0xE) == BITYPE ? p->right : p;
	}
	cerror("bad getlr: %c", c);
	return 0;
}

/*
 * Find a table entry for the tree p and the cookie, generate the code and rewrite the tree.
 * Returns MDONE, NOMATCH, or (REWRITE entries) the rewrite request of the entry.
 */
int
match(p, cookie)
	register struct node *p;
	register int cookie;
{
	register struct optab *q;
	register struct node *l, *r;
	register int kind, mask;
	int rv;

	rcount();
	kind = dope[p->op] & 0xE;
	if (kind == LTYPE) {
		if (p->op == NAME) {
			fixname(p);
#ifdef OLD_D2C
			/* v0: a name (not a parameter) found in a scratch register is that register, which the tree takes over */
			if (p->op == REG && (cookie & STAREG) && (rstatus[p->rval] & STAREG) && p->type != 5 && p->type != 15 &&
			    p->sname && p->sname->sclass != 9 &&
			    (!(p->rall & 0x1000) || (p->rall & ~0x1000) == p->rval)) {
				symdrop(p->rval);
				return MDONE;
			}
#endif
		}
	} else if (kind == UTYPE) {
		l = p->left;
		if (l->op == NAME)
			fixname(l);
		else if (l->op == SCONV && l->left->op == NAME)
			fixname(l->left);
	} else if (kind == BITYPE) {
		l = p->left;
		if (l->op == NAME && (dope[p->right->op] & 0xE) == LTYPE) {
			if (dope[p->op] & 1) {		/* an assignment: the target must stay in memory */
				symuse(l->sname);
				oregfix(l);
			} else
				fixname(l);
		}
		r = p->right;
		if (r->op == NAME) {
			if ((p->type == 5 || p->type == 15) && ((dope[p->op] & 0x800) || (dope[p->op] & 0x80))) {
				rv = r->rval;
				/* mult and div use r4..r7: a right operand in r6 (or r4 for div) goes to memory */
				if (rv == 6 || ((dope[p->op] & 0x80) && rv == 4)) {
					oregfix(r);
					goto select;
				}
			}
			if ((dope[p->left->op] & 0xE) == LTYPE || p->left->op == FLD)
				fixname(r);
		}
	}
select:
	q = cookie == FORREW ? rwdefault : opptr[p->op];
	for (;; q++) {
		if (q->op == FREE)
			return NOMATCH;
		if (q->op < 0x1000) {
			if (q->op != p->op)
				continue;
		} else {
			mask = opclass[q->op - 0x1000];
			if (mask & 0x4000) {
				if (!ltypeok(p->op, p))
					continue;
			} else if (mask != ((mask | 1) & dope[p->op]))
				continue;
		}
		if (!(q->visit & cookie))
			continue;
		l = getlr(p, 'L');
		if (!tshape(l, q->lshape) || !ttype(l->type, q->ltype))
			continue;
#ifdef OLD_D10
		/* the old codgen v0 shifts a long by a constant through a register (ld A1,#n; sdal): no OIZB AL,#n */
		if (q->lshape == 0x6 && q->rshape == 0x80 && q->op == 0x1013 && (l->type == 5 || l->type == 15) &&
		    (p->op == LS || p->op == RS || p->op == ASG LS || p->op == ASG RS))
			continue;
#endif
		r = getlr(p, 'R');
		if (!tshape(r, q->rshape) || !ttype(r->type, q->rtype))
			continue;
		if (q->ltype == 0x4000 || q->rtype == 0x4000) {		/* both operands of the same type */
			if (r->type != l->type)
				continue;
		} else if (q->ltype == 0x8000 || q->rtype == 0x8000) {	/* of the same size */
			if (bsize(l->type) != bsize(r->type))
				continue;
		}
		if ((p->type == 6 || p->type == 7) && q->ltype == 0x8000 && q->rtype == 0x8000)
			continue;
		if (q->needs & REWRITE)
			return q->rewrite;
		if (!allo(p, q))
			continue;
#ifdef OLD_D2A
		rv = p->op;
#endif
		expand(p, cookie, q->cstring);
		reclaim(p, q->rewrite, cookie);
#ifdef OLD_D2A
		/* v0: the value of a long assignment is copied into a new pair, which keeps the name */
		if (rv == ASSIGN && !(cookie & 01) && p->op == REG && (p->type == 5 || p->type == 15) && p->rval < 8) {
			register int nr;

			nr = getreg(3, -1, 0x1000, (struct sym *)0);
			if (nr >= 0 && nr != p->rval) {
				rmove(nr, p->rval, p->type);
				rfree(nr, p->type);
			}
		}
#endif
		return MDONE;
	}
}
