/*
 * lpinv2.c - gopt: the loop invariant code motion (segment 5). Written from /lib/gopt (no source known); names guessed, the
 * callees by the address (fnXXXX).
 */
static char sccsid[] = "@[$]lpinv2.c\t1.10  03/04/83 15:58:16 Copr. 1981 87wega3.2.";
#include "gopt.h"

static int lblcnt = 15000;

struct bl { struct bl *next; int blk; };	/* a list of blocks */
struct loop { struct bl *bls; int hdr; struct loop *link; struct bl *exits; int tail; int cnt; unsigned b15:1, b14:1, b13:1, b12:1, b11:1, b10:1; };
struct aln { int x0; struct aln *next; struct aln *link; };
struct hrec { unsigned *f0; unsigned *f4; unsigned *f8; int w12; };	/* the record of a block that the header quad points to at 6 (guessed) */
#define QHREC(q)	(*(struct hrec **)((char *)(q) + 6))
extern struct quad **blks;
extern int *idfn;
struct bt { int x0; int x1; int x[2]; unsigned f15:1, f14:1, bopt:1, rest:13; };
extern struct bt *betatbl;
extern void prquad();
extern int blkno, s4d86;

struct dl { int x0; struct quad *q; long l6; long l10; struct qa e14; int x20; unsigned f15:1, f14:1, f13:1, f12:1, f11:1, f10:1, rest:10; struct dl *next; };	/* a node of the list of the definitions (28 bytes, guessed) */
extern struct dl *a0e0;
extern int sameopnd();
extern int a0e4;
extern struct sym *symtab;
extern struct quad *tmpsearch();
extern void addlpdef(), setopt();
extern int isglobal(), isdefined();
extern void clrsymflag(), cerror();
extern char ascii[];
extern struct dl *a0f8, *alloclpdef(), *srchdflist();
extern char *taballoc();
struct pr { int x; int y; };
extern struct pr *edges;
extern struct quad *skiphdrs();
#define BBIT(j, n)	((*(int *)((char *)betatbl + ((j) * 10 + 8)) >> (n)) & 1)
extern int *altab;
extern struct aln *checkalias();
extern int lvwords;
extern unsigned *a0e8;
extern void clrvec(), orvec();
struct dnode { struct dnode *next; struct dnode *down; int dblkno; };	/* the dominator tree (see dom2.c) */
extern struct dnode *a0f4;
extern int bitpos(), tstlbit();
extern void prvec();
extern int issym();
extern int a0fc, a0f2, vafc;
extern struct dl *a100;
extern void chkbscindvar(), marklpinv(), prlpdefs();
extern int invopnds(), domexits(), notlive(), domuse(), samebetalpdef(), qparvbetalpdef();
extern struct dnode *domroot;
extern struct loop *loops;
extern int maxblk;
extern int blkno, s4d88, s4d90;
extern struct quad *qhead, *a0d6;
extern struct quad *insertquad();
extern void prgraph(), deledge(), addedge(), updatelooplist();
extern struct pr *edges;
extern int *vertex, a0f0, a0da, a0e6, a0ec, a0ee, a0fe;
extern struct quad *insrtstmpmove(), *qmove(), *chkfamindvar();
extern void deltmplpdefs();
extern char ctpos[];
extern int hdrblk, s4d92, s4d80, s4d94, a0d4;
extern struct dl *a104;
extern void freelist(), tabfree(), livevars(), clearflags(), ebbopt(), prquads(), prquad();
/* forward declarations (the functions are in the order of the original) */
int inbl();

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

