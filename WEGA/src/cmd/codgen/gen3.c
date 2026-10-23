/*
 * gen3.c - calls, the next cookie, and the small tree fixes used by the order of evaluation (codgen / scodgen)
 *
 * Reconstructed from /lib/codgen: genscall() 0x4520, gencall() 0x4534, nextcook() 0x4674, lastchance() 0x46ae,
 * icright() 0x46d0, canon2() 0x46fe, mycanon() 0x4804, stoasg() 0x487e, mkadrs() 0x48a0. Names are guesses.
 */
#include "cg.h"

#define FORREW	0x4000
#define MDONE	0x1001
#define FREE_	100

extern int cerror();
extern int match();
extern void order(), rfree(), rfreeall(), genargs(), fwalk();
extern int ltypeok();
extern struct node *stotree;
extern int stocook;
extern int argstop;		/* order3.c, 0xa156 */
extern long autooff;		/* 0x8fcc: the size of the automatic variables, ~L1 */
extern long delayoff;		/* 0x69d4 */
extern int dope[];
extern struct node *canon();

/* generate a call: the function address (an expression) is made addressable, the arguments are placed, then the call itself */
int
gencall(p, cookie)
	register struct node *p;
	int cookie;
{
	register struct node *f;
	register int fop;
	int indirect;

	f = p->left;
	fop = f->op;
	indirect = 0;
	argstop = 0;
	if (fop != ICON) {
		if (p->right) {
			indirect = 1;
			order(f, 0x1000);
		} else if (fop != REG || f->rval == 0) {
			order(f, 6);
			if (f->rval == 0)
				order(f, 6);
			rfree(f->rval, 0x14);
		}
	}
	if (p->right)
		genargs(p->right);
	if (indirect) {
		if (autooff + f->lval != 0)
			printf("\tld\tr1,~L1+%ld(fp)\n", f->lval);
		else
			printf("\tld\tr1,@fp\n");
		f->rval = 1;
		f->lval = 0;
		f->sname = 0;
	}
	if (f->op == REG) {
		f->op = OREG;
		f->stsize = 0;
	}
	p->op = UCALL;
	rfreeall(1);
	delayoff = 0;
	return match(p, 0x16) != MDONE;
}

int
genscall(p, cookie)
	struct node *p;
	int cookie;
{
	return gencall(p, cookie);
}

/* the next cookie to try when no table entry matched */
int
nextcook(p, cookie)
	struct node *p;
	int cookie;
{
	if (cookie == FORREW)
		return 0;
	if (!(cookie & 0x14))
		return 0x14;
	if (!(cookie & 0x1000) && (dope[p->op] & 1))
		return 0x101E;
	return FORREW;
}

/* the last chance: a name or an OREG may go to a register by the rewrite tables */
int
lastchance(p, cookie)
	struct node *p;
	int cookie;
{
	if (cookie == 0x2000 && (p->op == OREG || p->op == NAME))
		return 1;
	return 0;
}

/* a constant operand of an AND goes to the right */
void
icright(p)
	register struct node *p;
{
	register struct node *t;

	if (p->op == AND && p->left->op == ICON) {
		t = p->left;
		p->left = p->right;
		p->right = t;
	}
}

/* canon, then the constants of the ANDs to the right */
void
canon2(p)
	struct node *p;
{
	fwalk(canon(p), icright);
}

static int rev[] = { EQ, NE, GE, GT, LE, LT, UGE, UGT, ULE, ULT };

/* a comparison with a register on the right goes with the operands exchanged */
void
mycanon(p)
	register struct node *p;
{
	register struct node *t;

	if (p->op < EQ || p->op > UGT)
		return;
	if (p->right->op != REG)
		return;
	if (p->left->op == REG)
		return;
	if (p->left->op == ICON && p->left->lval == 0)
		return;
	t = p->left;
	p->left = p->right;
	p->right = t;
	p->op = rev[p->op - EQ];
}

/* an assignment under an indirection: nothing to store, the check only */
int
stoasg(p, o)
	struct node *p;
	int o;
{
	return ltypeok(p->left->op, p->left);
}

/* store the subtree of p with the larger register need: stotree and stocook for codgen */
void
mkadrs(p)
	register struct node *p;
{
	register int o;
	register struct node *l, *r;
	struct node *t;

	o = p->op;
	l = p->left;
	r = p->right;
	if (dope[o] & 1) {
		if (l->su > r->su || (l->su == r->su && l->fsu >= r->fsu)) {
			if (l->op == STAR) {
				if (l->su > 0 || l->fsu > 0) {
					t = l->left;
					goto set;
				}
				if (r->su > 0 || r->fsu > 0) {
					t = r;
					goto set;
				}
				cerror("store finds both sides trivial");
				return;
			}
			if (l->op == FLD && l->left->op == STAR) {
				t = l->left->left;
				goto set;
			}
			t = l;
		} else
			t = r;
	} else {
		if (l->su > r->su || (l->su == r->su && l->fsu > r->fsu))
			t = l;
		else
			t = r;
	}
set:
	stotree = t;
	stocook = 0x1000;
}
