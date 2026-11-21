/*
 * beta2.c - gopt: the beta table (segment 4): every temporary and every alias class gets an entry; the operands of the
 * quads are renumbered to it. Written from /lib/gopt (no source known); names guessed.
 */
static char sccsid[] __attribute__((unused)) = "@[$]beta2.c\t1.5  01/07/83 13:38:14 Copr. 1981 87wega3.2.";
#include "gopt.h"

extern struct bt *betatbl;
extern struct quad *qhead;
extern struct sym *symtab;
extern char ascii[];
extern char ctpos[];
extern int s4d80, s4d94, a0d4, a108;

int betainit(void)
{
	struct quad *q;
	struct bt *t;
	int x, y, flag;
	int i;

	betatbl = (struct bt *)taballoc(0, 5);
	s4d94++;
	for (i = 0; i < s4d94; i++)
		betanew(i);
	a0d4 = s4d94;
	for (q = qhead; q != 0; q = q->next) {
		if (q->c.b2 == 3 && ctpos[q->c.w4] == 1) {
			q = delquad(q);
			continue;
		}
		if (q->a.b2 == 3)
			betaset(&q->a);
		if (q->b.b2 == 3)
			betaset(&q->b);
		if (q->c.b2 == 3)
			betaset(&q->c);
		flag = 0;
		switch (q->op) {
		case 3:
			flag = 1;
		/* fall through */
		case 25:
			if (q->a.b2 == 8 && bbit(q->a.w4, 11)) {
				if (q->b.b2 == 1 || q->b.b2 == 7)
					flag = 1;
			}
		/* fall through */
		case 24:
		case 30:
			if (q->a.b2 == 4) {
				x = almap(&q->a, q->op == 25 ? 3 : q->op);
				y = q->a.w4;
			} else if (q->a.b2 == 8) {
				x = betatbl[q->a.w4].x0;
				y = betatbl[q->a.w4].x1;
			} else
				continue;
			goto alloc;
		case 2:
		case 20:
			if (q->a.b2 == 4) {
				x = almap(&q->a, q->op);
				y = q->a.w4;
			} else if (q->a.b2 == 8) {
				x = betatbl[q->a.w4].x0;
				y = betatbl[q->a.w4].x1;
			} else
				continue;
			if (q->a.b2 == 8 && bbit(q->a.w4, 11))
				flag = 1;
			goto alloc;
		case 60:
			if (q->a.b2 != 4 || (q->a.w0 & 0x30) != 0x10)
				continue;
			x = almap(&q->a, q->op);
			y = q->a.w4;
			goto alloc;
		default:
			if (q->a.b2 == 8 && q->c.b2 == 3) {
				x = betatbl[q->a.w4].x0;
				y = betatbl[q->a.w4].x1;
				goto alloc;
			}
			if (q->b.b2 != 8 || q->c.b2 != 3)
				continue;
			x = betatbl[q->b.w4].x0;
			y = betatbl[q->b.w4].x1;
		}
	alloc:
		t = (struct bt *)taballoc((int)sizeof(struct bt), 5);
		t->x0 = x;
		t->x1 = y;
		t->x[0] = q->c.w4;
		t->x[1] = -1;
		t->bopt = 0;
		t->b12 = 0;
		t->f15 = 0;
		t->fl = flag;
		t->f14 = 0;
		t->b10 = 0;
		q->c.b2 = 8;
		q->c.w4 = a0d4;
		a0d4++;
	}
	if (s4d80)
		betaprint();
	a108 = a0d4;
	return 0;	/* host: the original has no return; nobody uses the result */
}

int betaset(struct qa *a)
{
	int i;

	for (i = s4d94; i < a0d4; i++)
		if (betatbl[i].x[0] == a->w4) {
			a->b2 = 8;
			a->w4 = i;
			break;
		}
	return 0;	/* host: the original has no return; nobody uses the result */
}

int betanew(int n)
{
	struct bt *t;

	t = (struct bt *)taballoc((int)sizeof(struct bt), 5);
	t->f14 = 1;
	t->f15 = 0;
	t->bopt = 0;
	t->b12 = 0;
	t->b10 = 0;
	t->x[0] = n;
	t->x[1] = -1;
	t->x0 = 0;
	t->x1 = 0;
	return 1;
}

int betaprint(void)
{
	int i;

	printf("BETA TABLE\n");
	for (i = 0; i < a0d4; i++) {
		printf("// (%d)", i);
		printf("variable = %s, ", ascii + symtab[betatbl[i].x1].nameoff);
		printf("aliaspos = %d, ", betatbl[i].x0);
		printf("tempno = %d, ", betatbl[i].x[0]);
		if (!bbit(i, 13))
			printf("boptimize = FALSE\n");
		else
			printf("boptimize = TRUE\n");
	}
	return 0;	/* host: the original has no return; nobody uses the result */
}

int isvarty(int x)
{
	if (x == 1 || x == 2 || x == 7)
		return 1;
	return 0;
}

int eqopnd(struct qa *a, struct qa *b)
{
	if (a->b2 == b->b2)
		if (a->w4 == b->w4)
			return 1;
	return 0;
}
