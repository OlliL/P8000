/*
 * comm3.c - errors, the tree pool, tree walks, small helpers, memory
 */
#include "cg.h"
#include "cg2.h"

static void tfree1(NODE *p);
static void prline(int c);

extern int dope[];
extern int ifline;
extern char ifname[];
extern struct dopest indope[];
extern NODE *lastfree;
extern int nerrors;
extern NODE node[];
extern char *opst[];
extern int revrel[];
extern char *rnames[];
extern struct rs *rsfree;
extern void rsreturn();
extern struct rs *rsuse;
static char sccsid[] __attribute__((unused)) = "@[$]comm3.c\t2.1  11/15/82 12:48:27 Copr. 1981 87wega3.2.";
char unused6[16] = { 0 };
char ifname[16] = { 0 };
static char commsccs[] __attribute__((unused)) = "@[$]common\t2.5  08/24/83 16:19:58 Copr. 1981 87wega3.2.";
int nerrors = 0;
int unused8[9] = { 0 };
#ifdef SEG
#include "ops_scodgen.c"
#else
#include "ops_codgen.c"
#endif


#define MAXRCOUNT 5000

#define RLEFT	01

#define RRIGHT	02

#define RESC1	04

#define RESC2	010

#define RESC3	020

#define RESCC	04000

#define RESC	02000		/* rewrite & 0x400: the second register of a long result */

#define RNULL	0

#define RNOP	010000

/* number of words a temporary for the result of p needs (a macro in the original: it is expanded twice in allo) */
#ifdef SEG
#define PTRWORDS	2
#else
#define PTRWORDS	1
#endif

#define TEMPWORDS(p, nfield) \
	((p->in.op == STCALL || p->in.op == STARG || p->in.op == USTCALL || p->in.op == STASG) ? ((unsigned)((p->stn.stsize << 3) + 15)) >> 4 : \
	 (p->in.type == 5 || p->in.type == 15) ? 2 : (p->in.type & 0x30) == 0x10 ? PTRWORDS : \
	 p->in.type == 7 ? 4 : p->in.type == 6 ? 2 : (nfield))

#define NERRMAX	30




static void prline();
static void tfree1();

/* an error in the source program */
void
syserror(char *s)
{
	fprintf(stderr, "system call error: ");
	perror(s);
	dexit(0x40);
}

void
uerror(char *s, ...)
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
void
cerror(char *s, ...)
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

void
werror(char *s, ...)
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
void
tinit(void)
{
	NODE *p;

	for (p = node; p <= &node[NTREE - 1]; p++)
		p->in.op = FREE;
	lastfree = node;
}

/* a free node from the pool, searching on from the last one */
NODE *
talloc(void)
{
	NODE *p, *start;

	start = lastfree;
	p = start == &node[NTREE - 1] ? node : start + 1;
	while (p != start) {
		if (p->in.op == FREE) {
			p->in.sname = 0;
			return lastfree = p;
		}
		p = p == &node[NTREE - 1] ? node : p + 1;
	}
	cerror("out of tree space; simplify expression");
}

/* after the function: every node is free again, otherwise say which are not */
void
tcheck(void)
{
	NODE *p;

	if (!nerrors)
		for (p = node; p <= &node[NTREE - 1]; p++)
			if (p->in.op != FREE)
				cerror("wasted space: %%%x(%d)", p, p->in.op);
	tinit();
}

void
tfree(NODE *p)
{
	if (p->in.op != FREE)
		walkf(p, tfree1);
}

static void
tfree1(NODE *p)
{
	if (p == 0)
		cerror("freeing blank tree!");
	else
		p->in.op = FREE;
}

