/*
 * order3.c - the order of evaluation (codgen / scodgen)
 *
 * Reconstructed from /lib/codgen, the range 0x49c0 .. 0x5c7a: sucomp 0x49f0 (the register need of a subtree).
 * A node has two numbers: su, the word registers its evaluation needs, and fsu, the float registers (bytes 6 and 7).
 */
#include "cg.h"

extern int dope[];
extern int szty();
extern void rallo();
extern int shumul();
extern int cerror();
extern int order();		/* 0x0ba2 */
extern int rstatus[];
extern char *opst[];
extern char *rnames[];
extern struct node *talloc(), *tcopy();
extern struct node *canon();		/* 0x1884 */
extern int allo(), argsize();
extern void tfree(), reclaim();
extern struct node resc[];

#ifdef SEG
#define OFFSTARL(l)	offstar(l)
#else
#define OFFSTARL(l)	offstar((l)->left)
#endif
#define OFFSTARA(l)	offstar((l)->left)

#define MAX(a, b)	((a) < (b) ? (b) : (a))
#define MIN(a, b)	((a) > (b) ? (b) : (a))

/* 0x49c0: is the register (rval of a REG node) unusable as the base of an address: r0, or class 1 in the high byte */
int
notoff(t, r, off)
	int t;
	register int r;
	long off;
{
	if (r >= 0x100)
		return (char)(r >> 8) - 1 == 0;
	return r == 0;
}

int fregs;		/* 0x8f8a, set to 8 by the start of the program */
int ffregs;		/* 0x8f88, set to 4 */

void
sucomp(p)
	struct node *p;
{
	register int o, arity, nr, sul, sur, fr;
	int nf, fl, t;
	register struct node *l, *r;

	o = p->op;
	arity = dope[o] & 0xE;
	t = p->type;
	if (t == 6 || t == 7) {
		nf = 1;
		nr = 0;
	} else {
		nf = 0;
		nr = szty(t) > (dope[o] & 0x800 ? 2 : 1) ? szty(t) : (dope[o] & 0x800 ? 2 : 1);
	}
	if (dope[o] & 0x400) {			/* a call takes all registers */
		p->su = fregs;
		p->fsu = ffregs;
		return;
	}
	l = p->left;
	if (arity == 2) {			/* a leaf */
		p->su = 0;
		p->fsu = 0;
		return;
	}
	if (arity == 4) {			/* unary */
		if (o == STARG && (unsigned)p->stsize > 4) {
			p->su = l->su > fregs - 3 ? l->su : fregs - 3;
			p->fsu = l->fsu;
			return;
		}
		p->su = nr >= l->su ? nr : l->su;
		p->fsu = l->fsu <= nf ? nf : l->fsu;
#ifdef SEG
		if (o == STAR && p->su <= 2)	/* an indirection through a long pointer needs a register pair */
			p->su = 2;
#endif
		return;
	}
	if (arity != 8)
		return;
	r = (struct node *)p->right;
	sul = l->su;
	fl = l->fsu;
	sur = r->su;
	fr = r->fsu;
	if (o == 0x17 || o == 0x18) {		/* && and || */
		p->su = MAX(nr, MAX(sul, sur));
		p->fsu = MAX(nf, MAX(fl, fr));
		return;
	}
	if (dope[o] & 1) {			/* assignments */
		if (sur == 0) {
			if (sul == 0)
				p->su = nr;
			else
				p->su = MAX(nr, sul);
		} else if (sul <= sur && sul + nr > fregs)
			p->su = fregs + nr;
		else if (sul == 0)
			p->su = MAX(sur, nr + nr);
		else
			p->su = MAX(sur, MAX(sul, nr + nr + 1));
		if (fr == 0) {
			if (fl == 0)
				p->fsu = nf;
			else
				p->fsu = MAX(fl, nf);
		} else if (fl <= fr && fl + nf > ffregs)
			p->fsu = ffregs + nf;
		else if (fl == 0)
			p->fsu = MAX(fr, nf + nf);
		else
			p->fsu = MAX(fr, MAX(fl, nf + nf + 1));
		return;
	}
	if ((dope[o] & 0x800) && (l->type == 5 || l->type == 15)) {	/* long * / % */
		p->su = MAX(fregs - 1, MIN(MAX(sul, nr + sur), MAX(sur, nr + sul)));
		if (sur != sul)
			p->fsu = MAX(fl, fr);
		else
			p->fsu = MAX(ffregs - 1, MIN(MAX(fl, nf + fr), MAX(fr, nf + fl)));
		return;
	}
	if ((dope[o] & 0x800) && (dope[l->op] & 0xE) == 2 && nr > szty(l->type)) {
		p->su = MAX(nr, MAX(sul, sur));
		if (sur != sul)
			p->fsu = MAX(fl, fr);
		else
			p->fsu = MAX(nf, MAX(fl, fr));
		return;
	}
	p->su = MAX(nr + nr, MIN(MAX(sul, nr + sur), MAX(sur, nr + sul)));
	if (sur != sul)
		p->fsu = MAX(nf + nf, MAX(MAX(fl, nf + fr), MAX(fr, nf + fl)));
	else
		p->fsu = MAX(nf + nf, MIN(MAX(fl, nf + fr), MAX(fr, nf + fl)));
}

