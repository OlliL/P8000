/*
 * order3.c - the order of evaluation: Sethi-Ullman numbers, rewrites, arguments
 */
#include "cg.h"
#include "cg2.h"

extern void adrput();
extern int allo();
extern int argfreg;
extern struct arg argl[];
extern int argreg;
extern int argsize();
extern int argstop;
extern NODE *canon();
extern int cerror();
extern int crslab;
extern NODE *d16acmp;
extern int d2r0reg;
extern NODE *d2ranode;
extern int d2rareg;
extern void deflab();
extern int dope[];
extern int ffregs;
extern int fltused;
extern struct sym *fpsym1;
extern struct sym *fpsym2;
extern int fregs;
extern void genargs();
extern int getlab();
extern int getreg();
extern int shltype();
extern void mkadrs();
extern int mkoreg();
extern void mkrall();
extern int nargs;
extern int niceuty();
extern NODE node[];
extern int notoff();
extern void offstar();
extern char *opst[];
extern void order();
extern int ordflag;
extern void oreg2();
extern void oregfix();
extern void rallo();
extern void reclaim();
extern NODE resc[];
extern void rfree();
extern void recl2();
extern char *rnames[];
extern struct rs *rsfree;
extern int rstatus[];
extern struct rs *rsuse;
extern int setasg();
extern int setasop();
extern int setbin();
extern int setstr();
extern int shumul();
extern int stoasg();
extern int stocook;
extern NODE *stotree;
extern void sucomp();
extern struct sym *symtab0;
extern int szty();
extern NODE *tcopy();
extern void tfree();
static char sccsid[] = "@[$]order3.c\t2.4  08/24/83 13:27:43 Copr. 1981 87wega3.2.";
int fltused = 0;

#define FORREW	0x4000

#define MDONE	0x1001

#define FREE_	100

#ifdef SEG
#define OFFSTARL(l)	offstar(l)
#else
#define OFFSTARL(l)	offstar((l)->left)
#endif

#define OFFSTARA(l)	offstar((l)->left)

#define max(a, b)	((a) > (b) ? (a) : (b))

#define min(a, b)	((a) < (b) ? (a) : (b))

#ifdef OLD_D3K
/* v0: not the register of a parameter (it is copied before an operator changes it) */
#define istnode(q)	((q)->op == REG && (rstatus[(q)->rval] & 0x14) && !((q)->sname && (q)->sname->sclass == 9))
#else
#define istnode(q)	((q)->op == REG && (rstatus[(q)->rval] & 0x14))
#endif

static int orderadd();
static int lmuldiv();
static NODE *argscan();
static void argsort();
static void fixstarg();
static void argstrip();
static void d3aw();
static void d22arg();
static void argplace();

/* an assignment under an indirection: nothing to store, the check only */
int
stoasg(p, o)
	register NODE *p;
	int o;
{
	return shltype(p->left->op, p->left);
}

/* store the subtree of p with the larger register need: stotree and stocook for codgen */
void
mkadrs(p)
	register NODE *p;
{
	register int o;

	o = p->op;
	if asgop(o) {
		if (p->left->su > p->right->su ||
		    (p->left->su == p->right->su && p->left->fsu >= p->right->fsu)) {
			if (p->left->op == STAR) {
				if (p->left->su > 0 || p->left->fsu > 0)
					stotree = p->left->left;
				else if (p->right->su > 0 || p->right->fsu > 0)
					stotree = p->right;
				else {
					cerror("store finds both sides trivial");
					return;
				}
			} else if (p->left->op == FLD && p->left->left->op == STAR)
				stotree = p->left->left->left;
			else
				stotree = p->left;
		} else
			stotree = p->right;
	} else {
		if (p->left->su > p->right->su || (p->left->su == p->right->su && p->left->fsu > p->right->fsu))
			stotree = p->left;
		else
			stotree = p->right;
	}
	stocook = 0x1000;
}

/* 0x49c0: is the register (rval of a REG node) unusable as the base of an address: r0, or class 1 in the high byte */
int
notoff(t, r, off)
	int t;
	register int r;
	long off;
{
	if (r >= 0x100)
		return (r >> 8) - 1 == 0;
	return r == 0;
}

