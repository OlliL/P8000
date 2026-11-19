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
extern int *dfn, *label, *vertex, *ancest, *child, *size, *parent, *dom, *idfn;
extern int maxblk, s70e6;
extern void tabfree();
extern int ndfs;
struct dnode { struct dnode *next; struct dnode *down; int dblkno; };	/* the dominator tree: 10 bytes (guessed) */
struct bl { struct bl *next; int blk; };		/* a list of blocks */
struct loop { struct bl *bls; int hdr; struct loop *link; struct bl *exits; int tail; int cnt; unsigned b15:1, b14:1, b13:1, b12:1, b11:1; };	/* a loop: the blocks, the header block, the next loop (6 bytes at 6: guessed) */
extern struct dnode **darr;
extern struct loop *loops;
extern struct dnode *domroot;
extern struct bl *pheaderlist, *looplist, *freebl;
extern int nbl, nbl2, tailblk, nloops, hdrblk;
extern int *stk;
extern void pushblk();
extern int s4d92, s0b5e, tabavail();
extern void fn5_5298();
extern int *bucket;
extern struct edge *bent;
extern int nbent;
extern char *taballoc();
extern void prlist();
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

/* the immediate dominators of the blocks (Lengauer and Tarjan); dom: the result, idfn: the inverse of the numbering */
void
dominators()
{
	int i, u, v, w, e, n;

	n = maxblk + 1;
	dom = (int *)taballoc(n + n, 4);
	vertex = (int *)taballoc(n + n, 4);
	idfn = (int *)taballoc(n + n, 4);
	parent = (int *)taballoc(n + n, 4);
	ancest = (int *)taballoc(n + n, 4);
	label = (int *)taballoc(n + n, 4);
	dfn = (int *)taballoc(n + n, 4);
	child = (int *)taballoc(n + n, 4);
	size = (int *)taballoc(n + n, 4);
	bucket = (int *)taballoc(n + n, 4);
	nbent = 0;
	bent = (struct edge *)taballoc(0, 4);
	if (s4d82 > 2)
		printf("dominators: finished storage allocation\n");
	dom[0] = -1;
	for (v = 0; v < n; v++) {
		dfn[v] = 0;
		vertex[v] = -1;
		bucket[v] = -1;
		dom[v] = -1;
	}
	vertex[n] = -1;
	if (s4d82 > 2) {
		printf("dominators: finished initializing arrays\n");
		printf("\nstartblock = %d\n", s70e6);
	}
	ndfs = 0;
	dfs(s70e6 + 1);
	if (s4d82 > 1) {
		printf("dominators: finished DFST numbering\n");
		printf("\tblock\tdfn\tidfn\tsemi\tparent\tancestor\tlabel\n");
		for (i = 0; i < blkno; i++)
			printf("\t%d\t%d\t%d\t%d\t%d\t%d\t\t%d\n", i, vertex[i], idfn[i], dfn[i], parent[i], ancest[i], label[i]);
	}
	size[0] = 0;
	dfn[0] = 0;
	label[0] = 0;
	for (i = ndfs; i >= 2; i--) {
		w = vertex[i];
		for (e = blks[w - 1]->a.w4; e >= 0; e = edges[e].next) {
			v = edges[e].blk + 1;
			u = eval(v);
			if (dfn[u] < dfn[w])
				dfn[w] = dfn[u];
		}
		addbucket(vertex[dfn[w]], w);
		linkp(parent[w], w);
		e = bucket[parent[w]];
		if (s4d82 > 2)
			printf("dominators: finished step 2\n");
		while (e >= 0) {
			v = bent[e].blk;
			delbucket(parent[w], v);
			u = eval(v);
			dom[v] = dfn[u] < dfn[v] ? u : parent[w];
			e = bent[e].next;
		}
	}
	if (s4d82 > 2)
		printf("dominators: finished step 3\n");
	for (i = 2; i < n; i++) {
		idfn[i] = -1;
		w = vertex[i];
		if (w >= 0)
			if (dom[w] != vertex[dfn[w]])
				dom[w] = dom[dom[w]];
	}
	idfn[1] = -1;
	idfn[0] = -1;
	dom[n] = -1;
	for (i = 0; i < n - 1; i++) {
		dom[i] = dom[i + 1] == -1 ? -1 : dom[i + 1] - 1;
		vertex[i] = vertex[i + 1] == -1 ? -1 : vertex[i + 1] - 1;
		if (vertex[i] >= 0)
			idfn[vertex[i]] = i;
	}
	tabfree(4, (char *)parent);
	if (s4d82) {
		printf("\n\tblock\tdominator\tdfn\tidfn\n");
		for (i = 0; i < blkno; i++)
			printf("\t%d\t%d\t\t%d\t%d\n", i, dom[i], vertex[i], idfn[i]);
		printf("\n\n");
	}
}

