/*
 * regfile3.c - what the register status lists know about the registers after a table entry's template
 * ran (codgen / scodgen)
 *
 * Reconstructed from /lib/codgen: rnassign 0x9722, rnnullp 0x97cc, rnnullcall 0x9816, rnnullall 0x983c,
 * rnnulll 0x986a, rnnullw 0x988c, rnnull 0x989e, nxwork 0x98e4 (the messages call it "regfile").
 * nxwork is called with the field nx of the table entry (0 .. 0x22); the cases 33 and 34 save the
 * work registers r4/r5 (rr4) that a call destroys into other registers, which is code that is there twice
 * in the original, so it is a macro here.
 */
#include "cg.h"

extern int cerror();
extern struct node *getlr();
extern int rclass();
extern struct rs *rfind();
extern int usable();
extern int getreg();
extern void rfree();
extern void rmove();
extern char *opst[];
extern int rmode, rlmode;

int nsave;		/* 0xa14a: registers saved by the cases 33 and 34 */
int saveorig[3];	/* 0xa14e: the register that was saved */
int savecls[3];		/* 0xa15c: its class */
int savereg[3];		/* 0xa160: where it went */

static void rnnull();

/* p1 now holds what p2 holds: copy the symbol and offset in the register file */
static void
rnassign(p1, p2)
	register struct node *p1, *p2;
{
	register struct rs *a, *b;
	long l;

	if (p1->op != REG)
		return;
	a = rfind(p1->rval, rclass(p1->type, p1->rval));
	if (a == 0)
		return;
	switch (a->cls) {
	case 5:
	case 6:
	case 7:
	case 8:
		return;
	}
	if (a->kind == 6)
		return;
	if (p2->op == REG) {
		b = rfind(p2->rval, rclass(p2->type, p2->rval));
		if (b) {
			a->sym = b->sym;
			l = b->off;
		} else {
			a->sym = 0;
			l = 0;
		}
	} else {
		a->sym = p2->sname;
		l = p2->lval;
	}
	a->off = l;
	a->flags &= ~0x8000;
	a->busy = 0;
}

/* the register of the REG node p no longer holds anything known */
static void
rnnullp(p)
	register struct node *p;
{
	register struct rs *n;

	n = rfind(p->rval, rclass(p->type, p->rval));
	if (n) {
		n->sym = 0;
		n->busy = 0;
		n->off = 0;
		n->flags &= ~0x8000;
		n->kind = 8;
	}
}

/* a call destroys r4 .. r7, rr4, rr6 */
static void
rnnullcall()
{
	rnnull(4, 4);
	rnnull(5, 4);
	rnnull(6, 4);
	rnnull(7, 4);
	rnnull(4, 5);
	rnnull(6, 5);
}

static void
rnnullall()
{
	int i;

	for (i = 0; i <= 7; i++)
		rnnull(i, 4);
	for (i = 0; i <= 6; i += 2)
		;
	rnnull(i, 5);
}

static void
rnnulll(r)
	register int r;
{
	rnnull(r, 5);
	rnnull(r, 4);
	rnnull(r + 1, 4);
}

static void
rnnullw(r)
	int r;
{
	rnnull(r, 4);
}

static void
rnnull(reg, type)
	int reg, type;
{
	struct rs *n;

	n = rfind(reg, rclass(type, reg));
	if (n) {
		n->sym = 0;
		n->busy = 0;
		n->off = 0;
		n->flags &= ~0x8000;
		n->kind = 8;
	}
}

#define SUBST(from) \
	if ((p->left->op == REG || p->left->op == OREG) && p->left->rval == from) \
		p->left->rval = r; \
	if ((p->right->op == REG || p->right->op == OREG) && p->right->rval == from) \
		p->right->rval = r

