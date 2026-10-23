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
extern void order(), rfree(), rsfreeall(), genargs(), fwalk();
extern int ltypeok();
extern struct node *stotree;
extern int stocook;
extern int argstop;		/* order3.c, 0xa156 */
extern int argreg;		/* order3.c: the next argument register */
extern long freetemp();
extern long maxoff;		/* 0x8fcc: the size of the automatic variables, ~L1 */
extern long tmpoff;		/* 0x69d4 */
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
	register int indirect;
#ifdef SEG
	int r;
	long save;
#endif

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
#ifdef SEG
	if (indirect) {
		if (argstop == 0 && argreg >= 3) {
			/* the address is still in reach of a free register pair */
			f->rall = 0x1002;
			order(f, 6);
			rfree(f->rval, 0x14);
			indirect = 0;
		} else {
			/* rr8 holds the address during the call; its old contents wait in a temporary */
			save = freetemp(2) / 8;
			printf("\tldl\t_stkseg+~L1+%ld(fp),rr8\n", save);
			printf("\tldl\trr8,_stkseg+~L1+%ld(fp)\n", f->lval);
			f->rval = 8;
			f->lval = 0;
			f->sname = 0;
		}
	}
#else
	if (indirect) {
		if (maxoff + f->lval != 0)
			printf("\tld\tr1,~L1+%ld(fp)\n", f->lval);
		else
			printf("\tld\tr1,@fp\n");
		f->rval = 1;
		f->lval = 0;
		f->sname = 0;
	}
#endif
	if (f->op == REG) {
		f->op = OREG;
		f->stsize = 0;
	}
	p->op = UCALL;
	rsfreeall(1);
	tmpoff = 0;
#ifdef SEG
	r = match(p, 0x16);
	if (indirect)
		printf("\tldl\trr8,_stkseg+~L1+%ld(fp)\n", save);
	return r != MDONE;
#else
	return match(p, 0x16) != MDONE;
#endif
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

	o = p->op;
	if (dope[o] & 1) {
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