/* the children of the node q in the dominator tree are linked (debug output with s4d82) */
void
linkdom(q)
	struct dnode *q;
{
	q->down = darr[q->dblkno];
	q = q->down;
	if (s4d82 > 1)
		printf("linkdom: q->dblkno=%d\n", q->dblkno);
	for (; q != 0; q = q->next)
		if (darr[q->dblkno] != 0)
			linkdom(q);
}

/* the list of blocks (debug) */
void
prlist(q)
	struct bl *q;
{
	struct bl *p;

	p = q;
	printf("list:");
	for (; p != 0; p = p->next)
		printf(" %d", p->blk);
	printf("\n");
}

/* the loops (debug) */
void
prloops()
{
	struct loop *q;

	for (q = loops; q != 0; q = q->link) {
		printf("\nheader blk = %d\nloop ", q->hdr);
		prlist(q->bls);
	}
}

/* the dominator tree: the lists of the children of every block in darr, the root domroot (name guessed) */
void
mkdomtree()
{
	int i, d;
	struct dnode *p;

	darr = (struct dnode **)taballoc(blkno << 2, 4);
	for (i = 0; i < blkno; i++)
		darr[i] = 0;
	for (i = 1; i < blkno; i++) {
		d = dom[i];
		if (d == -1)
			continue;
		if (s4d82 > 3)
			printf("block=%d, dom block=%d\n", i, d);
		if (darr[d] != 0) {
			if (s4d82 > 3)
				printf("\t\tnext , i=%d, di=%d\n", i, d);
			p = darr[d];
			darr[d] = (struct dnode *)taballoc(12, 4);
			darr[d]->next = p;
			p = darr[d];
		} else {
			if (s4d82 > 3)
				printf("\t\tfirst, i=%d, di=%d\n", i, d);
			p = darr[d] = (struct dnode *)taballoc(12, 4);
			p->next = 0;
		}
		p->down = 0;
		p->dblkno = i;
	}
	domroot = (struct dnode *)taballoc(12, 4);
	domroot->next = 0;
	domroot->dblkno = s70e6;
	if (s4d82 > 3) {
		for (i = 0; i < blkno; i++)
			if (darr[i] != 0) {
				p = darr[i];
				printf("\nlinkd[%d]:\n", i);
				for (; p != 0; p = p->next)
					printf(" %d", p->dblkno);
			}
		putchar('\n');
	}
	if (domroot != 0)
		linkdom(domroot);
}

/* the block blk is appended to the list *lp of blocks */
void
addblk(lp, blk)
	struct bl **lp;
	int blk;
{
	struct bl *p, *q;

	if (s4d82 > 1) {
		if (lp == &pheaderlist)
			printf("addblk(pheaderlist,%d)\n", blk);
		else if (lp == &looplist)
			printf("addblk(looplist,%d)\n", blk);
	}
	for (q = p = *lp; q != 0; q = q->next)
		p = q;
	if (freebl != 0) {
		q = freebl;
		freebl = freebl->next;
	} else
		q = (struct bl *)taballoc(6, 4);
	*(*lp != 0 ? (struct bl **)p : lp) = q;
	q->next = 0;
	q->blk = blk;
	nbl++;
	if (s4d82 > 1)
		prlist(*lp);
}

