/*
 * flow2.c - gopt: the flow graph (the basic blocks of the quads and the edges between them). Written from /lib/gopt (no source known);
 * names guessed, the callees of other modules by the address (fnXXXX).
 */
static char sccsid[] __attribute__((unused)) = "@[$]flow2.c\t1.6  03/04/83 15:55:39 Copr. 1981 87wega3.2.";
#include "gopt.h"


extern int aflag;
extern int blkno;
extern int nedge;
extern struct quad **blks;
extern struct edge *edges;
extern struct quad *qhead;
extern int s4d88, maxblk, s70e6;
extern int s4d74, s4d76, s4d78;

/* forward declarations (the functions are in the order of the original) */
int blkof();
void marklab();
void addedge();
int jmplabel();

/* the basic blocks of the quads: a quad of the op 0x3b is inserted at the start of every block, the edges between the blocks are collected in its operands a.w4 (the predecessors) and b.w4 (the successors) */
int flow_graph(void)
{
	struct quad *q, *r;
	int lb, i,  n;
	int nb, jmp;
	int lab[512];
	struct quad *last = 0;	/* host: = 0 (the original: what the frame holds) */
	struct qa s1, s2, s3;
	int ret;

	nb = 0;
	jmp = 0;
	ret = 1;
	if (s4d88 > 1)
		prquads();
	tabfree(8, (char *)0);
	blks = (struct quad **)tabzalloc(0, 0, 8);
	blkno = 0;
	for (q = qhead; q != 0; q = q->next)
		if (q->op == 0x3b)
			q = delquad(q);
	for (q = qhead; q != 0; q = q->next) {
		if (q->op != 0x1c && q->op != 6 && q->op != 0xf && nb != 0) {
			lab[blkno] = -1;
			tabzalloc((int)sizeof(struct quad *), 4, 8);
			blks[blkno] = last;
			blkno++;
			nb = 0;
			if (s4d88 > 4)
				printf("// new basic block blkno = %d\n", blkno);
		}
		switch (q->op) {
		case 0x1c:
			lab[blkno] = q->a.w4;
			tabzalloc((int)sizeof(struct quad *), 4, 8);
			blks[blkno] = last;
			blkno++;
			nb = 0;
			if (s4d88 > 4)
				printf("// label encountered blkno = %d\n", blkno);
			break;
		case 0x11:
		case 0x1a:
		case 0x1b:
		case 0x2e:
			nb = 1;
			break;
		}
		last = q;
	}
	if (s4d88 > 2)
		printf("// number of basic blocks (blkno) %d\n", blkno);
	if (blkno >= 512)
		cerror("table overflow in flow_graph");
	if (blkno < 10)
		maxblk = blkno + 10;
	else
		maxblk = blkno * 2 / 3 + blkno;
	tabzalloc((maxblk - blkno) * (int)sizeof(struct quad *), (maxblk - blkno) * 4, 8);
	s1.w4 = -1;
	s2.w4 = -1;
#ifdef SHIPPED
	/* the kinds of the operands are not set: the original has what its frame holds there; a kind 4 (a name) of the shipped build
	   makes eqopnd() take the edge list in a.w4 for a name (usefirst(), reg2.c) */
	s1.b2 = 0;
	s2.b2 = 0;
#endif
	s3.b2 = 0;
	for (i = 0; i < blkno; i++) {
		s1.w0 = i;
		blks[i] = insertquad(0x3b, &s1, &s2, &s3, blks[i]);
	}
	edges = (struct edge *)tabzalloc(0, 0, 8);
	nedge = 0;
	for (i = 0; i < blkno; i++) {
		for (q = blks[i]->next; q != 0 && q->op != 0x3b; q = q->next) {
			switch (q->op) {
			case 0x1c:
				if (jmp == 0 && i != 0) {
					addedge(&blks[i]->a.w4, i - 1);
					addedge(&blks[i - 1]->b.w4, i);
					continue;
				}
				jmp = 0;
				continue;
			case 0x1b:
				lb = blkof(q->b.w4, lab);
				marklab(blks[lb]);
				addedge(&blks[lb]->a.w4, i);
				addedge(&blks[i]->b.w4, lb);
				if (blks[i + 1]->next->op != 0x1c) {
					addedge(&blks[i + 1]->a.w4, i);
					addedge(&blks[i]->b.w4, i + 1);
				}
				continue;
			case 0x1a:
				lb = blkof(q->a.w4, lab);
				marklab(blks[lb]);
				addedge(&blks[lb]->a.w4, i);
				addedge(&blks[i]->b.w4, lb);
			/* fall through */
			case 0xb:
				jmp = 1;
				continue;
			case 0xa:
				lb = blkof(q->a.w4, lab);
				marklab(blks[lb]);
				addedge(&blks[lb]->a.w4, i);
				addedge(&blks[i]->b.w4, lb);
				for (r = blks[lb]; ; r = r->next)
					if (r->op == 0x1c) {
						r->c.w0 = 1;
						break;
					}
				continue;
			case 6:
			case 0xf:
				continue;
			default:
				jmp = 0;
			}
		}
	}
	optflow();
	n = ((struct edge *)tabzalloc(0, 0, 8) - edges) * 4;
	if (n < 10)
		tabzalloc(10 * (int)sizeof(struct edge), 40, 8);
	else
		tabzalloc(n * (int)sizeof(struct edge), n * 4, 8)	/* n * 4 bytes of the original: n entries */;
	return ret;
}

