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
	struct node *p;

	for (p = node; p <= &node[NTREE - 1]; p++)
		p->op = FREE;
	lastfree = node;
}

/* a free node from the pool, searching on from the last one */
struct node *
talloc()
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
}

/* after the function: every node is free again, otherwise say which are not */
void
tcheck()
{
	struct node *p;

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
	if ((t & 0x30) == 0x10)
#ifdef SEG
		return 4;
#else
		return 2;		/* no effect: the original has the test in both versions */
#endif
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

#ifdef OLD_D3K
void d3kjump();
#endif
void
branch(n)
	int n;
{
#ifdef OLD_D3K
	d3kjump(n);
#endif
	printf("\tjpr\tL%d\n", n);
}

#ifdef OLD_D3K
/* v0: the labels a jump has named already in this function (a parameter stays known in its register there) */
#define D3KMAX	16384
static char d3kbits[D3KMAX / 8];
#ifdef OLD_D3J
#define D3JMAX	8
static struct d3j {
	int	lab;
	int	n;
	int	jumps;
	int	reg[4], cls[4];
	struct sym *sym[4];
} d3jtab[D3JMAX];
static int d3jfree();
static void d3jmark();
void d3jsave();
extern void rfree(), rsreturn();
#endif

void
d3kjump(l)
	register int l;
{
	if (l >= 0 && l < D3KMAX)
		d3kbits[l >> 3] |= 1 << (l & 7);
#ifdef OLD_D3J
	d3jsave(l);
#endif
}

int
d3knamed(l)
	register int l;
{
	return l >= 0 && l < D3KMAX && (d3kbits[l >> 3] >> (l & 7) & 1);
}

void
d3kreset()
{
	register int i;

	for (i = 0; i < D3KMAX / 8; i++)
		d3kbits[i] = 0;
#ifdef OLD_D3J
	for (i = 0; i < D3JMAX; i++)
		d3jtab[i].lab = -1;
#endif
}
#endif

#ifdef OLD_D3J
/* v0p: the parameters known in registers at a conditional jump; the label gets them back when the code in front of it
   ends with a jump (its state is that of the jumps to it) */
void
d3jsave(lab)
	int lab;
{
	register struct d3j *j;
	register struct rs *n;
	register int i, k;
	struct d3j *f;

	f = 0;
	for (j = d3jtab; j < &d3jtab[D3JMAX]; j++) {
		if (j->lab == lab)
			break;
		if (j->lab <= 0 && f == 0)
			f = j;
	}
	if (j == &d3jtab[D3JMAX]) {	/* the first jump: the known parameters */
		if ((j = f) == 0)
			return;
		j->lab = lab;
		j->n = 0;
		j->jumps = 1;
		for (n = rsuse; n; n = n->next)
			if (n->cls <= 3 && n->sym && n->sym->sclass == 9 && j->n < 4) {
				j->reg[j->n] = n->reg;
				j->cls[j->n] = n->cls;
				j->sym[j->n] = n->sym;
				j->n++;
			}
		return;
	}
	j->jumps++;
	for (i = 0; i < j->n; i++) {	/* a later one: those known at both */
		for (n = rsuse; n; n = n->next)
			if (n->reg == j->reg[i] && n->cls == j->cls[i] && n->sym == j->sym[i])
				break;
		if (n == 0) {
			for (k = i + 1; k < j->n; k++) {
				j->reg[k - 1] = j->reg[k];
				j->cls[k - 1] = j->cls[k];
				j->sym[k - 1] = j->sym[k];
			}
			j->n--;
			i--;
		}
	}
}

/* the label lab: the parameters known at all the jumps to it (and at the code in front of it unless dead: that ends
   with a jump) */
void
d3jlabel(lab, dead)
	int lab, dead;
{
	register struct d3j *j;
	register int i, r;
	register struct rs *n;
	struct rs *next;

	for (j = d3jtab; j < &d3jtab[D3JMAX]; j++)
		if (j->lab == lab)
			break;
	if (j == &d3jtab[D3JMAX])
		return;
	j->lab = -1;
	for (n = rsuse; n; n = next) {	/* a parameter not known at every jump is forgotten */
		next = n->next;
		if (n->cls > 3 || !n->sym || n->sym->sclass != 9 || n->busy > 0)
			continue;
#ifdef OLD_D3JM
		if (!dead || j->jumps != 1) {
#else
		if (!dead) {
#endif
			for (i = 0; i < j->n; i++)
				if (n->reg == j->reg[i] && n->cls == j->cls[i] && n->sym == j->sym[i])
					break;
			if (i < j->n)
				continue;
		}
		rsreturn(n->cls, n->reg);
	}
	if (!dead || j->jumps != 1)
		return;
	for (i = 0; i < j->n; i++)
		if (d3jfree(j->cls[i], j->reg[i])) {
			r = getreg(j->cls[i], j->reg[i], 0, j->sym[i]);
			if (r == j->reg[i]) {
				rfree(r, j->cls[i] == 3 ? 5 : j->cls[i] == 1 ? 2 : 4);
				d3jmark(j->cls[i], r);
			}
		}
}

/* the node of (cls, reg) got its parameter back at a label: it still needs its home in the frame (regargs) */
static void
d3jmark(cls, reg)
	int cls, reg;
{
	register struct rs *n;

	for (n = rsuse; n; n = n->next)
		if (n->cls == cls && n->reg == reg)
			n->flags |= 0x4000;
}

/* the register reg of the class cls is on the free list */
static int
d3jfree(cls, reg)
	int cls, reg;
{
	register struct rs *n;

	for (n = rsfree; n; n = n->next)
		if (n->cls == cls && n->reg == reg)
			return 1;
		else if (n->cls == cls + 1 && cls <= 2 && n->reg == (cls == 1 ? reg & 7 & ~1 : reg & ~1))
			return 1;
	return 0;
}
#endif

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
	n++;
	n &= ~1;
	if (curbrk + n >= endbrk) {
		if (n > 2000)
			step = n;
		else
			step = 2000;
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
