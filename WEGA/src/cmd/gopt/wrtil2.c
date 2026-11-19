/*
 * wrtil2.c - gopt: the quads are made into trees again and written as the intermediate language. Written from /lib/gopt (no source
 * known); names guessed, the callees of other modules by the address (fnXXXX).
 */
#include "gopt.h"

static char sccsid[] = "@[$]wrtil2.c\t1.7  03/04/83 16:06:00 Copr. 1981 87wega3.2.";

#define INCREF(t)	((((t) & ~0xF) << 2) | 0x10 | ((t) & 0xF))

extern NODE *talloc();
extern int qflag, ntl;
extern int tl[];
extern char ctpos[], ctneg[];
extern int ifline;
extern char ifname[];
extern int *itab;
extern int nitab;

static int treeeq();
static void wrtree();

/* a name that is not a pointer: its address is taken */
void
addrof(p)
	NODE *p;
{
	NODE *q;

	if (p->in.left->in.op == NAME && (p->in.left->in.type & 0x30) != 0x10) {
		q = talloc();
		q->in.rall = 0;
		q->in.op = UAND;
		q->in.left = p->in.left;
		q->in.type = INCREF(p->in.left->in.type);
		p->in.left = q;
	}
}

/* does the tree below p need a star */
int
needustar(p)
	NODE *p;
{
	NODE *q;

	if (p->in.left->in.op != TEMP) {
		for (q = p->in.left; q->in.op == STAR; q = q->in.left)
			;
		if (qflag > 4)
			printf("// needustar: q->su = %d\n", q->tn.w6);
		if (q->in.op == TEMP && q->tn.w6 == 0)
			return 1;
		return !(optype(q->in.op) == LTYPE || q->in.op == FLD);
	}
	return 1;
}

/* is the operand in the list of the temporaries that have to be kept */
int
tlfind(a)
	struct qa *a;
{
	int i;

	for (i = 0; i < ntl; i++)
		if (a->w4 == tl[i])
			return 1;
	return 0;
}

/* the line and the tree */
void
wrline(p)
	NODE *p;
{
	if (p->in.op != FREE) {
		putchar('.');
		printf("%d\t%s\n", ifline, ifname);
		wrtree(p);
	}
}

/* are the two trees equal */
static int
treeeq(p, q)
	register NODE *p, *q;
{
	register int op;

	op = p->in.op;
	if (op != q->in.op)
		return 0;
	switch (optype(op)) {
	case LTYPE:
		return p->tn.rval == q->tn.rval && (p->in.op == TEMP || p->tn.lval == q->tn.lval);
	case BITYPE:
		if (!treeeq(p->in.right, q->in.right))
			return 0;
	case UTYPE:
		return treeeq(p->in.left, q->in.left);
	}
}

/* x = x op y becomes x op= y */
void
asgfold(pp)
	NODE **pp;
{
	int op;
	NODE *l, *rl, *p;

	p = *pp;
	op = p->in.right->in.op;
	l = p->in.left;
	if (op == 6 || op == 8 || op == 0xb || op == 0x3c) {
		rl = p->in.right->in.left;
		if (treeeq(l, rl)) {
			p->in.op = FREE;
			p->in.left->in.op = FREE;
			p = p->in.right;
			p->in.op = p->in.op + 1;
			*pp = p;
		}
	}
}

/* the counts of the temporaries (debug) */
void
prtcounts()
{
	int i;

	printf("// TEMP COUNTS: temp number, temp count, stmp count\n\n");
	for (i = 0; i < 1000; i++)
		if (ctpos[i] || ctneg[i])
			printf("// (%d)\t%d\t%d\n", i, ctpos[i], ctneg[i]);
}

int
itfind(a)
	struct qa *a;
{
	int i;

	for (i = 1; i < nitab; i++)
		if (itab[i] == a->w4)
			return i;
	return 0;
}