void
sucomp(p)
	register NODE *p;
{
	register int o, ty, sul, sur, nr;
	int fl, fr, nf, t;

	o = p->op;
	ty = optype(o);
	if ((t = p->type) == 6 || t == 7) {
		nf = 1;
		nr = 0;
	} else {
		nf = 0;
		nr = max(szty(t), dope[p->op] & MULFLG ? 2 : 1);
	}
	if callop(o) {			/* a call takes all registers */
		p->su = fregs;
		p->fsu = ffregs;
		return;
	}
	switch (ty) {
	case LTYPE:
		p->su = 0;
		p->fsu = 0;
		return;
	case UTYPE:
		if (p->op == STARG && (unsigned)p->stsize > 4) {
			p->su = max(p->left->su, fregs - 3);
			p->fsu = p->left->fsu;
			return;
		}
		p->su = max(p->left->su, nr);
#ifdef SEG
		if (o == STAR && p->su <= 2)	/* an indirection through a long pointer needs a register pair */
			p->su = 2;
#endif
		p->fsu = max(p->left->fsu, nf);
		return;
	case BITYPE:
		sul = p->left->su;
		fl = p->left->fsu;
		sur = p->right->su;
		fr = p->right->fsu;
		if (o == 0x17 || o == 0x18) {		/* && and || */
			p->su = max(max(sul, sur), nr);
			p->fsu = max(max(fl, fr), nf);
			return;
		}
		if asgop(o) {			/* assignments */
			if (sur == 0) {
				if (sul == 0)
					p->su = nr;
				else
					p->su = max(sul, nr);
			} else if (sul <= sur && sul + nr > fregs)
				p->su = fregs + nr;
			else if (sul == 0)
				p->su = max(sur, nr + nr);
			else
				p->su = max(sur, max(sul, 1 + nr + nr));
			if (fr == 0) {
				if (fl == 0)
					p->fsu = nf;
				else
					p->fsu = max(fl, nf);
			} else if (fl <= fr && fl + nf > ffregs)
				p->fsu = ffregs + nf;
			else if (fl == 0)
				p->fsu = max(fr, nf + nf);
			else
				p->fsu = max(fr, max(fl, 1 + nf + nf));
			return;
		}
		if ((dope[p->op] & MULFLG) && (p->left->type == 5 || p->left->type == 15)) {	/* long * / % */
			p->su = max(fregs - 1, min(max(sul, nr + sur), max(nr + sul, sur)));
			if (sur == sul)
				p->fsu = max(ffregs - 1, min(max(fl, nf + fr), max(nf + fl, fr)));
			else
				p->fsu = max(fl, fr);
			return;
		}
		if ((dope[p->op] & MULFLG) && optype(p->left->op) == 2 && nr > szty(p->left->type)) {
			p->su = max(nr, max(sul, sur));
			if (sur == sul)
				p->fsu = max(nf, max(fl, fr));
			else
				p->fsu = max(fl, fr);
			return;
		}
		p->su = max(nr + nr, min(max(sul, nr + sur), max(sur, nr + sul)));
		if (sur == sul)
			p->fsu = max(nf + nf, min(max(fl, nf + fr), max(fr, nf + fl)));
		else
			p->fsu = max(nf + nf, max(max(fl, nf + fr), max(fr, nf + fl)));
	}
}

/* make sure the use of p gets done with register r: simulates offstar */
void
mkrall(p, r)
	register NODE *p;
	register int r;
{
	if (p->op == FLD) {
		p->left->rall = p->rall;
		p = p->left;
	}
	if (p->op != STAR)
		return;
	p = p->left;
	if (p->op == STAR) {
		p->rall = r;
		p = p->left;
	}
	if (p->op == PLUS && p->right->op == ICON) {
		p->rall = r;
		p = p->left;
	}
	rallo(p, r);
}

