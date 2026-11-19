/*
 * reg2.c - gopt: the optimizer driver and the register allocation (segment 5). Written from /lib/gopt (no source known);
 * names guessed, the callees by the address (fnXXXX).
 */
#include "gopt.h"

static char sccsid[] = "@[$]reg2.c\t1.11  03/04/83 16:04:05 Copr. 1981 87wega3.2.";

struct bl { struct bl *next; int blk; };	/* a list of blocks */
struct loop { struct bl *bls; int hdr; struct loop *link; struct bl *exits; int tail; int cnt; unsigned b15:1, b14:1, b13:1, b12:1, b11:1, b10:1; };
struct hrec { unsigned *f0; unsigned *f4; unsigned *f8; int w12; int callfnd; };
#define QHREC(q)	(*(struct hrec **)((char *)(q) + 6))
extern struct quad **blks;
extern struct loop *loops;
extern struct quad *qhead;
extern struct sym *symtab;
extern int s4d90;
extern char a0010[];
extern struct bt *betatbl;
struct bt { int x0; int x1; int x[2]; unsigned f15:1, f14:1, bopt:1, rest:13; };
#define BBIT(j, n)	((*(int *)((char *)betatbl + ((j) * 10 + 8)) >> (n)) & 1)
extern int s4d72, s7144, s7158;
extern int a118, a108;
extern unsigned *a110;
struct edge { int k; struct edge *next; };
struct gn { int row; struct gn *link; struct edge *edges; int cnt; int x12; int x14; int cost; unsigned f15:1; };
extern struct gn *a11e;
struct gnn { int x0; struct quad *q; int blk; int kind; };
extern int s714e;
extern struct quad **s7154;
extern int *srchdef();
extern int tstlbit(), testbit();
extern void clrlbit(), setlbit(), setbit();
extern void clrmat(), orvec(), mergevectors(), updatevector();
extern struct gn *setedges();
extern int fn5_91fc(), fn5_9372();
#define QF(q, o)	(*(unsigned **)(*(char **)((char *)(q) + 6) + (o)))
extern void propnd();
extern int a114, a116, a10e;
extern struct gnn *a11a;
#define GF(i, o, t)	(*(t *)((char *)a11a + ((i) * 10 + (o))))
extern char *taballoc();
extern int notlive(), fn5_9adc();
extern void mergevectors(), updatevector(), degrees();
extern void prquad(), prvec(), include(), buildgraph(), fn5_7cb4(), fn5_8198(), fn5_9698();
extern void clearflags(), livevars(), reachdefs(), prquads(), ebbopt(), clearblocks(), marklpblocks();

extern void marklps(), symflags(), chkcalls(), assignreg();

/* the optimizer of the quads: the flow information, the loop information, the value numbering (name guessed) */
void
optimize()
{
	if (s4d90) {
		printf("// quads before register allocation\n");
		prquads();
	}
	clearflags();
	livevars();
	marklps();
	symflags();
	reachdefs();
	assignreg(0);
	assignreg(1);
}

/* the blocks of every loop are marked, and the calls of the loop are looked for (name guessed) */
void
marklps()
{
	struct loop *lp;

	clearblocks();
	for (lp = loops; lp != 0; lp = lp->link) {
		marklpblocks(lp, 1);
		chkcalls(lp);
	}
}

/* a call in a block of the loop sets callfnd in all blocks of the loop (name from the debug output) */
void
chkcalls(lp)
struct loop *lp;
{
	struct bl *p;
	struct quad *q;
	int x;

	p = (struct bl *)lp;
	for (;;) {
		p = p->next;
		if (p == 0)
			break;
		x = blks[p->blk]->a.w0;
		for (q = blks[p->blk]; (q = q->next) != 0; ) {
			if (q->op == 59)
				break;
			if (q->op == 7) {
				if (s4d90 > 4)
					printf("// call found in loop\n");
				for (p = lp->bls; p != 0; p = p->next) {
					if (s4d90 > 4)
						printf("// setting callfnd in block %d\n", p->blk);
					QHREC(blks[p->blk])->callfnd = 1;
				}
				return;
			}
		}
	}
	p = lp->bls;
}

