/*
 * lpinv2.c - gopt: the loop invariant code motion (segment 5). Written from /lib/gopt (no source known); names guessed, the
 * callees by the address (fnXXXX).
 */
#include "gopt.h"

static char sccsid[] = "@[$]lpinv2.c\t1.10  03/04/83 15:58:16 Copr. 1981 87wega3.2.";

struct bl { struct bl *next; int blk; };	/* a list of blocks */
struct loop { struct bl *bls; int hdr; struct loop *link; struct bl *exits; int tail; int cnt; unsigned b15:1, b14:1, b13:1, b12:1, b11:1; };
struct aln { int x0; struct aln *next; struct aln *link; };
struct hrec { unsigned *f0; unsigned *f4; unsigned *f8; int w12; };	/* the record of a block that the header quad points to at 6 (guessed) */
#define QHREC(q)	(*(struct hrec **)((char *)(q) + 6))
extern struct quad **blks;
extern int *idfn;
struct bt { int x0; int x1; int x[2]; unsigned f15:1, f14:1, bopt:1, rest:13; };
extern struct bt *betatbl;
extern void prquad();
extern int blkno, s4d86;

/* the loop number of every block is cleared (name from the debug output) */
void
clearblocks()
{
	int i;
	struct hrec *p;

	if (s4d86 > 3)
		printf("clearblocks()\n");
	for (i = 0; i < blkno; i++)
		if (idfn[i] >= 0) {
			p = QHREC(blks[i]);
			p->w12 = 0;
		}
}

/* the blocks of the loop lp get the number n (name from the debug output) */
void
marklpblocks(lp, n)
struct loop *lp;
int n;
{
	int k;
	struct hrec *h;
	struct bl *b;

	if (s4d86 > 3)
		printf("marklpblocks(hdr=%d)\n", lp->hdr);
	for (b = lp->bls; b != 0; b = b->next) {
		h = QHREC(blks[b->blk]);
		h->w12 = n;
	}
}

/* a store of the quad q (name guessed) */
struct quad *
qmkstore(q)
struct quad *q;
{
	static struct quad tq;

	if (q->op == 52 && q->a.b2 == 8) {
		tq.op = 47;
		tq.c = q->a;
		return &tq;
	}
	return q;
}

struct dl { int x0; struct quad *q; long l6; long l10; int x14[4]; unsigned f15:1, f14:1, f13:1, f12:1, rest:12; struct dl *next; };	/* a node of the list of the definitions (28 bytes, guessed) */
extern struct dl *a0e0;
extern int sameopnd();

/* the node of the list a0e0 with the quad of the same operand as q; *flag is set when an operand of a node differs (name from the debug output) */
struct dl *
srchdflist(q, flag)
struct quad *q;
int *flag;
{
	struct dl *p;
	struct quad *x;
	int v10, v12, v14, v16;

	if (s4d86 > 3)
		printf("srchdflist()\n");
	q = qmkstore(q);
	p = a0e0;
	v12 = (char)(q->c.b2 == 8);
	v10 = v12 && q->op == 47 ? 1 : 0;
	v14 = v12 && (q->op == 24 || q->op == 47 || q->op == 30);
	v16 = v12 && (q->op != 24 && q->op != 47 && q->op != 30);
	while (p != 0) {
		x = qmkstore(p->q);
		if (sameopnd(x, q, 1, 1)) {
			if (v14 && !(x->op == 47 || x->op == 24 || x->op == 30))
				goto next;
			if (v16 && (x->op == 47 || x->op == 24 || x->op != 30))
				goto next;
			if (x->c.b2 == q->c.b2 && x->c.w4 == q->c.w4)
				return p;
			*flag = 1;
		}
		if (v10 && x->c.b2 == 8 && x->op == 47 && betatbl[q->c.w4].x1 == betatbl[x->c.w4].x1) {
			if (x->c.b2 == q->c.b2 && x->c.w4 == q->c.w4)
				return p;
			*flag = 1;
		}
next:		p = p->next;
	}
	return 0;
}

extern struct sym *symtab;

