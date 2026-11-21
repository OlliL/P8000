/*
 * comm2.c - gopt: errors, the tree pool, tree walks, small helpers (the pcc file "common", SCCS 2.4, included in the
 * original by comm2.c 1.1 07/07/82 of the optimizer). Written from /lib/gopt (no source known); the relative is
 * ../codgen/comm3.c (the same file one version later: 2.5 08/24/83).
 */
static char sccsid[] __attribute__((unused)) = "@[$]comm2.c\t1.1  07/07/82 15:54:49 Copr. 1981 87wega3.2.";
#include "gopt.h"
static void prline(int c);
static void tfree1(NODE *p);

static char commsccs[] __attribute__((unused)) = "@[$]common\t2.4  03/04/83 15:24:28 Copr. 1981 87wega3.2.";

extern char ifname[];
extern int ifline;
extern NODE node[];
extern NODE *lastfree;

int nerrors = 0;
static NODE nodinit;
#include "ops_gopt.c"

#define NERRMAX	30

extern int qflag;
extern char *arhigh[];
static void tfree1();
static void prline();

/* an error in the source program */
void uerror(char *s, ...)
{
	va_list ap;

	++nerrors;
	prline('u');
	va_start(ap, s);
	vfprintf(stderr, s, ap);
	va_end(ap);
	fprintf(stderr, "\n");
	if (nerrors > NERRMAX)
		cerror("too many errors");
}

/* an error of the compiler itself: the program stops */
int cerror(char *s, ...)
{
	va_list ap;

	prline('c');
	if (nerrors && nerrors <= NERRMAX)
		fprintf(stderr, "cannot recover from earlier errors\n");
	else {
		fprintf(stderr, "compiler error: ");
		va_start(ap, s);
		vfprintf(stderr, s, ap);
		va_end(ap);
		putc('\n', stderr);
	}
	dexit(0x40);
}

void werror(char *s, ...)
{
	va_list ap;

	prline('w');
	fprintf(stderr, "warning: ");
	va_start(ap, s);
	vfprintf(stderr, s, ap);
	va_end(ap);
	fprintf(stderr, "\n");
}

/* all nodes are free */
void tinit(void)
{
	NODE *p;

	for (p = node; p <= &node[NTREE - 1]; ++p)
		p->in.op = FREE;
	lastfree = node;
}

/* a free node from the pool, searching on from the last one */
NODE *talloc(void)
{
	NODE *p, *q;

	q = lastfree;
	for (p = q == &node[NTREE - 1] ? node : q + 1; p != q; p = p == &node[NTREE - 1] ? node : p + 1)
		if (p->in.op == FREE) {
			*p = nodinit;
			return lastfree = p;
		}
	q = lastfree;
	for (p = q == &node[NTREE - 1] ? node : q + 1; p != q; p = p == &node[NTREE - 1] ? node : p + 1)
		printf("talloc: opcode = %d\n", p->in.op);
	cerror("out of tree space; simplify expression");
}

/* after the function: every node is free again, otherwise say which are not */
void tcheck(void)
{
	NODE *p;

	if (!nerrors)
		for (p = node; p <= &node[NTREE - 1]; ++p)
			if (p->in.op != FREE)
				cerror("wasted space: %%%x(%d)", p, p->in.op);
	tinit();
}

void tfree(NODE *p)
{
	if (p->in.op != FREE)
		walkf(p, tfree1);
}

static void tfree1(NODE *p)
{
	if (p == 0)
		cerror(" freeing blank tree!");
	else
		p->in.op = FREE;
}

/* walk the tree before the operands, with the value passed down: f(p, down, &down1, &down2) */
void fwalk(NODE *p, void (*f)(), int down)
{
	int down1, down2;

again:
	down1 = down2 = 0;
	(*f)(p, down, &down1, &down2);
	switch (optype(p->in.op)) {
	case BITYPE:
		fwalk(p->in.left, f, down1);
		p = p->in.right;
		down = down2;
		goto again;
	case UTYPE:
		p = p->in.left;
		down = down1;
		goto again;
	}
}

/* walk the tree after the operands */
void walkf(NODE *p, void (*f)())
{
	int kind;

	if (p == 0)
		return;
	kind = optype(p->in.op);
	if (kind != LTYPE)
		walkf(p->in.left, f);
	if (kind == BITYPE)
		walkf(p->in.right, f);
	(*f)(p);
}

/* dope[] and opst[] from the table of the operators */
void mkdope(void)
{
	register struct dopest *q;

	for (q = indope; q->dopeop >= 0; q++) {
		dope[q->dopeop] = q->dopeval;
		opst[q->dopeop] = q->opst;
	}
}

