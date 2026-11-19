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
extern int s70ea;
extern unsigned *a10a;
extern struct quad *fn5_9892();
extern int fn5_992e(), addpreheader();
extern char *fn5_9020(), *addreg();
extern void addrestore();
extern void markreg();
extern struct quad *addsave(), *fn5_913c();
extern struct quad *insertquad();
extern int fn4_fc06();
extern void fn5_8ba8(), fn5_8838(), fn5_89ca(), prgraph();
extern void clrbit(), fn5_964c(), restoredges(), rmnode();
extern int findnode();
extern int werror();
extern int pickcolor(), findnode();
extern void restoredges();
extern void clrmat(), orvec(), mergevectors(), updatevector();
extern struct gn *setedges();
extern void colorgraph(), savecolors();
extern int fn5_91fc(), fn5_9372();
#define QF(q, o)	(*(unsigned **)(*(char **)((char *)(q) + 6) + (o)))
extern void propnd();
extern int a114, a116, a10e;
extern struct gnn *a11a;
#define GF(i, o, t)	(*(t *)((char *)a11a + ((i) * 10 + (o))))
extern char *taballoc();
extern int notlive(), fn5_9adc();
extern void mergevectors(), updatevector(), degrees();
extern void prquad(), prvec(), include(), buildgraph(), colorgraph(), savecolors(), fn5_9698();
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
		colorgraph(flag);
		if (s4d90) {
			printf("// graph after coloring\n");
			fn5_9698();
		}
		savecolors();
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

/* the nodes of the graph are colored: the nodes with few neighbours are put on a stack and get their color in the reverse order (name from the debug output) */
void
colorgraph(flag)
int flag;
{
	struct gn *p;
	struct gn **stack;
	int sp, again, spill, k, j;

	sp = 0;
	if (flag == 0) {
		k = 7;
		if (s4d72)
			k--;
	} else
		k = 4;
	again = 1;
	stack = (struct gn **)taballoc(a116 * 4, 0);
	for (;;) {
		while (again) {
			again = 0;
			spill = 0;
			for (p = a11e; p != 0; p = p->link) {
				if (s4d90 > 4)
					printf("// colorgraph: inside for loop - node %d, degree %d\n", p->row, p->cnt);
				if (p->cnt + p->x14 <= k && p->f15 == 0) {
					p->f15 = 1;
					stack[sp++] = p;
					again = 1;
					for (j = 1; j < a10e; j++) {
						if (testbit(a110, j, p->row, a118)) {
							clrbit(a110, j, p->row, a118);
							clrbit(a110, p->row, j, a118);
							fn5_964c(j);
							--p->cnt;
						}
					}
				} else if (!p->f15)
					spill = 1;
			}
		}
		if (spill == 0) {
			while (--sp >= 0) {
				p = stack[sp];
				if (s4d90 > 2)
					printf("// picking color for node %d\n", p->row);
				restoredges();
				p->x12 = pickcolor(p, k);
			}
			return;
		}
		rmnode();
		again = 1;
	}
}

/* the edges of the graph are entered into the matrix again (name guessed) */
void
restoredges()
{
	struct gn *p;
	struct edge *r;

	for (p = a11e; p != 0; p = p->link) {
		for (r = p->edges; r != 0; r = r->next) {
			setbit(a110, r->k, p->row, a118);
			setbit(a110, p->row, r->k, a118);
		}
	}
}

/* the color of the node p, the neighbours' colors are avoided; -1 if there is none (name guessed) */
int
pickcolor(p, k)
struct gn *p;
int k;
{
	int x, c, j, again, hi, y;

	if (p->x14 == 2)
		k = 5;
	if (k == 4) {
		k = k + 7;
		hi = 8;
	} else
		hi = 1;
	again = 1;
	while (again) {
		again = 0;
		for (j = 1; j < a10e; j++) {
			if (testbit(a110, j, p->row, a118)) {
				c = findnode(j, &x);
				if (k == c || (x == 2 && c + 1 == k) || (p->x14 == 2 && k + 1 == c)) {
					k -= p->x14;
					if (k < hi) {
						werror("compiler error: coloring failed");
						return -1;
					}
					again = 1;
					break;
				}
			}
		}
	}
	return k;
}

/* the color and the class of the node j (name guessed) */
int
findnode(j, px)
int j;
int *px;
{
	struct gn *p;

	for (p = a11e; p != 0; p = p->link) {
		if (p->row == j) {
			*px = p->x14;
			return p->x12;
		}
	}
	cerror("cannot find node %d", j);
}

/* the node with the lowest cost is removed from the graph (name from the debug output) */
void
rmnode()
{
	struct gn *p, *best;
	int min, j;

	min = 9999;
	for (p = a11e; p != 0; p = p->link) {
		if (!p->f15 && p->cost < min) {
			min = p->cost;
			best = p;
		}
	}
	if (s4d90 > 4)
		printf("// rmnode : removing node %d\n", best->row);
	best->f15 = 1;
	for (j = 1; j < a10e; j++) {
		if (testbit(a110, j, best->row, a118)) {
			clrbit(a110, j, best->row, a118);
			clrbit(a110, best->row, j, a118);
			fn5_964c(j);
			--best->cnt;
		}
	}
}

