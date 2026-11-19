/*
 * flow2.c - gopt: the flow graph (the basic blocks of the quads and the edges between them). Written from /lib/gopt (no source known);
 * names guessed, the callees of other modules by the address (fnXXXX).
 */
#include "gopt.h"

static char sccsid[] = "@[$]flow2.c\t1.6  03/04/83 15:55:39 Copr. 1981 87wega3.2.";

struct edge { int next; int blk; };	/* an entry of the edge lists: 4 bytes (guessed) */

extern int aflag;
extern int blkno;
extern int nedge;
extern struct quad **blks;
extern struct edge *edges;
extern int cerror();
extern char *taballoc();
extern void tabfree(), prquads(), fn98a6();
extern struct quad *qhead;
extern struct quad *delquad(), *insertquad();
extern int s4d88, maxblk;

/* the number of the basic block of the label l in the table t */
int
blkof(l, t)
	int l;
	int *t;
{
	int i;

	for (i = 0; i < blkno; i++)
		if (t[i] == l)
			return i;
	cerror("cannot find the basic block associated with label %d", l);
	return -1;
}

/* the first label quad from q on gets the mark (guessed) */
void
marklab(q)
	struct quad *q;
{
	while (q->op != 0x1c)
		q = q->next;
	q->c.b2 = 1;
	q->c.w4 = 1;
}

/* the block v is appended to the edge list q (a list of entries of edges) */
void
addedge(q, v)
	int *q;
	int v;
{
	int x;

	taballoc(4, 8);
	edges[nedge].blk = v;
	edges[nedge].next = -1;
	if (*q >= 0) {
		x = *q;
		while (edges[x].next >= 0)
			x = edges[x].next;
		q = &edges[x].next;
	}
	*q = nedge++;
}

/* the basic blocks of the quads: a quad of the op 0x3b is inserted at the start of every block, the edges between the blocks are collected in its operands a.w4 (the predecessors) and b.w4 (the successors) */
int
flow_graph()
{
	struct quad *q, *r;
	int lb, i, x14, n;
	int nb, jmp;
	int lab[512];
	struct quad *last;
	struct qa s1, s2, s3;
	int ret;

	nb = 0;
	jmp = 0;
	ret = 1;
	if (s4d88 > 1)
		prquads();
	tabfree(8, (char *)0);
	blks = (struct quad **)taballoc(0, 8);
	blkno = 0;
	for (q = qhead; q != 0; q = q->next)
		if (q->op == 0x3b)
			q = delquad(q);
	for (q = qhead; q != 0; q = q->next) {
		if (q->op != 0x1c && q->op != 6 && q->op != 0xf && nb != 0) {
			lab[blkno] = -1;
			taballoc(4, 8);
			blks[blkno] = last;
			blkno++;
			nb = 0;
			if (s4d88 > 3)
				printf("// new basic block blkno = %d\n", blkno);
		}
		switch (q->op) {
		case 0x1c:
			lab[blkno] = q->a.w4;
			taballoc(4, 8);
			blks[blkno] = last;
			blkno++;
			nb = 0;
			if (s4d88 > 3)
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
	taballoc((maxblk - blkno) * 4, 8);
	s1.w4 = -1;
	s2.w4 = -1;
	s3.b2 = 0;
	for (i = 0; i < blkno; i++) {
		s1.w0 = i;
		blks[i] = insertquad(0x3b, &s1, &s2, &s3, blks[i]);
	}
	edges = (struct edge *)taballoc(0, 8);
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
	fn98a6();
	n = ((struct edge *)taballoc(0, 8) - edges) * 4;
	if (n < 10)
		taballoc(40, 8);
	else
		taballoc(n * 4, 8);
	return ret;
}