/* r4 and r5 hold values that are still needed: move them to other registers */
#define SAVEREGS \
	n = rfind(4, 3); \
	if (n && n->busy > 0) { \
		r = getreg(3, -1, 0x1000, p->sname); \
		if (r < 0) \
			cerror("regfile: case A4UW can't free rr4 on %s\n", opst[p->op]); \
		rmode++; \
		savereg[nsave] = r; \
		saveorig[nsave] = 4; \
		savecls[nsave] = 3; \
		nsave++; \
		rfind(r, 3)->busy = 1; \
		rmove(r, 4, 5); \
		SUBST(4); \
	} else { \
		n = rfind(4, 2); \
		if (n && n->busy > 0) { \
			r = getreg(2, -1, 0x1000, p->sname); \
			if (r < 0) \
				cerror("regfile: case A4UW can't free r4 on %s\n", opst[p->op]); \
			savereg[nsave] = r; \
			saveorig[nsave] = 4; \
			savecls[nsave] = 2; \
			nsave++; \
			rfind(r, 2)->busy = 1; \
			rmove(r, 4, 4); \
			SUBST(4); \
		} \
		n = rfind(5, 2); \
		if (n && n->busy > 0) { \
			r = getreg(2, -1, 0x1000, p->sname); \
			if (r < 0) \
				cerror("regfile: case A4UW can't free r5 on %s\n", opst[p->op]); \
			if (r == 4) { \
				r = getreg(2, -1, 0x1000, p->sname); \
				if (r < 0) \
					cerror("regfile: case A4UW can't free r5 on %s\n", opst[p->op]); \
				rfree(4, 4); \
			} \
			savereg[nsave] = r; \
			saveorig[nsave] = 5; \
			savecls[nsave] = 2; \
			nsave++; \
			rfind(r, 2)->busy = 1; \
			rmove(r, 5, 4); \
			SUBST(5); \
		} \
	} \
	rmode = m; \
	rlmode = l; \
	rnnullcall()

void
nxwork(p, nx)
	register struct node *p;
	register int nx;
{
	register struct node *a, *b;
	register struct rs *n;
	register int r;
	int m, l;

	switch (nx) {
	case 8:
	case 9:
		a = getlr(p, 'R');
		b = getlr(p, 'L');
		rnassign(b, a);
		break;
	case 10:
		a = getlr(p, '1');
		b = getlr(p, 'R');
		rnassign(b, a);
		break;
	case 14:
		a = getlr(p, 'R');
		b = getlr(p, '1');
		rnassign(b, a);
		break;
	case 15:
		a = getlr(p, 'L');
		b = getlr(p, '1');
		rnassign(b, a);
		break;
	case 18:
		rnnullw(getlr(p, '1')->rval);
		break;
	case 19:
		rnnulll(getlr(p, '1')->rval);
		break;
	case 20:
		rnnullp(getlr(p, '1'));
		break;
	case 21:
		rnnullw(getlr(p, '1')->rval);
		rnnullw(getlr(p, '2')->rval);
		break;
	case 22:
		rnnulll(getlr(p, '1')->rval);
		rnnulll(getlr(p, '2')->rval);
		break;
	case 24:
		rnnullw(getlr(p, 'L')->rval);
		break;
	case 25:
		rnnulll(getlr(p, 'L')->rval);
		break;
	case 26:
		rnnullp(getlr(p, 'L'));
		break;
	case 27:
		rnnullp(getlr(p, 'R'));
		break;
	case 28:
		rnnullp(getlr(p, 'L'));
		rnnullp(getlr(p, 'R'));
		break;
	case 29:
		rnnullw(getlr(p, 'R')->rval);
		rnnullw(getlr(p, 'L')->rval);
		break;
	case 30:
		rnnullw(getlr(p, '1')->rval);
		rnnullw(getlr(p, 'L')->rval);
		break;
	case 31:
		rnnullp(getlr(p, '1'));
		rnnullp(getlr(p, 'L'));
		break;
	case 32:
		rnnullall();
		break;
	case 33:
		nsave = 0;
		if (usable(p, 'R', 4)) {
			rnnullcall();
			break;
		}
		m = rmode;
		l = rlmode;
		rmode = 0;
		rlmode = 0;
		SAVEREGS;
		break;
	case 34:
		m = rmode;
		l = rlmode;
		rmode = 0;
		rlmode = 0;
		nsave = 0;
		SAVEREGS;
		break;
	default:
		if (nx > 0x22)
			cerror("regfile: illegal register update case\n");
	}
}
