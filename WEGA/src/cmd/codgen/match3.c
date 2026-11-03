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
extern int dope[];
extern void expand();
extern void fixname();
extern int fldoff;
extern int fldshf;
extern int fldsz;
extern struct sym *fpsym1;
extern NODE *getlr();
extern int getreg();
extern void hopcode();
extern void insput();
extern int shltype();
extern int ltypeop();
extern int match();
extern NODE node[];
extern int nosuffix;
extern int opclass[];
extern struct optab *opptr[];
extern void oregfix();
extern void prlong();
extern int rclass();
extern void rcount();
extern void reclaim();
extern NODE resc[];
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
	register NODE *p;
	int shape;
{
	register int o = p->op;
	register int r;

	if (shape == SANY)
		return 1;
	if (shape & SPECIAL) {
		switch (shape) {
		case SZERO:		/* the constant 0 without a name */
			if (o != ICON || p->sname != 0 || p->rval < 0)
				return 0;
			return p->lval == 0;
		default:
			return special(p, shape);
		}
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
		o = p->rval;
		fldsz = o & 0x3F;
		fldoff = o >> 6;
		fldshf = 16 - fldsz - (o >> 6);
		return 1;
	case CCODES:
		return shape & SCC;
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
	case STAR:
		return shumul(p->left) & shape;
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
					else if (shltype(op, (NODE *)0))
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
	register NODE *p;
	register int cookie;
{
	register struct optab *q;
	register NODE *r, *l;
	register int spare;
	int x = 0;
	int t;
	int y;

	rcount();
	switch optype(p->op) {
	case BITYPE:
		if (p->left->op == NAME && optype(p->right->op) == LTYPE) {
			if asgop(p->op)
				goto asg;
			fixname(p->left);
		}
		goto rgt;
	elsq:
		q = opptr[p->op];
		goto top;
	fxl:
		fixname(p->left);
	cont:
		if (cookie != FORREW)
			goto elsq;
		q = rwdefault;
		goto top;
	case LTYPE:
		if (p->op != NAME)
			goto cont;
		fixname(p);
#ifdef OLD_D2C
		/* v0: a name (not a parameter) found in a scratch register is that register, which then loses the name */
		if (p->op == REG && (cookie & STAREG) && (rstatus[p->rval] & STAREG) && p->type != 5 && p->type != 15 &&
		    p->sname && p->sname->sclass != 9 &&
		    (!(p->rall & MUSTDO) || (p->rall & ~MUSTDO) == p->rval)) {
			symdrop(p->rval);
			return MDONE;
		}
#endif
		goto cont;
	case UTYPE:
#ifdef OLD_D3L
		/* v0: a word cast of a global long is not taken from the register that holds the long */
		if (p->op == 110 && p->left->op == NAME && (p->left->type == 5 || p->left->type == 15) && p->type != 5 && p->type != 15 &&
		    p->left->sname->sclass != 9 && p->left->sname->sclass != 1 && p->left->sname->sclass != 4)
			goto cont;
#endif
		if (p->left->op == NAME)
			goto fxl;
		if (p->left->op == SCONV && p->left->left->op == NAME)
			fixname(p->left->left);
		goto cont;
	asg:		/* an assignment: the target must stay in memory */
		t = (int)p->left;
		symuse(((NODE *)t)->sname);
		oregfix((NODE *)t);
	rgt:
		if (p->right->op == NAME) {
			if ((p->type == 5 || p->type == 15) && ((dope[p->op] & MULFLG) || (dope[p->op] & DIVFLG))) {
				register int rv;

				rv = p->right->rval;
				/* mult and div use r4..r7: a right operand in r6 (or r4 for div) goes to memory */
				if (rv == 6 || ((dope[p->op] & DIVFLG) && rv == 4)) {
					oregfix(p->right);
					goto cont;
				}
			}
#ifdef OLD_D3AW
			/* v0p: a parameter added to an address constant is not taken from its register */
			if (p->op == PLUS && p->left->op == ICON && p->left->sname && p->right->sname->sclass == 9)
				oregfix(p->right);
			else
#endif
			if (optype(p->left->op) == LTYPE || p->left->op == FLD)
				fixname(p->right);
		}
		goto cont;
	}
	goto cont;
aa:
	if (q->op == p->op)
		goto cand;
	goto next;
zz:
	if (p->op == NAME)
		goto cand;
	if (p->op == ICON)
		goto cand;
	if (p->op == OREG)
		goto cand;
	if (shltype(p->op, p))
		goto cand;
next:
	++q;
top:
	if (q->op == FREE)
		goto nom;
	if (q->op < 0x1000)
		goto aa;
	{
		register int opmtemp;

		if ((opmtemp = opclass[q->op - 0x1000]) & 0x4000)
			goto zz;
		if ((dope[p->op] & (opmtemp | 1)) == opmtemp) {
cand:
		if (!(q->visit & cookie))
			goto next;
		l = r = getlr(p, 'L');
		if (!tshape(r, q->lshape))
			goto next;
		if (!ttype(r->type, q->ltype))
			goto next;
#ifdef OLD_D10
		/* the old codgen v0 shifts a long by a constant through a register (ld A1,#n; sdal): no OIZB AL,#n */
		if (q->lshape == 0x6 && q->rshape == 0x80 && q->op == 0x1013 && (r->type == 5 || l->type == 15) &&
		    (p->op == LS || p->op == RS || p->op == ASG LS || p->op == ASG RS))
			goto next;
#endif
		r = getlr(p, 'R');
		if (!tshape(r, q->rshape))
			goto next;
		if (!ttype(r->type, q->rtype))
			goto next;
#ifdef OLD_D16A
		/* v0p: a frame slot incremented by a constant whose value may go to a temporary is loaded, incremented in
		   the register and stored (ld A1,AL; Zi A1,AR; ld AL,A1), not incremented in memory */
		if (p->op == ASG PLUS && (cookie & INTEMP) && l->op == OREG && l->rval == 15 &&
		    r->op == ICON && r->sname == 0) {
			if (q->rshape == 0x8002)
				goto next;
			if (q->needs == 1 && q->rewrite == 0x804 && allo(p, q)) {
				expand(p, cookie, "\tldZB\tA1,AL\n\tZi\tA1,AR\n\tldZB\tAL,A1\n");
				reclaim(p, q->rewrite, cookie);
				return MDONE;
			}
		}
#endif
		if (q->ltype == 0x4000 || q->rtype == 0x4000) {		/* both operands of the same type */
			if (r->type != l->type)
				goto next;
		} else if (q->ltype == 0x8000 || q->rtype == 0x8000) {	/* of the same size */
			t = bsize(l->type);
			if (bsize(r->type) != t)
				goto next;
		}
		if ((p->type == 6 || p->type == 7) && q->ltype == 0x8000 && q->rtype == 0x8000)
			goto next;
		if (q->needs & REWRITE)
			return q->rewrite;
		if (!allo(p, q))
			goto next;
#ifdef OLD_D2A
		t = p->op;
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
		if (t == ASSIGN && !(cookie & 01) && p->op == REG && (p->type == 5 || p->type == 15) && p->rval < 8 &&
		    !(p->rall & MUSTDO)) {	/* (not when the value goes to a register wanted for it: an argument) */
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
		goto next;
	}
nom:
	return NOMATCH;
}

/* expand the template cp for the tree p */
int tflag = 0;

void
expand(p, cookie, cp)
	register NODE *p;
	register int cookie;
	register char *cp;
{
	/* generate code by interpreting table entry */
	register long val;
	register NODE *r;
	register struct rs *n;

	tflag = 0;
	for (; *cp; ++cp) {
		switch (*cp) {
		default:
			putchar(*cp);
			continue;	/* this is the usual case... */
		case 'A':		/* address of */
			adrput(getlr(p, *++cp));
			continue;
		case 'B':		/* byte offset in word */
			val = getlr(p, *++cp)->lval;
			val = val & 1;
			printf("%ld", val);
			continue;
		case 'C':		/* for constant value only */
			/* the result register of a call holds the value of the right operand: note it */
			r = getlr(p, *++cp);
			n = rfind(resc[0].rval, rclass(p->type, resc[0].rval));
			if (p->op == UCALL)
				n->sym = 0;
			else {
				n->off = r->lval;
				n->sym = fpsym1;
				n->flags |= 0x8000;
			}
			conput(r);
			continue;
		case 'F':		/* this line deleted if FOREFF is active */
			if (cookie & SANY)
				while (*++cp != '\n')
					;
			continue;
		case 'H':		/* field shift */
			printf("%d", fldshf);
			continue;
		case 'I':		/* in instruction */
			insput(getlr(p, *++cp));
			continue;
		case 'L':		/* output special label field */
			printf("%d", p->label);
			continue;
		case 'M':		/* field mask */
		case 'N':		/* complement of field mask */
			val = 1;
			val <<= fldsz;
			--val;
			val <<= fldshf;
			prlong(*cp == 'M' ? val : ~val);
			continue;
		case 'O':		/* opcode string */
			hopcode(p);
			cp++;
			continue;
		case 'S':		/* field size */
			printf("%d", fldsz);
			continue;
		case 'T':		/* rewrite register type is suppressed */
			tflag = 1;
			continue;
		case 'U':		/* for upper half of address, only */
			upput(getlr(p, *++cp));
			continue;
		case 'V':
			rmode++;
			continue;
		case 'W':
			rlmode++;
			continue;
		case 'X':
			rlmode = 0;
			rmode = 0;
			continue;
		case 'Y':
			r = getlr(p, 'R');
			if (r->op == FLD)
				r = r->left;
			if (r->op == ICON)
				acon(r);
			else
				adrput(getlr(p, 'R'));
			continue;
		case 'Z':		/* special machine dependent operations */
			zzzcode(p, *++cp);
			continue;
		}
	}
	nosuffix = 0;
	rlmode = 0;
	rmode = 0;
}

/* ---- match() ----------------------------------------------------------------------------------------- */

/* the node an operand letter of a template stands for */
NODE *
getlr(p, c)
	register NODE *p;
	char c;
{
	switch (c) {
	case '1': case '2': case '3':
		return &resc[c - '1'];
	case 'L': case 'l':
		return optype(p->op) == LTYPE ? p : p->left;
	case 'R': case 'r':
		return optype(p->op) == BITYPE ? p->right : p;
	}
	cerror("bad getlr: %c", c);
}
