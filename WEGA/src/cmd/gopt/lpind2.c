/*
 * lpind2.c - gopt: the induction variables of the loops (segment 5). Written from /lib/gopt (no source known); names guessed,
 * the callees by the address (fnXXXX).
 */
#include "gopt.h"

static char sccsid[] = "@[$]lpind2.c\t1.5  02/07/83 15:54:28 Copr. 1981 87wega3.2.";

extern int s4d86;
extern long lconst[];
extern int ispow2();
extern struct quad *a0d6;
extern int a0e6;
extern struct quad *movequad();
extern void prquad();

/* 2 to the power n (name guessed) */
long
lpow2(n)
int n;
{
	long r;

	r = 1;
	while (n > 0) {
		r = r << 1;
		n--;
	}
	return r;
}

/* the value of the operation op of the constants a and b (name from the debug output) */
long
ceval(a, op, b)
struct qa a;
int op;
struct qa b;
{
	long x, y, l, m, r;

	x = b.b2 == 1 ? (long)b.w4 : lconst[b.w4];
	y = a.b2 == 1 ? (long)a.w4 : lconst[a.w4];
	switch (op) {
	case 1:
		r = x;
		break;
	case 48:
		r = -x;
		break;
	case 34:
		r = x * y;
		break;
	case 44:
		r = x * lpow2((int)y);
		break;
	case 14:
	case 45:
		if ((l = ispow2(x)) > 0 && (m = ispow2(y)) > 0 && l > m)
			r = l - m;
		r = 0;
		break;
	}
	if (s4d86 > 3)
		printf("%ld = ceval()\n", r);
	return r;
}

/* the quad q is moved behind a0d6 (name guessed) */
struct quad *
qmove(q)
struct quad *q;
{
	struct qa u1, u2, u3;
	struct quad *u4;

	if (s4d86 > 5)
		prquad(q);
	q = movequad(q, a0d6);
	a0d6 = a0d6->next;
	a0e6++;
	return q;
}

extern int s4d94, a0ec, a0ee;
extern void propnd();
extern int fn4_faa4();
extern struct quad *insertquad(), *qmove(), *moveindquad();
extern int a0da;
extern char *taballoc();
extern int copyopndstmp();
extern long ceval();

/* a move of the result of q to a new stemp is inserted and the quads are moved (name from the debug output) */
struct quad *
insrtstmpmove(q)
struct quad *q;
{
	struct quad *nq;
	struct qa a, b, c;

	if (s4d86 > 2) {
		printf("insrtstmpmove(), q:\n");
		prquad(q);
	}
	a = q->c;
	b.b2 = 6;
	b.w4 = s4d94;
	fn4_faa4(s4d94);
	s4d94++;
	a0ec++;
	b.w0 = a.w0;
	c.b2 = 0;
	nq = insertquad(60, &a, &c, &b, q);
	nq = insertquad(60, &b, &c, &a, nq);
	nq->f3 = 0;
	nq->f7 = 0;
	nq->f4 = 0;
	if (s4d86 > 3) {
		printf("insrtstmpmove(), before move, invq, invq->link\n");
		prquad(q);
		prquad(q->next);
	}
	q = qmove(q);
	if (s4d86 > 3) {
		printf("insrtstmpmove(), after first move invq, invq->link\n");
		prquad(q);
		prquad(q->next);
	}
	q = qmove(q->next);
	if (s4d86 > 3) {
		printf("insrtstmpmove(), after second move, invq, invq->link\n");
		prquad(q);
		prquad(q->next);
	}
	return q;
}

struct nd { struct qa c0; unsigned f15:1, f14:15; long v; struct qa c12; struct nd *link; };	/* a node of the induction variable lists: 22 bytes (guessed) */
struct dl { int x0; struct quad *q; struct nd *l6; struct dl *l10; struct qa e14; int x20; unsigned f15:1, f14:1, f13:1, f12:1, f11:1, f10:1, rest:10; struct dl *next; };
extern void denorm(), normop();

