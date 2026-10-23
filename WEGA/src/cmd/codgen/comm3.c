/*
 * comm3.c - messages, the tree pool, the tree walkers, a few small helpers (codgen / scodgen)
 *
 * Reconstructed from /lib/codgen: uerror() 0x8570, cerror() 0x85ae, werror() 0x862e, tinit() 0x866c, talloc() 0x869c,
 * tcheck() 0x86fa, tfree() 0x873c, fwalkd() 0x8774, fwalk() 0x87ec, dopeinit() 0x8844, tbytes() 0x8970, rallkind() 0x89a2,
 * errexit() 0x89ce, branch() 0x89e8, ncopy() 0x8a02, alloc() 0x8b6e, allocinit() 0x8bea, prline() 0x8952.
 * Names are guesses. The nodes come from a pool of 500 nodes of 0x12 bytes.
 */
#include "cg.h"

#define NTREE	500
#define NERRMAX	30

extern int cerror();
extern void fwalk(), errexit();
extern char *sbrk();
extern int dope[];
extern char *opst[];
extern struct indope indope[];	/* 0x3b92: op, dope flags, name; ends with op -1 */
char ifname[72];		/* 0x3b36: the file name of the intermediate file line */
int dope[128];			/* 0x8d0a */
char *opst[128];		/* 0x8e1a */
extern int ifline;		/* 0x8fc8 */
int nerrors;			/* 0x3b7e */
struct node node[NTREE];	/* 0x69e2 */
struct node *lastfree = node;	/* 0xa7b4 */
static char *curbrk, *endbrk;	/* 0x4060, 0x405e */
static int firstalloc = 1;	/* 0x4062 */

/* clear the stack below the caller, where the work area of printf will be (see cg.h) */
static void
cgwipe()
{
	char buf[400];
	register int i;

	for (i = 0; i < 400; i++)
		buf[i] = 0;
}

/* printf with the stack cleared; up to five words of arguments (a long takes two) */
#undef printf
int
cgprintf(fmt, a, b, c, d, e)
	char *fmt;
	int a, b, c, d, e;
{
	cgwipe();
	return printf(fmt, a, b, c, d, e);
}

/* the file and line of the intermediate file in front of an error message */
static void
prline(c)
	int c;
{
	fprintf(stderr, "%s:%d: ", ifname, ifline);
}

/* an error in the source program */
void
uerror(s, a)
	char *s;
	int a;
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
	if (nerrors && nerrors <= NERRMAX) {
		fprintf(stderr, "cannot recover from earlier errors\n");
		errexit(0x40);
	}
	fprintf(stderr, "compiler error: ");
	fprintf(stderr, s, a, b, c);
	putc('\n', stderr);
	errexit(0x40);
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
	register struct node *p;

	for (p = node; p <= &node[NTREE - 1]; p++)
		p->op = FREE;
	lastfree = node;
}

/* a free node from the pool, searching on from the last one */
struct node *
talloc()
{
	register struct node *p, *start;

	start = lastfree;
	p = start == &node[NTREE - 1] ? node : start + 1;
	while (p != start) {
		if (p->op == FREE) {
			p->sname = 0;
			lastfree = p;
			return p;
		}
		p = p == &node[NTREE - 1] ? node : p + 1;
	}
	cerror("out of tree space; simplify expression");
}

/* after the function: every node is free again, otherwise say which are not */
void
tcheck()
{
	register struct node *p;

	if (!nerrors)
		for (p = node; p <= &node[NTREE - 1]; p++)
			if (p->op != FREE)
				cerror("wasted space: %%%x(%d)", p, p->op);
	tinit();
}

static void
free1(p)
	struct node *p;
{
	if (p == 0)
		cerror("freeing blank tree!");
	else
		p->op = FREE;
}

void
tfree(p)
	struct node *p;
{
	if (p->op != FREE)
		fwalk(p, free1);
}

/* walk the tree before the operands, with the value passed down: f(p, down, &down1, &down2) */
void
fwalkd(p, f, down)
	register struct node *p;
	int (*f)();
	int down;
{
	int down1, down2;

	for (;;) {
		down1 = 0;
		down2 = 0;
		(*f)(p, down, &down1, &down2);
		switch (dope[p->op] & 0xE) {
		case BITYPE:
			fwalkd(p->left, f, down1);
			p = p->right;
			down = down2;
			continue;
		case UTYPE:
			p = p->left;
			down = down1;
			continue;
		}
		return;
	}
}

/* walk the tree after the operands */
void
fwalk(p, f)
	register struct node *p;
	int (*f)();
{
	register int kind;

	if (p == 0)
		return;
	kind = dope[p->op] & 0xE;
	if (kind != LTYPE)
		fwalk(p->left, f);
	if (kind == BITYPE)
		fwalk(p->right, f);
	(*f)(p);
}

/* dope[] and opst[] from the table of the operators */
void
dopeinit()
{
	register struct indope *q;

	for (q = indope; q->op >= 0; q++) {
		dope[q->op] = q->flags;
		opst[q->op] = q->name;
	}
}

/* the size in bytes of a type: 1, 2 or 4 */
int
tbytes(t)
	int t;
{
	switch (t) {
	case 2:
	case 12:
		return 1;
	case 3:
	case 13:
		return 2;
	case 5:
	case 15:
		return 4;
	}
	return 2;
}

/* the cookie that serves the type: B registers for floats, A registers otherwise */
int
rallkind(p)
	register struct node *p;
{
	if (p && (p->type == 7 || p->type == 6))
		return 0x10;
	return 2;
}

void
errexit(n)
	int n;
{
	fflush(stdout);
	exit(n);
}

void
branch(n)
	int n;
{
	printf("\tjpr\tL%d\n", n);
}

/* *q = *p for a node */
void
ncopy(q, p)
	struct node *q, *p;
{
	*q = *p;
}

/* memory from the system in steps of at most 2000 bytes, never freed */
char *
alloc(n)
	register int n;
{
	register char *p;
	int step;

	if (firstalloc) {
		firstalloc = 0;
		curbrk = endbrk = sbrk(0);
	}
	n = (n + 1) & ~1;
	if (curbrk + n >= endbrk) {
		step = n > 2000 ? n : 2000;
		if ((int)sbrk(step) == -1)
			cerror("Code generator: out of space\n");
		endbrk += step;
	}
	p = curbrk;
	curbrk += n;
	return p;
}

void
allocinit(base)
	char *base;
{
	brk(base);
	endbrk = curbrk = base;
	endbrk--;
}
