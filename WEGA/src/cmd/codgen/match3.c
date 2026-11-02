/*
 * match3.c - the template matcher
 */
#include "cg.h"
#include "cg2.h"

extern void acon();
extern void adrput();
extern int allo();
extern int bsize();
extern int cerror();
extern void conput();
extern struct node *d16acmp;
extern struct node *d2ranode;
extern int d2rareg;
extern int dope[];
extern void expand();
extern void fixname();
extern int fldoff;
extern int fldshf;
extern int fldsz;
extern struct sym *fpsym1;
extern struct node *getlr();
extern int getreg();
extern void hopcode();
extern void insput();
extern int ltypeok();
extern int ltypeop();
extern int match();
extern struct node node[];
extern int nosuffix;
extern int opclass[];
extern struct optab *opptr[];
extern void oregfix();
extern void prlong();
extern int rclass();
extern void rcount();
extern void reclaim();
extern struct node resc[];
extern struct rs *rfind();
extern void rfree();
extern int rlmode;
extern int rmode;
extern void rmove();
extern int rstatus[];
extern struct optab *rwdefault;
extern void setrew();
extern int shtemp();
extern int shumul();
extern int special();
extern void symdrop();
extern void symuse();
extern int szty();
extern int tflag;
extern int tshape();
extern int ttype();
extern void upput();
extern void zzzcode();
static char sccsid[] = "@[$]match3.c\t2.4  08/24/83 13:18:24 Copr. 1981 87wega3.2.";
int opclass[] = {
	0x20, 0x21, 0x40, 0x41, 0x800, 0x801, 0x80, 0x81, 0x4, 0xe, 0x2, 0xe, 0x0, 0x2001, 0x10, 0xe,
	0x100, 0x101, 0x1000, 0x1001, 0x8000, 0x8001, 0x200, 0x201, 0x4000, 0xe,
};
char *ocnames[] = {
	"OPSIMP",
	"ASG OPSIMP",
	"OPCOMM",
	"ASG OPCOMM",
	"OPMUL",
	"ASG OPMUL",
	"OPDIV",
	"ASG OPDIV",
	"OPUNARY",
	"ASG OPUNARY",
	"OPLEAF",
	"ASG OPLEAF",
	"OPANY",
	"ASG OPANY",
	"OPLOG",
	"ASG OPLOG",
	"OPLONG",
	"ASG OPLONG",
	"OPSHFT",
	"ASG OPSHFT",
	"OPBIT",
	"ASG OPBIT",
	"OPFLOAT",
	"ASG OPFLOAT",
	"OPLTYPE",
	"ASG OPLTYPE",
};

/* register data (regs3): flag bits of a register, by register number */
#define RBFLAG	0x0008		/* a register of the B class (long registers) */

#define RTEMPFLAGS 0x0014	/* the register is a scratch (STAREG or STBREG) register */

#define NOMATCH	0x1000

#define MDONE	0x1001

#define FORREW	0x4000

#define SCONV	110

#define REWRITE	0x1000		/* in q->needs: the entry only rewrites the tree, q->rewrite is the answer */

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
	case 8: case 9: return tword & TSTRUCT;
	case 4:  return tword & TINT;
	case 14: return tword & TUNSIGNED;
	case 13: return tword & TUSHORT;
	case 12: return tword & TUCHAR;
	case 15: return tword & TULONG;
	case 5:  return tword & TLONG;
	case 6:  return tword & TFLOAT;
	case 7:  return tword & TDOUBLE;
	}
	return 0;			/* ENUMTY, MOETY, FARG */
}

/*
 * The operator classes of the table: an entry whose op is 0x1000 + k stands for every operator whose dope flags
 * match opclass[k] (OPSIMP, ASG OPSIMP, OPCOMM, ... in pairs). Data at 0x3530 of /lib/codgen.
 */

/* build opptr[] */
void
setrew()
{
	register struct optab *q;
	register int op, mask;

	for (q = table; q->op != FREE; q++)
		if (q->needs == 0x1000) {
			rwdefault = q;
			goto more;
		}
	cerror("bad setrew");
more:
	for (op = 0; op < 129; op++)
		if (dope[op]) {
			for (q = table; q->op != FREE; q++) {
				if (q->op < 0x1000) {
					if (q->op == op)
						break;
				} else if ((mask = opclass[q->op - 0x1000]) & 0x4000) {
					if (op == NAME || op == ICON || op == OREG)
						break;
					else if (ltypeok(op, (struct node *)0))
						break;
				} else if ((dope[op] & (mask | 1)) == mask)
					break;
			}
			opptr[op] = q;
		}
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
#ifdef OLD_D3AW
			/* v0p: a parameter added to an address constant is not taken from its register */
			if (p->op == PLUS && p->left->op == ICON && p->left->sname && r->sname->sclass == 9)
				oregfix(r);
			else
#endif
			if ((dope[p->left->op] & 0xE) == LTYPE || p->left->op == FLD)
				fixname(r);
		}
	}