/* register preferences for the subtrees */
void
rallo(p, down)
	register NODE *p;
	int down;
{
	register int o, down1, down2, ty;

	if (p->type == 6 || p->type == 7) {
		fltused++;
		return;
	}
	down2 = NOPREF;
	p->rall = down;
	down &= ~MUSTDO;
	down1 = down;
	o = p->op;
	ty = optype(o);
	switch (o) {
	case ASSIGN:
		down1 = NOPREF;
		down2 = down;
		break;
	case ANDAND:
	case OROR:
	case CALL:
	case NOT:
	case EQ:
	case NE:
	case LE:
	case LT:
	case GE:
	case GT:
	case STASG:
	case STARG:
		down1 = NOPREF;
		break;
	case MUL:
		if (p->left->type == 5 || p->left->type == 15) {
			down1 = MUSTDO | 6;
#ifndef OLD_D2U
			if (p->right->op != REG)
				down2 = MUSTDO | 4;	/* (OLD_D2U: v0 wants no register for the right operand) */
#endif
		}
		break;
	case ASG MUL:
		if (p->left->type == 5 || p->left->type == 15) {
			down1 = MUSTDO | 6;
			down2 = MUSTDO | 4;
		}
		break;
	case DIV:
	case ASG DIV:
	case MOD:
	case ASG MOD:
		if (p->left->type == 5 || p->left->type == 15) {
			down1 = MUSTDO | 6;
			down2 = MUSTDO | 2;
		}
#ifdef OLD_D2D
		else if (down == NOPREF)
			down1 = MUSTDO | 1;		/* the odd half of rr0 */
		else if (!(down & 1) && down < 8)
			down1 = MUSTDO | (down + 1);	/* the odd half of the pair of the wanted register */
#endif
		break;
#ifdef OLD_D5T
	case PLUS:
		if ((p->rall & MUSTDO) && p->right->op == REG && p->right->rval == 15 && p->left->op == PLUS)
			down1 = NOPREF;	/* an element of a local array as an address: as OLD_D5A */
		break;
#endif
#ifdef OLD_D5A
	case STAR:
		if ((p->rall & MUSTDO) && p->left->op == PLUS && p->left->right->op == REG && p->left->right->rval == 15)
			down1 = NOPREF;	/* v0p: the address of a local array element wanted in a given register goes anywhere */
		break;
#endif
	case RET:
		if (p->type != 6 && p->type != 7)
			down1 = MUSTDO | 2;
		else
			down1 = MUSTDO | 0x10;
		break;
	}
	if (ty != 2)
		rallo(p->left, down1);
	if (ty == 8)
		rallo(p->right, down2);
}

/* order the operands of an address sum that are not yet addressable; returns the number of orders done */
static int
orderadd(p)
	register NODE *p;
{
	register int n;

	n = 0;
	if (p->op == PLUS || p->op == MINUS) {
		if (p->left->op == CALL || p->left->op == 72) {
			if (p->right->op == CALL || p->right->op == 72) {
				order(p->right, INTEMP);
				n++;
				order(p->left, 6);
				n++;
			} else {
				order(p->left, 6);
				n++;
				if (p->right->op != ICON && p->right->op != REG) {
					order(p->right, 6);
					n++;
				}
			}
		} else {
			if (p->right->op != ICON && p->right->op != REG) {
				order(p->right, 6);
				n++;
			}
			if (p->left->op != ICON && p->left->op != REG) {
				order(p->left, 6);
				n++;
			}
		}
	}
	return n;
}

#ifdef SEG
extern struct sym *symtab0;
extern void oreg2();

/* an indirection through a register plus a constant or a register plus a register becomes an OREG
   (rval: ((base + 1) << 8) | (index + 1)); 1 when it did */
int
mkoreg(p)
	register NODE *p;
{
	register NODE *q, *l, *r;
	int rv;
	long off;
	struct sym *sym;

	if (p->op != STAR)
		return 0;
	q = p->left;
	l = q->left;
	r = q->right;
	if (q->op == PLUS || q->op == MINUS) {
		if (r->op == ICON && l->op == REG && p->type != 6 && p->type != 7) {
			off = r->lval;
			rv = (l->rval + 1) << 8;
			sym = r->sname;
			if (q->op == MINUS) {
				if (sym ? sym->nameoff : 0)
					return 0;
				off = -off;
			}
			goto set;
		}
	}
	if (q->op == PLUS && r->op == REG && l->op == REG && p->type != 6 && p->type != 7) {
		if (r->rval == 0)
			return 0;
		rv = ((l->rval + 1) << 8) | (r->rval + 1);
		off = 0;
		sym = r->sname;
		goto set;
	}
	oreg2(p);
	return p->op == OREG;
set:
	if ((rv >> 8) == 1)
		return 0;
	p->op = OREG;
	p->rval = rv;
	p->lval = off;
	p->stsize = 0;
	if (sym == symtab0 || sym == fpsym1 || sym == fpsym2)
		p->sname = sym;
	else
		p->sname = 0;
	tfree(q);
	return 1;
}

