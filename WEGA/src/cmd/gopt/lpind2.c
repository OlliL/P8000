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