/* the colored temporaries get their registers and the save/restore code (name guessed) */
void
savecolors()
{
	struct gn *n;
	struct quad *q;
	char *r, *r2;
	int i, f, blk, x1, x2;
	struct quad *hdr;

	a10a = (unsigned *)taballoc(s70ea * 2, 0);
	for (n = a11e; n != 0; n = n->link) {
		if (n->x12 <= 0)
			continue;
		for (i = 0; i < a116; i++) {
			if (GF(i, 0, int) != n->row)
				continue;
			q = GF(i, 2, struct quad *);
			f = 0;
			if (QHREC(blks[GF(i, 6, int)])->w12) {
				if (s4d90 > 2) {
					prquad(q);
					printf("before calling getheader, block = %d\n", GF(i, 6, int));
				}
				hdr = fn5_9892(GF(i, 6, int));
				blk = addpreheader(hdr, 1);
				f = 1;
			}
			if (f && !notlive(q, QHREC(blks[blk])->f0)) {
				if (s4d90 > 4)
					printf("// inloop = %x\n", QHREC(blks[GF(i, 6, int)])->w12);
				r = fn5_9020(blk, &x1);
				r = addreg(n->x12, q->c, x1, r);
				r2 = fn5_9020(GF(i, 6, int), &x2);
				if (!fn5_992e(&GF(i, 0, int), r, r2))
					continue;
				addrestore(n->x12, q->c, blk, r);
			} else {
				r = fn5_9020(GF(i, 6, int), &x1);
				r = addreg(n->x12, q->c, x1, r);
				if (f)
					continue;
				if (!fn5_992e(&GF(i, 0, int), r, r))
					continue;
				addrestore(n->x12, q->c, x1, r);
			}
		}
	}
	if (s4d90) {
		printf("// quads before propagation\n");
		prquads();
		prgraph();
	}
	fn5_8ba8();
	if (s4d90) {
		printf("// quads before addsavequads\n");
		prquads();
	}
	fn5_8838();
	fn5_89ca();
}

/* the quad that loads the register reg for the value c is looked for in the block of x or inserted (name from the debug output) */
char *
addreg(reg, c, x, p)
int reg;
struct qa c;
int x;
struct quad *p;
{
	struct quad *qq;
	struct qa tmp;
	int out;

	if (s4d90) {
		printf("addreg : reg = %d rslt = ", reg);
		propnd(&c);
		printf("\n");
	}
	qq = (struct quad *)fn5_9020(x, &out);
	for (qq = *(struct quad **)qq; qq != 0; qq = qq->next) {
		if (qq->op == 28)
			break;
		if (qq->op != 40)
			continue;
		if (qq->a.w4 != reg)
			continue;
		if (!fn4_fc06(&qq->b, &c))
			continue;
		if (s4d90) {
			printf("// addreg (1) returning : ");
			prquad(qq);
		}
		return (char *)qq;
	}
	markreg(x, reg);
	tmp.b2 = 1;
	tmp.w4 = reg;
	tmp.w0 = 4;
	qq = insertquad(40, &tmp, &c, (struct qa *)0, p);
	if (s4d90) {
		printf("// addreg (2) returning : ");
		prquad(qq);
	}
	return (char *)qq;
}

/* the quad that restores the register reg is looked for in the block of x or inserted (name from the debug output) */
void
addrestore(reg, c, x, p)
int reg;
struct qa c;
int x;
struct quad *p;
{
	struct quad *qq;
	struct qa tmp;
	int out, y;

	if (s4d90) {
		printf("addrestore : reg = %d rslt = ", reg);
		propnd(&c);
		printf("\n");
	}
	qq = (struct quad *)fn5_9020(x, &out);
	for (qq = *(struct quad **)qq; qq != 0; qq = qq->next) {
		if (qq->op == 28)
			break;
		if (qq->op != 41)
			continue;
		if (qq->a.w4 == reg)
			return;
	}
	tmp.b2 = 1;
	tmp.w4 = reg;
	tmp.w0 = 4;
	insertquad(41, &tmp, &c, (struct qa *)0, p);
}

/* the register reg is marked in the set of the block x (name from the debug output) */
void
markreg(x, reg)
int x, reg;
{
	if (s4d90 > 1)
		printf("// markreg(%d, %d)\n", x, reg);
	a10a[x] = a10a[x] | (1 << reg);
}

/* the quad that saves the register reg is looked for in the block blk or inserted (name guessed) */
struct quad *
addsave(reg, blk)
int reg, blk;
{
	struct quad *qq, *p;
	struct qa tmp;
	int out;

	qq = (struct quad *)fn5_9020(blk, &out);
	for (qq = *(struct quad **)qq; qq != 0; qq = qq->next) {
		if (qq->op == 28)
			break;
		if (qq->op != 43)
			continue;
		if (qq->a.w4 == reg)
			return qq;
	}
	tmp.b2 = 1;
	tmp.w4 = reg;
	tmp.w0 = 4;
	p = fn5_913c(blk);
	qq = insertquad(43, &tmp, (struct qa *)0, (struct qa *)0, p);
	return qq;
}