/* walk the tree before the operands, with the value passed down: f(p, down, &down1, &down2) */
void
fwalk(NODE *p, void (*f)(NODE *, int, int *, int *), int down)
{
	int down1, down2;

again:
	down1 = down2 = 0;
	(*f)(p, down, &down1, &down2);
	switch optype(p->in.op) {
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
walkf(NODE *p, void (*f)(NODE *))
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
mkdope(void)
{
	register struct dopest *q;

	for (q = indope; q->dopeop >= 0; q++) {
		dope[q->dopeop] = q->dopeval;
		opst[q->dopeop] = q->opst;
	}
}

/* size of a type in bits (pointers 16, segmented 32); SHORT and USHORT do not occur (cparse maps them to int) */
char *rnames[] = {
#ifdef SEG
	"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "r13", "sp", "fp",
#else
	"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "fp",
#endif
	"f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7",
};

int
bsize(int t)
{
	switch (t) {
	case 2: case 12: return 8;			/* CHAR UCHAR */
	case 4: case 14: return 16;			/* INT UNSIGNED */
	case 5: case 6: case 15: return 32;		/* LONG FLOAT ULONG */
	case 7: return 64;				/* DOUBLE */
	}
	if ((t & 0x30) == 0x10)
#ifdef SEG
		return 32;				/* a pointer */
#else
		return 16;				/* a pointer */
#endif
	cerror("illegal bsize type: %%%.2x\n", t);
}

/* size of a type in words: long, float and unsigned long 2, double 4, everything else 1 */
int
szty(int t)
{
	switch (t) {
	case 4: case 10: case 14: return 1;
	case 5: case 6: case 15: return 2;
	case 7: return 4;
	}
	if ((t & 0x30) == 0x10)
#ifdef SEG
		return 2;
#else
		return 1;		/* no effect: the original has the test in both versions */
#endif
	return 1;
}

/* size of an argument in words (0x88f6): a structure argument by its size, a float counts as a double */
int
argsize(NODE *p)
{
	int t, u;

	if (p->in.op == STARG)
		return (unsigned)p->stn.stsize >> 1;
	if (p->in.op == NAME && p->in.type == 8)
		return p->in.sname->ssize / 16;
	t = u = szty(p->in.type == 6 ? 7 : p->in.type);
	return t;
}

/* the file and line of the intermediate file in front of an error message */
static void
prline(int c)
{
	(void)c;
	fprintf(stderr, "%s:%d: ", ifname, ifline);
}

/* the size in bytes of a type: 1, 2 or 4 */
int
tbytes(int t)
{
	switch (t) {
	case 5:
	case 15:
		return 4;
	case 2:
	case 12:
		return 1;
	case 3:
	case 13:
		return 2;
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
callreg(NODE *p)
{
	if (p && (p->in.type == 7 || p->in.type == 6))
		return 0x10;
	return 2;
}

void
dexit(int n)
{
	fflush(stdout);
	exit(n);
}

void
branch(int n)
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
extern void rfree(), rsreturn();
#endif

void
d3kjump(int l)
{
	if (l >= 0 && l < D3KMAX)
		d3kbits[l >> 3] |= 1 << (l & 7);
#ifdef OLD_D3J
	d3jsave(l);
#endif
}

int
d3knamed(int l)
{
	return l >= 0 && l < D3KMAX && (d3kbits[l >> 3] >> (l & 7) & 1);
}

void
d3kreset(void)
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
/* v0p: all jump records are forgotten */
void
d3jclear(void)
{
	register int i;

	for (i = 0; i < D3JMAX; i++)
		d3jtab[i].lab = -1;
}

/* v0p: the parameters known in registers at a conditional jump; the label gets them back when the code in front of it
   ends with a jump (its state is that of the jumps to it) */
void
d3jsave(int lab)
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
d3jlabel(int lab, int dead)
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
		if (!dead) {
			for (i = 0; i < j->n; i++)
				if (n->reg == j->reg[i] && n->cls == j->cls[i] && n->sym == j->sym[i])
					break;
			if (i < j->n)
				continue;
		}
		rsreturn(n->cls, n->reg);
	}
	if (!dead)
		return;
	for (i = 0; i < j->n; i++)
		if (d3jfree(j->cls[i], j->reg[i])) {
			r = getreg(j->cls[i], j->reg[i], 0, j->sym[i]);
			if (r == j->reg[i]) {
				rfree(r, j->cls[i] == 3 ? 5 : j->cls[i] == 1 ? 2 : 4);
				if (j->cls[i] == 3)
					d3jmark(j->cls[i], r);
			}
		}
}

/* the node of (cls, reg) got its parameter back at a label: it still needs its home in the frame (regargs) */
static void
d3jmark(int cls, int reg)
{
	register struct rs *n;

	for (n = rsuse; n; n = n->next)
		if (n->cls == cls && n->reg == reg)
			n->flags |= 0x4000;
}

/* the register reg of the class cls is on the free list */
static int
d3jfree(int cls, int reg)
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
ncopy(NODE *q, NODE *p)
{

	*q = *p;
}

/* ---- trees (the pool and the walkers are in comm3.c) ---- */

/* a copy of the tree; the registers it uses count as busy once more */
NODE *
tcopy(NODE *p)
{
	register NODE *q;
	register int r;

	ncopy(q = talloc(), p);
	r = p->tn.rval;
	if (p->in.op == REG)
		rbusy(r, p->in.type, p->in.sname, p->tn.lval);
	else if (p->in.op == OREG) {
		if (r >= 0x100) {
			rbusy((r & 0xFF) - 1, 0x14, p->in.sname, p->tn.lval);
			if (r & 0xFF)
				rbusy((r >> 8) - 1, 4, 0, 0L);	/* two arguments in the original: name and offset are what the registers hold; rbusy does not use them */
		} else if (p->tn.lval != 0 || p->in.sname != 0 || (p->in.op == OREG && p->stn.stsize < 0))
			rbusy(r, 4, p->in.sname, p->tn.lval);
		else
			rbusy(r, 0x14, p->in.sname, p->tn.lval);
	}
	switch optype(q->in.op) {
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
int
ispow2(long c)
{
	register int n;

	if (c <= 0 || (c & (c - 1)))
		return -1;
	for (n = 0; c > 1; c >>= 1, n++)
		;
	return n;
}

/* is p->tn.lval an operand that a one word instruction takes: an unnamed constant */
int
plainicon(NODE *p)
{
	return p->in.op == ICON && p->in.sname == 0 && p->tn.rval >= 0;
}

int revrel[] = { EQ, NE, GE, GT, LE, LT, UGE, UGT, ULE, ULT };
/*
 * The allocator of the original: a bump allocator on the break (sbrk), the symbol tables are reserved one behind the other and
 * allocinit() sets the break back. Here one arena replaces the break (malloc of the host uses the break itself); the blocks are
 * rounded to 8 bytes, a multiple of the size of the structures, so that tables stay contiguous.
 */
static char arena[4 << 20] __attribute__((aligned(16)));
static char *endbrk = arena + sizeof arena, *curbrk = arena;

char *
alloc(int n)
{
	register char *p;

	n = (n + 7) & ~7;
	if (curbrk + n >= endbrk)
		cerror("Code generator: out of space\n");
	p = curbrk;
	curbrk += n;
	return p;
}

/* does p point into the arena (a symbol) and not at a number that a node holds in its sname word, a label for instance? */
int
inarena(const void *p)
{
	return (const char *)p >= arena && (const char *)p < arena + sizeof arena;
}

void
allocinit(char *base)
{
	curbrk = base;
}