/* the flags of the operands of q: the symbols and the betatbl entries of c (and of a of a parameter store) are marked (name from the debug output) */
void
setopt(q)
struct quad *q;
{
	if (s4d86 > 2) {
		printf("setopt()\n");
		prquad(q);
	}
	if (q->c.b2 == 4) {
		symtab[q->c.w4].sflags |= 2;
		if (s4d86 > 3)
			printf("setopt(), rslt sym=%s, lpopt=%d\n", ascii + symtab[q->c.w4].nameoff, (symtab[q->c.w4].sflags >> 1) & 1);
	} else if (q->c.b2 == 6 || q->c.b2 == 8)
		*(int *)((char *)betatbl + (q->c.w4 * 10 + 8)) |= 4096;
	if (q->c.b2 == 8 && q->op == 47) {
		symtab[betatbl[q->c.w4].x1].sflags |= 2;
		if (s4d86 > 3)
			printf("setopt(), rslt beta=%s, lpopt=%d\n", ascii + symtab[betatbl[q->c.w4].x1].nameoff, (symtab[betatbl[q->c.w4].x1].sflags >> 1) & 1);
	}
	if (q->op == 52 && q->a.b2 == 8) {
		*(int *)((char *)betatbl + (q->a.w4 * 10 + 8)) |= 4096;
		symtab[betatbl[q->a.w4].x1].sflags |= 2;
		if (s4d86 > 3)
			printf("setopt(), qparv beta=%s, lpopt=%d\n", ascii + symtab[betatbl[q->a.w4].x1].nameoff, (symtab[betatbl[q->a.w4].x1].sflags >> 1) & 1);
	}
}

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

/* the header quads in front of q that only lead to q are skipped (name guessed) */
struct quad *
skiphdrs(q)
struct quad *q;
{
	struct quad *x, *y;

	while (q->op == 59) {
		if (q->a.w4 < 0 || edges[q->a.w4].x != -1)
			break;
		y = blks[edges[q->a.w4].y];
		for (x = y->next; x != 0; x = x->next)
			if (x->op == 59)
				break;
		if (x != q)
			break;
		q = y;
	}
	return q;
}

/* the definition q in the block is entered in the list of the definitions of the loops (name from the debug output) */
void
addlpdef(q, block)
struct quad *q;
int block;
{
	struct dl *p, *r;
	int fl;
	struct quad *x;

	if (s4d86 > 2) {
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
		if (s4d86 > 3)
			printf("\tonedef=%d\n", p->f15);
	} else {
		if (r->q->op != 47 && q->op == 47)
			r->q = q;
		if (r->x0 == block) {
			r->f15 = 0;
			if (s4d86 > 3) {
				printf("\t\tonedef=%d, same quad:\n", r->f15);
				prquad(r->q);
			}
		} else {
			r->f15 = 0;
			if (s4d86 > 3)
				printf("\tr->onedef=%d\n", r->f15);
		}
	}
}

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
		if ((symtab[altab[p->x0]].sflags >> 2) & 1) {
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
		a0e8 = (unsigned *)taballoc(lvwords * 2, 2);
	clrvec(a0e8, lvwords);
	e = lp->exits;
	if (s4d86 > 3)
		printf("liveonexit(), returned from nullvec(), entering while loop\n");
	while (e != 0) {
		i = blks[e->blk]->b.w4;
		if (s4d86 > 3)
			printf("\nexit block = %d, successors = ", e->blk);
		while (i >= 0) {
			s = edges[i].y;
			if (s4d86 > 3)
				printf(" %d", s);
			y = QHREC(blks[s]);
			if (y->w12 == 0) {
				if (s4d86 > 3) {
					printf("//calling vecunion(%X, %X, %d),q=%X\n", a0e8, y->f4, lvwords, y);
					prquad(blks[s]);
				}
				orvec(a0e8, y->f4, lvwords);
			}
			i = edges[i].x;
		}
		e = e->next;
	}
	if (s4d86 > 3)
		printf("\n");
}

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