/* the flags of the operand ot (1 c, 2 a, 3 b) of q are cleared (name from the debug output) */
void
clrsymflag(q, ot)
struct quad *q;
int ot;
{
	int i, t;

	if (s4d86 > 2) {
		printf("clrsymflag(q=%x,opndtype=%s)\n", q, ot == 2 ? "OP1" : "OP2");
		prquad(q);
	}
	if (ot == 2) {
		i = q->a.w4;
		t = q->a.b2;
	} else if (ot == 3) {
		i = q->b.w4;
		t = q->b.b2;
	} else if (ot == 1) {
		i = q->c.w4;
		t = q->c.b2;
	}
	if (t == 4)
		symtab[i].sflags &= ~2;
	else if (t == 6)
		*(int *)((char *)betatbl + (i * 10 + 8)) &= ~4096;
	else if (t == 8) {
		*(int *)((char *)betatbl + (i * 10 + 8)) &= ~4096;
		if ((ot == 2 && q->op == 24) || (ot == 1 && q->op == 47))
			symtab[betatbl[i].x1].sflags &= ~2;
	}
}

extern struct quad *tmpsearch();
extern void addlpdef(), setopt();
extern int isglobal();
extern void clrsymflag(), cerror();

/* the flags of the operands of q are cleared; a temporary is followed to its definition (name from the debug output) */
void
clropt(q, firstq)
struct quad *q, *firstq;
{
	struct quad *p;

	if (s4d86 > 2) {
		printf("clropt(firstq = %X), quad (%X) = \n", firstq, q);
		prquad(q);
	}
	if (q->a.b2 == 3) {
		p = tmpsearch(firstq, q->a.w4);
		clropt(p, firstq);
		if (q->b.b2 == 0)
			return;
	} else if (q->a.b2 == 4 || q->a.b2 == 6 || q->a.b2 == 8) {
		clrsymflag(q, 2);
		if (q->b.b2 == 0)
			return;
	} else if (q->a.b2 == 1 || q->a.b2 == 2 || q->a.b2 == 7 || q->a.b2 == 5) {
		if (q->b.b2 == 0)
			return;
	} else {
		cerror("clropt(): illegal op1 opnd");
		return;
	}
	if (q->op != 7) {
		if (q->b.b2 == 3) {
			p = tmpsearch(firstq, q->b.w4);
			clropt(p, firstq);
		} else if (q->b.b2 == 4 || q->b.b2 == 6 || q->b.b2 == 8)
			clrsymflag(q, 3);
		else if (!(q->b.b2 == 1 || q->b.b2 == 2 || q->b.b2 == 7))
			cerror("clropt(): illegal op2 opnd");
	}
}

extern char ascii[];

/* the flags of the operands of q: the symbols and the betatbl entries of c (and of a of a parameter store) are marked (name from the debug output) */
void
setopt(q)
struct quad *q;
{
	if (s4d86 > 1) {
		printf("setopt()\n");
		prquad(q);
	}
	if (q->c.b2 == 4) {
		symtab[q->c.w4].sflags |= 2;
		if (s4d86 > 2)
			printf("setopt(), rslt sym=%s, lpopt=%d\n", ascii + symtab[q->c.w4].nameoff, (symtab[q->c.w4].sflags >> 1) & 1);
	} else if (q->c.b2 == 6 || q->c.b2 == 8)
		*(int *)((char *)betatbl + (q->c.w4 * 10 + 8)) |= 4096;
	if (q->c.b2 == 8 && q->op == 47) {
		symtab[betatbl[q->c.w4].x1].sflags |= 2;
		if (s4d86 > 2)
			printf("setopt(), rslt beta=%s, lpopt=%d\n", ascii + symtab[betatbl[q->c.w4].x1].nameoff, (symtab[betatbl[q->c.w4].x1].sflags >> 1) & 1);
	}
	if (q->op == 52 && q->a.b2 == 8) {
		*(int *)((char *)betatbl + (q->a.w4 * 10 + 8)) |= 4096;
		symtab[betatbl[q->a.w4].x1].sflags |= 2;
		if (s4d86 > 2)
			printf("setopt(), qparv beta=%s, lpopt=%d\n", ascii + symtab[betatbl[q->a.w4].x1].nameoff, (symtab[betatbl[q->a.w4].x1].sflags >> 1) & 1);
	}
}

extern struct dl *a0f8, *alloclpdef(), *srchdflist();
extern char *taballoc();