/* the last block of the list *lp is taken out (it must be blk) and put on the free list */
void
deletblk(lp, blk)
	struct bl **lp;
	int blk;
{
	struct bl *q, *p;

	if (s4d82 > 1 && lp == &pheaderlist)
		printf("deletblk(pheaderlist,%d)\n", blk);
	for (q = p = *lp; q->next != 0; p = q, q = q->next)
		;
	if (q->blk != blk)
		cerror("deletblk: not deleting the last block\n", blkno);
	q = freebl;
	freebl = p->next;
	freebl->next = q;
	p->next = 0;
	if (s4d82 > 1)
		prlist(*lp);
	nbl--;
}

/* the loops with the header h: marks (bit 1: the same header twice, bit 0: an outer loop contains it); name guessed */
int
chkhdr(h)
	int h;
{
	struct loop *q;
	struct bl *p;
	int found;

	q = loops;
	found = 0;
	while (q != 0) {
		if (q->hdr == h) {
			found |= 2;
			q->b15 = 1;
			q->b12 = 0;
			if (s4d82)
				printf("common header loop, header=%d\n", h);
		}
		for (p = q->bls; p != 0; p = p->next)
			if (p->blk == h) {
				q->b13 = 1;
				q->b12 = 0;
				found |= 1;
				++q->cnt;
				if (s4d82)
					printf("outer loop found,   header=%d\n", q->hdr);
				break;
			}
		q = q->link;
	}
	return found;
}

/* the blocks of the list l that have a successor outside of it (a new list; name guessed) */
struct bl *
exitblks(l)
	struct bl *l;
{
	struct bl *p, *m, *t, *r;
	int e, b;

	r = 0;
	for (p = l; p != 0; p = p->next) {
		e = blks[p->blk]->b.w4;
		while (e >= 0) {
			b = edges[e].blk;
			e = edges[e].next;
			for (m = l; m != 0; m = m->next)
				if (b == m->blk)
					break;
			if (m != 0)
				continue;
			if (s4d82)
				printf(" %d", p->blk);
			if (r == 0) {
				if (freebl != 0) {
					r = freebl;
					freebl = freebl->next;
				} else
					r = (struct bl *)taballoc(6, 4);
				r->next = 0;
			} else {
				t = r;
				if (freebl != 0) {
					r = freebl;
					freebl = freebl->next;
				} else
					r = (struct bl *)taballoc(6, 4);
				r->next = t;
			}
			nbl2++;
			r->blk = p->blk;
			break;
		}
	}
	if (s4d82)
		printf("\n");
	return r;
}

/* a loop record with the header h is entered in the sorted list of the loops (looplist is its list of blocks) */
void
addhdrlist(h)
	int h;
{
	struct loop *p, *q, *n;
	int f, d;

	if (s4d82 > 1)
		printf("addhdrlist(%d)\n", h);
	n = (struct loop *)taballoc(20, 4);
	f = 0;
	if (s4d82 > 2) {
		printf("looplist on entry:\n");
		prlist(looplist);
	}
	d = idfn[h];
	if (loops == 0) {
		loops = n;
		p = 0;
	} else {
		f = chkhdr(h);
		q = p = loops;
		while (p != 0) {
			if (idfn[p->hdr] <= d)
				break;
			q = p;
			p = p->link;
			if (s4d82 > 2)
				printf(" %d ", q->hdr);
		}
		if (p == q)
			loops = n;
		else
			q->link = n;
	}
	n->link = p;
	n->hdr = h;
	n->tail = tailblk;
	n->bls = looplist;
	n->b12 = 1;
	n->b11 = 0;
	n->cnt = 0;
	if (f & 2) {
		n->b15 = 1;
		n->b12 = 0;
	} else
		n->b15 = 0;
	if (f & 1) {
		n->b14 = 1;
		n->b12 = 1;
	} else
		n->b14 = 0;
	n->b13 = 0;
	if (s4d82) {
		if (s4d82 > 2) {
			printf("looplist before exit computation:\n");
			prlist(looplist);
		}
		printf("tail block  = %d\n", tailblk);
		printf("\nloop header = %d, exits: ", h);
	}
	n->exits = exitblks(n->bls);
	looplist = 0;
	nloops++;
	if (s4d82 > 1)
		prlist(n->bls);
}

