/*
 * reg2.c - gopt: the optimizer driver and the register allocation (segment 5). Written from /lib/gopt (no source known);
 * names guessed, the callees by the address (fnXXXX).
 */
static char sccsid[] __attribute__((unused)) = "@[$]reg2.c\t1.11  03/04/83 16:04:05 Copr. 1981 87wega3.2.";
#include "gopt.h"

extern struct quad **blks;
extern struct loop *loops;
extern struct quad *qhead;
extern struct sym *symtab;
extern int s4d90;
extern char ctneg[];
extern struct bt *betatbl;
extern int s4d72, dfwords, lvwords;
extern int a118, a108;
extern unsigned *a110;
extern struct gn *a11e;
extern int ndefs;
extern struct quad **defs;
extern int maxblk;
extern unsigned *a10a;
extern struct pr *edges;
extern int blkno;
extern int a114, a116, a10e;
extern struct gnn *a11a;


/* the optimizer of the quads: the flow information, the loop information, the value numbering (name guessed) */
void optimize(void)
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
void marklps(void)
{
	struct loop *lp;

	clearblocks();
	for (lp = loops; lp != 0; lp = lp->link) {
		marklpblocks(lp, 1);
		chkcalls(lp);
	}
}

/* a call in a block of the loop sets callfnd in all blocks of the loop (name from the debug output) */
void chkcalls(struct loop *lp)
{
	struct bl *p;
	int x __attribute__((unused));
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
	if (q->op == 59)
		goto nextp;
	if (q->op != 7)
		goto qs;
	if (s4d90 > 4)
		printf("// call found in loop\n");
	for (p = lp->bls; p != 0; p = p->next) {
		if (s4d90 > 4)
			printf("// setting callfnd in block %d\n", p->blk);
		(blks[p->blk])->rec->callfnd = 1;
	}
}

/* the symbols that are stored by a quad of type 3 get a flag cleared; the value numbering is called then (name guessed) */
void symflags(void)
{
	int found;
	struct quad *q;

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
void assignreg(int flag)
{
	struct quad *q, *last;
	unsigned *dfptr = 0;	/* host: = 0 (the original: what the frame holds) */

	a11a = (struct gnn *)tabzalloc(0, 0, 0);
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
				if ((blks[a114])->rec->w12)
					printf("// block %d in loop\n", a114);
				if ((blks[a114])->rec->callfnd)
					printf("// call in loop\n");
				printf("// assignreg:dfptr = %X, bblockno = %d\n", (unsigned)(uintptr_t)((q)->rec), a114);
			}
			dfptr = (q)->rec->f0;
			if (s4d90 > 4) {
				printf("// assignreg : quad =");
				prquad(q);
				prvec(dfptr, 5, 1);
			}
			continue;
		}
		if (inreach(q)) {
			switch (q->c.b2) {
			case 4:
				if ((symtab[q->c.w4].sflags & 1) || q->c.fl)
					include(flag, q, last);
				break;
			case 8:
				if (bbit(q->c.w4, 13)) {
					if ((notlive(q, dfptr) == 0 || q->op == 60) && q->op != 47)
						include(flag, q, last);
				}
				break;
			case 6:
				if (bbit(q->c.w4, 13) && ctneg[q->c.w4] > 2 && q->op != 47)
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
			prcgraph();
		}
		colorgraph(flag);
		if (s4d90) {
			printf("// graph after coloring\n");
			prcgraph();
		}
		savecolors();
		if (s4d90) {
			printf("// quads after register allocation\n");
			prquads();
		}
	}
}