/* the quad opqd gets a new stemp as its result operand; the stemp is entered into the definition (name from the debug output) */
struct quad *
moveindquad(x, opqd, out, d, d2, kind)
struct quad *x, *opqd;
struct qa *out;
struct dl *d, *d2;
int kind;
{
	if (s4d86 > 2) {
		printf("moveindquad(), opqd:\n");
		prquad(opqd);
	}
	if (opqd->op == 34 && opqd->next->op == 13) {
		opqd = qmove(opqd);
		opqd = opqd->next;
	}
	denorm(&x->a);
	x->a.b2 = 6;
	denorm(&opqd->c);
	opqd->c.b2 = 6;
	x->a.w4 = s4d94;
	normop(&x->a);
	d2->x20 = s4d94;
	fn4_faa4(s4d94);
	opqd->c.w4 = s4d94;
	s4d94++;
	normop(&opqd->c);
	a0ec++;
	*out = opqd->c;
	if (d->f13) {
		if (kind == 2) {
			denorm(&opqd->a);
			opqd->a.b2 = 6;
			opqd->a.w4 = d->x20;
			normop(&opqd->a);
		} else if (kind == 3) {
			denorm(&opqd->b);
			opqd->b.b2 = 6;
			opqd->b.w4 = d->x20;
			normop(&opqd->b);
		}
	}
	opqd = qmove(opqd);
}

extern int constlook(), s000e, insrtopstmp();

/* the operand is set to the integer v (w0 as its first word); t == 1: v is used as it is, else it is negated (name guessed) */
void
mkconstopnd(p, v, t, w0)
struct qa *p;
long v;
int t, w0;
{
	p->w0 = w0;
	if (t == 1) {
		if (v > 32767) {
			p->b2 = 7;
			p->w4 = constlook(v);
		} else {
			p->b2 = 1;
			p->w4 = v;
		}
	} else if (v < -32767) {
		p->b2 = 7;
		p->w4 = constlook(-v);
	} else {
		p->b2 = 1;
		p->w4 = -v;
	}
}

/* a new stemp is made for the operation of the definitions p and t (name from the debug output) */
int
insrtopstmp(opqd, p, t, kind, outq)
struct quad *opqd;
struct dl *p, *t;
int kind;
struct quad *outq;
{
	static struct qa zero;
	struct qa a, b, c;
	int op;

	if (s4d86 > 2) {
		printf("insrtopstmp(), opqd:, t->dfquad, p->dfquad\n");
		prquad(opqd);
		prquad(t->q);
		prquad(p->q);
	}
	switch (opqd->op) {
	case 1:
		op = 47;
		a = p->e14;
		b = zero;
		break;
	case 48:
		op = 48;
		mkconstopnd(&a, 0L, 1, p->e14.w0);
		b = p->e14;
		break;
	case 34:
	case 44:
		a = p->e14;
		op = opqd->op;
		if (kind == 2)
			b = opqd->b;
		else
			b = opqd->a;
		break;
	default:
		if (s4d86 > 3)
			printf("insrtopstmp(), returning FALSE\n");
		return 0;
	}
	if (op == 34) {
		c.w0 = 5;
		c.b2 = 3;
		c.w4 = s000e++;
		a0ee++;
		a0d6 = insertquad(op, &a, &b, &c, a0d6);
		b.b2 = 0;
		a.b2 = 6;
		fn4_faa4(s4d94);
		a.w4 = s4d94;
		s4d94++;
		a0d6 = insertquad(13, &c, &b, &a, a0d6);
		c = a;
	} else {
		c.b2 = 6;
		fn4_faa4(s4d94);
		c.w4 = s4d94;
		s4d94++;
		c.w0 = a.w0;
		a0d6 = insertquad(op, &a, &b, &c, a0d6);
	}
	a0ec++;
	t->e14 = c;
	outq->a = c;
	if (s4d86 > 3) {
		printf("insrtopnd(), ip->opndstmp=");
		propnd(&c);
		printf("\n");
	}
	return 1;
}