/* the operand ot (2 a, 3 b) of q is a variable that is defined in the loop (name from the debug output) */
int
isdefined(q, ot)
struct quad *q;
int ot;
{
	int t, i;

	if (s4d86 > 2) {
		printf("isdefined(q=%x,opndtype=%s)\n", q, ot == 2 ? "OP1" : "OP2");
		prquad(q);
	}
	if (ot == 2) {
		if (q->op == 3 && q->a.b2 == 4)
			return 0;
		t = q->a.b2;
		i = q->a.w4;
	} else {
		t = q->b.b2;
		i = q->b.w4;
	}
	if (t == 4) {
		if (s4d86 > 3)
			printf("isdefined(), symbol=%s, lpopt=%d\n", ascii + symtab[i].nameoff, (symtab[i].sflags >> 1) & 1);
		return (symtab[i].sflags >> 1) & 1;
	}
	if (t == 6) {
		if (s4d86 > 3)
			printf("isdefined(), stmp=%s, lpopt=%d\n", ascii + symtab[betatbl[i].x1].nameoff, (symtab[betatbl[i].x1].sflags >> 1) & 1);
		return BBIT(i, 12);
	}
	if (t == 8) {
		if ((q->op == 24 || q->op == 47) && ot == 2) {
			if (s4d86 > 3)
				printf("isdefined(), op1=%s, lpopt=%d\n", ascii + symtab[betatbl[i].x1].nameoff, (symtab[betatbl[i].x1].sflags >> 1) & 1);
			return ((symtab[betatbl[i].x1].sflags >> 1) & 1) || BBIT(i, 12);
		}
		if (s4d86 > 3)
			printf("isdefined(), beta=%s, lpopt=%d\n", ascii + symtab[betatbl[i].x1].nameoff, (symtab[betatbl[i].x1].sflags >> 1) & 1);
		return BBIT(i, 12);
	}
	cerror("isdefined(): illegal opnd");
}

/* the block dominates all exits of the loop (name from the debug output) */
int
domexits(lp, block)
struct loop *lp;
int block;
{
	struct bl *e;

	if (s4d86 > 2)
		printf("domexits(hdr=%d, block=%d)\n", lp->hdr, block);
	for (e = lp->exits; e != 0; e = e->next)
		if (!isdom(a0f4, block, e->blk))
			return 0;
	return 1;
}

/* the operands of q (and the quads that define its temporaries) are invariant in the loop (name from the debug output) */
int
invopnds(q, firstq)
struct quad *q, *firstq;
{
	struct quad *p;

	if (s4d86 > 1) {
		printf("invopnds(firstq = %X), quad (%X) = \n", firstq, q);
		prquad(q);
	}
	if (q->a.b2 == 3) {
		p = tmpsearch(firstq, q->a.w4);
		if (!invopnds(p, firstq))
			return 0;
		if (q->b.b2 == 0)
			return 1;
	} else if (q->a.b2 == 4 || q->a.b2 == 6 || q->a.b2 == 8) {
		if (!(q->a.b2 == 8 && BBIT(q->a.w4, 11) && q->b.b2 != 0)) {
			if (isdefined(q, 2))
				return 0;
			if (q->b.b2 == 0)
				return 1;
		}
	} else if (q->a.b2 == 1 || q->a.b2 == 2 || q->a.b2 == 7 || q->a.b2 == 5) {
		if (q->b.b2 == 0)
			return 1;
	} else {
		cerror("invopnd(): illegal op1 opnd");
		return;
	}
	if (q->op == 7)
		return 0;
	if (q->b.b2 == 3) {
		p = tmpsearch(firstq, q->b.w4);
		if (!invopnds(p, firstq))
			return 0;
		return 1;
	} else if (q->b.b2 == 4 || q->b.b2 == 6 || q->b.b2 == 8) {
		if (q->b.b2 == 8 && BBIT(q->b.w4, 11))
			return 1;
		if (isdefined(q, 3))
			return 0;
		return 1;
	} else if (q->b.b2 == 1 || q->b.b2 == 2 || q->b.b2 == 7)
		return 1;
	cerror("invopnd(): illegal op2 opnd");
}

