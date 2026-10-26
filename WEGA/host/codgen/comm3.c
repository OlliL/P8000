/*
 * comm3.c - messages, the tree pool, the tree walkers, a few small helpers (codgen / scodgen)
 *
 * Reconstructed from /lib/codgen: uerror() 0x8570, cerror() 0x85ae, werror() 0x862e, tinit() 0x866c, talloc() 0x869c,
 * tcheck() 0x86fa, tfree() 0x873c, fwalkd() 0x8774, fwalk() 0x87ec, dopeinit() 0x8844, tbytes() 0x8970, rallkind() 0x89a2,
 * errexit() 0x89ce, branch() 0x89e8, ncopy() 0x8a02, alloc() 0x8b6e, allocinit() 0x8bea, prline() 0x8952.
 * Names are guesses. The nodes come from a pool of 500 nodes of 0x12 bytes.
 */
#include <stdarg.h>
#include "cg.h"

#define NTREE	500
#define NERRMAX	30

extern void fwalk(), errexit();
extern int dope[];
extern char *opst[];
extern struct indope indope[];	/* 0x3b92: op, dope flags, name; ends with op -1 */
char ifname[72];		/* 0x3b36: the file name of the intermediate file line */
int dope[129];		/* 129: setrew() runs to op 128; that entry is 0 (not an operator) */			/* 0x8d0a */
char *opst[128];		/* 0x8e1a */
extern int ifline;		/* 0x8fc8 */
int nerrors;			/* 0x3b7e */
struct node node[NTREE];	/* 0x69e2 */
struct node *lastfree = node;	/* 0xa7b4 */
/* the memory of the tree pool and of the symbol table: one arena, allocated by alloc() upwards, reset by allocinit() */
#define ARENA	(4L * 1024 * 1024)
static char arena[ARENA];
static char *curbrk = arena;		/* 0x4060 */
static char *endbrk = arena + ARENA;	/* 0x405e */

/* the file and line of the intermediate file in front of an error message */
static void prline(int c)
{
	fprintf(stderr, "%s:%d: ", ifname, ifline);
}

/* an error in the source program */
int uerror(char *s, ...)
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
	return 0;
}

/* an error of the compiler itself: the program stops */
int cerror(char *s, ...)
{
	va_list ap;

	prline('c');
	if (nerrors && nerrors <= NERRMAX) {
		fprintf(stderr, "cannot recover from earlier errors\n");
		errexit(0x40);
	}
	fprintf(stderr, "compiler error: ");
	va_start(ap, s);
	vfprintf(stderr, s, ap);
	va_end(ap);
	putc('\n', stderr);
	errexit(0x40);
	return 0;
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
	struct node *p;

	for (p = node; p <= &node[NTREE - 1]; p++)
		p->op = FREE;
	lastfree = node;
}

/* a free node from the pool, searching on from the last one */
struct node *talloc(void)
{
	struct node *p, *start;

	start = lastfree;
	p = start != &node[NTREE - 1] ? start + 1 : node;
	while (p != start) {
		if (p->op == FREE) {
			p->sname = 0;
			lastfree = p;
			return p;
		}
		p = p != &node[NTREE - 1] ? p + 1 : node;
	}
	cerror("out of tree space; simplify expression");
	return 0;		/* not reached: cerror ends the run */
}

/* after the function: every node is free again, otherwise say which are not */
void tcheck(void)
{
	struct node *p;

	if (!nerrors)
		for (p = node; p <= &node[NTREE - 1]; p++)
			if (p->op != FREE)
				cerror("wasted space: %%%lx(%d)", (unsigned long)p, p->op);
	tinit();
}

static void free1(struct node * p)
{
	if (p == 0)
		cerror("freeing blank tree!");
	else
		p->op = FREE;
}

void tfree(struct node * p)
{
	if (p->op != FREE)
		fwalk(p, free1);
}

/* walk the tree before the operands, with the value passed down: f(p, down, &down1, &down2) */
void fwalkd(register struct node *p, int (*f)(), int down)
{
	if (p == 0)		/* a PLUS node without right operand (fixfld) reaches this; the original reads address 0 here */
		return;
	int down1, down2;

	for (;;) {
		if (p == 0)
			return;		/* the right operand of a PLUS node made by fixfld is 0 (see above) */
		down1 = 0;
		down2 = 0;
		(*f)(p, down, &down1, &down2);
		switch (dope[p->op] & 0xE) {
		case UTYPE:
			p = p->left;
			down = down1;
			continue;
		case BITYPE:
			fwalkd(p->left, f, down1);
			p = p->right;
			down = down2;
			continue;
		}
		return;
	}
}

/* walk the tree after the operands */
void fwalk(register struct node *p, int (*f)())
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
void dopeinit(void)
{
	register struct indope *q;

	for (q = indope; q->op >= 0; q++) {
		dope[q->op] = q->flags;
		opst[q->op] = q->name;
	}
}

/* the size in bytes of a type: 1, 2 or 4 */
int tbytes(int t)
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
	if ((t & 0x30) == 0x10)
#ifdef SEG
		return 4;
#else
		return 2;		/* no effect: the original has the test in both versions */
#endif
	return 2;
}

/* the cookie that serves the type: B registers for floats, A registers otherwise */
int rallkind(struct node * p)
{
	if (p && (p->type == 7 || p->type == 6))
		return 0x10;
	return 2;
}

void errexit(int n)
{
	fflush(stdout);
	exit(n);
}

void branch(int n)
{
	printf("\tjpr\tL%d\n", n);
}

/* *q = *p for a node */
void ncopy(struct node *q, struct node *p)
{
	*q = *p;
}

/* memory from the system in steps of at most 2000 bytes, never freed */
char *alloc(register int n)
{
	register char *p;

	n++;
	n &= ~1;
	if (endbrk - curbrk < n)
		cerror("Code generator: out of space\n");
	p = curbrk;
	curbrk += n;
	return p;
}

void allocinit(char *base)
{
	curbrk = base;
}
