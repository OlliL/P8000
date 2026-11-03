/*
 * tr3.c - trees: rewriting names and members
 */
#include "cg.h"
#include "cg2.h"

extern NODE *bcon();
extern NODE *buildstar();
extern int cerror();
extern NODE *clocal();
extern NODE *foldadd();
extern NODE *makeblock();
extern NODE *member();
extern NODE node[];
extern NODE *talloc();
extern void uerror();
static char sccsid[] = "@[$]tr3.c\t2.3  08/24/83 13:57:35 Copr. 1981 87wega3.2.";

#define SCONV	110

#define INCREF(t)	((((t) & ~0xF) << 2) | 0x10 | ((t) & 0xF))

#define DECREF(t)	((((unsigned)(t) >> 2) & ~0xF) | ((t) & 0xF))

/* a new node */
NODE *
makeblock(op, l, r, type)
	int op;
	register NODE *l, *r;
	int type;
{
	register NODE *n;

	n = talloc();
	n->op = op;
	n->left = l;
	n->right = r;
	n->type = type;
	n->su = 0;
	n->fsu = 0;
	n->rall = 0;
	return n;
}

/* a constant added to or subtracted from an address that has one already: folded */
NODE *
foldadd(p)
	register NODE *p;
{
	register NODE *l, *r, *q;
	register int lo;
	int x;

	l = p->left;
	r = p->right;
	lo = l->op;
	q = lo == PLUS || lo == MINUS ? l->left : l;
	if (q->op == ICON && r->op == ICON) {
		x = (int)q->lval;
		switch (p->op) {
		case PLUS:
			x += (int)r->lval;
			break;
		case MINUS:
			x -= (int)r->lval;
			break;
		default:
			cerror("bad op in convert");
		}
		q->lval = (long)x;
		p->op = FREE;
		r->op = FREE;
		l->type = p->type;
		return l;
	}
	if (lo == PLUS || lo == MINUS) {
		if (lo == MINUS)
			p->op = p->op == MINUS ? PLUS : MINUS;
		l->right = buildstar(p->op, l->right, r);
		p->op = FREE;
		p = l;
	}
	return p;
}

/* a unary tree of the operator op over l (and r): * and & cancel each other, the type is the one below or above */
NODE *
buildstar(op, l, r)
	int op;
	register NODE *l, *r;
{
	register NODE *n;
	register int spare1, spare2, spare3, spare4;	/* unused: the original reserves these registers */

	n = makeblock(op, l, r, 4);
	switch (n->op) {
	case STAR:
		if (l->op == UAND) {
			l->op = FREE;
			n->op = FREE;
			n = l->left;
		}
		if ((l->type & 0x30) != 0x10)
			uerror("illegal indirection");
		n->type = DECREF(l->type);
		n->su = l->su;
		n->fsu = l->fsu;
		n->rall = l->rall;
		break;
	case UAND:
		if (l->op == STAR) {
			l->op = FREE;
			n->op = FREE;
			n = l->left;
			n->type = INCREF(l->type);
			n->su = l->su;
			n->fsu = l->fsu;
			n->rall = l->rall;
		}
		break;
	}
	n = clocal(n);
	return n;
}

/* an integer constant of the given number of bits, as bytes */
NODE *
bcon(bits)
	long bits;
{
	register NODE *n;

	n = makeblock(ICON, 0, 0, 4);
	n->sname = 0;
	n->lval = bits / 8;
	return n;
}

/* the address of a structure member: the base pointer plus the offset of the member, then the indirection */
NODE *
member(p)
	register NODE *p;
{
	register int nt;
	register int spare;		/* unused: the original keeps a register here */
	register int sz, cl;		/* set from the symbol and not used afterwards */
	register struct sym *m;
	long o;
	NODE *off, *n;

	m = p->right->sname;
	cl = m->sclass;
	p->right->op = FREE;
	p->op = FREE;
	p = p->left;
	nt = p->type = INCREF(m->stype);
	sz = (int)m->ssize;
	o = m->soffset;
	off = bcon(o);
	n = makeblock(PLUS, p, off, nt);
	p = clocal(n);
	if ((p->type & 0x30) != 0x10)
		p->type = p->type + 16;
	p = buildstar(STAR, p, 0);
	return p;
}