/* the optimizations of the flow graph (redundant labels, jumps, dead blocks) with the debug output; name guessed */
void optflow(void)
{
	int tab[512];
	int i;
	struct quad *q;

	if (s4d88) {
		if (s4d88 > 2)
			prquads();
		printf("// flow graph before optimizations:\n");
		prgraph();
		prstats();
	}
	for (i = 0; i < blkno; i++)
		if (blks[i] != 0 && blks[i]->next->op == 0x1c)
			tab[i] = blks[i]->next->a.w4;
		else
			tab[i] = -1;
	rmlabels(tab);
	if (s4d88 > 4) {
		printf("// after redundant labels are removed:\n");
		prquads();
	}
	rmjumps(tab);
	if (s4d88 > 4) {
		printf("// after jumps are removed:\n");
		prquads();
	}
	for (q = qhead; q->op != 0x3b; q = q->next)
		;
	s70e6 = q->a.w0;
	rm_deadblks();
	if (s4d88) {
		if (s4d88 > 2) {
			printf("// after dead blocks are removed: \n");
			prquads();
		}
		printf("// flow graph after optimizations:\n");
		prgraph();
		prstats();
	}
}

/* the number of the basic block of the label l in the table t */
int blkof(int l, int *t)
{
	int i;

	for (i = 0; i < blkno; i++)
		if (t[i] == l)
			return i;
	cerror("cannot find the basic block associated with label %d", l);
	return -1;
}

/* the first label quad from q on gets the mark (guessed) */
void marklab(struct quad *q)
{
	while (q->op != 0x1c)
		q = q->next;
	q->c.b2 = 1;
	q->c.w4 = 1;
}

/* the block v is appended to the edge list q (a list of entries of edges) */
void addedge(int *q, int v)
{
	int x;

	tabzalloc((int)sizeof(struct edge), 4, 8);
	edges[nedge].blk = v;
	edges[nedge].next = -1;
	if (*q < 0)
		*q = nedge++;
	else {
		x = *q;
		while (edges[x].next >= 0)
			x = edges[x].next;
		edges[x].next = nedge++;
	}
}

/* the block v is taken out of the edge list q (name guessed) */
void deledge(int *q, int v)
{
	int x, prev;

	if (0)
		v = 0;
	if (*q < 0)
		return;
	x = *q;
	if (edges[x].blk == v)
		*q = edges[x].next;
	else {
		goto in;	/* the first block is known to differ: into the loop */
		while (v != edges[x].blk) {
		in:
			prev = x;
			x = edges[x].next;
			if (x < 0)
				break;
		}
		if (x >= 0)
			edges[prev].next = edges[x].next;
	}
}