static char *tnames[] = {
	"undef", "farg", "char", "short", "int", "long", "float", "double", "strty", "unionty", "enumty", "moety",
	"uchar", "ushort", "unsigned", "ulong", "?", "?",
};

/* print a type */
void tprint(unsigned t, FILE *fp)
{
	for (;;) {
		if ((t & 060) == 020)
			fprintf(fp, "PTR ");
		else if ((t & 060) == 040)
			fprintf(fp, "FTN ");
		else if ((t & 060) == 060)
			fprintf(fp, "ARY ");
		else {
			if (t > 0x11)
				t = 0x11;
			fputs(tnames[t], fp);
			return;
		}
		t = ((t >> 2) & ~017) | (t & 017);
	}
}

char *rnames[] = {
	"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "fp",
	"f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7",
};

/* size of a type in bits */
int bsize(int t)
{
	switch (t) {
	case 2: case 12: return 8;			/* CHAR UCHAR */
	case 4: case 14: return 16;			/* INT UNSIGNED */
	case 5: case 6: case 15: return 32;		/* LONG FLOAT ULONG */
	case 7: return 64;				/* DOUBLE */
	}
	if ((t & 060) == 020)
		return 16;				/* a pointer */
	cerror("illegal bsize type: %%%x\n", t);	/* the original: %0.2x; its printf has no precision */
}

/* size of a type in words: long, float and unsigned long 2, double 4, everything else 1 */
int szty(int t)
{
	switch (t) {
	case 5: case 6: case 15: return 2;
	case 7: return 4;
	}
	if ((t & 060) == 020)
		return 1;		/* no effect */
	return 1;
}

/* size of an argument in words: a structure argument by its size, a float counts as a double */
int argsize(NODE *p)
{
	int t;

	if (p->in.op == STARG)
		return p->stn.stsize / 2;
	if (p->in.op == NAME && p->in.type == 8)
		return p->in.sname->ssize / 16;
	t = szty(p->in.type == 6 ? 7 : p->in.type);
	return t;
}

/* the file and line of the intermediate file in front of an error message */
static void prline(int c)
{
	(void)c;
	fprintf(stderr, "%s:%d: ", ifname, ifline);
}

/* the size in bytes of a type: 1, 2 or 4 */
int tbytes(int t)
{
	switch (t) {
	case 5:
	case 15:
		return 4;
	case 2:
	case 12:
		return 1;
	}
	if ((t & 060) == 020)
		return 2;		/* no effect */
	return 2;
}

/* a table of the relational operators (0x50 .. 0x59) that no function of the pass uses (it is in the data of the original) */
static int relops[] __attribute__((unused)) = { 0x50, 0x51, 0x54, 0x55, 0x52, 0x53, 0x58, 0x59, 0x56, 0x57 };

void dexit(int n)
{
	fflush(stdout);
	exit(n);
}

void branch(int n)
{
	printf("\tjpr\tL%d\n", n);
}

void ncopy(NODE *q, NODE *p)
{

	*q = *p;
}

/* a copy of the tree */
NODE *tcopy(NODE *p)
{
	register NODE *q;
	register int r __attribute__((unused));

	ncopy(q = talloc(), p);
	r = p->tn.rval;
	switch (optype(q->in.op)) {
	case BITYPE:
		q->in.right = tcopy(p->in.right);
	/* fall through */
	case UTYPE:
		q->in.left = tcopy(p->in.left);
		break;
	}
	return q;
}

/* log2 of a power of two, else -1 */
int ispow2(long c)
{
	register int n;

	if (c <= 0 || (c & (c - 1)))
		return -1;
	for (n = 0; c > 1; c >>= 1, n++)
		;
	return n;
}

int plainicon(NODE *p)
{
	return p->in.op == ICON && p->in.sname == 0 && p->tn.rval >= 0;
}

/* the areas of the tables: each has a segment of its own (area 1 may move to another one).
   host: each area is one block of ARSIZE bytes; beside it the bytes of the original are counted (z*: the offset in
   the segment, the end the program knows, the size of the segment as the system rounds it: mkseg() and ssgbrk() in
   units of 256 bytes, at most 0x10000), so that gopt stops with "C Optimizer: out of space" where the original does
   (area 1 goes on in a new segment, its offsets from 0). A log of the allocations (host pointer, offset) gives
   tabfree() and tabzoff() the offset of a pointer. */