/* the result operand of q is not live in the vector vec (name from the debug output) */
int
notlive(q, vec)
struct quad *q;
unsigned *vec;
{
	int k;

	if (s4d86 > 2) {
		printf("notlive(q=%x)\n", q);
		prquad(q);
		prvec(vec, 3, 1);
	}
	if (q->c.b2 == 4)
		k = bitpos(q, 1, 4);
	else if (q->c.b2 == 6)
		k = bitpos(q, 1, 6);
	else if (q->c.b2 == 8) {
		if (q->op == 47)
			k = bitpos(q, 1, 4);
		else
			k = bitpos(q, 1, 6);
	} else
		cerror("notlive(): illegal rslt opnd");
	if (s4d86 > 3)
		printf("notlive() = %s\n", (!tstlbit(vec, k) != 0) ? "TRUE" : "FALSE");
	return !tstlbit(vec, k);
}

/* the block blk dominates the uses of the variable of q in the loop lp (name from the debug output) */
int
domuse(lp, blk, q)
struct loop *lp;
int blk;
struct quad *q;
{
	struct bl *b;
	struct quad *x;
	int v, k1, k2, i, s;

	if (s4d86 > 2) {
		printf("domuse(hdr=%d,blk=%d,q=%X), quad=\n", lp->hdr, blk, q);
		prquad(q);
	}
	s = issym(q, 4);
	k1 = bitpos(q, 1, s);
	for (b = lp->bls; b != 0; b = b->next) {
		i = b->blk;
		for (x = blks[i]->next; x != 0; x = x->next) {
			if (x->op == 59)
				break;
			v = 0;
			if (x->f6 && (v = issym(x, 2)))
				k2 = bitpos(x, 2, v);
			else if (x->f5 && (v = issym(x, 3)))
				k2 = bitpos(x, 3, v);
			if (v && k2 == k1 && !isdom(a0f4, blk, i))
				return 0;
		}
	}
	return 1;
}

/* q and the quads that define its temporaries are marked as invariant (name from the debug output) */
void
marklpinv(q, firstq)
struct quad *q, *firstq;
{
	struct quad *p;

	if (s4d86 > 1) {
		printf("//  marklpinv: \n");
		prquad(q);
	}
	q->f3 = 1;
	if (q->a.b2 == 3) {
		p = tmpsearch(firstq, q->a.w4);
		marklpinv(p, firstq);
	}
	if (q->b.b2 == 3 && q->op != 7) {
		p = tmpsearch(firstq, q->b.w4);
		marklpinv(p, firstq);
	}
}

/* the quads of the list of the definitions are printed (name guessed) */
void
prlpdefs(p)
struct dl *p;
{
	for (; p != 0; p = p->next)
		prquad(p->q);
}

/* the definition d of a quad that adds a constant or an invariant to its own result is a basic induction variable (name from the debug output) */
void
chkbscindvar(d)
struct dl *d;
{
	struct quad *s, *x, *h;

	if (s4d86 > 1)
		printf("//  chkbscindvar: \n");
	x = d->q;
	if (x->op == 47 && x->a.b2 == 3) {
		h = skiphdrs(blks[d->x0]);
		s = tmpsearch(h, x->a.w4);
		if (s->op == 1 || s->op == 48) {
			if (s->b.b2 == 1 || s->b.b2 == 7 || ((s->b.b2 == 4 || s->b.b2 == 6 || s->b.b2 == 8) && !isdefined(s, 3))) {
				if (sameopnd(s, x, 2, 1)) {
					if (s4d86 > 1) {
						printf("found basic induction var:\n");
						prquad(s);
					}
					d->f14 = 1;
					d->e14 = s->b;
					if (s->b.b2 == 1 || s->b.b2 == 7)
						d->f11 = 1;
					else
						d->f11 = 0;
					if (s->op == 1)
						d->f10 = 1;
					else
						d->f10 = 0;
					a0e4 = 1;
					x->f2 = 1;
				}
			}
		}
	}
}

/* a definition in the list a0e0 has the same result operand as q (name from the debug output) */
int
samebetalpdef(q)
struct quad *q;
{
	struct dl *t;

	for (t = a0e0; t != 0; t = t->next) {
		if (t->q->c.b2 == q->c.b2 && t->q->c.w4 == q->c.w4) {
			if (s4d86 > 2) {
				printf("samebetalpdef(), t->dfquad, q:\n");
				prquad(t->q);
				printf(q);
			}
			return 1;
		}
	}
	return 0;
}

