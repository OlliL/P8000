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

extern int s4d94, a0ec;
extern int fn4_faa4();
extern struct quad *insertquad(), *qmove();

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

struct dl { int x0; struct quad *q; long l6; long l10; struct qa e14; int x20; unsigned f15:1, f14:1, f13:1, f12:1, f11:1, f10:1, rest:10; struct dl *next; };
extern void denorm(), normop();

/* the quad opqd gets a new stemp as its result operand; the stemp is entered into the definition (name from the debug output) */
void
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