/* the symbols that are stored by a quad of type 3 get a flag cleared; the value numbering is called then (name guessed) */
void
symflags()
{
	struct quad *q;
	int found;

	found = 0;
	for (q = qhead; q != 0; q = q->next) {
		if (q->op == 3 && q->a.b2 == 4) {
			symtab[q->a.w4].sflags &= ~1;
			found = 1;
		}
	}
	if (found)
		ebbopt();
}

/* the registers are assigned to the temporaries (name from the debug output) */
void
assignreg(flag)
int flag;
{
	struct quad *q, *last;
	unsigned *dfptr;

	a11a = (struct gnn *)taballoc(0, 0);
	a116 = 0;
	a10e = 1;
	last = 0;
	for (q = qhead; q != 0; q = q->next) {
		if (s4d90 > 4) {
			printf("// assignreg: quad =");
			prquad(q);
		}
		if (q->op == 59) {
			a114 = q->a.w0;
			if (s4d90 > 4) {
				if (QHREC(blks[a114])->w12)
					printf("// block %d in loop\n", a114);
				if (QHREC(blks[a114])->callfnd)
					printf("// call in loop\n");
				printf("// assignreg:dfptr = %X, bblockno = %d\n", QHREC(q), a114);
			}
			dfptr = QHREC(q)->f0;
			if (s4d90 > 4) {
				printf("// assignreg : quad =");
				prquad(q);
				prvec(dfptr, 5, 1);
			}
			continue;
		}
		if (fn5_9adc(q)) {
			switch (q->c.b2) {
			case 4:
				if ((symtab[q->c.w4].sflags & 1) || q->f7)
					include(flag, q, last);
				break;
			case 8:
				if (BBIT(q->c.w4, 13)) {
					if ((notlive(q, dfptr) == 0 || q->op == 60) && q->op != 47)
						include(flag, q, last);
				}
				break;
			case 6:
				if (BBIT(q->c.w4, 13) && a0010[q->c.w4] > 2 && q->op != 47)
					include(flag, q, last);
				break;
			}
		}
		last = q;
	}
	if (a116) {
		buildgraph();
		degrees();
		if (s4d90) {
			printf("// graph before coloring\n");
			fn5_9698();
		}
		fn5_7cb4(flag);
		if (s4d90) {
			printf("// graph after coloring\n");
			fn5_9698();
		}
		fn5_8198();
		if (s4d90) {
			printf("// quads after register allocation\n");
			prquads();
		}
	}
}

/* the quad q gets an entry in the list of the quads that need a register (name from the debug output) */
void
include(flag, q, last)
int flag;
struct quad *q, *last;
{
	struct { int x0; struct quad *q; int blk; int kind; } *t;

	if (q->op == 7 || q->op == 33)
		return;
	if (s4d90 > 4) {
		printf("// include : called with");
		propnd(&q->c);
		printf("\n");
	}
	if (flag == 1 && q->c.w0 != 6 && q->c.w0 != 7)
		return;
	if (flag == 0 && (q->c.w0 == 6 || q->c.w0 == 7))
		return;
	if (flag == 0 && q->c.w0 == 2)
		return;
	if (QHREC(blks[a114])->w12 && QHREC(blks[a114])->callfnd && q->c.b2 == 4) {
		if ((symtab[q->c.w4].sflags >> 2) & 1)
			return;
		if (symtab[q->c.w4].sclass == 9)
			return;
	}
	if (s4d90 > 4) {
		printf("// include :");
		propnd(&q->c);
		printf("\n");
	}
	t = (void *)taballoc(10, 0);
	t->q = q;
	t->x0 = 0;
	t->blk = a114;
	if (q->c.w0 == 5 || (s4d72 && (q->c.w0 & 0x30) == 0x10))
		t->kind = 2;
	else
		t->kind = 1;
	a116++;
}