/* the stemp of the definition p is used for t (name from the debug output) */
int
copyopndstmp(opqd, p, t, kind, outq)
struct quad *opqd;
struct dl *p, *t;
int kind;
struct quad *outq;
{
	if (s4d86 > 2) {
		printf("copyopndstmp(), opqd:\n");
		prquad(opqd);
	}
	if (!p->f11 &&
	    ((kind == 2 && (opqd->b.b2 == 1 || opqd->b.b2 == 7) && (opqd->op == 1 || opqd->op == 48)) ||
	     (kind == 3 && (opqd->a.b2 == 1 || opqd->a.b2 == 7) && (opqd->op == 1 || opqd->op == 48)))) {
		t->e14 = p->e14;
		if (s4d86 > 3) {
			printf("copyopndstmp(), copied indopnd: ");
			propnd(&p->e14);
			printf("\n");
		}
		return 1;
	}
	return insrtopstmp(opqd, p, t, kind, outq);
}

extern struct nd *a104;
extern struct dl *a0f8, *a0e0;
extern void freelist();

/* the stemp of the definition p is used for the operand of opqd of t; a node of the induction variable lists is filled (name from the debug output) */
struct quad *
fillindvar(qd, opqd, kind, p, t)
struct quad *qd, *opqd;
int kind;
struct dl *p, *t;
{
	struct nd *n, *tmp;
	int ok;

	if (s4d86 > 2) {
		printf("fillindvar(), OP=%s, opqd:\n", kind == 2 ? "OP1" : "OP2");
		prquad(opqd);
	}
	if (a104 != 0) {
		n = a104;
		a104 = a104->link;
	} else
		n = (struct nd *)taballoc(22, 7);
	t->f11 = 0;
	n->f15 = 0;
	n->v = 0;
	ok = 0;
	if (kind == 3) {
		if (p->f11) {
			if (opqd->a.b2 == 1 || opqd->a.b2 == 7 || opqd->op == 1 || opqd->op == 48) {
				n->f15 = 1;
				n->v = ceval(opqd->a, opqd->op, p->e14);
				t->f11 = 1;
				if (n->v != 0) {
					if (n->v > 0) {
LA:						mkconstopnd(&t->e14, n->v, 1, p->e14.w0);
						ok = 1;
					} else {
LB:						mkconstopnd(&t->e14, n->v, 0, p->e14.w0);
						ok = 1;
					}
				}
			} else
				ok = copyopndstmp(opqd, p, t, kind, n);
		} else
			ok = copyopndstmp(opqd, p, t, kind, n);
	} else if (kind == 2) {
		if (p->f11) {
			if (opqd->b.b2 == 1 || opqd->b.b2 == 7 || opqd->op == 1 || opqd->op == 48) {
				n->f15 = 1;
				n->v = ceval(opqd->b, opqd->op, p->e14);
				if (n->v != 0) {
					t->f11 = 1;
					if (n->v > 0)
						goto LA;
					else
						goto LB;
				}
			} else
				ok = copyopndstmp(opqd, p, t, kind, n);
		} else
			ok = copyopndstmp(opqd, p, t, kind, n);
	}
done:
	if (ok) {
		a0da = 1;
		if (p->f14) {
			tmp = p->l6;
			p->l6 = n;
			n->link = tmp;
			t->f13 = 1;
			t->f12 = 0;
			t->f14 = 0;
			t->l10 = p;
		} else if (p->f13) {
			tmp = p->l10->l6;
			p->l10->l6 = n;
			n->link = tmp;
			t->f12 = 1;
			t->f13 = 0;
			t->f14 = 0;
			t->l10 = p->l10;
			t->f10 = p->f10;
		}
		opqd = moveindquad(qd, opqd, (struct qa *)n, p, t, kind);
		return opqd;
	}
	freelist(0, n);
	return 0;
}

extern int a0fe;