/* the blocks of the loop with the header hdrblk and the tail block tailblk: the flow graph is traversed backwards from the tail */
void
traversefg()
{
	int i, e, b;

	if (s4d82 > 1)
		printf("traversefg(), headerblk=%d, tailblk=%d\n", hdrblk, tailblk);
	addblk(&looplist, hdrblk);
	i = -1;
	if (hdrblk != tailblk) {
		pushblk(tailblk, &i);
		while (i >= 0) {
			if (s4d82 > 3)
				printf("traversefg(): in while: i is %d\n", i);
			b = stk[i];
			i--;
			for (e = blks[b]->a.w4; e >= 0; e = edges[e].next) {
				if (s4d82 > 4)
					printf("traversefg(): in inner while: pred = %d, its block is %d\n", e, edges[e].blk);
				pushblk(edges[e].blk, &i);
			}
		}
	}
	addhdrlist(hdrblk);
}

/* the block b is put on the stack stk (index *ip) and into the loop list when it is not in it yet (name guessed) */
void
pushblk(b, ip)
	int b;
	int *ip;
{
	int x;
	struct bl *p;

	for (p = looplist; p != 0; p = p->next)
		if (p->blk == b)
			return;
	stk[++*ip] = b;
	addblk(&looplist, b);
}

/* a loop whose tail is the block b: the blocks that jump back to a header in pheaderlist (name from the string) */
int
checkloop(b)
	int b;
{
	int pb, x, e;
	struct bl *p;
	int found;

	if (s4d82 > 1)
		printf("checkloop(%d)\n", b);
	e = blks[b]->b.w4;
	if (e < 0)
		return 0;
	found = 0;
	while (e >= 0) {
		pb = edges[e].blk;
		for (p = pheaderlist; p != 0; p = p->next)
			if (pb == p->blk) {
				hdrblk = pb;
				tailblk = b;
				traversefg();
				found = 1;
			}
		e = edges[e].next;
	}
	return found;
}

/* the loops below the node q of the dominator tree are found (name from the string) */
void
findloop(q)
	struct dnode *q;
{
	if (s4d82 > 1)
		printf("findloop(), p->dblkno=%d\n", q->dblkno);
	for (q = q->down; q != 0; q = q->next) {
		addblk(&pheaderlist, q->dblkno);
		if (q->down != 0)
			findloop(q);
		checkloop(q->dblkno);
		deletblk(&pheaderlist, q->dblkno);
	}
}

/* the loops of the flow graph: the lists are cleared, the tree is traversed from its root (name guessed) */
void
mkloops()
{
	stk = (int *)taballoc(blkno + blkno, 4);
	loops = 0;
	pheaderlist = 0;
	looplist = 0;
	freebl = 0;
	if (domroot != 0) {
		addblk(&pheaderlist, domroot->dblkno);
		findloop(domroot);
		checkloop(domroot->dblkno);
		if (s4d82)
			prloops();
	}
}

/* the table area for the dominators and the loops is made available (name guessed) */
int
domspace()
{
	int n, a, b, x, c;

	n = blkno + 1;
	a = n * 4 * 2;
	b = n * 12 + n * 20 + n * 6;
	c = n + n + n * 6 + n * 5 * 2;
	return tabavail(a + b + c, 4);
}

/* the statistics of the loop analysis (debug; name guessed) */
void
prlstats()
{
	printf("//\n//Number of basic blocks \t\t= %d\n", blkno);
	printf("//Number of basic blocks in loops \t= %d\n", nbl);
	printf("//Number of loops \t\t\t= %d\n", nloops);
	printf("//Number of loop exits \t\t\t= %d\n", nbl2);
}

/* the dominators and the loops (name guessed); 0 when there is no space */
int
domloops()
{
	if (!domspace())
		return 0;
	dominators();
	mkdomtree();
	nbl2 = 0;
	nloops = 0;
	nbl = 0;
	mkloops();
	if (s4d92)
		prlstats();
	if (s0b5e > 3) {
		fn5_5298();
		return 1;
	}
}