select:
#ifdef OLD_D16B
	/* v0p: an unsigned word in memory changed by an assignment operator: no entry, the tree is rewritten as
	   a = a op b (order) */
	if ((p->op == ASG PLUS || p->op == ASG MINUS) && cookie != FORREW && (p->left->op == OREG ||
	    p->left->op == STAR) && p->left->type == 14)
		return NOMATCH;
#endif
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
#ifdef OLD_D16A
		/* v0p: a frame slot incremented by a constant whose value may go to a temporary is loaded, incremented in
		   the register and stored (ld A1,AL; Zi A1,AR; ld AL,A1), not incremented in memory */
		if ((p->op == ASG PLUS || p->op == ASG MINUS) && (cookie & INTEMP) && l->op == OREG && l->rval == 15 &&
		    r->op == ICON && r->sname == 0 && p != d16acmp) {
			if (q->rshape == 0x8002)
				continue;
			if (q->needs == 1 && q->rewrite == 0x804 && allo(p, q)) {
				expand(p, cookie, "\tldZB\tA1,AL\n\tZi\tA1,AR\n\tldZB\tAL,A1\n");
				reclaim(p, q->rewrite, cookie);
				return MDONE;
			}
		}
#endif
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
#ifdef OLD_D2RA
		if (p->op == PLUS && l->op == REG && l->rval == 0 && r->op == REG && szty(p->type) == 1) {
			d2ranode = p;
			d2rareg = r->rval;
		}
#endif
#ifdef OLD_D2A
		rv = p->op;
#endif
#ifdef OLD_D6S
		/* v0p: an assignment of a register to itself (the value of a switch into its register) prints nothing */
		if (p->op == ASSIGN && p->left->op == REG && p->right->op == REG && p->left->rval == p->right->rval &&
		    p->left->type == p->right->type) {
			reclaim(p, q->rewrite, cookie);
			return MDONE;
		}
#endif
#ifdef OLD_D22
		/* v0p: a floating compare with memory without the rounding fld of the left operand */
		if (q->op == 0x100e && q->lshape == 0x18 && q->rshape == 0x2c0)
			expand(p, cookie, "\tfcpZfZ>\tAL,AR\n\tfldctlb\tcmpflg\nZI");
		else
#endif
		expand(p, cookie, q->cstring);
		reclaim(p, q->rewrite, cookie);
#ifdef OLD_D2A
		/* v0: the value of a long assignment is copied into a new pair, which keeps the name */
		if (rv == ASSIGN && !(cookie & 01) && p->op == REG && (p->type == 5 || p->type == 15) && p->rval < 8 &&
		    !(p->rall & 0x1000)) {	/* (not when the value goes to a register wanted for it: an argument) */
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

/* expand the template cp for the tree p */
int tflag = 0;

void
expand(p, cookie, cp)
	register struct node *p;
	register int cookie;
	register char *cp;
{
	register long m;
	register struct node *r;
	register struct rs *n;
	int k;

	tflag = 0;
	for (; *cp; cp++) {
		switch (*cp) {
		case 'A':
			adrput(getlr(p, *++cp));
			break;
		case 'B':
			r = getlr(p, *++cp);
			printf("%ld", r->lval & 1);
			break;
		case 'F':			/* the line is only for a result that is used */
			if (cookie & SANY)
				while (*++cp != '\n')
					;
			break;
		case 'H':
			printf("%d", fldshf);
			break;
		case 'I':
			insput(getlr(p, *++cp));
			break;
		case 'L':
			printf("%d", p->label);
			break;
		case 'M': case 'N':
			m = ((1L << fldsz) - 1) << fldshf;
			prlong(*cp == 'M' ? m : ~m);
			break;
		case 'O':
			hopcode(p);
			cp++;
			break;
		case 'S':
			printf("%d", fldsz);
			break;
		case 'T':
			tflag = 1;
			break;
		case 'U':
			upput(getlr(p, *++cp));
			break;
		case 'V':
			rmode++;
			break;
		case 'W':
			rlmode++;
			break;
		case 'X':
			rlmode = rmode = 0;
			break;
		case 'Y':
			r = getlr(p, 'R');
			if (r->op == FLD)
				r = r->left;
			if (r->op == ICON)
				acon(r);
			else
				adrput(getlr(p, 'R'));
			break;
		case 'Z':
			zzzcode(p, *++cp);
			break;
		case 'C':
			/* the result register of a call holds the value of the right operand: note it for the register file */
			r = getlr(p, *++cp);
			k = rclass(p->type, resc[0].rval);
			n = rfind(resc[0].rval, k);
			if (p->op == UCALL)
				n->sym = 0;
			else {
				n->off = r->lval;
				n->sym = fpsym1;
				n->flags |= 0x8000;
			}
			conput(r);
			break;
		default:
			putchar(*cp);
		}
	}
	nosuffix = rlmode = rmode = 0;
}

/* ---- match() ----------------------------------------------------------------------------------------- */

/* the node an operand letter of a template stands for */
struct node *
getlr(p, c)
	register struct node *p;
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
}