/* the induction variable ind is entered in the program by an operation behind opqd (name from the debug output) */
void
indupdate(ind, opqd, opcode)
struct nd *ind;
struct quad *opqd;
int opcode;
{
	struct qa c;

	if (s4d86 > 2) {
		printf("indupdate(), opcode=%d, ind->cnst=%d, ind->incsize=%d, q:\n", opcode, ind->f15, ind->v);
		prquad(opqd);
	}
	if (ind->f15) {
		if (ind->v > 0)
			mkconstopnd(&c, ind->v, 1, ind->c0.w0);
		else {
			if (opcode == 1)
				opcode = 48;
			else
				opcode = 1;
			mkconstopnd(&c, ind->v, 0, ind->c0.w0);
		}
		opqd = insertquad(opcode, &ind->c0, &c, &ind->c0, opqd);
	} else {
		opqd = insertquad(opcode, &ind->c0, &ind->c12, &ind->c0, opqd);
		if (s4d86 > 3) {
			printf("indupdate(), else clause, ind->stmp, ind->opndstmp\n");
			propnd(&ind->c0);
			printf("\n");
			propnd(&ind->c12);
			printf("\n");
		}
	}
	opqd->f3 = 0;
	opqd->f7 = 0;
	opqd->f4 = 0;
	a0fe++;
}

/* the node n is put at the end of the list of the free definitions (flag) or of the free induction nodes (name guessed) */
void
freelist(flag, n)
int flag;
struct nd *n;
{
	struct dl *x;
	struct nd *y;

	if (flag && a0e0 != 0) {
		x = a0f8;
		if (x == 0)
			a0f8 = a0e0;
		else {
			for (; x->next != 0; x = x->next)
				;
			x->next = a0e0;
		}
	} else if (!flag) {
		y = a104;
		if (y == 0)
			a104 = n;
		else {
			for (; y->link != 0; y = y->link)
				;
			y->link = n;
		}
	}
}

extern int a0f0;
extern struct dl *alloclpdef();

/* the result of q is moved to a new temporary: a move quad is inserted behind q and made a loop definition (name guessed) */
struct quad *
insrtlpdef(q, blk)
struct quad *q;
int blk;
{
	struct quad *nq;
	struct qa a, b, c;

	if (q->op == 34 && q->next->op == 13)
		q = q->next;
	a = q->c;
	denorm(&q->c);
	b.b2 = 3;
	b.w4 = s000e++;
	a0ee++;
	b.w0 = a.w0;
	q->c = b;
	normop(&q->c);
	c.b2 = 0;
	nq = insertquad(60, &b, &c, &a, q);
	a0f0 = 1;
	alloclpdef(nq, blk);
	nq->f3 = 0;
	nq->f7 = 0;
	nq->f4 = 0;
	return nq;
}

/* the definitions of the loops with a temporary as the result are moved to the list of the free ones (name from the debug output) */
void
deltmplpdefs()
{
	struct dl *t, *prev, *u;

	if (s4d86 > 2) {
		printf("deltmplpdefs(), on entry, lpdefs:\n");
		for (u = a0e0; u != 0; u = u->next)
			prquad(u->q);
		printf("flpdefs:\n");
		for (u = a0f8; u != 0; u = u->next)
			prquad(u->q);
	}
	a0f0 = 0;
	prev = t = a0e0;
	while (t != 0) {
		if (t->q->c.b2 == 3) {
			if (t == a0e0)
				prev = a0e0 = a0e0->next;
			else
				prev->next = t->next;
			if (a0f8 != 0) {
				u = a0f8;
				a0f8 = t;
				t->next = u;
			} else {
				a0f8 = t;
				t->next = 0;
			}
			t = prev;
		} else {
			prev = t;
			t = t->next;
		}
	}
	if (s4d86 > 3) {
		printf("deltmplpdefs(), on exit, lpdefs:\n");
		for (u = a0e0; u != 0; u = u->next)
			prquad(u->q);
		printf("flpdefs:\n");
		for (u = a0f8; u != 0; u = u->next)
			prquad(u->q);
	}
}