/* redundant labels: a label that nothing jumps to is deleted, blocks are joined; tab: the label of every block (name guessed) */
void rmlabels(int *tab)
{
	int i, j, e, pi, pj, x, y;
	struct quad *p, *nx;

	for (i = 0; i < blkno - 1; i++) {
		if (blks[i] == 0)
			continue;
		if (blks[i]->next->op != 0x1c)
			continue;
		if (blks[i]->next->c.w4 == 0) {
			x = blks[i]->a.w4;
			if (x < 0)
				continue;
			x = edges[x].blk;
			y = blks[i]->a.w0;
			delquad(blks[i]->next);
			for (p = blks[x]->next; p->op != 0x3b; p = p->next)
				if (p->op == 0x1b || p->op == 0xb)
					x = -1;
			if (x >= 0) {
				delquad(blks[i]);
				joinblk(y, x, 2);
				blks[i] = 0;
			}
			s4d74++;
			continue;
		}
		if (!blkempty(blks[i]))
			continue;
		for (j = i + 1; j < blkno; j++)
			if (blks[j] != 0)
				break;
		if (j >= blkno)
			return;
		if (blks[i]->next->c.w0 == 1) {
			if (blkempty(blks[j]))
				continue;
			if (blks[j]->next->c.w0 == 1)
				continue;
			e = blks[j]->a.w4;
			pi = i;
			pj = j;
			i--;
		} else {
			e = blks[i]->a.w4;
			pi = j;
			pj = i;
		}
		blks[pi]->next->c.w4 |= blks[pj]->next->c.w4;
		for (; e >= 0; e = edges[e].next)
			for (p = blks[edges[e].blk]->next; p && p->op != 0x3b; p = p->next) {
				switch (p->op) {
				case 0x1a:
					p->a.w4 = tab[pi];
					break;
				case 0x1b:
					if (p->next != blks[pj])
						p->b.w4 = tab[pi];
					break;
				}
			}
		if (blkempty(blks[pj])) {
			delrun(blks[pj]);
		} else {
			for (p = blks[pj]; p != 0 && (p->op == 0x3b || p->op == 0x1c); p = nx) {
				nx = p->next;
				delquad(p);
			}
		}
		joinblk(pj, pi, 1);
		s4d74++;
		blks[pj] = 0;
		tab[pj] = -1;
	}
}

/* 1: the block of the header quad q contains no quad that does something (only the ops 6, 0xf and the labels); name guessed */
int blkempty(struct quad *q)
{
	for (q = q->next; q != 0; q = q->next) {
		if (q->op == 0x3b)
			return 1;
		switch (q->op) {
		case 6:
		case 0xf:
		case 0x1c:
			break;
		default:
			return 0;
		}
	}
	return 1;
}

/* blocks that only jump: the jumps to them are turned to the target, the block is removed; tab: the label of every block (name guessed) */
void rmjumps(int *tab)
{
	int i, lbl, e;
	int *pp __attribute__((unused)), *sp;
	int ok, flag, pb, lb2;
	struct quad *p;
	int k;

	for (i = 0; i < blkno - 1; i++) {
		if (blks[i] == 0)
			continue;
		lbl = jmplabel(blks[i]);
		if (lbl == 0)
			continue;
		e = blks[i]->a.w4;
		ok = 1;
		for (; e >= 0; e = edges[e].next) {
			flag = 0;
			pb = edges[e].blk;
			pp = &blks[edges[e].blk]->a.w4;
			sp = &blks[edges[e].blk]->b.w4;
			for (p = blks[edges[e].blk]->next; p->op != 0x3b; p = p->next) {
				switch (p->op) {
				case 0x1a:
					p->a.w4 = lbl;
					deledge(&blks[i]->a.w4, pb);
					deledge(sp, i);
					lb2 = blkof(lbl, tab);
					addedge(sp, lb2);
					addedge(&blks[lb2]->a.w4, pb);
					flag = 1;
					break;
				case 0x1b:
					if (p->next == blks[i] || (p->next->op == 6 && p->next->next == blks[i])) {
						flag = 0;
						break;
					}
					flag = 1;
					p->b.w4 = lbl;
					deledge(&blks[i]->a.w4, pb);
					deledge(sp, i);
					lb2 = blkof(lbl, tab);
					addedge(sp, lb2);
					addedge(&blks[lb2]->a.w4, pb);
					break;
				}
			}
			ok = ok && flag;
		}
		if (ok) {
			delrun(blks[i]);
			k = blkof(lbl, tab);
			joinblk(i, k, 3);
			s4d76++;
			blks[i] = 0;
			tab[i] = -1;
		}
	}
}