/* a parameter store in the list a0e0 stores to the result operand of q (name from the debug output) */
int
qparvbetalpdef(q)
struct quad *q;
{
	struct dl *t;

	for (t = a0e0; t != 0; t = t->next) {
		if (t->q->op == 52 && t->q->a.b2 == q->c.b2 && t->q->a.w4 == q->c.w4) {
			if (s4d86 > 2) {
				printf("qparvbetalpdef(), t->dfquad, q:\n");
				prquad(t->q);
				printf(q);
			}
			return 1;
		}
	}
	return 0;
}

/* the loop invariant definitions are searched in the list of the definitions of the loop lp, moved to the list a100 and marked (name from the debug output) */
void
lpinv(lp)
struct loop *lp;
{
	struct dl *t;
	struct quad *q;
	struct dl *u, *prev;
	struct quad *h;

	a0fc = 0;
	if (a0e0 == 0)
		return;
	if (s4d86 > 2) {
		printf("lpinv(hdr=%d)\n", lp->hdr);
		printf("lpdefs:\n");
		prlpdefs(a0e0);
	}
	a100 = 0;
	t = prev = a0e0;
	while (t != 0) {
		q = t->q;
		if (s4d86 > 3) {
			printf("lpinv(), qd->lpinv = %d, processing quad:\n", q->f3);
			prquad(q);
		}
		if (q->f3 || !t->f15)
			goto L1;
		h = skiphdrs(blks[t->x0]);
		if (!invopnds(q, h))
			goto L2;
		if (!domexits(lp, t->x0) && !notlive(q, a0e8))
			goto L2;
		if (q->c.b2 == 4 && symtab[q->c.w4].sx6 > vafc)
			goto L3;
		if (q->c.b2 == 8 && q->op == 47 && symtab[betatbl[q->c.w4].x1].sx6 > vafc)
			goto L3;
		if (s4d86 > 3) {
			printf("moving def to possible inv list\n");
			prquad(q);
		}
		if (a100 != 0) {
			u = a100;
			a100 = t;
			if (t == prev) {
				a0e0 = prev = t->next;
			} else
				prev->next = t->next;
			t = t->next;
			a100->next = u;
		} else {
			a100 = t;
			if (t == prev) {
				a0e0 = prev = t->next;
			} else
				prev->next = t->next;
			t = t->next;
			a100->next = 0;
		}
		continue;
L2:		chkbscindvar(t);
L1:		prev = t;
L3:		t = t->next;
	}
	if (s4d86 > 2) {
		printf("\nlpinvhdr\n");
		prlpdefs(a100);
		printf("\nlpdefs:\n");
		prlpdefs(a0e0);
	}
	t = a100;
	if (t == 0)
		return;
	for (; t != 0; t = t->next) {
		if (!domuse(lp, t->x0, t->q))
			continue;
		if (t->q->op == 47 && t->q->c.b2 == 8 && samebetalpdef(t->q))
			continue;
		if (s4d86 > 2 && a0fc != 1)
			printf("\n");
		a0fc = 1;
		a0f2 = 1;
		if (!qparvbetalpdef(t->q))
			clrsymflag(t->q, 1);
		h = skiphdrs(blks[t->x0]);
		marklpinv(t->q, h);
	}
	if (a0fc) {
		u = a0e0;
		if (u != 0) {
			while (u->next != 0)
				u = u->next;
			u->next = a100;
		} else
			a0e0 = a100;
	}
}

/* a node of the dominator tree for the block phblk (the new preheader) is put above the block hblk (name from the debug output) */
void
insrtdomnode(hblk, phblk)
int hblk, phblk;
{
	struct dnode *p, *x;

	if (s4d86 > 2)
		printf("// insrtdomnode(hblk=%d,phblk=%d)\n", hblk, phblk);
	p = findsubdtree(domroot, hblk);
	if (p->down != 0)
		x = p->down;
	else
		x = 0;
	p->down = (struct dnode *)taballoc(12, 4);
	p->dblkno = phblk;
	p->down->dblkno = hblk;
	p->down->down = x;
}