/* handle indirections: get the address of *p into a register; p is the STAR itself or its operand */
void
offstar(p)
	register NODE *p;
{
	register NODE *l;

	if (p->op == STAR) {
		l = p->left;
		if (l->op == PLUS || l->op == MINUS) {
			orderadd(l);
			if (mkoreg(p)) {
				order(p, p->type == 6 || p->type == 7 ? 0x18 : 6);
				return;
			}
		}
		if (l->op == REG) {
			oreg2(p);
			if (p->op == OREG && !(p->rall & MUSTDO))
				return;
		}
		if (p->rall & MUSTDO) {
			if (optype(p->op) != LTYPE) {
				orderadd(l);
				order(l, 6);
			}
			order(p, p->type == 6 || p->type == 7 ? 0x18 : 6);
			return;
		}
		p = l;
	}
	orderadd(p);
	order(p, 6);
}
#else
/* handle indirections: get the address of *p into a register */
void
offstar(p)
	register NODE *p;
{
	register NODE *l;		/* not used here: the declarations are those of the segmented version */
	register NODE *star;
	register int save;

	star = 0;
	if (p->op == STAR) {
		if (p->rall & MUSTDO)
			star = p;
		p = p->left;
	}
	if (orderadd(p) == 0) {
		if (p->op == REG && p->rval == 0) {
			save = ordflag;
			ordflag = 0x14;
#ifdef OLD_D2RA
			/* v0p: an address r0 + rN moves to rN when that is free again */
			if (p == d2ranode) {
				register struct rs *n;

				for (n = rsfree; n; n = n->next)
					if ((n->cls == 2 && n->reg == d2rareg) || (n->cls == 3 && n->reg == (d2rareg & ~1))) {
						p->rall = d2rareg | MUSTDO;
						break;
					}
			}
#endif
			order(p, 6);
			ordflag = save;
#ifdef OLD_D2R0
			d2r0reg = p->op == REG ? p->rval : -1;	/* the address moved out of r0 */
#endif
		} else
			order(p, 6);
	}
	if (star) {
		if (star->type == 6 || star->type == 7)
			order(star, 0x18);
		else
			order(star, 6);
	}
}
#endif

/* is p an indirection that offstar can make addressable */
int
niceuty(p)
	register NODE *p;
{
	register int t;

	return p->op == STAR && (t = p->type) != 2 && t != 12 && t != 6 && t != 7 && shumul(p->left) != 0x800;
}

/* order the operands of a binary operator; 1 when something was ordered, 0 when the template can be used */
int
setbin(p)
	register NODE *p;
{
	register NODE *r, *l;

	r = p->right;
	l = p->left;
#ifdef OLD_D16A
	if (p->op >= EQ && p->op <= UGT)
		d16acmp = l;
#endif
	if (p->right->su == 0 && p->right->fsu == 0) {
		if (!logop(p->op))
			goto tail;
		if (l->op == STAR && shumul(l->left) != 0x800)
			OFFSTARL(l);
		else
			order(l, INAREG|INTAREG|INBREG|INTBREG|INTEMP);
		return 1;
	nice:
		if (niceuty(l))
			goto off;
	ord:
		order(l, INTAREG|INTBREG);
		return 1;
	}
	if logop(p->op) {
		if (r->su > l->su || (r->su == l->su && r->fsu >= l->fsu)) {
			if (niceuty(r)) {
				OFFSTARA(r);
				return 1;
			}
			if (r->su > l->su || (r->su == l->su && r->fsu > l->fsu)) {
				if (!istnode(r)) {
					order(r, INAREG|INTAREG|INBREG|INTBREG|INTEMP);
					return 1;
				}
			}
		}
		if (niceuty(l)) {
			OFFSTARL(l);
			return 1;
		}
		if (!istnode(l)) {
			order(l, INAREG|INTAREG|INBREG|INTBREG|INTEMP);
			return 1;
		}
		if (niceuty(r)) {
			OFFSTARA(r);
			return 1;
		}
		if (!istnode(r)) {
			order(r, INAREG|INTAREG|INBREG|INTBREG|INTEMP);
			return 1;
		}
		cerror("setbin can't deal with %s", opst[p->op]);
	}
	if (!istnode(r) && (r->su > l->su || (r->su == l->su && r->fsu > l->fsu))) {
		if (niceuty(r)) {
			OFFSTARA(r);
			return 1;
		}
		order(r, INAREG|INTAREG|INBREG|INTBREG|INTEMP);
		return 1;
	}
	if (istnode(l))
		goto r0;
	if (!niceuty(l))
		goto ord;
off:
	offstar(l);
	return 1;
tail:
	if (!istnode(l))
		goto nice;
r0:
	return 0;
}