#define NAREA	11
#define STEP	2000
#define ARSIZE	(64L << 20)
static char *arbase[NAREA] = { 0 };	/* host: the start of the memory of the area */
static char *arlim[NAREA] = { 0 };	/* end of the memory of the area */
static char *arptr[NAREA] = { 0 };	/* the next free byte */
static long zptr[NAREA], zlim[NAREA], zsize[NAREA];	/* host: the offset, the end and the segment of the original */
static struct zlog { char *p; long z; } *zlog[NAREA];	/* host: the allocations of the area in their order */
static long nzlog[NAREA], mzlog[NAREA];

#define R256(n)	(((long)(n) + 255) & ~255L)

/* n bytes of the area a, cleared; zn: the bytes of the original (names taballoc, tabfree, tabavail guessed) */
char *tabzalloc(int n, int zn, int a)
{
	char *p;
	int *q;
	long m;

	if (arptr[a] == 0) {
		arbase[a] = arptr[a] = malloc(ARSIZE);
		if (arptr[a] == 0)
			cerror("C Optimizer: out of space\n");
		arlim[a] = arptr[a] + ARSIZE;
		zptr[a] = 0;			/* mkseg(0, STEP) */
		zlim[a] = STEP;
		zsize[a] = R256(STEP);
	}
	n++;
	n &= ~1;
	zn++;
	zn &= ~1;
	if (zptr[a] + zn >= zlim[a]) {
		m = zn > STEP ? zn : STEP;
		if (zsize[a] + R256(m) > 0x10000) {	/* ssgbrk() refuses */
			if (a != 1)
				cerror("C Optimizer: out of space\n");
			zptr[a] = zlim[a] = 0;		/* mkseg(0, m): a new segment */
			zsize[a] = R256(m);
		} else
			zsize[a] += R256(m);
		zlim[a] += m;
	}
	if (n % 8 == 0)	/* host: a structure with pointers (and the start of a table) on 8 bytes; other sizes stay
			   packed, as the arrays of ints that the original overruns into the next one (dominators()) */
		arptr[a] = arbase[a] + ((arptr[a] - arbase[a] + 7) & ~7L);
	if (arptr[a] + n >= arlim[a])
		cerror("C Optimizer: out of space\n");
	p = arptr[a];
	if (nzlog[a] == mzlog[a]) {
		mzlog[a] = mzlog[a] ? 2 * mzlog[a] : 1024;
		zlog[a] = realloc(zlog[a], mzlog[a] * sizeof(struct zlog));
		if (zlog[a] == 0)
			cerror("C Optimizer: out of space\n");
	}
	zlog[a][nzlog[a]].p = p;
	zlog[a][nzlog[a]++].z = zptr[a];
	zptr[a] += zn;
	arptr[a] += n;
	if (arptr[a] > arhigh[a])
		arhigh[a] = arptr[a];
	for (q = (int *)p; q < (int *)arptr[a]; q++)
		*q = 0;
	return p;
}

/* host: the offset in its segment that the original has for p of the area a (an allocation of the log) */
long tabzoff(int a, char *p)
{
	long lo = 0, hi = nzlog[a] - 1, k;

	while (lo <= hi) {
		k = (lo + hi) / 2;
		if (zlog[a][k].p < p)
			lo = k + 1;
		else if (zlog[a][k].p > p || (k > 0 && zlog[a][k - 1].p == p))
			hi = k - 1;
		else
			return zlog[a][k].z;
	}
	return lo < nzlog[a] ? zlog[a][lo].z : zptr[a];
}

/* give the memory of the area a back down to p (0: to the start of its segment) */
void tabfree(int a, char *p)
{
	long z;

	if (p == 0) {
		p = arbase[a];
		z = 0;		/* the start of the segment that the area is in now */
	} else {
		z = tabzoff(a, p);
		if (a == 1 && z > zptr[a]) {	/* back into an earlier segment (sgbrk): its end is p */
			zlim[a] = z;
			zsize[a] = R256(z);
		}
	}
	while (nzlog[a] > 0 && zlog[a][nzlog[a] - 1].p >= p)
		nzlog[a]--;
	zptr[a] = z;
	arptr[a] = p;
}

/* is there room for n bytes in the area a (it is allocated and given back) */
int tabavail(int n, int a)
{
	char *p, *q;
	int r;

	p = tabzalloc(0, 0, a);
	q = tabzalloc(n, n, a);	/* host: n bytes of the original */
	if (q == 0)	/* the original: 0x7F00FFFF, the -1 of mkseg(); taballoc() stops before */
		r = 0;
	else
		r = 1;
	tabfree(a, p);
	return r;
}

/* host: the start of the area a (the space used: arhigh[a] - tabbase(a); the original prints the offset of arhigh) */
char *tabbase(int a)
{
	return arbase[a];
}