/* the new preheader phblk of the loop lp is put into the lists of blocks of the outer loops (name from the debug output) */
void
updatelooplist(lp, phblk)
struct loop *lp;
int phblk;
{
	struct loop *p;
	struct bl *x, *y;
	int hblk;

	hblk = lp->hdr;
	if (s4d86 > 1)
		printf("// updatelooplist(hblk=%d,phblk=%d)\n", hblk, phblk);
	if (phblk > maxblk)
		cerror("Too many basic blocks\n");
	idfn[phblk] = idfn[hblk];
	for (p = loops; p != 0; p = p->link) {
		if (p->b13 && p->cnt > 0) {
			for (y = x = p->bls; x != 0; y = x, x = x->next)
				if (x->blk == hblk)
					break;
			if (x != 0) {
				if (s4d86 > 2)
					printf("//update, found outer loop, header=%d\n", p->hdr);
				--p->cnt;
				if (x == p->bls) {
					p->bls = (struct bl *)taballoc(6, 4);
					p->bls->next = x;
					y = p->bls;
				} else {
					y->next = (struct bl *)taballoc(6, 4);
					y->next->next = x;
					y = y->next;
				}
				y->blk = phblk;
			}
		}
	}
	insrtdomnode(hblk, phblk);
}

/* a preheader block is added in front of the header of the loop lp (name from the debug output); the number of the new block is returned */
int
addpreheader(lp, flag)
struct loop *lp;
int flag;
{
	int i, p, hblk, lbl, nl, chg, adj;
	struct quad *q, *cq, *hq, *pq, *nq, *pv;
	struct qa qa1, qa2, qa3;

	chg = 0;
	adj = 0;
	if (s4d86 > 2)
		printf("// addpreheader()\n");
	if ((s4d90 || s4d86) && s4d88) {
		printf("addpreheader : before insertion, blockno is %d\n", lp->hdr);
		if (flag != 1)
			prgraph();
	}
	hblk = lp->hdr;
	updatelooplist(lp, blkno);
	hq = blks[hblk];
	if (s4d90 > 1) {
		printf("// addpreheader: hblk = %d\n", hblk);
		prquad(hq->next);
	}
	if (hq->next->op != 28)
		cerror("loop without a label");
	else
		lbl = hq->next->a.w4;
	for (i = hq->a.w4; i >= 0; i = edges[i].x) {
		p = edges[i].y;
		pq = blks[p];
		if (s4d86 > 3)
			printf("checking block = %d\n", p);
		if (!inbl(lp->bls, p)) {
			if (s4d86 > 3)
				printf("checking non loop block = %d\n", p);
			pv = pq;
			while (pq != 0) {
				if (pq->op == 59)
					break;
				pv = pq;
				pq = pq->next;
			}
			if (pq == hq)
				break;
		}
	}
	pq = pv = qhead;
	while (pq != 0) {
		if (pq == hq)
			break;
		pv = pq;
		pq = pq->next;
	}
	if (pq == 0)
		cerror("loop header quad not found");
	if (s4d86 > 3) {
		printf("found quad before the header:\n");
		prquad(pv);
	}
	qa1.w4 = -1;
	qa1.w0 = blkno;
	qa2.w4 = -1;
	qa3.b2 = 0;
	if (flag == 1 && lp->b10) {
		a0d6 = pv;
		return p;
	}
	lp->b10 = 1;
	nq = insertquad(59, &qa1, &qa2, &qa3, pv);
	a0d6 = nq;
	for (i = hq->a.w4; i >= 0; i = edges[i].x) {
		p = edges[i].y;
		pq = blks[p];
		if (inbl(lp->bls, p))
			continue;
		q = pq->next;
		if (s4d86 > 3) {
			printf("header block=%d, header label=%d\n", hblk, lbl);
			printf("predecessor block=%d, first quad:\n", p);
			prquad(q);
		}
		cq = 0;
		while (q != 0) {
			if (q->op == 59 || q->op == 26 || q->op == 27 || q->op == 10)
				break;
			if (q->op == 8)
				cq = q;
			q = q->next;
		}
		if (s4d86 > 3) {
			printf("predecessor last quad:\n");
			prquad(q);
		}
		if (q != 0) {
			if (q->op == 26) {
				if (q->a.w4 == lbl) {
					if (!chg)
						nl = lblcnt++;
					if (s4d86 > 3)
						printf("jump label changed from %d to %d\n", q->a.w4, nl);
					q->a.w4 = nl;
					chg = 1;
				}
			} else if (q->op == 27) {
				if (q->b.w4 == lbl) {
					if (!chg)
						nl = lblcnt++;
					if (s4d86 > 3)
						printf("jump label changed from %d to %d\n", q->b.w4, nl);
					chg = 1;
					q->b.w4 = nl;
				}
			} else if (q->op == 10) {
				if (q->a.w4 == lbl) {
					if (!chg)
						nl = lblcnt++;
					if (s4d86 > 3)
						printf("case label changed from %d to %d\n", q->a.w4, nl);
					chg = 1;
					q->a.w4 = nl;
					if (cq == 0)
						cerror("addpreheader: no case quad previously");
					if (-cq->b.w4 == lbl) {
						cq->b.w4 = -nl;
						if (s4d86 > 3) {
							printf("changed default label for quad\n");
							prquad(cq);
						}
					}
				}
			}
		}
		if (chg && !adj) {
			qa1.b2 = 1;
			qa1.w4 = nl;
			a0d6 = insertquad(28, &qa1, (struct qa *)0, (struct qa *)0, nq);
			adj = 1;
		}
		deledge(&hq->a.w4, p);
		deledge(&pq->b.w4, hblk);
		addedge(&nq->a.w4, p);
		addedge(&pq->b.w4, blkno);
		if (s4d86 > 3)
			printf("replaced link from pred = %d\n", p);
	}
	addedge(&nq->b.w4, hblk);
	addedge(&hq->a.w4, blkno);
	QHREC(nq) = (struct hrec *)taballoc(28, 2);
	QHREC(nq)->f4 = QHREC(hq)->f4;
	if (flag == 1)
		QHREC(nq)->f0 = QHREC(hq)->f4;
	blks[blkno] = nq;
	blkno++;
	if (blkno > maxblk) {
		printf("blkno=%d, maxblocks=%d, hblk=%d\n", blkno, maxblk, hblk);
		cerror("addpreheader(): Too many pre-headers added");
	}
	if ((s4d90 || s4d86) && s4d88) {
		printf("addpreheader : dgf after header insertion\n");
		prgraph();
	}
	return blkno - 1;
}

