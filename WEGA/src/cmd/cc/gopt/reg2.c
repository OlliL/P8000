/*
 * reg2.c - gopt: the optimizer driver and the register allocation (segment 5). Written from /lib/gopt (no source known);
 * names guessed, the callees by the address (fnXXXX).
 */
static char sccsid[] = "@[$]reg2.c\t1.11  03/04/83 16:04:05 Copr. 1981 87wega3.2.";
#include "mfile2.h"

struct bl { struct bl *next; int blk; };	/* a list of blocks */
struct loop { struct bl *bls; int hdr; struct loop *link; struct bl *exits; int tail; int cnt; unsigned b15:1, b14:1, b13:1, b12:1, b11:1, b10:1; };
struct hrec { unsigned *f0; unsigned *f4; unsigned *f8; int w12; int callfnd; };
#define QHREC(q)	(*(struct hrec **)((char *)(q) + 6))
extern struct quad **blks;
extern struct quad *qhead;
extern struct bt *betatbl;
struct bt { int x0; int x1; int x[2]; unsigned f15:1, f14:1, bopt:1, rest:13; };
#define BBIT(j, n)	((*(int *)((char *)betatbl + ((j) * 10 + 8)) >> (n)) & 1)
extern int matwords;
extern unsigned *adjmat;
struct edge { int k; struct edge *next; };
struct gn { int row; struct gn *link; struct edge *edges; int cnt; int x12; int x14; int cost; unsigned f15:1; };
extern struct gn *cgraph;
struct gnn { int x0; struct quad *q; int blk; int kind; };
extern void clrlbit(), setlbit();
extern unsigned *regused;
extern struct loop *getheader();
extern struct quad *findend();
extern char *addreg();
extern void addrestore();
extern void markreg();
extern struct quad *addsave(), *ebbtail();
extern void propagate(), addsavequads(), rmrestorequad(), prcgraph();
struct pr { int x; int y; };
extern struct pr *edges;
#define PY(i)	(*(int *)((char *)edges + ((i) * 4 + 2)))
extern struct quad *rquad();
extern void addsavequads(), rmrestorequad(), propagate();
extern void clrbit(), decdeg(), restoredges(), rmnode();
extern void restoredges();
extern void clrmat(), mergevectors(), updatevector();
extern struct gn *setedges();
extern void colorgraph(), savecolors();
#define QF(q, o)	(*(unsigned **)(*(char **)((char *)(q) + 6) + (o)))
extern int bblockno, nnodes, ncolors;
extern struct gnn *gnodes;
#define GF(i, o, t)	(*(t *)((char *)gnodes + ((i) * 10 + (o))))
extern void mergevectors(), updatevector(), degrees();
extern void include(), buildgraph(), colorgraph(), savecolors(), prcgraph();
extern void reachdefs(), clearblocks(), marklpblocks();

extern void marklps(), symflags(), chkcalls(), assignreg();

