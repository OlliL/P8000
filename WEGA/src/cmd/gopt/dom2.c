/*
 * dom2.c - gopt: the dominators of the flow graph (depth first search; the arrays are indexed by the block number + 1). Written from
 * /lib/gopt (no source known); names guessed, the callees of other modules by the address (fnXXXX).
 */
#include "gopt.h"

static char sccsid[] = "@[$]dom2.c\t1.2  01/07/83 13:38:58 Copr. 1981 87wega3.2.";

struct edge { int next; int blk; };	/* the edge lists of flow2.c */

extern int s4d82;
extern int blkno;
extern struct quad **blks;
extern struct edge *edges;
extern int *dfn, *label, *vertex, *ancest, *child, *size, *parent;
extern int ndfs;
extern int *bucket;
extern struct edge *bent;
extern int nbent;
extern char *taballoc();
extern int cerror();

/* the depth first search from the block v */
void
dfs(v)
	int v;
{
	int e, w;

	if (s4d82 > 2)
		printf("dfs(%d)\n", v);
	dfn[v] = ndfs = ndfs + 1;
	vertex[ndfs] = label[v] = v;
	ancest[v] = 0;
	child[v] = 0;
	size[v] = 1;
	for (e = blks[v - 1]->b.w4; e >= 0; e = edges[e].next) {
		w = edges[e].blk + 1;
		if (dfn[w] == 0) {
			parent[w] = v;
			dfs(w);
		}
	}
}

/* the path of v is compressed (the ancestors that are no root) */
void
compress(v)
	int v;
{
	if (s4d82 > 2)
		printf("compress(%d)\n", v);
	if (ancest[ancest[v]] != 0) {
		compress(ancest[v]);
		if (dfn[label[ancest[v]]] < dfn[label[v]])
			label[v] = label[ancest[v]];
		ancest[v] = ancest[ancest[v]];
	}
}

/* the vertex with the smallest semidominator number on the path above v */
int
eval(v)
	int v;
{
	int t;

	if (s4d82 > 2)
		printf("eval(%d)\n", v);
	if (ancest[v] == 0)
		t = label[v];
	else {
		compress(v);
		if (dfn[label[ancest[v]]] < dfn[label[v]])
			t = label[ancest[v]];
		else
			t = label[v];
	}
	return t;
}

/* the tree of w is linked below v (balanced: child, size) */
void
linkp(v, w)
	int v, w;
{
	int s, t;

	if (s4d82 > 2)
		printf("linkp(v=%d,w=%d)\n", v, w);
	s = w;
	while (dfn[label[w]] < dfn[label[child[s]]]) {
		if (size[s] + size[child[child[s]]] >= 2 * size[child[s]]) {
			ancest[child[s]] = s;
			child[s] = child[child[s]];
		} else {
			size[child[s]] = size[s];
			s = ancest[s] = child[s];
		}
	}
	label[s] = label[w];
	size[v] = size[v] + size[w];
	if (size[v] < 2 * size[w]) {
		t = s;
		s = child[v];
		child[v] = t;
	}
	while (s != 0) {
		ancest[s] = v;
		s = child[s];
	}
}

/* the block blk is entered in the bucket i (a list of the entries bent) */
void
addbucket(i, blk)
	int i, blk;
{
	int t, prev;

	if (s4d82 > 2)
		printf("addbucket(i=%d,blk=%d)\n", i, blk);
	t = bucket[i];
	taballoc(4, 4);
	if (t == -1)
		bucket[i] = nbent;
	else {
		while (t >= 0) {
			prev = t;
			t = bent[t].next;
		}
		bent[prev].next = nbent;
	}
	bent[nbent].blk = blk;
	bent[nbent].next = -1;
	nbent++;
}

/* the block blk is taken out of the bucket i */
void
delbucket(i, blk)
	int i, blk;
{
	int t, prev;

	if (s4d82 > 2)
		printf("delbucket(i=%d,blk=%d)\n", i, blk);
	t = prev = bucket[i];
	while (t >= 0) {
		if (bent[t].blk == blk)
			break;
		prev = t;
		t = bent[t].next;
	}
	if (t < 0)
		cerror("block blk not found in delbucket = %d\n", blk);
	if (prev == t)
		bucket[i] = bent[t].next;
	else
		bent[prev].next = bent[t].next;
}