/* a node of the list of the definitions of the loops is made for q (name from the debug output) */
struct dl *
alloclpdef(q, n)
struct quad *q;
int n;
{
	struct dl *p, *x;

	if (a0f8 != 0) {
		p = a0f8;
		a0f8 = a0f8->next;
	} else
		p = (struct dl *)taballoc(28, 7);
	p->x0 = n;
	p->q = q;
	p->l6 = 0;
	p->l10 = 0;
	p->f15 = 1;
	p->f14 = 0;
	p->f13 = 0;
	p->f12 = 0;
	if (s4d86 > 2) {
		printf("alloclpdef(), q=%X, lpdefs=%X\n", p, a0e0);
		prquad(p->q);
		if (a0e0 != 0)
			prquad(a0e0->q);
	}
	x = a0e0;
	a0e0 = p;
	p->next = x;
}

struct pr { int x; int y; };
extern struct pr *a70aa;

/* the header quads in front of q that only lead to q are skipped (name guessed) */
struct quad *
skiphdrs(q)
struct quad *q;
{
	struct quad *x, *y;

	while (q->op == 59) {
		if (q->a.w4 < 0 || a70aa[q->a.w4].x != -1)
			break;
		y = blks[a70aa[q->a.w4].y];
		for (x = y->next; x != 0; x = x->next)
			if (x->op == 59)
				break;
		if (x != q)
			break;
		q = y;
	}
	return q;
}

extern struct quad *skiphdrs();

/* the definition q in the block is entered in the list of the definitions of the loops (name from the debug output) */
void
addlpdef(q, block)
struct quad *q;
int block;
{
	struct dl *p, *r;
	int fl;
	struct quad *x;

	if (s4d86 > 1) {
		printf("addlpdef(block=%d), loop def found:\n", block);
		prquad(q);
	}
	fl = 0;
	r = srchdflist(q, &fl);
	if (r == 0) {
		p = alloclpdef(q, block);
		x = skiphdrs(blks[block]);
		clropt(q, x);
		if (fl)
			p->f15 = 0;
		if (s4d86 > 2)
			printf("\tonedef=%d\n", p->f15);
	} else {
		if (r->q->op != 47 && q->op == 47)
			r->q = q;
		if (r->x0 == block) {
			r->f15 = 0;
			if (s4d86 > 2) {
				printf("\t\tonedef=%d, same quad:\n", r->f15);
				prquad(r->q);
			}
		} else {
			r->f15 = 0;
			if (s4d86 > 2)
				printf("\tr->onedef=%d\n", r->f15);
		}
	}
}

#define BBIT(j, n)	((*(int *)((char *)betatbl + ((j) * 10 + 8)) >> (n)) & 1)
extern int *a70da;
extern struct aln *checkalias();

/* the operand c of q is a global variable or may be changed by the alias of a global one (name from the debug output) */
int
isglobal(q)
struct quad *q;
{
	struct aln *p;

	if (q->op == 3)
		return 0;
	if ((q->op == 25 || q->op == 2) && q->a.b2 == 8 && BBIT(q->a.w4, 11))
		return 0;
	if (s4d86 > 2) {
		printf("isglobal(),  quad:\n");
		prquad(q);
	}
	if ((q->c.b2 == 4 && ((symtab[q->c.w4].sflags >> 2) & 1)) ||
	    (q->c.b2 == 8 && ((symtab[betatbl[q->c.w4].x1].sflags >> 2) & 1))) {
		if (s4d86 > 3)
			printf("isglobal(), returning TRUE\n");
		return 1;
	}
	for (p = checkalias(q, 1); p != 0; p = p->next) {
		if ((symtab[a70da[p->x0]].sflags >> 2) & 1) {
			if (s4d86 > 3)
				printf("isglobal(), returning TRUE\n");
			return 1;
		}
	}
	if (s4d86 > 3)
		printf("isglobal(), returning FALSE\n");
	return 0;
}

/* the quads of the blocks of the loop lp are entered as definitions; the definitions that are register candidates are marked (name from the debug output) */
void
findlpdefs(lp)
struct loop *lp;
{
	struct quad *q;
	struct bl *b;
	int n;
	struct dl *t;
	int callf;

