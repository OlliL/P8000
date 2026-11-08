/*
 * tr3.c - trees: rewriting names and members
 */
#include "cg.h"
#include "cg2.h"

extern NODE node[];
static char sccsid[] __attribute__((unused)) = "@[$]tr3.c\t2.3  08/24/83 13:57:35 Copr. 1981 87wega3.2.";

#define SCONV	110

#define INCREF(t)	((((t) & ~0xF) << 2) | 0x10 | ((t) & 0xF))

#define DECREF(t)	((((unsigned)(t) >> 2) & ~0xF) | ((t) & 0xF))

/* a new node */
NODE *
makeblock(int op, NODE *l, NODE *r, int type)
{
	register NODE *n;

	n = talloc();
	n->in.op = op;
	n->in.left = l;
	n->in.right = r;
	n->in.type = type;
	n->in.su = 0;
	n->in.fsu = 0;
	n->in.rall = 0;
	return n;
}

/* a constant added to or subtracted from an address that has one already: folded */
NODE *
foldadd(NODE *p)
{
	register NODE *l, *r, *q;
	register int lo;
	int x;

	l = p->in.left;
	r = p->in.right;
	lo = l->in.op;
	q = lo == PLUS || lo == MINUS ? l->in.left : l;
	if (q->in.op == ICON && r->in.op == ICON) {
		x = (int)q->tn.lval;
		switch (p->in.op) {
		case PLUS:
			x += (int)r->tn.lval;
			break;
		case MINUS:
			x -= (int)r->tn.lval;
			break;
		default:
			cerror("bad op in convert");
		}
		q->tn.lval = (long)x;
		p->in.op = FREE;
		r->in.op = FREE;
		l->in.type = p->in.type;
		return l;
	}
	if (lo == PLUS || lo == MINUS) {
		if (lo == MINUS)
			p->in.op = p->in.op == MINUS ? PLUS : MINUS;
		l->in.right = buildstar(p->in.op, l->in.right, r);
		p->in.op = FREE;
		p = l;
	}
	return p;
}

/* a unary tree of the operator op over l (and r): * and & cancel each other, the type is the one below or above */
NODE *
buildstar(int op, NODE *l, NODE *r)
{
	register NODE *n;

	n = makeblock(op, l, r, 4);
	switch (n->in.op) {
	case STAR:
		if (l->in.op == UAND) {
			l->in.op = FREE;
			n->in.op = FREE;
			n = l->in.left;
		}
		if ((l->in.type & 0x30) != 0x10)
			uerror("illegal indirection");
		n->in.type = DECREF(l->in.type);
		n->in.su = l->in.su;
		n->in.fsu = l->in.fsu;
		n->in.rall = l->in.rall;
		break;
	case UAND:
		if (l->in.op == STAR) {
			l->in.op = FREE;
			n->in.op = FREE;
			n = l->in.left;
			n->in.type = INCREF(l->in.type);
			n->in.su = l->in.su;
			n->in.fsu = l->in.fsu;
			n->in.rall = l->in.rall;
		}
		break;
	}
	n = clocal(n);
	return n;
}

/* an integer constant of the given number of bits, as bytes */
NODE *
bcon(long bits)
{
	register NODE *n;

	n = makeblock(ICON, 0, 0, 4);
	n->in.sname = 0;
	n->tn.lval = bits / 8;
	return n;
}

/* the address of a structure member: the base pointer plus the offset of the member, then the indirection */
NODE *
member(NODE *p)
{
	register int nt;
	register struct sym *m;
	long o;
	NODE *off, *n;

	m = p->in.right->in.sname;
	p->in.right->in.op = FREE;
	p->in.op = FREE;
	p = p->in.left;
	nt = p->in.type = INCREF(m->stype);
	o = m->soffset;
	off = bcon(o);
	n = makeblock(PLUS, p, off, nt);
	p = clocal(n);
	if ((p->in.type & 0x30) != 0x10)
		p->in.type = p->in.type + 16;
	p = buildstar(STAR, p, 0);
	return p;
}