/* the blocks that nothing leads to (except the first one) are removed until there are none */
void rm_deadblks(void)
{
	int i, x, again;

	again = 1;
	while (again) {
		again = 0;
		for (i = 0; i < blkno; i++) {
			if (i == s70e6 || blks[i] == 0 || blks[i]->a.w4 >= 0)
				continue;
			if (s4d88 > 4 && blks[i] != 0)
				printf("// rm_deadblks :blk(%d,%d,%d)\n", i, blks[i]->a.w4, blks[i]->b.w4);
			for (x = blks[i]->b.w4; x >= 0; x = edges[x].next) {
				again = 1;
				deledge(&blks[edges[x].blk]->a.w4, i);
			}
			delrun(blks[i]);
			s4d78++;
			blks[i] = 0;
		}
	}
}

/* the block a is joined to the block b: its edges are appended to those of b; mode 1, 2, 3: which edges between them are dropped (name guessed) */
void joinblk(int a, int b, int mode)
{
	int k;

	for (k = blks[b]->a.w4; k >= 0 && edges[k].next >= 0; k = edges[k].next)
		;
	if (k >= 0)
		edges[k].next = blks[a]->a.w4;
	else
		blks[b]->a.w4 = blks[a]->a.w4;
	for (k = blks[b]->b.w4; k >= 0 && edges[k].next >= 0; k = edges[k].next)
		;
	if (k >= 0)
		edges[k].next = blks[a]->b.w4;
	else
		blks[b]->b.w4 = blks[a]->b.w4;
	if (mode == 2 || mode == 1)
		deledge(&blks[b]->b.w4, a);
	deledge(&blks[b]->a.w4, b);
	if (mode == 3 || mode == 1) {
		deledge(&blks[b]->b.w4, b);
		deledge(&blks[b]->a.w4, a);
	}
	for (k = 0; k < nedge; k++)
		if (edges[k].blk == a)
			edges[k].blk = b;
	for (k = blks[b]->b.w4; k >= 0; k = edges[k].next)
		if (edges[k].blk == b)
			addedge(&blks[b]->a.w4, b);
}

/* the label of the jump (op 0x1a) the block of the header quad q consists of; 0 when it is something else (name guessed) */
int jmplabel(struct quad *q)
{
	for (q = q->next; q != 0; q = q->next) {
		if (q->op == 0x3b)
			return 0;
		switch (q->op) {
		case 6:
		case 0xf:
		case 0x1c:
			continue;
		case 0x1a:
			return q->a.w4;
		default:
			return 0;
		}
	}
	return 0;
}

/* the flow graph (debug): the successors and the predecessors of every block */
void prgraph(void)
{
	register int i, e;

	for (i = 0; i < blkno; i++) {
		if (blks[i] == 0)
			continue;
		printf("// block %d\n// \tsuccessors: ", i);
		for (e = blks[i]->b.w4; e >= 0; e = edges[e].next)
			printf("%d ", edges[e].blk);
		printf("\n\tpredecessors: ");
		for (e = blks[i]->a.w4; e >= 0; e = edges[e].next)
			printf("%d ", edges[e].blk);
		printf("\n");
	}
}