/* structure assignment */
int
setstr(p)
	register NODE *p;
{
	if (p->right->op != REG) {
		order(p->right, 4);
		return 1;
	}
	p = p->left;
	if (p->op != NAME && p->op != OREG) {
		if (p->op != STAR)
			cerror("bad setstr");
		order(p->left, 4);
		return 1;
	}
	return 0;
}

/* setup for the assignment operator */
int
setasg(p)
	register NODE *p;
{
	register NODE *l, *r;

	l = p->left;
	r = p->right;
	if (l->su > r->su && l->op == STAR) {
		OFFSTARA(l);
		return 1;
	}
	if (r->su != 0 && r->op != REG) {
		if (r->op == STAR)
			OFFSTARL(r);
		else
			order(r, 0x2ca);
		return 1;
	}
	if (r->op != REG && (p->type == 6 || p->type == 7)) {
		order(p->right, 0x18);
		return 1;
	}
	if (l->op == STAR) {
		OFFSTARA(l);
		return 1;
	}
	if (l->op == FLD && l->left->op == STAR) {
		OFFSTARA(l->left);
		return 1;
	}
	if (r->op != REG) {
		order(r, 10);
		return 1;
	}
	return 0;
}

/* ASG MUL, ASG DIV, ASG MOD on a long */
static int
lmuldiv(p)
	register NODE *p;
{
	switch (p->op) {
	case ASG MUL:
	case ASG DIV:
	case ASG MOD:
		if (p->left->type == 5 || p->left->type == 15)
			return 1;
	default:
		return 0;
	}
}

/* setup for the assignment operators (=ops) */
int
setasop(p)
	register NODE *p;
{
	register int sul, sur;
	register NODE *q, *c;
	register int fl, fr;

	sul = p->left->su;
	sur = p->right->su;
	fl = p->left->fsu;
	fr = p->right->fsu;
	if (sur == 0 && fr == 0) {
leftadr:
		if (sul == 0 && fl == 0)
			goto rew;
		if (p->left->op == STAR) {
			if (lmuldiv(p)) {
#ifdef SEG
				p->left->left->rall = 0x1002;
				p->left->left->left->rall = 0x1002;
#else
				p->left->left->rall = 0x1001;
				p->left->left->left->rall = 0x1001;
#endif
			}
			OFFSTARA(p->left);
			return 1;
		}
		if (p->left->op == FLD && p->left->left->op == STAR) {
			if (lmuldiv(p)) {
#ifdef SEG
				p->left->left->left->rall = 0x1002;
				p->left->left->left->left->rall = 0x1002;
#else
				p->left->left->left->rall = 0x1001;
				p->left->left->left->left->rall = 0x1001;
#endif
			}
			OFFSTARA(p->left->left);
			return 1;
		}
rew:
		q = p->left;
		if (q->op == FLD)
			q = q->left;
		if (q->op != STAR || shumul(q->left) != 0x800)
			return 0;
		c = tcopy(p);
		p->op = ASSIGN;
		reclaim(p->right, 0, 0);
		p->right = c;
		if (q->left->op == INCR) {
			q = c->left;
			if (q->op == FLD)
				q = q->left;
			if (q->left->op != INCR)
				cerror("bad incr rewrite");
		} else if (q->left->op != ASG MINUS)
			cerror(" bad -= rewrite");
		q->left->right->op = FREE;
		q->left->op = FREE;
		q->left = q->left->left;
		p = canon(p);
		rallo(p, p->rall);
		order(c->left, INTAREG|INTBREG);
		order(c, INTAREG|INTBREG);
		return 1;
	}
	if (p->op == ASG MOD || p->op == ASG DIV) {
		if (p->type == 5 || p->type == 15) {
#ifdef SEG
			if (sul) {
				p->right->rall = 0x2000;
				order(p->right, INTEMP);
				return 1;
			}
#endif
			order(p->right, 4);
			return 1;
		}
	}
	if (sul > sur && p->left->op == STAR) {
#ifndef SEG
		if (lmuldiv(p))
			p->left->left->rall = 0x1002;
#endif
		OFFSTARA(p->left);
		return 1;
	}
	if (p->right->op == STAR) {
		OFFSTARL(p->right);
		return 1;
	}
	if (p->right->op == REG)
		goto leftadr;
	order(p->right, 10);
	return 1;
}

