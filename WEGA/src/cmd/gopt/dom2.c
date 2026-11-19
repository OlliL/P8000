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