/* the graph of the temporaries that are alive at the same time is built (name from the debug output) */
void
buildgraph()
{
	struct quad *q;
	struct gn *nd;
	unsigned *tmp, *dfp, *live, *prev;
	int i, k;

	i = 0;
	a118 = (a116 + 16) / 16;
	a110 = (unsigned *)taballoc((a116 + 1) * 2 * a118, 0);
	clrmat(a110, a116 + 1, a118);
	a11e = 0;
	tmp = (unsigned *)taballoc(s7144 * 2, 0);
	live = 0;
	for (q = qhead; q != 0; q = q->next) {
		if (q->op == 59) {
			if (s4d90 > 4) {
				printf("// buildgraph : processing quad -\n");
				prquad(q);
			}
			dfp = QF(q, 0x18);
			prev = live;
			live = QF(q, 4);
			if (q->next->op != 28 && prev != 0)
				orvec(live, prev, s7158);
			for (k = 0; k < s7144; k++)
				tmp[k] = dfp[k];
			mergevectors(live, tmp);
		} else if (q == GF(i, 2, struct quad *)) {
			updatevector(q, tmp);
			nd = setedges(i, tmp);
			if (q->c.b2 == 4) {
				if (symtab[q->c.w4].sbyte5)
					nd->cost = 999;
				else if (q->f7)
					nd->cost = 800;
				else
					nd->cost = 0;
			} else if (q->c.b2 == 6 && q->c.w4 > a108)
				nd->cost = 700;
			else
				nd->cost = 0;
			if (QHREC(blks[GF(i, 6, int)])->w12)
				++nd->cost;
			i++;
		}
	}
}

/* the number of the neighbours of the nodes of the graph is counted (name guessed) */
void
degrees()
{
	struct gn *p;
	int j;

	for (p = a11e; p != 0; p = p->link) {
		for (j = 1; j < a10e; j++)
			if (testbit(a110, p->row, j, a118))
				++p->cnt;
	}
}

/* the temporaries that are not live any more are removed from the vector b (name from the debug output) */
void
mergevectors(a, b)
unsigned *a, *b;
{
	int i;

	if (s4d90 > 4) {
		printf("mergevectors(), livev:\n");
		prvec(a, 3, 1);
		printf("//\treachv:\n");
		prvec(b, 3, 2);
	}
	for (i = 0; i < s714e; i++) {
		if (tstlbit(b, i) && notlive(s7154[i], a)) {
			if (s4d90 > 4) {
				printf("// mergevectors: clearing bit for i = %d, quad = ", i);
				prquad(s7154[i]);
			}
			clrlbit(b, i);
		}
	}
	if (s4d90 > 4) {
		printf("// mergevector returning :\n");
		prvec(b, 3, 2);
	}
}

/* the vector v is updated for the quad q (name from the debug output) */
void
updatevector(q, v)
struct quad *q;
unsigned *v;
{
	int *p;

	if (q->op == 7)
		return;
	for (p = srchdef(q, 1); *p != -1; p++) {
		if (q != s7154[*p]) {
			if (s4d90 > 4) {
				printf("// updatevector: clearing bit for quad ");
				prquad(s7154[*p]);
			}
			clrlbit(v, *p);
		} else {
			if (s4d90 > 1) {
				printf("// updatevector: setting bit for quad ");
				prquad(q);
			}
			setlbit(v, *p);
		}
	}
}

/* the node of the temporary of the quad number i is looked for in the graph, or entered; the edges to the live temporaries are added (name from the debug output) */
struct gn *
setedges(i, vec)
int i;
unsigned *vec;
{
	struct gn *p;
	struct edge *e;
	struct quad *q, *q2;
	int j, k, c;

	q = GF(i, 2, struct quad *);
	c = fn5_91fc(q);
	for (p = a11e; p != 0; p = p->link)
		if (p->row == c)
			break;
	if (p == 0) {
		p = (struct gn *)taballoc(20, 0);
		p->link = a11e;
		a11e = p;
		p->edges = 0;
		p->row = c;
		p->cnt = 0;
		p->f15 = 0;
		p->x12 = -1;
		p->x14 = GF(i, 8, int);
	}
	for (j = 0; j < s714e; j++) {
		k = fn5_91fc(s7154[j]);
		if (k < 0 || c == k)
			continue;
		q2 = s7154[j];
		if (!tstlbit(vec, j) && !fn5_9372(q, q2))
			continue;
		if (s4d90 > 2)
			printf("// setedge : (%d, %d)\n", c, k);
		if (!testbit(a110, c, k, a118)) {
			e = (struct edge *)taballoc(6, 0);
			e->k = k;
			e->next = p->edges;
			p->edges = e;
			if (s4d90 > 4)
				printf("// just added edge %d to node %d\n", e->k, p->row);
			setbit(a110, c, k, a118);
			setbit(a110, k, c, a118);
		}
	}
	return p;
}