/* the optimizer of the quads: the flow information, the loop information, the value numbering (name guessed) */
void
optimize()
{
	if (rdebug) {
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
	int x;
	struct quad *q;

	p = lp->bls;
outer:
	if (p == 0) {
		p = lp->bls;
		return;
	}
	x = blks[p->blk]->a.w0;
	q = blks[p->blk]->next;
	goto qt;
qs:
	q = q->next;
qt:
	if (q == 0) {
nextp:
		p = p->next;
		goto outer;
	}
	if (q->op == QBBLOCK)
		goto nextp;
	if (q->op != QCALL)
		goto qs;
	if (rdebug > 4)
		printf("// call found in loop\n");
	for (p = lp->bls; p != 0; p = p->next) {
		if (rdebug > 4)
			printf("// setting callfnd in block %d\n", p->blk);
		QHREC(blks[p->blk])->callfnd = 1;
	}
}

/* the symbols that are stored by a quad of type 3 get a flag cleared; the value numbering is called then (name guessed) */
void
symflags()
{
	int found;
	struct quad *q;

	found = 0;
	for (q = qhead; q != 0; q = q->next) {
		if (q->op == QADDR && q->a.b2 == OANAME) {
			symtab[q->a.w4].sflags &= ~SFOPT;
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

	gnodes = (struct gnn *)taballoc(0, 0);
	nnodes = 0;
	ncolors = 1;
	last = 0;
	for (q = qhead; q != 0; q = q->next) {
		if (rdebug > 4) {
			printf("// assignreg: quad =");
			prquad(q);
		}
		if (q->op == QBBLOCK) {
			bblockno = q->a.w0;
			if (rdebug > 4) {
				if (QHREC(blks[bblockno])->w12)
					printf("// block %d in loop\n", bblockno);
				if (QHREC(blks[bblockno])->callfnd)
					printf("// call in loop\n");
				printf("// assignreg:dfptr = %X, bblockno = %d\n", QHREC(q), bblockno);
			}
			dfptr = QHREC(q)->f0;
			if (rdebug > 4) {
				printf("// assignreg : quad =");
				prquad(q);
				prvec(dfptr, 5, 1);
			}
			continue;
		}
		if (inreach(q)) {
			switch (q->c.b2) {
			case OANAME:
				if ((symtab[q->c.w4].sflags & SFOPT) || q->c.fl)
					include(flag, q, last);
				break;
			case OABTEMP:
				if (BBIT(q->c.w4, 13)) {
					if ((notlive(q, dfptr) == 0 || q->op == QCOPY) && q->op != QSTORE)
						include(flag, q, last);
				}
				break;
			case OASTEMP:
				if (BBIT(q->c.w4, 13) && ctneg[q->c.w4] > 2 && q->op != QSTORE)
					include(flag, q, last);
				break;
			}
		}
		last = q;
	}
	if (nnodes) {
		buildgraph();
		degrees();
		if (rdebug) {
			printf("// graph before coloring\n");
			prcgraph();
		}
		colorgraph(flag);
		if (rdebug) {
			printf("// graph after coloring\n");
			prcgraph();
		}
		savecolors();
		if (rdebug) {
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

	if (q->op == QCALL || q->op == QMOVE)
		return;
	if (rdebug > 4) {
		printf("// include : called with");
		propnd(&q->c);
		printf("\n");
	}
	if (flag == 1 && q->c.w0 != FLOAT && q->c.w0 != DOUBLE)
		return;
	if (flag == 0 && (q->c.w0 == FLOAT || q->c.w0 == DOUBLE))
		return;
	if (flag == 0 && q->c.w0 == CHAR)
		return;
	if (QHREC(blks[bblockno])->w12 && QHREC(blks[bblockno])->callfnd && q->c.b2 == OANAME) {
		if (SFISGLOBAL(symtab[q->c.w4].sflags))
			return;
		if (symtab[q->c.w4].sclass == PARAM)
			return;
	}
	if (rdebug > 4) {
		printf("// include :");
		propnd(&q->c);
		printf("\n");
	}
	t = (void *)taballoc(10, 0);
	t->q = q;
	t->x0 = 0;
	t->blk = bblockno;
	if (q->c.w0 == LONG || (zflag && ISPTR(q->c.w0)))
		t->kind = 2;
	else
		t->kind = 1;
	nnodes++;
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
	matwords = (nnodes + 16) / 16;
	adjmat = (unsigned *)taballoc((nnodes + 1) * 2 * matwords, 0);
	clrmat(adjmat, nnodes + 1, matwords);
	cgraph = 0;
	tmp = (unsigned *)taballoc(dfwords * 2, 0);
	live = 0;
	for (q = qhead; q != 0; q = q->next) {
		if (0) {
l59:
			if (rdebug > 4) {
				printf("// buildgraph : processing quad -\n");
				prquad(q);
			}
			dfp = QF(q, 0x18);
			prev = live;
			live = QF(q, 4);
			if (q->next->op != QLABEL && prev != 0)
				orvec(live, prev, lvwords);
			for (k = 0; k < dfwords; k++)
				tmp[k] = dfp[k];
			mergevectors(live, tmp);
			continue;
		}
		if (q->op == QBBLOCK)
			goto l59;
		if (q == GF(i, 2, struct quad *)) {
			updatevector(q, tmp);
			nd = setedges(i, tmp);
			if (q->c.b2 == OANAME) {
				if (symtab[q->c.w4].sbyte5)
					nd->cost = 999;
				else if (q->c.fl)
					nd->cost = 800;
				else
					nd->cost = 0;
			} else if (q->c.b2 == OASTEMP && q->c.w4 > nbeta0)
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

	for (p = cgraph; p != 0; p = p->link) {
		for (j = 1; j < ncolors; j++)
			if (testbit(adjmat, p->row, j, matwords))
				++p->cnt;
	}
}

/* the temporaries that are not live any more are removed from the vector b (name from the debug output) */
void
mergevectors(a, b)
unsigned *a, *b;
{
	int i;

	if (rdebug > 4) {
		printf("mergevectors(), livev:\n");
		prvec(a, 3, 1);
		printf("//\treachv:\n");
		prvec(b, 3, 2);
	}
	for (i = 0; i < ndefs; i++) {
		if (tstlbit(b, i) && notlive(defs[i], a)) {
			if (rdebug > 4) {
				printf("// mergevectors: clearing bit for i = %d, quad = ", i);
				prquad(defs[i]);
			}
			clrlbit(b, i);
		}
	}
	if (rdebug > 4) {
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

	if (q->op == QCALL)
		return;
	for (p = srchdef(q, 1); *++p != -1; ) {
		if (q != defs[*p]) {
			if (rdebug > 4) {
				printf("// updatevector: clearing bit for quad ");
				prquad(defs[*p]);
			}
			clrlbit(v, *p);
		} else {
			if (rdebug > 1) {
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
	int j, k, c;
	struct gn *p;
	struct edge *e;
	struct quad *q, *q2;

	q = GF(i, 2, struct quad *);
	c = nodeno(q);
	for (p = cgraph; p != 0; p = p->link)
		if (p->row == c)
			break;
	if (p == 0) {
		p = (struct gn *)taballoc(20, 0);
		p->link = cgraph;
		cgraph = p;
		p->edges = 0;
		p->row = c;
		p->cnt = 0;
		p->f15 = 0;
		p->x12 = -1;
		p->x14 = GF(i, 8, int);
	}
	for (j = 0; j < ndefs; j++) {
		k = nodeno(defs[j]);
		if (k < 0 || c == k)
			continue;
		q2 = defs[j];
		if (!tstlbit(vec, j) && !insameebbo(q, q2))
			continue;
		if (rdebug > 2)
			printf("// setedge : (%d, %d)\n", c, k);
		if (!testbit(adjmat, c, k, matwords)) {
			e = (struct edge *)taballoc(6, 0);
			e->k = k;
			e->next = p->edges;
			p->edges = e;
			if (rdebug > 4)
				printf("// just added edge %d to node %d\n", e->k, p->row);
			setbit(adjmat, c, k, matwords);
			setbit(adjmat, k, c, matwords);
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
	int spill, again, k, sp, j;
	struct gn **stack;

	sp = 0;
	if (flag == 0) {
		k = 7;
		if (zflag)
			k--;
	} else
		k = 4;
	again = 1;
	stack = (struct gn **)taballoc(nnodes * 4, 0);
	for (;;) {
		while (again) {
			again = 0;
			spill = 0;
			for (p = cgraph; p != 0; p = p->link) {
				if (rdebug > 4)
					printf("// colorgraph: inside for loop - node %d, degree %d\n", p->row, p->cnt);
				if (p->cnt + p->x14 <= k && p->f15 == 0) {
					p->f15 = 1;
					stack[sp++] = p;
					again = 1;
					for (j = 1; j < ncolors; j++) {
						if (testbit(adjmat, j, p->row, matwords)) {
							clrbit(adjmat, j, p->row, matwords);
							clrbit(adjmat, p->row, j, matwords);
							decdeg(j);
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
				if (rdebug > 2)
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

	for (p = cgraph; p != 0; p = p->link) {
		for (r = p->edges; r != 0; r = r->next) {
			setbit(adjmat, r->k, p->row, matwords);
			setbit(adjmat, p->row, r->k, matwords);
		}
	}
}

/* the color of the node p, the neighbours' colors are avoided; -1 if there is none (name guessed) */
int
pickcolor(p, k)
struct gn *p;
int k;
{
	int again, j, x, c, hi, y;
	int unused;	/* the frame of the original has one more word */

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
		for (j = 1; j < ncolors; j++) {
			if (testbit(adjmat, j, p->row, matwords)) {
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

	for (p = cgraph; p != 0; p = p->link) {
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
	for (p = cgraph; p != 0; p = p->link) {
		if (!p->f15 && p->cost < min) {
			min = p->cost;
			best = p;
		}
	}
	if (rdebug > 4)
		printf("// rmnode : removing node %d\n", best->row);
	best->f15 = 1;
	for (j = 1; j < ncolors; j++) {
		if (testbit(adjmat, j, best->row, matwords)) {
			clrbit(adjmat, j, best->row, matwords);
			clrbit(adjmat, best->row, j, matwords);
			decdeg(j);
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
	char *r;
	struct qa unused1, unused2;	/* 12 unused bytes in the frame of the original (two struct qa: a guess) */
	int i, f, blk, x1, x2;
	struct loop *hdr;
	char *r2;

	regused = (unsigned *)taballoc(maxblk * 2, 0);
	for (n = cgraph; n != 0; n = n->link) {
		if (n->x12 <= 0)
			continue;
		for (i = 0; i < nnodes; i++) {
			if (GF(i, 0, int) != n->row)
				continue;
			q = GF(i, 2, struct quad *);
			f = 0;
			if (QHREC(blks[GF(i, 6, int)])->w12) {
				if (rdebug > 2) {
					prquad(q);
					printf("before calling getheader, block = %d\n", GF(i, 6, int));
				}
				hdr = getheader(GF(i, 6, int));
				blk = addpreheader(hdr, 1);
				f = 1;
			}
			if (f && !notlive(q, QHREC(blks[blk])->f0)) {
				if (rdebug > 4)
					printf("// inloop = %x\n", QHREC(blks[GF(i, 6, int)])->w12);
				r = findend(blk, &x1);
				r = addreg(n->x12, q->c, x1, r);
				r2 = findend(GF(i, 6, int), &x2);
				if (!usefirst(&GF(i, 0, int), r, r2))
					continue;
				addrestore(n->x12, q->c, blk, r);
			} else {
				r = findend(GF(i, 6, int), &x1);
				r = addreg(n->x12, q->c, x1, r);
				if (f)
					continue;
				if (!usefirst(&GF(i, 0, int), r, r))
					continue;
				addrestore(n->x12, q->c, x1, r);
			}
		}
	}
	if (rdebug) {
		printf("// quads before propagation\n");
		prquads();
		prgraph();
	}
	propagate();
	if (rdebug) {
		printf("// quads before addsavequads\n");
		prquads();
	}
	addsavequads();
	rmrestorequad();
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

	if (rdebug) {
		printf("addreg : reg = %d rslt = ", reg);
		propnd(&c);
		printf("\n");
	}
	qq = (struct quad *)findend(x, &out);
	for (qq = *(struct quad **)qq; qq != 0; qq = qq->next) {
		if (qq->op == QLABEL)
			break;
		if (qq->op != QREGISTER)
			continue;
		if (qq->a.w4 != reg)
			continue;
		if (!eqopnd(&qq->b, &c))
			continue;
		if (rdebug) {
			printf("// addreg (1) returning : ");
			prquad(qq);
		}
		return (char *)qq;
	}
	markreg(x, reg);
	tmp.b2 = OAICON;
	tmp.w4 = reg;
	tmp.w0 = INT;
	qq = insertquad(QREGISTER, &tmp, &c, (struct qa *)0, p);
	if (rdebug) {
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

	if (rdebug) {
		printf("addrestore : reg = %d rslt = ", reg);
		propnd(&c);
		printf("\n");
	}
	qq = (struct quad *)findend(x, &out);
	for (qq = *(struct quad **)qq; qq != 0; qq = qq->next) {
		if (qq->op == QLABEL)
			break;
		if (qq->op != QRESTORE)
			continue;
		if (qq->a.w4 == reg)
			return;
	}
	tmp.b2 = OAICON;
	tmp.w4 = reg;
	tmp.w0 = INT;
	insertquad(QRESTORE, &tmp, &c, (struct qa *)0, p);
}

/* the quad that saves the register reg is looked for in the block blk or inserted (name guessed) */
struct quad *
addsave(reg, blk)
int reg, blk;
{
	struct quad *qq;
	struct qa tmp;
	int out;
	struct quad *p;

	qq = (struct quad *)findend(blk, &out);
	for (qq = *(struct quad **)qq; qq != 0; qq = qq->next) {
		if (qq->op == QLABEL)
			break;
		if (qq->op != QSAVE)
			continue;
		if (qq->a.w4 == reg)
			return qq;
	}
	tmp.b2 = OAICON;
	tmp.w4 = reg;
	tmp.w0 = INT;
	p = ebbtail(blk);
	qq = insertquad(QSAVE, &tmp, (struct qa *)0, (struct qa *)0, p);
	return qq;
}

/* the register reg is marked in the set of the block x (name from the debug output) */
void
markreg(x, reg)
int x, reg;
{
	if (rdebug > 1)
		printf("// markreg(%d, %d)\n", x, reg);
	regused[x] = regused[x] | (1 << reg);
}

/* the save code of the registers that are used in a block is added to the blocks on the path (name from the debug output) */
void
addsavequads()
{
	int b, r;
	struct quad *q;	/* q, tmp, t: unused (the locals of propagate; the frame of the original has them) */
	int e, s, c, out;
	struct qa tmp;
	struct qa *t, *pa, *pb;

	for (b = 0; b < blkno; b++) {
		if (blks[b] == 0)
			continue;
		if (QHREC(blks[b])->w12)
			continue;
		for (r = 1; r <= 11; r++) {
			if (regused[b] & (1 << r)) {
				e = blks[b]->a.w4;
				if (e < 0)
					continue;
				for (s = PY(e), c = e; c >= 0; c = edges[c].x, s = PY(c)) {
					findend(s, &out);
					if (regused[out] & (1 << r)) {
						pa = &rquad(out, r)->b;
						pb = &rquad(b, r)->b;
						if (!eqopnd(pb, pa))
							addsave(r, out);
					}
				}
			}
		}
	}
}

/* the restore quads that are not needed are removed (name from the debug output) */
void
rmrestorequad()
{
	int b, reg, e, s, c, out;
	struct quad *q;
	struct quad *t;
	int ok;

	for (b = 0; b < blkno; b++) {
		if (rdebug)
			printf("// rmrestorequad, i = %d\n", b);
		if (blks[b] == 0)
			continue;
		for (q = blks[b]->next; q != 0; q = q->next) {
			if (q->op == QBBLOCK)
				break;
			if (q->op != QRESTORE)
				continue;
			if (rdebug)
				printf("// rmrestorequad, found RESTORE\n");
			reg = q->a.w4;
			e = blks[b]->a.w4;
			if (e < 0)
				continue;
			if (rdebug)
				printf("// rmrestorequad, pred = %d\n", e);
			s = PY(e);
			ok = 1;
			for (c = e; c >= 0; c = edges[c].x, s = PY(c)) {
				findend(s, &out);
				t = rquad(out, reg);
				if (t == 0 || !eqopnd(&q->b, &t->b))
					ok = 0;
			}
			if (ok == 1)
				q = delquad(q);
		}
	}
}

/* the saves and restores of the registers are propagated through the blocks (name from the debug output) */
void
propagate()
{
	int b, r, c, i, e;
	struct quad *q;
	struct qa tmp;
	struct qa *t, *pb;

	for (b = 0; b < blkno; b++) {
		if (blks[b] == 0 || blks[b]->next->op != QLABEL)
			continue;
		e = blks[b]->next->a.w4;
		if (rdebug > 1)
			printf("// propagate : block %d\n", b);
		for (r = 1; r <= 11; r++) {
			for (i = 0; i < blkno; i++)
				regused[i] &= ~1;
			c = r_used(b, r);
			if (c < 0)
				continue;
			if (rdebug > 1)
				printf("// propagate : register %d used in block %d\n", r, c);
			if (!(regused[b] & (1 << r))) {
				tmp.b2 = OAICON;
				tmp.w4 = r;
				tmp.w0 = INT;
				t = &rquad(c, r)->b;
				insertquad(QREGISTER, &tmp, t, (struct qa *)0, blks[b]->next);
				markreg(b, r);
			} else {
				t = &rquad(c, r)->b;
				pb = &rquad(b, r)->b;
				if (!eqopnd(pb, t))
					continue;
				q = rquad(b, r)->next;
				if (rdebug > 1) {
					printf("// propagate : check quad -");
					prquad(q);
				}
				if (q->op == QRESTORE && q->a.w4 == r)
					delquad(q);
			}
		}
	}
}

/* the register r is used in the block b or in a block on the path to it: the block, else -1 (name from the debug output) */
int
r_used(b, r)
int b, r;
{
	int e, s, c, t;

	if (rdebug > 1)
		printf("// r_used(%d, %d)\n", b, r);
	regused[b] |= 1;
	e = blks[b]->a.w4;
	if (e < 0)
		return -1;
	s = PY(e);
	for (c = e; c >= 0; c = edges[c].x, s = PY(c))
		if (regused[s] & (1 << r))
			return s;
	s = PY(e);
	for (c = e; c >= 0; c = edges[c].x, s = PY(c)) {
		if (!(regused[s] & 1)) {
			t = r_used(s, r);
			if (t >= 0)
				return t;
		}
	}
	return -1;
}

/* the last quad of the block blk that loads the register reg (name from the debug output) */
struct quad *
rquad(blk, reg)
int blk, reg;
{
	struct quad *q, *res;

	if (rdebug > 1)
		printf("// rquad(%d, %d)\n", blk, reg);
	res = 0;
	if (blks[blk] == 0)
		cerror("rquad called with bad block");
	for (q = blks[blk]->next; q != 0; q = q->next) {
		if (q->op == QBBLOCK)
			break;
		if (q->op == QREGISTER && q->a.w4 == reg)
			res = q;
	}
	return res;
}

/* the block with the end of the extended basic block of b; its number in *px (name guessed) */
struct quad *
findend(b, px)
int b;
int *px;
{
	int c;
	struct quad *q;
	int e;

	e = blks[b]->a.w4;
	c = b;
	while (c >= 0) {
		if (blks[c]->a.w4 < 0) {
			*px = c;
			return blks[c];
		}
		for (q = blks[c]->next; q != 0; q = q->next) {
			if (q->op == QBBLOCK)
				break;
			if (q->op == QLABEL) {
				*px = c;
				return q;
			}
		}
		c = PY(e);
		e = blks[c]->a.w4;
	}
	*px = 0;
	return 0;
}

/* the last quad of the extended basic block of b (name from the debug output) */
struct quad *
ebbtail(b)
int b;
{
	struct quad *q, *last;

	last = blks[b];
	for (q = blks[b]->next; q != 0; q = q->next) {
		if (q->op == QEND || (q->op == QBBLOCK && q->next->op == QLABEL) || q->op == QJP)
			return last;
		last = q;
	}
	cerror("error in ebbtail");
}

/* the node number of the quad q: the nodes of equal values share a number (name guessed) */
int
nodeno(q)
struct quad *q;
{
	int i, j;
	struct qa *c, *c2;

	c = (struct qa *)((char *)q + 6);
	for (i = 0; i < nnodes; i++)
		if (GF(i, 2, struct quad *) == q) {
			j = i;
			break;
		}
	if (i == nnodes)
		return -1;
	if (GF(j, 0, int) > 0)
		return GF(j, 0, int);
	for (i = 0; i < nnodes; i++) {
		c2 = &GF(i, 2, struct quad *)->c;
		if (GF(i, 0, int) > 0 && c->b2 == c2->b2 && c->w4 == c2->w4 && i != j && canshare(j, i)) {
			GF(j, 0, int) = GF(i, 0, int);
			break;
		}
	}
	if (GF(j, 0, int) <= 0) {
		GF(j, 0, int) = ncolors;
		ncolors++;
	}
	return GF(j, 0, int);
}

/* the quads p and q are in the same extended basic block (name from the message) */
int
insameebbo(p, q)
struct quad *p, *q;
{
	int i, ip, jq, e1, e2;

	for (i = 0; i < nnodes; i++)
		if (GF(i, 2, struct quad *) == p) {
			ip = i;
			break;
		}
	if (i == nnodes)
		cerror("insameebbo can't find quad");
	for (i = 0; i < nnodes; i++)
		if (GF(i, 2, struct quad *) == q) {
			jq = i;
			break;
		}
	if (i == nnodes)
		cerror("insameebbo can't find quad");
	findend(GF(ip, 6, int), &e1);
	findend(GF(jq, 6, int), &e2);
	if (e1 == e2)
		return 1;
	return 0;
}

/* the nodes a and b are not alive at the same time and can share a register (name guessed) */
int
canshare(a, b)
int a, b;
{
	struct quad *p, *q;
	int ka, kb, k;

	p = GF(a, 2, struct quad *);
	q = GF(b, 2, struct quad *);
	ka = *srchdef(p, 2);
	kb = *srchdef(q, 2);
	if (tstlbit(QF(blks[GF(a, 6, int)], 0x18), kb) && !notlive(q, QHREC(blks[GF(a, 6, int)])->f4))
		return 1;
	if (tstlbit(QF(blks[GF(b, 6, int)], 0x18), ka) && !notlive(p, QHREC(blks[GF(b, 6, int)])->f4))
		return 1;
	for (k = 0; k < blkno; k++) {
		if (blks[k] != 0 && !notlive(p, QHREC(blks[k])->f4) && !notlive(q, QHREC(blks[k])->f4))
			return 1;
	}
	return 0;
}

/* the degree of the node n is decremented (name guessed) */
void
decdeg(n)
int n;
{
	struct gn *p;

	for (p = cgraph; p != 0; p = p->link) {
		if (n == p->row) {
			--p->cnt;
			return;
		}
	}
	cerror("register map missing nodes n  = %d", n);
}

/* the graph is printed (name from the debug output) */
void
prcgraph()
{
	struct gn *p;
	struct edge *e;
	int i, j;

	for (i = 0; i < nnodes; i++) {
		printf("// node number = %d, ", GF(i, 0, int));
		propnd(&GF(i, 2, struct quad *)->c);
		printf("quadno = %X,", GF(i, 2, struct quad *));
		printf("\n");
	}
	for (p = cgraph; p != 0; p = p->link) {
		printf("// node number = %d, color = %d, degree = %d, weight = %d\n", p->row, p->x12, p->cnt, p->cost);
		for (e = p->edges; e != 0; e = e->next)
			printf("//     edge = %d\n", e->k);
	}
	printf("\n// itfs :\n");
	for (i = 0; i < ncolors; i++) {
		printf("// ");
		for (j = 0; j < ncolors; j++) {
			if (testbit(adjmat, i, j, matwords))
				putc('1', stdout);
			else
				putc('0', stdout);
		}
		putc('\n', stdout);
	}
}

/* the loop that has the block n as its header (name from the debug output) */
struct loop *
getheader(n)
int n;
{
	struct loop *lp;
	struct bl *b;

	if (rdebug > 2)
		printf("// getheader n = %d\n", n);
	for (lp = loops; lp != 0; lp = lp->link) {
		for (b = lp->bls; b != 0; b = b->next) {
			if (rdebug > 2)
				printf("// getheader : lblkno = %d\n", b->blk);
			if (b->blk == n)
				return lp;
		}
	}
	cerror("getheader: can't find header");
}

/* the register of the quad r is used first, before the quad r3 or the end of the block (name from the debug output) */
int
usefirst(g, r, r3)
struct gnn *g;
struct quad *r, *r3;
{
	struct quad *c, *end;

	if (rdebug) {
		printf("// usefirst: regquad = ");
		prquad(r);
	}
	end = g->q;
	for (c = r3->next; c != 0; c = c->next) {
		if (c == end || c->op == QLABEL || c->op == QEND)
			return 0;
		if (c->op == QREGISTER)
			continue;
		if (eqopnd(&r->b, &c->a))
			return 1;
		if (eqopnd(&r->b, &c->b))
			return 1;
	}
	return 0;
}

/* the operands of the quad s are used again behind it in the block (name guessed) */
int
usedlater(n, u, s)
int n, u;
struct quad *s;
{
	struct quad *c;

	for (c = s; c != 0; c = c->next) {
		if (c->op == QLABEL)
			return 0;
		if (c->op == QJPC && c->b.w4 == n)
			return 0;
		if (c->op != QREGISTER && c->op != QRESTORE) {
			if (eqopnd(&s->b, &c->a))
				return 1;
			if (eqopnd(&s->b, &c->b))
				return 1;
			if (eqopnd(&s->b, &c->c))
				return 1;
		}
	}
	return 0;
}

/* the quad q is in the set of the quads that reach (name from the debug output) */
int
inreach(q)
struct quad *q;
{
	int i;

	for (i = 0; i < ndefs; i++)
		if (defs[i] == q)
			return 1;
	if (rdebug > 3) {
		printf("// inreach returning FALSE for quad -");
		prquad(q);
	}
	return 0;
}