/* the block list of the loop lp is searched for p (name guessed) */
int
inbl(list, p)
struct bl *list;
int p;
{
	for (; list != 0; list = list->next)
		if (list->blk == p)
			return 1;
	return 0;
}

/* the list of blocks is put into the order of the array vertex (name guessed) */
void
orderbl(pp)
struct bl **pp;
{
	struct bl *x, *y;
	int i, t;

	x = *pp;
	for (i = 0; i < blkno; i++) {
		if (vertex[i] >= 0) {
			for (y = x; y != 0; y = y->next)
				if (y->blk == vertex[i])
					break;
			if (y != 0) {
				t = y->blk;
				y->blk = x->blk;
				x->blk = t;
				if ((x = x->next) == 0)
					return;
			}
		}
	}
}

/* the loop invariant quads of the loop lp are moved out of the loop (name from the debug output) */
void
proceslp(lp)
struct loop *lp;
{
	int blk, k1, k2;
	struct quad *q, *r;
	struct bl *b;

	if (s4d86)
		printf("// proceslp : moving loop inv quads:\n");
	a0da = 0;
	for (b = lp->bls; b != 0; b = b->next) {
		blk = b->blk;
		if (s4d86 > 1)
			printf("proceslp(): looking at block=%d\n", blk);
		if (QHREC(blks[blk])->w12) {
			if (s4d86 > 2)
				printf("proceslp(): looking at loop block=%d\n", blk);
			for (q = blks[blk]->next; q != 0; q = q->next) {
				if (q->op == 59)
					break;
				if (s4d86 > 2) {
					printf("proceslp: looking at quad, q->lpinv=%d, q->sdef=%d, q->lpind=%d\n", q->f3, q->f1, q->f2);
					prquad(q);
				}
				if (q->f3) {
					if (q->c.b2 == 3 && ctpos[q->c.w4] > 2)
						q = insrtstmpmove(q);
					else
						q = qmove(q);
				} else if (q->c.b2 == 3 || q->c.b2 == 8) {
					r = chkfamindvar(q, blk);
					if (r != 0)
						q = r;
				}
			}
			if (a0f0)
				deltmplpdefs();
		}
	}
}