	if (s4d86 > 1)
		printf("findlpdefs()\n");
	callf = 0;
	for (b = lp->bls; b != 0; b = b->next) {
		n = blks[b->blk]->a.w0;
		if (s4d86 > 2)
			printf("// findlpdefs: block = %d\n", n);
		for (q = blks[b->blk]->next; q != 0; q = q->next) {
			if (q->op == 59)
				break;
			if (q->f7 || q->f4 || (q->op == 52 && q->a.b2 == 8)) {
				addlpdef(q, n);
				if (q->op == 7)
					callf = 1;
			}
			q->f3 = 0;
			q->f2 = 0;
			q->f1 = 0;
		}
	}
	if (a0e0 == 0)
		return;
	t = a0e0;
	while (t != 0) {
		if (s4d86 > 3) {
			printf("findlpdefs(), t=%X, t->dfnxt=%X\n", t, t->next);
			prquad(t->q);
			if (t->next != 0)
				prquad(t->next->q);
		}
		setopt(t->q);
		if (t->f15) {
			if (t->q->op == 52 && t->q->a.b2 == 8)
				t->f15 = 0;
			else if (callf && isglobal(t->q))
				t->f15 = 0;
			else {
				t->q->f1 = 1;
				if (t->q->c.b2 == 4 && isglobal(t->q)) {
					t->q->c.fl = 1;
					if (s4d86 > 3) {
						printf("reg_candidate set for:\n");
						prquad(t->q);
					}
				}
			}
		}
		t->f14 = 0;
		t->f13 = 0;
		t->f12 = 0;
		t = t->next;
	}
}

extern int s7158;
extern unsigned *a0e8;
extern void clrvec(), orvec();

/* the variables that are live on the exits of the loop lp are collected in a0e8 (name from the debug output) */
void
liveonexit(lp)
struct loop *lp;
{
	struct bl *e;
	struct quad *h;
	struct hrec *y;
	int i, s;

	if (s4d86 > 2) {
		printf("liveonexit(), exit=%x(x)\n", lp->exits);
		if (lp->exits != 0)
			printf("lblkno=%d\n", lp->exits->blk);
	}
	if (a0e8 == 0)
		a0e8 = (unsigned *)taballoc(s7158 * 2, 2);
	clrvec(a0e8, s7158);
	e = lp->exits;
	if (s4d86 > 3)
		printf("liveonexit(), returned from nullvec(), entering while loop\n");
	while (e != 0) {
		i = blks[e->blk]->b.w4;
		if (s4d86 > 3)
			printf("\nexit block = %d, successors = ", e->blk);
		while (i >= 0) {
			s = a70aa[i].y;
			if (s4d86 > 3)
				printf(" %d", s);
			y = QHREC(blks[s]);
			if (y->w12 == 0) {
				if (s4d86 > 3) {
					printf("//calling vecunion(%X, %X, %d),q=%X\n", a0e8, y->f4, s7158, y);
					prquad(blks[s]);
				}
				orvec(a0e8, y->f4, s7158);
			}
			i = a70aa[i].x;
		}
		e = e->next;
	}
	if (s4d86 > 3)
		printf("\n");
}

struct dnode { struct dnode *next; struct dnode *down; int dblkno; };	/* the dominator tree (see dom2.c) */

/* the subtree of the dominator tree n with the block blk (name from the debug output) */
struct dnode *
findsubdtree(n, blk)
struct dnode *n;
int blk;
{
	struct dnode *r;

	if (s4d86 > 2)
		printf("findsubdtree(hdr=%d,blkno=%d)\n", n->dblkno, blk);
	if (n->dblkno == blk)
		return n;
	for (n = n->down; n != 0; n = n->next) {
		if (n->dblkno == blk)
			return n;
		if (n->down != 0) {
			r = findsubdtree(n, blk);
			if (r != 0)
				return r;
		}
	}
	return 0;
}

/* blk1 is the root of a subtree that contains blk2: the root is returned (name from the debug output) */
struct dnode *
isdom(hdr, blk1, blk2)
struct dnode *hdr;
int blk1, blk2;
{
	struct dnode *x;

	if (s4d86 > 2)
		printf("isdom(hdr=%d,blk1=%d,blk2=%d)\n", hdr->dblkno, blk1, blk2);
	x = findsubdtree(hdr, blk1);
	if (findsubdtree(x, blk2))
		return x;
	return 0;
}