/* the quad q gets an entry in the list of the quads that need a register (name from the debug output) */
void include(int flag, struct quad *q, struct quad *last)
{
	(void)last;
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
	if ((blks[a114])->rec->w12 && (blks[a114])->rec->callfnd && q->c.b2 == 4) {
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
	t = (void *)tabzalloc((int)sizeof(struct gnn), 10, 0);
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
void buildgraph(void)
{
	struct quad *q;
	struct gn *nd;
	unsigned *tmp, *dfp, *live, *prev;
	int i, k;

	i = 0;
	a118 = (a116 + 16) / 16;
	a110 = (unsigned *)tabzalloc((a116 + 1) * a118 * (int)sizeof(unsigned), (a116 + 1) * 2 * a118, 0);
	clrmat((int *)a110, a116 + 1, a118);
	a11e = 0;
	tmp = (unsigned *)tabzalloc(dfwords * (int)sizeof(unsigned), dfwords * 2, 0);
	live = 0;
	for (q = qhead; q != 0; q = q->next) {
		if (0) {
l59:
			if (s4d90 > 4) {
				printf("// buildgraph : processing quad -\n");
				prquad(q);
			}
			dfp = (q)->rec->f18;
			prev = live;
			live = (q)->rec->f4;
			if (q->next->op != 28 && prev != 0)
				orvec(live, prev, lvwords);
			for (k = 0; k < dfwords; k++)
				tmp[k] = dfp[k];
			mergevectors(live, tmp);
			continue;
		}
		if (q->op == 59)
			goto l59;
		if (q == a11a[i].q) {
			updatevector(q, tmp);
			nd = setedges(i, tmp);
			if (q->c.b2 == 4) {
				if (symtab[q->c.w4].sbyte5)
					nd->cost = 999;
				else if (q->c.fl)
					nd->cost = 800;
				else
					nd->cost = 0;
			} else if (q->c.b2 == 6 && q->c.w4 > a108)
				nd->cost = 700;
			else
				nd->cost = 0;
			if ((blks[a11a[i].blk])->rec->w12)
				++nd->cost;
			i++;
		}
	}
}

/* the number of the neighbours of the nodes of the graph is counted (name guessed) */
void degrees(void)
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
void mergevectors(unsigned *a, unsigned *b)
{
	int i;

	if (s4d90 > 4) {
		printf("mergevectors(), livev:\n");
		prvec(a, 3, 1);
		printf("//\treachv:\n");
		prvec(b, 3, 2);
	}
	for (i = 0; i < ndefs; i++) {
		if (tstlbit(b, i) && notlive(defs[i], a)) {
			if (s4d90 > 4) {
				printf("// mergevectors: clearing bit for i = %d, quad = ", i);
				prquad(defs[i]);
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
void updatevector(struct quad *q, unsigned *v)
{
	int *p;

	if (q->op == 7)
		return;
	for (p = srchdef(q, 1); *++p != -1; ) {
		if (q != defs[*p]) {
			if (s4d90 > 4) {
				printf("// updatevector: clearing bit for quad ");
				prquad(defs[*p]);
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
struct gn *setedges(int i, unsigned *vec)
{
	int j, k, c;
	struct gn *p;
	struct gedge *e;
	struct quad *q, *q2;

	q = a11a[i].q;
	c = nodeno(q);
	for (p = a11e; p != 0; p = p->link)
		if (p->row == c)
			break;
	if (p == 0) {
		p = (struct gn *)tabzalloc((int)sizeof(struct gn), 20, 0);
		p->link = a11e;
		a11e = p;
		p->edges = 0;
		p->row = c;
		p->cnt = 0;
		p->f15 = 0;
		p->x12 = -1;
		p->x14 = a11a[i].kind;
	}
	for (j = 0; j < ndefs; j++) {
		k = nodeno(defs[j]);
		if (k < 0 || c == k)
			continue;
		q2 = defs[j];
		if (!tstlbit(vec, j) && !insameebbo(q, q2))
			continue;
		if (s4d90 > 2)
			printf("// setedge : (%d, %d)\n", c, k);
		if (!testbit(a110, c, k, a118)) {
			e = (struct gedge *)tabzalloc((int)sizeof(struct gedge), 6, 0);
			e->k = k;
			e->next = p->edges;
			p->edges = e;
			if (s4d90 > 4)
				printf("// just added edge %d to node %d\n", e->k, p->row);
			setbit((int *)a110, c, k, a118);
			setbit((int *)a110, k, c, a118);
		}
	}
	return p;
}

/* the nodes of the graph are colored: the nodes with few neighbours are put on a stack and get their color in the reverse order (name from the debug output) */
void colorgraph(int flag)
{
	struct gn *p;
	int spill, again, k, sp, j;
	struct gn **stack;

	sp = 0;
	if (flag == 0) {
		k = 7;
		if (s4d72)
			k--;
	} else
		k = 4;
	again = 1;
	stack = (struct gn **)tabzalloc(a116 * (int)sizeof(struct gn *), a116 * 4, 0);
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
							clrbit((int *)a110, j, p->row, a118);
							clrbit((int *)a110, p->row, j, a118);
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
void restoredges(void)
{
	struct gn *p;
	struct gedge *r;

	for (p = a11e; p != 0; p = p->link) {
		for (r = p->edges; r != 0; r = r->next) {
			setbit((int *)a110, r->k, p->row, a118);
			setbit((int *)a110, p->row, r->k, a118);
		}
	}
}

/* the color of the node p, the neighbours' colors are avoided; -1 if there is none (name guessed) */
int pickcolor(struct gn *p, int k)
{
	int again, j, x, c, hi;

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
int findnode(int j, int *px)
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
void rmnode(void)
{
	struct gn *p, *best = 0;	/* host: = 0 (the original: what the frame holds) */
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
			clrbit((int *)a110, j, best->row, a118);
			clrbit((int *)a110, best->row, j, a118);
			decdeg(j);
			--best->cnt;
		}
	}
}

/* the colored temporaries get their registers and the save/restore code (name guessed) */
void savecolors(void)
{
	struct gn *n;
	struct quad *q;
	struct quad *r;
	int i, f, blk, x1, x2;
	struct loop *hdr;
	struct quad *r2;

	a10a = (unsigned *)tabzalloc(maxblk * (int)sizeof(unsigned), maxblk * 2, 0);
	for (n = a11e; n != 0; n = n->link) {
		if (n->x12 <= 0)
			continue;
		for (i = 0; i < a116; i++) {
			if (a11a[i].x0 != n->row)
				continue;
			q = a11a[i].q;
			f = 0;
			if ((blks[a11a[i].blk])->rec->w12) {
				if (s4d90 > 2) {
					prquad(q);
					printf("before calling getheader, block = %d\n", a11a[i].blk);
				}
				hdr = getheader(a11a[i].blk);
				blk = addpreheader(hdr, 1);
				f = 1;
			}
			if (f && !notlive(q, (blks[blk])->rec->f0)) {
				if (s4d90 > 4)
					printf("// inloop = %x\n", (blks[a11a[i].blk])->rec->w12);
				r = findend(blk, &x1);
				r = addreg(n->x12, q->c, x1, r);
				r2 = findend(a11a[i].blk, &x2);
				if (!usefirst(&a11a[i], r, r2))
					continue;
				addrestore(n->x12, q->c, blk, r);
			} else {
				r = findend(a11a[i].blk, &x1);
				r = addreg(n->x12, q->c, x1, r);
				if (f)
					continue;
				if (!usefirst(&a11a[i], r, r))
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
	propagate();
	if (s4d90) {
		printf("// quads before addsavequads\n");
		prquads();
	}
	addsavequads();
	rmrestorequad();
}

/* the quad that loads the register reg for the value c is looked for in the block of x or inserted (name from the debug output) */
struct quad *addreg(int reg, struct qa c, int x, struct quad *p)
{
	struct quad *qq;
	struct qa tmp;
	int out;

	if (s4d90) {
		printf("addreg : reg = %d rslt = ", reg);
		propnd(&c);
		printf("\n");
	}
	qq = (struct quad *)findend(x, &out);
	for (qq = *(struct quad **)qq; qq != 0; qq = qq->next) {
		if (qq->op == 28)
			break;
		if (qq->op != 40)
			continue;
		if (qq->a.w4 != reg)
			continue;
		if (!eqopnd(&qq->b, &c))
			continue;
		if (s4d90) {
			printf("// addreg (1) returning : ");
			prquad(qq);
		}
		return qq;
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
	return qq;
}

/* the quad that restores the register reg is looked for in the block of x or inserted (name from the debug output) */
void addrestore(int reg, struct qa c, int x, struct quad *p)
{
	struct quad *qq;
	struct qa tmp;
	int out;

	if (s4d90) {
		printf("addrestore : reg = %d rslt = ", reg);
		propnd(&c);
		printf("\n");
	}
	qq = (struct quad *)findend(x, &out);
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

/* the quad that saves the register reg is looked for in the block blk or inserted (name guessed) */
struct quad *addsave(int reg, int blk)
{
	struct quad *qq;
	struct qa tmp;
	int out;
	struct quad *p;

	qq = (struct quad *)findend(blk, &out);
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
	p = ebbtail(blk);
	qq = insertquad(43, &tmp, (struct qa *)0, (struct qa *)0, p);
	return qq;
}

/* the register reg is marked in the set of the block x (name from the debug output) */
void markreg(int x, int reg)
{
	if (s4d90 > 1)
		printf("// markreg(%d, %d)\n", x, reg);
	a10a[x] = a10a[x] | (1 << reg);
}

/* the save code of the registers that are used in a block is added to the blocks on the path (name from the debug output) */
void addsavequads(void)
{
	int b, r;
	int e, s, c, out;
	struct qa  *pa, *pb;

	for (b = 0; b < blkno; b++) {
		if (blks[b] == 0)
			continue;
		if ((blks[b])->rec->w12)
			continue;
		for (r = 1; r <= 11; r++) {
			if (a10a[b] & (1 << r)) {
				e = blks[b]->a.w4;
				if (e < 0)
					continue;
				for (s = edges[e].y, c = e; c >= 0; c = edges[c].x, s = edges[c].y) {
					findend(s, &out);
					if (a10a[out] & (1 << r)) {
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
void rmrestorequad(void)
{
	int b, reg, e, s, c, out;
	struct quad *q;
	struct quad *t;
	int ok;

	for (b = 0; b < blkno; b++) {
		if (s4d90)
			printf("// rmrestorequad, i = %d\n", b);
		if (blks[b] == 0)
			continue;
		for (q = blks[b]->next; q != 0; q = q->next) {
			if (q->op == 59)
				break;
			if (q->op != 41)
				continue;
			if (s4d90)
				printf("// rmrestorequad, found RESTORE\n");
			reg = q->a.w4;
			e = blks[b]->a.w4;
			if (e < 0)
				continue;
			if (s4d90)
				printf("// rmrestorequad, pred = %d\n", e);
			s = edges[e].y;
			ok = 1;
			for (c = e; c >= 0; c = edges[c].x, s = edges[c].y) {
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
void propagate(void)
{
	int b, r, c, i, e __attribute__((unused));
	struct quad *q;
	struct qa tmp;
	struct qa *t, *pb;

	for (b = 0; b < blkno; b++) {
		if (blks[b] == 0 || blks[b]->next->op != 28)
			continue;
		e = blks[b]->next->a.w4;
		if (s4d90 > 1)
			printf("// propagate : block %d\n", b);
		for (r = 1; r <= 11; r++) {
			for (i = 0; i < blkno; i++)
				a10a[i] &= ~1;
			c = r_used(b, r);
			if (c < 0)
				continue;
			if (s4d90 > 1)
				printf("// propagate : register %d used in block %d\n", r, c);
			if (!(a10a[b] & (1 << r))) {
				tmp.b2 = 1;
				tmp.w4 = r;
				tmp.w0 = 4;
				t = &rquad(c, r)->b;
				insertquad(40, &tmp, t, (struct qa *)0, blks[b]->next);
				markreg(b, r);
			} else {
				t = &rquad(c, r)->b;
				pb = &rquad(b, r)->b;
				if (!eqopnd(pb, t))
					continue;
				q = rquad(b, r)->next;
				if (s4d90 > 1) {
					printf("// propagate : check quad -");
					prquad(q);
				}
				if (q->op == 41 && q->a.w4 == r)
					delquad(q);
			}
		}
	}
}

/* the register r is used in the block b or in a block on the path to it: the block, else -1 (name from the debug output) */
int r_used(int b, int r)
{
	int e, s, c, t;

	if (s4d90 > 1)
		printf("// r_used(%d, %d)\n", b, r);
	a10a[b] |= 1;
	e = blks[b]->a.w4;
	if (e < 0)
		return -1;
	s = edges[e].y;
	for (c = e; c >= 0; c = edges[c].x, s = edges[c].y)
		if (a10a[s] & (1 << r))
			return s;
	s = edges[e].y;
	for (c = e; c >= 0; c = edges[c].x, s = edges[c].y) {
		if (!(a10a[s] & 1)) {
			t = r_used(s, r);
			if (t >= 0)
				return t;
		}
	}
	return -1;
}

/* the last quad of the block blk that loads the register reg (name from the debug output) */
struct quad *rquad(int blk, int reg)
{
	struct quad *q, *res;

	if (s4d90 > 1)
		printf("// rquad(%d, %d)\n", blk, reg);
	res = 0;
	if (blks[blk] == 0)
		cerror("rquad called with bad block");
	for (q = blks[blk]->next; q != 0; q = q->next) {
		if (q->op == 59)
			break;
		if (q->op == 40 && q->a.w4 == reg)
			res = q;
	}
	return res;
}

/* the block with the end of the extended basic block of b; its number in *px (name guessed) */
struct quad *findend(int b, int *px)
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
			if (q->op == 59)
				break;
			if (q->op == 28) {
				*px = c;
				return q;
			}
		}
		c = edges[e].y;
		e = blks[c]->a.w4;
	}
	*px = 0;
	return 0;
}

/* the last quad of the extended basic block of b (name from the debug output) */
struct quad *ebbtail(int b)
{
	struct quad *q, *last;

	last = blks[b];
	for (q = blks[b]->next; q != 0; q = q->next) {
		if (q->op == 17 || (q->op == 59 && q->next->op == 28) || q->op == 26)
			return last;
		last = q;
	}
	cerror("error in ebbtail");
}

/* the node number of the quad q: the nodes of equal values share a number (name guessed) */
int nodeno(struct quad *q)
{
	int i, j = 0;	/* host: = 0 (the original: what the frame holds) */
	struct qa *c, *c2;

	c = &q->c;
	for (i = 0; i < a116; i++)
		if (a11a[i].q == q) {
			j = i;
			break;
		}
	if (i == a116)
		return -1;
	if (a11a[j].x0 > 0)
		return a11a[j].x0;
	for (i = 0; i < a116; i++) {
		c2 = &a11a[i].q->c;
		if (a11a[i].x0 > 0 && c->b2 == c2->b2 && c->w4 == c2->w4 && i != j && canshare(j, i)) {
			a11a[j].x0 = a11a[i].x0;
			break;
		}
	}
	if (a11a[j].x0 <= 0) {
		a11a[j].x0 = a10e;
		a10e++;
	}
	return a11a[j].x0;
}

/* the quads p and q are in the same extended basic block (name from the message) */
int insameebbo(struct quad *p, struct quad *q)
{
	int i, ip = 0, jq = 0, e1, e2;	/* host: = 0 (the original: what the frame holds) */

	for (i = 0; i < a116; i++)
		if (a11a[i].q == p) {
			ip = i;
			break;
		}
	if (i == a116)
		cerror("insameebbo can't find quad");
	for (i = 0; i < a116; i++)
		if (a11a[i].q == q) {
			jq = i;
			break;
		}
	if (i == a116)
		cerror("insameebbo can't find quad");
	findend(a11a[ip].blk, &e1);
	findend(a11a[jq].blk, &e2);
	if (e1 == e2)
		return 1;
	return 0;
}

/* the nodes a and b are not alive at the same time and can share a register (name guessed) */
int canshare(int a, int b)
{
	struct quad *p, *q;
	int ka, kb, k;

	p = a11a[a].q;
	q = a11a[b].q;
	ka = *srchdef(p, 2);
	kb = *srchdef(q, 2);
	if (tstlbit((blks[a11a[a].blk])->rec->f18, kb) && !notlive(q, (blks[a11a[a].blk])->rec->f4))
		return 1;
	if (tstlbit((blks[a11a[b].blk])->rec->f18, ka) && !notlive(p, (blks[a11a[b].blk])->rec->f4))
		return 1;
	for (k = 0; k < blkno; k++) {
		if (blks[k] != 0 && !notlive(p, (blks[k])->rec->f4) && !notlive(q, (blks[k])->rec->f4))
			return 1;
	}
	return 0;
}

/* the degree of the node n is decremented (name guessed) */
void decdeg(int n)
{
	struct gn *p;

	for (p = a11e; p != 0; p = p->link) {
		if (n == p->row) {
			--p->cnt;
			return;
		}
	}
	cerror("register map missing nodes n  = %d", n);
}

/* the graph is printed (name from the debug output) */
void prcgraph(void)
{
	struct gn *p;
	struct gedge *e;
	int i, j;

	for (i = 0; i < a116; i++) {
		printf("// node number = %d, ", a11a[i].x0);
		propnd(&a11a[i].q->c);
		printf("quadno = %X,", (unsigned)(uintptr_t)(a11a[i].q));
		printf("\n");
	}
	for (p = a11e; p != 0; p = p->link) {
		printf("// node number = %d, color = %d, degree = %d, weight = %d\n", p->row, p->x12, p->cnt, p->cost);
		for (e = p->edges; e != 0; e = e->next)
			printf("//     edge = %d\n", e->k);
	}
	printf("\n// itfs :\n");
	for (i = 0; i < a10e; i++) {
		printf("// ");
		for (j = 0; j < a10e; j++) {
			if (testbit(a110, i, j, a118))
				putc('1', stdout);
			else
				putc('0', stdout);
		}
		putc('\n', stdout);
	}
}

/* the loop that has the block n as its header (name from the debug output) */
struct loop *getheader(int n)
{
	struct loop *lp;
	struct bl *b;

	if (s4d90 > 2)
		printf("// getheader n = %d\n", n);
	for (lp = loops; lp != 0; lp = lp->link) {
		for (b = lp->bls; b != 0; b = b->next) {
			if (s4d90 > 2)
				printf("// getheader : lblkno = %d\n", b->blk);
			if (b->blk == n)
				return lp;
		}
	}
	cerror("getheader: can't find header");
}

/* the register of the quad r is used first, before the quad r3 or the end of the block (name from the debug output) */
int usefirst(struct gnn *g, struct quad *r, struct quad *r3)
{
	struct quad *c, *end;

	if (s4d90) {
		printf("// usefirst: regquad = ");
		prquad(r);
	}
	end = g->q;
	for (c = r3->next; c != 0; c = c->next) {
		if (c == end || c->op == 28 || c->op == 17)
			return 0;
		if (c->op == 40)
			continue;
		if (eqopnd(&r->b, &c->a))
			return 1;
		if (eqopnd(&r->b, &c->b))
			return 1;
	}
	return 0;
}

/* the operands of the quad s are used again behind it in the block (name guessed) */
int usedlater(int n, int u, struct quad *s)
{
	(void)u;
	struct quad *c;

	for (c = s; c != 0; c = c->next) {
		if (c->op == 28)
			return 0;
		if (c->op == 27 && c->b.w4 == n)
			return 0;
		if (c->op != 40 && c->op != 41) {
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
int inreach(struct quad *q)
{
	int i;

	for (i = 0; i < ndefs; i++)
		if (defs[i] == q)
			return 1;
	if (s4d90 > 3) {
		printf("// inreach returning FALSE for quad -");
		prquad(q);
	}
	return 0;
}