/* the loops with a count of zero are marked as done; the result tells whether there was one (name from the debug output) */
int
marksloop()
{
	struct loop *p;
	int f;

	f = 0;
	for (p = loops; p != 0; p = p->link) {
		if (p->b13 && p->cnt == 0) {
			p->b13 = 0;
			p->b12 = 1;
			f = 1;
			if (s4d86 > 3)
				printf("//marksloop(loopmarked=%d)\n", p->hdr);
		}
	}
	return f;
}

/* the statistics of the loop optimization are printed (name guessed) */
void
prlpstats()
{
	printf("//\n//Number of quads moved out of loops \t\t= %d\n", a0e6);
	printf("//Number of stemps created in loop optimization = %d\n", a0ec);
	printf("//Number of temps created in loop optimization \t= %d\n", a0ee);
	printf("//Number of induction vars. eliminated in loop opt. = %d\n", a0fe);
}

/* the loop optimization of the program (name guessed) */
void
loopopt()
{
	struct loop *lp;
	struct quad *q;
	int first;

	first = 1;
	if (s4d86) {
		printf("//Program before loop optimization\n");
		prquads();
	}
again:
	if (!first)
		tabfree(7, (char *)0);
	livevars();
	s4d94 = a0d4;
	clearblocks();
	lp = loops;
	a104 = 0;
	a0f8 = 0;
	for (; lp != 0; lp = lp->link) {
		if (first)
			orderbl((struct bl **)lp);
		if (lp->b15)
			continue;
		if (lp->b11)
			continue;
		if (!lp->b12)
			continue;
		if (s4d86 > 3)
			printf("//\n//processing loop with header %d\n", lp->hdr);
		lp->b11 = 1;
		a0e0 = 0;
		a0e8 = 0;
		hdrblk = lp->hdr;
		marklpblocks(lp, 1);
		findlpdefs(lp);
		liveonexit(lp);
		a0f4 = findsubdtree(domroot, hdrblk);
		a0fc = 1;
		a0f2 = 0;
		a0e4 = 0;
		a0f0 = 0;
		while (a0fc)
			lpinv(lp);
		if (a0f2 || a0e4) {
			addpreheader(lp, 0);
			proceslp(lp);
		}
		while (a0da)
			proceslp(lp);
		freelist(1);
		marklpblocks(lp, 0);
		if (s4d86 > 4) {
			printf("//Program after optimizing loop\n");
			for (q = qhead; q != 0; q = q->next)
				prquad(q);
		}
	}
	a0d4 = s4d94;
	if (s4d86) {
		printf("//Program after loop optimization\n");
		for (q = qhead; q != 0; q = q->next)
			prquad(q);
	}
	ebbopt();
	first = 0;
	if (marksloop()) {
		clearflags();
		goto again;
	}
	if (s4d80) {
		printf("//Program after basic block optimization\n");
		for (q = qhead; q != 0; q = q->next)
			prquad(q);
	}
	if (s4d92)
		prlpstats();
}