int crslab = 10000;	/* 0x3426 */

int
getlab()
{
	return crslab++;
}

void
deflab(l)
	int l;
{
	printf("L%d:\n", l);
}

void
genargs(p)
	register NODE *p;
{
	register NODE *rest;

	nargs = 0;
	argl[nargs].node = 0;
	argreg = 7;
	argfreg = 0x13;
	argstop = 0;
	rest = argscan(p);
	if (p != rest || (p == rest && argstop == 1))
		argstrip(p, rest);
	argsort();
	argplace();
}

static NODE *
argscan(p)
	register NODE *p;
{
	register NODE *rest, *q;

	rest = p;
	if (p->op == CM) {
		rest = argscan(p->left);
		if (argstop) {
		ret:
			return rest;
		}
#ifdef OLD_D22
		if (0) {	/* v0p: a double argument goes by its size (rq4 when it is the first one) */
#else
		if (p->right->type == 6 || p->right->type == 7) {
#endif
			if (argfreg < 0x10) {
				if (argstop)
					goto ret;
				argstop = 1;
				return p->right;
			}
		} else if (argreg < 2) {
			if (argstop)
				goto ret;
			argstop = 1;
			return p->right;
		}
		rest = p;
		p->op = FREE;
		p = p->right;
	}
#ifdef OLD_D22
	if (0) {
#else
	if (p->type == 6 || p->type == 7) {
#endif
		if (argfreg < 0x10)
			goto stop;
		argl[nargs].reg = argfreg;
		argl[nargs].node = p;
		nargs++;
		argl[nargs].node = 0;
		argfreg--;
		goto ret;
	} else
		goto sw;
def:
	argstop = 1;
	argreg = 1;
retp:
	return p;
c2:
	if (--argreg & 1)
		argreg--;
	if (argreg < 2)
		goto def;
c1:
	argl[nargs].reg = argreg;
	argl[nargs].node = p;
	nargs++;
	argl[nargs].node = 0;
	argreg--;
	goto ret;
stop:
	argstop = 1;
	goto retp;
sw:
	switch (argsize(p)) {
	case 4:
		if (argreg != 7)
			goto def;
		argl[nargs].reg = argreg - 3;
		argl[nargs].node = p;
		nargs++;
		argl[nargs].node = 0;
		argreg -= 4;
		goto ret;
	case 1:
		goto c1;
	case 2:
		goto c2;
	default:
		argstop = 1;
		argreg = 1;
		goto retp;
	}
}

/* the largest need first */
static void
argsort()
{
	register struct arg *ai, *aj;
	register int r;
	register NODE *n;

#ifdef OLD_D19A
	/* v0: a constant argument counts as needing less than any other one (it is loaded after them) */
#define ARGSU(q)	((q)->op == ICON ? -1 : (q)->su)
#else
#define ARGSU(q)	((q)->su)
#endif
	for (ai = argl; ai->node; ai++)
		for (aj = ai + 1; aj->node; aj++)
			if (ARGSU(ai->node) < ARGSU(aj->node)) {
				r = aj->reg;
				n = aj->node;
				aj->reg = ai->reg;
				aj->node = ai->node;
				ai->reg = r;
				ai->node = n;
			}
}

/* a structure argument: allocate its place on the stack as for a structure assignment */
static void
fixstarg(p)
	register NODE *p;
{
	struct optab q;

	q.needs = 0x400;
	q.nx = 0x20;
	q.cstring = "/* dummy from fixstarg */\n";
	if (!allo(p, &q))
		cerror("bad STARG allocation");
	resc[0].type = 8;
	p->op = STASG;
	p->right = p->left;
	p->left = tcopy(resc);
	p->type = 0x18;
}

/* remove the arguments that were collected from the tree up to rest and order the first of the rest */
static void
argstrip(p, rest)
	register NODE *p, *rest;
{
	if (rest == p->right) {
		p->op = FREE;
		p = p->right;
	}
	if (p != rest) {
		if (p->op == CM || p->op == FREE) {
			argstrip(p->left, rest);
			p->op = FREE;
			p = p->right;
		}
	}
	if (p->op == STARG) {
		fixstarg(p);
		order(p, 1);
		return;
	}
	if (p->op == CM || p->op == FREE)
		return;
	if (p->op != REG) {
		if (p->type == 2 || p->type == 12) {
			order(p, 4);
			if (p->type == 2)
				printf("\textsb\t%s\n", rnames[p->rval]);
			else
				printf("\tldb\trh%d,#0\n", p->rval);
			p->type = 4;
		}
	}
	order(p, 0x2000);
}

#ifdef OLD_D3AW
/* forget the registers of the parameters in p that are the whole tree (top) or under an indirection, when they lie in
   the registers lo .. hi the argument is wanted in */
static int d3awlo, d3awhi;

static void
d3aw(p, top)
	register NODE *p;
	int top;
{
	register struct rs *n;
	register int k;

	if (p->op == NAME && p->sname && p->sname->sclass == 9) {
		if (top)
			for (n = rsuse; n; n = n->next)
				if (n->cls <= 4 && n->sym == p->sname && n->reg <= d3awhi &&
				    n->reg + (n->cls == 3) >= d3awlo)
					n->sym = 0;
		return;
	}
	k = optype(p->op);
	if (k == LTYPE || p->op == QUEST)
		return;
	if (p->op == STAR)
		top = 1;		/* below an indirection: an address */
	else if (p->op != PLUS && p->op != MINUS && p->op != UMINUS && p->op != MUL && p->op != 110 /* SCONV */)
		top = 0;		/* below another operator: a computed value */
	d3aw(p->left, top);
	if (k == BITYPE)
		d3aw(p->right, top);
}
#endif

#ifdef OLD_D22
/* v0p: a double argument into the CPU registers reg .. reg+3: from memory with ldm, else through the EPU */
static void
d22arg(q, reg)
	register NODE *q;
	int reg;
{
	if (q->op == NAME)
		oregfix(q);
	if (q->op == NAME || q->op == OREG) {
		printf("\tldm\tr%d,", reg);
		adrput(q);
		printf(",#4\n");
		recl2(q);
	} else {
		order(q, 0x10);
		printf("\tfldd\tr%d,%s\n", reg, rnames[q->rval]);
		rfree(q->rval, q->type);
	}
	getreg(3, reg, 0, (struct sym *)0);
	getreg(3, reg + 2, 0, (struct sym *)0);
}
#endif

/* evaluate the arguments that go to registers; the ones that need much come first */
static void
argplace()
{
	register NODE *q;
	register int nleft, fleft, regs, i;
	int reg;
	int cnt[4];

	regs = 3;
	for (i = 1; i < 4; i++)
		cnt[i] = 0;
	nargs = 0;
	q = argl[nargs].node;
	if (q == 0)
		return;
	nleft = 8;
	fleft = 4;
	for (; (q = argl[nargs].node) != 0; nargs++) {
		if (nleft < q->su || fleft < q->fsu || (regs < 1 && q->su > 1)) {
			order(q, INTEMP);
			continue;
		}
#ifdef OLD_D22
		if ((q->type == 7 || q->type == 6) && argl[nargs].reg < 8) {
			nleft -= 4;
			continue;
		}
#endif
		if (q->type == 7 || q->type == 6) {
			fleft--;
			continue;
		}
		nleft -= argsize(q);
		regs = 3;
		++cnt[argl[nargs].reg / 2];
		for (i = 1; i < 4; i++)
			if (cnt[i])
				regs--;
	}
	for (nargs = 0; (q = argl[nargs].node) != 0; nargs++) {
		reg = argl[nargs].reg;
#ifdef OLD_D22
		if ((q->type == 6 || q->type == 7) && reg < 8) {
			d22arg(q, reg);
			tfree(q);
			continue;
		}
#endif
		q->rall = reg | MUSTDO;
#ifdef OLD_D3AW
		/* v0p: a parameter that is a whole argument or is used as an address in it is not taken from its
		   register (only there; in a computed value it is) */
		d3awlo = reg;
		d3awhi = reg + szty(q->type) - 1;
		d3aw(q, 1);
#endif
		if (q->type == 6 || q->type == 7)
			order(q, 0x10);
		else
			order(q, 4);
		if (q->type == 2)
			printf("\textsb\t%s\n", rnames[reg]);
		else if (q->type == 12)
			printf("\tldb\trh%d,#0\n", reg);
		tfree(q);
	}
}