#define NOPREF	0x2000
#define MUSTDO	0x1000

int fltused;		/* 0x33d4: floating point is used, the output gets .extern __fltuse */

/* make sure the use of p gets done with register r: simulates offstar */
void
mkrall(p, r)
	register struct node *p;
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
	register struct node *p;
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
	ty = dope[o] & 0xE;
	switch (o) {
	case ASSIGN:
		down1 = NOPREF;
		down2 = down;
		break;
	case MUL:
		if (p->left->type == 5 || p->left->type == 15) {
			down1 = MUSTDO | 6;
			if (p->right->op != REG)
				down2 = MUSTDO | 4;
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
	case RET:
		if (p->type == 6 || p->type == 7)
			down1 = MUSTDO | 0x10;
		else
			down1 = MUSTDO | 2;
		break;
	}
	if (ty != 2)
		rallo(p->left, down1);
	if (ty == 8)
		rallo(p->right, down2);
}

#define istnode(q)	((q)->op == REG && (rstatus[(q)->rval] & 0x14))

int ordflag;		/* 0xa16e: set to 0x14 while offstar orders a register that is not preferred */

/* order the operands of an address sum that are not yet addressable; returns the number of orders done */
static int
orderadd(p)
	register struct node *p;
{
	register int n;

	n = 0;
	if (p->op == PLUS || p->op == MINUS) {
		if (p->left->op == CALL || p->left->op == 72) {
			if (p->right->op == CALL || p->right->op == 72) {
				order(p->right, 0x1000);
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
	register struct node *p;
{
	register struct node *q, *l, *r;
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
	register struct node *p;
{
	register struct node *l;

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
			if ((dope[p->op] & 0xE) != LTYPE) {
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
	register struct node *p;
{
	register struct node *l;		/* not used here: the declarations are those of the segmented version */
	register struct node *star;
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
			order(p, 6);
			ordflag = save;
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
	register struct node *p;
{
	register int t;

	if (p->op == STAR) {
		t = p->type;
		if (t == 2 || t == 12 || t == 6 || t == 7)
			return 0;
		if (shumul(p->left) == 0x800)
			return 0;
		return 1;
	}
	return 0;
}

/* order the operands of a binary operator; 1 when something was ordered, 0 when the template can be used */
int
setbin(p)
	register struct node *p;
{
	register struct node *r, *l;

	r = p->right;
	l = p->left;
	if (r->su == 0 && r->fsu == 0) {
		if (dope[p->op] & 0x10) {
			if (l->op == STAR && shumul(l->left) != 0x800)
				OFFSTARL(l);
			else
				order(l, 0x101e);
			return 1;
		}
		if (istnode(l))
			return 0;
		if (niceuty(l))
			offstar(l);
		else
			order(l, 0x14);
		return 1;
	}
	if (dope[p->op] & 0x10) {
		if (r->su > l->su || (r->su == l->su && r->fsu >= l->fsu)) {
			if (niceuty(r)) {
				OFFSTARA(r);
				return 1;
			}
			if (r->su > l->su || (r->su == l->su && r->fsu > l->fsu)) {
				if (!istnode(r)) {
					order(r, 0x101e);
					return 1;
				}
			}
		}
		if (niceuty(l)) {
			OFFSTARL(l);
			return 1;
		}
		if (!istnode(l)) {
			order(l, 0x101e);
			return 1;
		}
		if (niceuty(r)) {
			OFFSTARA(r);
			return 1;
		}
		if (!istnode(r)) {
			order(r, 0x101e);
			return 1;
		}
		cerror("setbin can't deal with %s", opst[p->op]);
	}
	if (!istnode(r) && (r->su > l->su || (r->su == l->su && r->fsu > l->fsu))) {
		if (niceuty(r)) {
			OFFSTARA(r);
			return 1;
		}
		order(r, 0x101e);
		return 1;
	}
	if (istnode(l))
		return 0;
	if (niceuty(l))
		offstar(l);
	else
		order(l, 0x14);
	return 1;
}

/* structure assignment */
int
setstr(p)
	struct node *p;
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

/* ASG MUL, ASG DIV, ASG MOD on a long */
static int
lmuldiv(p)
	struct node *p;
{
	if (p->op == ASG MUL || p->op == ASG DIV || p->op == ASG MOD)
		if (p->left->type == 5 || p->left->type == 15)
			return 1;
	return 0;
}

/* setup for the assignment operators (=ops) */
int
setasop(p)
	register struct node *p;
{
	register int sul, sur;
	register struct node *q, *c;
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
				p->left->left->rall = 0x1001;
				p->left->left->left->rall = 0x1001;
			}
			OFFSTARA(p->left);
			return 1;
		}
		if (p->left->op == FLD && p->left->left->op == STAR) {
			if (lmuldiv(p)) {
				p->left->left->left->rall = 0x1001;
				p->left->left->left->left->rall = 0x1001;
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
		order(c->left, 0x14);
		order(c, 0x14);
		return 1;
	}
	if (p->op == ASG MOD || p->op == ASG DIV) {
		if (p->type == 5 || p->type == 15) {
#ifdef SEG
			if (sul) {
				p->right->rall = 0x2000;
				order(p->right, 0x1000);
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

/* setup for the assignment operator */
int
setasg(p)
	register struct node *p;
{
	register struct node *l, *r;

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

/*
 * The arguments of a call: the first ones go to registers. genargs walks the comma tree (argscan) and
 * collects the arguments that get a register in argl (register and node, ended by a node 0), argstrip removes
 * them from the tree, argsort puts the ones that need most registers first and argplace evaluates them.
 */
int nargs;		/* 0xa16a */
int argreg;		/* 0xa16c: the next word register for an argument, counts down from 7 */
int argfreg;		/* 0xa19e: the next float register, counts down from 0x13 */
int argstop;		/* 0xa156: the rest of the arguments go to the stack */
struct arg {
	int	reg;
	struct node *node;
} argl[8];		/* 0xa1a0 */

static struct node *
argscan(p)
	register struct node *p;
{
	register struct node *q, *rest;
	int sz;

	q = p;
	rest = p;
	if (p->op == CM) {
		rest = argscan(p->left);
		if (argstop)
			return rest;
		if (p->right->type == 6 || p->right->type == 7) {
			if (argfreg >= 0x10) {
				rest = p;
				p->op = FREE;
				q = p->right;
			} else {
				if (argstop)
					return rest;
				argstop = 1;
				return p->right;
			}
		} else {
			if (argreg < 2) {
				if (argstop)
					return rest;
				argstop = 1;
				return p->right;
			}
			rest = p;
			p->op = FREE;
			q = p->right;
		}
	}
	if (q->type == 6 || q->type == 7) {
		if (argfreg < 0x10) {
			argstop = 1;
			return q;
		}
		argl[nargs].reg = argfreg;
		argl[nargs].node = q;
		nargs++;
		argl[nargs].node = 0;
		argfreg--;
		return rest;
	}
	sz = argsize(q);
	if (sz == 1) {
push:
		argl[nargs].reg = argreg;
		argl[nargs].node = q;
		nargs++;
		argl[nargs].node = 0;
		argreg--;
		return rest;
	}
	if (sz == 2) {
		argreg--;
		if (argreg & 1)
			argreg--;
		if (argreg < 2) {
			argstop = 1;
			argreg = 1;
			return q;
		}
		goto push;
	}
	if (sz == 4) {
		if (argreg == 7) {
			argl[nargs].reg = argreg - 3;
			argl[nargs].node = q;
			nargs++;
			argl[nargs].node = 0;
			argreg -= 4;
			return rest;
		}
	}
	argstop = 1;
	argreg = 1;
	return q;
}

/* the largest need first */
static void
argsort()
{
	register struct arg *ai, *aj;
	int r;
	struct node *n;

	for (ai = argl; ai->node; ai++)
		for (aj = ai + 1; aj->node; aj++)
			if (ai->node->su < aj->node->su) {
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
	struct node *p;
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
	register struct node *p, *rest;
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

/* evaluate the arguments that go to registers; the ones that need much come first */
static void
argplace()
{
	register struct node *q;
	register int nleft, fleft, regs, i;
	int cnt[4];
	int k;
	int reg;

	regs = 3;
	for (i = 1; i < 4; i++)
		cnt[i] = 0;
	nargs = 0;
	q = argl[nargs].node;
	if (q == 0)
		return;
	nleft = 8;
	fleft = 4;
	while ((q = argl[nargs].node) != 0) {
		if (nleft < q->su || fleft < q->fsu || (regs < 1 && q->su > 1)) {
			order(q, 0x1000);
			nargs++;
			continue;
		}
		if (q->type == 7 || q->type == 6) {
			fleft--;
			nargs++;
			continue;
		}
		nleft -= argsize(q);
		regs = 3;
		cnt[argl[nargs].reg / 2]++;
		for (i = 1; i < 4; i++)
			if (cnt[i])
				regs--;
		nargs++;
	}
	nargs = 0;
	while ((q = argl[nargs].node) != 0) {
		reg = argl[nargs].reg;
		q->rall = reg | 0x1000;
		if (q->type == 6 || q->type == 7)
			order(q, 0x10);
		else
			order(q, 4);
		if (q->type == 2)
			printf("\textsb\t%s\n", rnames[reg]);
		else if (q->type == 12)
			printf("\tldb\trh%d,#0\n", reg);
		tfree(q);
		nargs++;
	}
}

void
genargs(p)
	register struct node *p;
{
	register struct node *rest;

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
