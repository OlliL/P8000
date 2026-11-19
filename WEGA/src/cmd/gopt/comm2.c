/*
 * comm2.c - gopt: errors, the tree pool, tree walks, small helpers (the pcc file "common", SCCS 2.4, included in the
 * original by comm2.c 1.1 07/07/82 of the optimizer). Written from /lib/gopt (no source known); the relative is
 * ../codgen/comm3.c (the same file one version later: 2.5 08/24/83).
 */
#include "gopt.h"

static char sccsid[] = "@[$]comm2.c\t1.1  07/07/82 15:54:49 Copr. 1981 87wega3.2.";
static char commsccs[] = "@[$]common\t2.4  03/04/83 15:24:28 Copr. 1981 87wega3.2.";

extern char ifname[];
extern int ifline;
extern NODE node[];
extern NODE *lastfree;

int nerrors = 0;
static NODE nodinit;
#include "ops_gopt.c"

#define NERRMAX	30

extern void dexit();
extern void walkf();
extern int cerror();
extern NODE *talloc();
extern void ncopy();
extern int szty();
static void tfree1();
static void prline();

/* an error in the source program */
void
uerror(s, a)
	char *s;
{
	++nerrors;
	prline('u');
	fprintf(stderr, s, a);
	fprintf(stderr, "\n");
	if (nerrors > NERRMAX)
		cerror("too many errors");
}

/* an error of the compiler itself: the program stops */
int
cerror(s, a, b, c)
	char *s;
{
	prline('c');
	if (nerrors && nerrors <= NERRMAX)
		fprintf(stderr, "cannot recover from earlier errors\n");
	else {
		fprintf(stderr, "compiler error: ");
		fprintf(stderr, s, a, b, c);
		putc('\n', stderr);
	}
	dexit(0x40);
}

void
werror(s, a, b)
	char *s;
{
	prline('w');
	fprintf(stderr, "warning: ");
	fprintf(stderr, s, a, b);
	fprintf(stderr, "\n");
}

/* all nodes are free */
void
tinit()
{
	NODE *p;

	for (p = node; p <= &node[NTREE - 1]; ++p)
		p->in.op = FREE;
	lastfree = node;
}

/* a free node from the pool, searching on from the last one */
NODE *
talloc()
{
	NODE *p, *q;
	int x;

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
void
tcheck()
{
	NODE *p;

	if (!nerrors)
		for (p = node; p <= &node[NTREE - 1]; ++p)
			if (p->in.op != FREE)
				cerror("wasted space: %%%x(%d)", p, p->in.op);
	tinit();
}

void
tfree(p)
	NODE *p;
{
	if (p->in.op != FREE)
		walkf(p, tfree1);
}

static void
tfree1(p)
	NODE *p;
{
	if (p == 0)
		cerror(" freeing blank tree!");
	else
		p->in.op = FREE;
}

/* walk the tree before the operands, with the value passed down: f(p, down, &down1, &down2) */
void
fwalk(p, f, down)
	register NODE *p;
	int (*f)();
	int down;
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
void
walkf(p, f)
	register NODE *p;
	int (*f)();
{
	register int kind;

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
void
mkdope()
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
void
tprint(t, fp)
	unsigned t;
	FILE *fp;
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
			fprintf(fp, tnames[t]);
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
int
bsize(t)
	register int t;
{
	switch (t) {
	case 2: case 12: return 8;			/* CHAR UCHAR */
	case 4: case 14: return 16;			/* INT UNSIGNED */
	case 5: case 6: case 15: return 32;		/* LONG FLOAT ULONG */
	case 7: return 64;				/* DOUBLE */
	}
	if ((t & 060) == 020)
		return 16;				/* a pointer */
	cerror("illegal bsize type: %%%0.2x\n", t);
}

/* size of a type in words: long, float and unsigned long 2, double 4, everything else 1 */
int
szty(t)
	int t;
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
int
argsize(p)
	register NODE *p;
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
static void
prline(c)
	int c;
{
	fprintf(stderr, "%s:%d: ", ifname, ifline);
}

/* the size in bytes of a type: 1, 2 or 4 */
int
tbytes(t)
	int t;
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

void
dexit(n)
	int n;
{
	fflush(stdout);
	exit(n);
}

void
branch(n)
	register int n;
{
	printf("\tjpr\tL%d\n", n);
}

void
ncopy(q, p)
	register NODE *q, *p;
{
	register int i;

	*q = *p;
}

/* a copy of the tree */
NODE *
tcopy(p)
	register NODE *p;
{
	register NODE *q;
	register int r;

	ncopy(q = talloc(), p);
	r = p->tn.rval;
	switch (optype(q->in.op)) {
	case BITYPE:
		q->in.right = tcopy(p->in.right);
	case UTYPE:
		q->in.left = tcopy(p->in.left);
		break;
	}
	return q;
}

/* log2 of a power of two, else -1 */
int
ispow2(c)
	register long c;
{
	register int n;

	if (c <= 0 || (c & (c - 1)))
		return -1;
	for (n = 0; c > 1; c >>= 1, n++)
		;
	return n;
}

int
plainicon(p)
	register NODE *p;
{
	return p->in.op == ICON && p->tn.lval == 0 && p->tn.rval >= 0;
}
