/*
 * z22.c -- popt: register tracking and local peephole transformations
 *
 * regs[] describes what each register is known to contain:
 *   regs[0..15]   byte registers (0-7 rl0-rl7, 8-15 rh0-rh7)
 *   regs[16..31]  word registers r0-r14
 *   regs[32..47]  long registers rr0-rr14
 *   regs[48],[49] the two operands of the instruction being looked at
 * Each entry is the operand text of the location the register was loaded
 * from, or "" when unknown.
 */
#include <ctype.h>
#include "z2.h"

/* zero n bytes (the WEGA libc has no memset) */
clearmem(p, n)
	char *p;
	int n;
{
	int i;

	for (i = 0; i < n; i++)
		p[i] = 0;
}

/* zeroed memory from malloc (the WEGA libc has no calloc) */
char *zalloc(n)
	int n;
{
	char *p;
	int i;

	p = malloc(n);
	if (p == 0)
		return 0;
	for (i = 0; i < n; i++)
		p[i] = 0;
	return p;
}

#define TB	0x00		/* byte */
#define TW	0x10		/* word */
#define TL	0x20		/* long */
#define TQ	0x22		/* quad (only used for names) */

static char conloc[32];		/* memory location known to hold conval */
static char conval[32];
static char ccloc[32];		/* operand the condition codes describe */
static char buf1[64], buf2[64];

static int findreg();
static int findbyte();
static int findword();
static int findlong();
static int findcopy();
static int memreg();
static void setreg();
static void makereg();
static int dest();
static void killreg();
static void cleargroup();
static int findslot();
static void ccinval();
static char *regprefix();
static void repladdr();
static void savereg();
static char *findcon();
static int compare();
static void setcon();
static void setcc();
static int natural();
static int adrlen();
static int toofar();

/*
 * Like the original: a bump allocator on the break (sbrk).  heapbase is the break at the start (main), heapcur the
 * next free byte, heaplim the end of the core asked for so far (grown in steps of 2000 bytes).  main sets heapcur back
 * to heapbase in every pass.  The -d statistics report heaplim.
 */
int heapbase, heaplim, heapcur;

char *
alloc(n)
int n;
{
	char *p;

	n = (n + 1) & ~1;
	if ((unsigned)(heapcur + n) >= (unsigned)heaplim) {
		if (sbrk(2000) == (char *)-1) {
			fprintf(stderr, "C Optimizer: out of space\n");
			exit(1);
		}
		heaplim += 2000;
	}
	p = (char *)heapcur;
	heapcur += n;
	return p;
}


struct node *
nonlab(p)
struct node *p;
{
	while (p && (p->op == LABEL || p->op == KCOMMENT))
		p = p->forw;
	return p;
}

void
decref(p)
struct node *p;
{
	if (--p->refc <= 0) {
		nrlab++;
		p->back->forw = p->forw;
		p->forw->back = p->back;
	}
}

int
equstr(a, b)
char *a;
char *b;
{
	for (;;) {
		if (*a != *b)
			return 0;
		if (*b++ == 0)
			return 1;
		a++;
	}
}

static int
absval(n)
int n;
{
	return n < 0 ? -n : n;
}

int
equop(p1, p2)
struct node *p1;
struct node *p2;
{
	register char *a, *b;

	if (p1 == 0 || p2 == 0)
		return 0;
	if (p2->op != p1->op || p2->subop != p1->subop)
		return 0;
	if (p1->op > 0 && p1->op < 7)
		return 0;
	a = p1->code;
	b = p2->code;
	if (a == 0)
		return b == 0;
	if (b == 0)
		return 0;
	for (;;) {
		if (*b++ != *a)
			return 0;
		if (*a++ == 0)
			return 1;
	}
}

/* --------------------------------------------------------------- */

void
clearreg()
{
	int i;

	for (i = 0; i < NREGS; i++)
		regs[i][0] = 0;
	conloc[0] = 0;
	ccloc[0] = 0;
}

/* regs[a+b] := first operand of s (not when it is register indirect) */
static void
savereg(a, s, b)
int a;
char *s;
int b;
{
	char *p, *d0, *d;

	d0 = d = regs[a + b];
	ccinval(s);
	p = s;
	for (;;) {
		*d++ = *p;
		if (*p == 0)
			break;
		if ((*p == '@' || *p == '(') && p[1] == 'r') {
			*d0 = 0;
			return;
		}
		if (*p++ == ',')
			break;
	}
	*--d = 0;
}

/* which register of class `base' holds the value s? */
static int
findreg(base, s)
int base;
char *s;
{
	int i;

	for (i = 0; i < 15; i++) {
		if (regs[base + i][0] && equstr(regs[base + i], s))
			return (base == 0 && i > 8) ? i - 8 : i;
	}
	return -1;
}

static int
findbyte(n)
int n;
{
	static char hi[] = "rhx", lo[] = "rlx";
	int r;

	hi[2] = n + '0';
	r = findreg(0, hi);
	if (r >= 0)
		return r;
	lo[2] = n + '0';
	return findreg(0, lo);
}

static int
findword(n)
int n;
{
	static char pat[4];
	int r;

	if (n < 10) {
		pat[0] = 'r';
		pat[1] = n + '0';
		pat[2] = 0;
	} else {
		pat[0] = 'r';
		pat[1] = '1';
		pat[2] = ((n - 10) & 0xf) + '0';
		pat[3] = 0;
	}
	r = findreg(TW, pat);
	if (r >= 0)
		return r;
	if (n >= 8)
		return -1;
	return findbyte(n);
}

static int
findlong(n)
int n;
{
	static char pat[5];
	int r;

	pat[0] = 'r';
	pat[1] = 'r';
	if (n < 10) {
		pat[2] = n + '0';
		pat[3] = 0;
	} else {
		pat[2] = '1';
		pat[3] = ((n - 10) & 0xf) + '0';
		pat[4] = 0;
	}
	r = findreg(TL, pat);
	if (r >= 0)
		return r;
	r = findword(n);
	if (r >= 0)
		return r;
	return findword(n + 1);
}

/* find a register holding a copy of the register named s */
static int
findcopy(s, type)
char *s;
int type;
{
	int r;

	r = isreg(s);
	if (r < 0)
		return -1;
	switch (type) {
	case TB:
	case TW:
		return findword(r);
	case TL:
		return findlong(r);
	case TQ:
		return findlong(r + 2);
	}
	return -1;
}

/*
 * Does a register hold (part of) the memory item s?  Memory items are
 * "sym+off"; a register holding sym+k covers bytes k.. k+size-1.
 * Result: register number, plus 0x100 for a word register, 0x200 for a
 * long register; -1 when none.
 */
static int
memreg(s, size)
char *s;
int size;
{
	int n, off, roff, i, reg;
	int len;
	char *p;

	roff = 0;
	off = 0;
	for (p = s, len = 0; *p; p++, len++) {
		if (*p == '+' && p[1] != '$') {
			off = atoi(p + 1);
			break;
		}
	}
	n = size == 0 ? 1 : size / 8;
	for (i = 0; i < 15; i++) {
		reg = TL + i;
		if (regs[reg][0] == 0)
			continue;
		if (strncmp(regs[reg], s, len) != 0)
			continue;
		if (regs[reg][len] == '+')
			roff = atoi(&regs[reg][len + 1]);
		if (off > roff + 3)
			continue;
		if (roff >= off + n)
			continue;
		return i + 0x200;
	}
	for (i = 0; i < 15; i++) {
		reg = TW + i;
		if (regs[reg][0] == 0)
			continue;
		if (strncmp(regs[reg], s, len) != 0)
			continue;
		if (regs[reg][len] == '+')
			roff = atoi(&regs[reg][len + 1]);
		if (off > roff + 1)
			continue;
		if (roff >= off + n)
			continue;
		return i + 0x100;
	}
	for (i = 0; i < 15; i++) {
		reg = TB + i;
		if (regs[reg][0] == 0)
			continue;
		if (strncmp(regs[reg], s, len) != 0)
			continue;
		if (regs[reg][len] == '+')
			roff = atoi(&regs[reg][len + 1]);
		if (off > roff)
			continue;
		if (roff >= off + n)
			continue;
		return i;
	}
	return -1;
}

/* the location s of class `type' was modified: forget what it affects */
static void
setreg(s, type)
char *s;
int type;
{
	int r, r2, t;
	char *p;
	char name[8];

	ccinval(s);
	r = isreg(s);
	if (r >= 0) {
		killreg(r, type);
		for (;;) {
			r2 = findcopy(s, type);
			if (r2 < 0)
				break;
			if (type == TW || type == TB)
				killreg(r2, TW);
			else if (type == TL || type == TQ)
				killreg(r2, TL);
		}
	}
	for (p = s; *p; p++)
		if (*p == '(' || *p == '@')
			break;
	if (*p) {
		if (p[1] == 'f' && p[2] == 'p')
			goto mem;
		cleargroup(NREGS, 1);
		return;
	}
mem:
	r = memreg(s, type);
	if (r < 0)
		return;
	t = (r & 0x0ff0) >> 4;
	makereg(r & 0xf, name, t);
	setreg(name, t);
}

/* name of register n of the class `type' */
static void
makereg(n, buf, type)
int n;
char *buf;
int type;
{
	char *p;

	p = buf;
	*p++ = 'r';
	if (type == TB) {
		*p++ = 'l';
		n = n % 8;
	} else if (type == TL)
		*p++ = 'r';
	else if (type == TQ)
		*p++ = 'q';
	if (n >= 10) {
		*p++ = '1';
		n = n % 10;
	}
	*p++ = n + '0';
	*p = 0;
}

/* register class of the destination of an instruction */
static int
dest(p)
struct node *p;
{
	int sub, op;

	sub = p->subop;
	op = p->op;
	if (op == EXTS) {
		if (sub == 0)
			sub = SLONG;
	} else if (op == LDA)
#ifdef SPOPT
		sub = SLONG;		/* an address is a segmented (long) address */
#else
		sub = 0;
#endif
	if (sub == 0)
		return TW;
	if (sub == SBYTE)
		return TB;
	if (sub == SLONG)
		return TL;
	return TW;
}

/* forget register n of class `type' and everything that overlaps it */
static void
killreg(n, type)
int n;
int type;
{
	int r;
	char name[8];

	switch (type) {
	case TB:
		regs[n][0] = 0;
		if (n >= 8)
			n -= 8;
		regs[n + 16][0] = 0;
		if (n & 1)
			regs[n + 0x1f][0] = 0;
		else
			regs[n + 0x20][0] = 0;
		break;
	case TW:
		regs[n + 16][0] = 0;
		if (n < 8) {
			regs[n][0] = 0;
			regs[n + 8][0] = 0;
		}
		if (n & 1)
			regs[n + 0x1f][0] = 0;
		else
			regs[n + 0x20][0] = 0;
		break;
	case TL:
	case TQ:
		if (type == TQ)
			n += 2;
		regs[n + 0x20][0] = 0;
		if (n < 8) {
			regs[n][0] = 0;
			regs[n + 1][0] = 0;
			regs[n + 8][0] = 0;
			regs[n + 9][0] = 0;
		}
		regs[n + 16][0] = 0;
		regs[n + 0x11][0] = 0;
		break;
	}
	makereg(n / 4, name, TQ);
	r = findslot(name, TQ);
	if (r >= 0) {
		killreg(r, TQ);
		return;
	}
	makereg(n / 1, name, TL);
	r = findslot(name, TL);
	if (r >= 0) {
		killreg(r, TL);
		return;
	}
	makereg(n, name, TW);
	r = findslot(name, TW);
	if (r >= 0) {
		killreg(r, TW);
		return;
	}
	if (n < 8) {
		makereg(n, name, TB);
		r = findslot(name, TB);
		if (r >= 0)
			killreg(r, TB);
	}
}

/* forget the first n registers (but keep "#const" ones when keep) */
static void
cleargroup(n, keep)
int n;
int keep;
{
	int i;

	for (i = 0; i < n; i++)
		if (!keep || regs[i][0] != '#')
			regs[i][0] = 0;
	conloc[0] = 0;
}

/*
 * A node without operand text makes the original read the code bytes at
 * address 0 of its address space; keep that behaviour (it matters for
 * the length of ".word Lnnn" nodes).
 */
char nullcode[] = { 0x21, 0xf7, 0x76, 0xf6, 0 };

void
singop(p)
struct node *p;
{
	strcpy(RT1, p->code ? p->code : nullcode);
	RT2[0] = 0;
}

/*
 * Split the operands of p: RT1 = source (second operand, or the only
 * operand), RT2 = destination (first operand when there are two).
 */
void
dualop(p)
struct node *p;
{
	char *s, *d;

	s = p->code ? p->code : nullcode;
	d = RT1;
	while (*s && *s != ',')
		*d++ = *s++;
	*d = 0;
	RT2[0] = 0;
	if (*s++ == ',') {
		strcpy(RT2, RT1);
		strcpy(RT1, s);
	}
}

static int
findslot(s, base)
char *s;
int base;
{
	int i;

	if (*s == 0)
		return -1;
	for (i = 0; i < 15; i++)
		if (regs[base + i][0] && equstr(regs[base + i], s))
			return i;
	return -1;
}

/* register number of operand s, or -1 */
int
isreg(s)
char *s;
{
	int hi, n;
	char *p;
	int c;

	hi = -1;
	p = s;
	if (*p != 'r')
		return -1;
	if (p[1] == 'r' || p[1] == 'l' || p[1] == 'h' || p[1] == 'q') {
		if (p[1] == 'h')
			hi = 1;
		p++;
	}
	n = atoi(p + 1);
	if (n < 0 || n >= 15)
		return -1;
	if (n < 10) {
		if (isalnum((unsigned char)p[2]))
			return -1;
		if (p[2] == '(')
			return -1;
		if (hi == 1)
			return n + 8;
		return n;
	}
	c = (unsigned char)p[3];
	if (isalnum(c))
		return -1;
	if (c != '(')
		return n;
	return -1;
}

static void
ccinval(s)
char *s;
{
	if (equstr(s, ccloc))
		ccloc[0] = 0;
}

static char *
regprefix(n, type)
int n;
int type;
{
	if (type == TB)
		return n > 8 ? "rh" : "rl";
	if (type == TL)
		return "rr";
	return "r";
}

/*
 * Replace the operands of p by registers that already hold the same
 * values.  f: also look for the destination.
 */
static void
repladdr(p, f)
struct node *p;
int f;
{
	int t, rd, rs, op;
	char *pfx;

	t = dest(p);
	rd = f ? findslot(RT2, t) : -1;
	rs = findslot(RT1, t);
	if (rs < 0 && rd < 0)
		return;
	if (p->subop == SBYTE && (rs >= 8 || rd >= 8))
		return;
	if (RT2[0]) {
		buf1[0] = ',';
		strcpy(buf1 + 1, RT1);
		strcpy(buf2, RT2);
	} else {
		strcpy(buf1, RT1);
		buf2[0] = 0;
	}
	if (rs >= 0) {
		if (RT2[0]) {
			op = p->op;
			if ((op == CP || op == BIT || op == SET || op == RES) &&
			    isreg(RT1) < 0)
				return;
			pfx = regprefix(rs, t);
			sprintf(buf1 + 1, "%s%d", pfx, rs);
		} else {
			pfx = regprefix(rs, t);
			sprintf(buf1, "%s%d", pfx, rs);
		}
		if (p->subop == SLDK)
			p->subop = 0;
		nsaddr++;
	}
	if (rd >= 0) {
		pfx = regprefix(rd, t);
		sprintf(buf2, "%s%d", pfx, rd);
		nsaddr++;
	}
	if (RT2[0])
		p->code = copy(2, buf2, buf1);
	else
		p->code = copy(1, buf1, 0);
}

/*
 * Move .data blocks to the front of the function and drop redundant
 * segment switches.
 */
void
movedat()
{
#ifdef SPOPT
	/*
	 * Sections are introduced by ".psec [name]" followed by ".code" or ".data". The PSEC node
	 * gets the kind of the following directive in its subop. Every data section moves in front
	 * of the code; then a section header that repeats the previous one is dropped.
	 */
	struct node hd, *r10, *r11, *r13, *r14;
	char *prev = "";	/* the original clears the byte at the address held in a register */
	int cur;

	if (first.forw == 0)
		return;
	clearmem(&hd, sizeof hd);
	r10 = &hd;
	for (r14 = first.forw; r14; r14 = r14->forw) {
		if (r14->op == PSEC && r14->forw &&
		    (r14->forw->op == CODE || r14->forw->op == DATA))
			r14->subop = r14->forw->op;
		if (r14->subop != DATA)
			continue;
		for (r13 = r14->forw; r13; r13 = r13->forw) {
			if (r13->op == CODE)
				break;
			if (r13->op == PSEC && r13->forw &&
			    (r13->forw->op == CODE || r13->forw->op == DATA))
				r13->subop = r13->forw->op;
		}
		if (r13 == 0)
			break;
		r11 = r14->back;
		r14->back->forw = r13->forw;
		r13->forw->back = r11;
		r13->forw = 0;
		r10->forw = r14;
		r14->back = r10;
		r14 = r11;
		r10 = r13;
	}
	if (hd.forw) {
		r10->forw = first.forw;
		first.forw->back = r10;
		hd.forw->back = &first;
		first.forw = hd.forw;
	}
	cur = -1;
	for (r14 = first.forw; r14; r14 = r14->forw) {
		if (r14->subop != CODE && r14->subop != DATA)
			continue;
		r13 = r14->forw;
		if (r13) {
			if (r13->op == r14->subop)
				r13 = r13->forw;
			if (r13 && r13->forw && r13->forw->op == r13->subop &&
			    (r13->subop == CODE || r13->subop == DATA)) {
				r14->back->forw = r13;
				r13->back = r14->back;
				r14 = r13;
			}
		}
		if (r14->subop == cur && equstr(prev, r14->code)) {
			if (r14->forw->op == r14->subop)
				r13 = r14->forw->forw;
			else
				r13 = r14->forw;
			r14->back->forw = r13;
			r13->back = r14->back;
			r14 = r14->back;
		} else {
			prev = r14->code;
			cur = r14->subop;
		}
	}
#else

	struct node hd, *r1, *r4, *r6, *r7;
	int cur;

	if (first.forw == 0)
		return;
	r7 = &hd;
	for (r1 = first.forw; r1; r1 = r1->forw) {
		if (r1->op != DATA)
			continue;
		for (r4 = r1->forw; r4 && r4->op != CODE; r4 = r4->forw)
			;
		if (r4 == 0)
			break;
		r6 = r1->back;
		if (r4->forw->op == PSEC)
			r4 = r4->forw;
		r1->back->forw = r4->forw;
		r4->forw->back = r6;
		r4->forw = 0;
		r7->forw = r1;
		r1->back = r7;
		r1 = r6;
		r7 = r4;
	}
	if (hd.forw) {
		r7->forw = first.forw;
		first.forw->back = r7;
		hd.forw->back = &first;
		first.forw = hd.forw;
	}
	cur = -1;
	for (r1 = first.forw; r1; r1 = r1->forw) {
		if (r1->op != CODE && r1->op != DATA)
			continue;
		r4 = r1->forw;
		if (r4) {
			if (r4->op == PSEC)
				r4 = r4->forw;
			if (r4->op == CODE || r4->op == DATA)
				r1->op = r4->op;
		}
		if (cur == r1->op) {
			if (r1->forw->op == PSEC)
				r4 = r1->forw->forw;
			else
				r4 = r1->forw;
			r1->back->forw = r4;
			r4->back = r1->back;
			r1 = r1->back;
		} else
			cur = r1->op;
	}
#endif
}

/*
 * p is a jump; if the target tests a value whose outcome is known here,
 * jump directly to the destination of that test.
 */
static void
redunbr(p)
struct node *p;
{
	struct node *t;
	char *a, *b;
	int ty, r;

	if (p->ref == 0)
		return;
	t = nonlab(p->ref);
	if (t->op == TST) {
		singop(t);
		savereg(0x31, "#0", 0);
	} else if (t->op == CP)
		dualop(t);
	else
		return;
	if (t->forw->op != CJP)
		return;
	ty = dest(t);
	a = findcon(0x30, ty);
	b = findcon(0x31, ty);
	t = t->forw;
	r = compare(t->subop, b, a);
	if (r <= 0)
		return;
	nredunj++;
	nchange++;
	decref(p->ref);
	p->ref = t->ref;
	p->labno = t->labno;
	p->ref->refc++;
}

/* known value of the operand held in regs[n] */
static char *
findcon(n, t)
int n;
int t;
{
	char *p;
	int r;

	p = regs[n];
	if (*p == '#')
		return p;
	r = isreg(p);
	if (r >= 0)
		return regs[r + t];
	if (equstr(p, conloc))
		return conval;
	return p;
}

/*
 * Evaluate the condition cc between the constants a and b (strings
 * "#n" or "#n+sym"): 1 true, 0 false, -1 unknown.
 */
static int
compare(cc, a, b)
int cc;
char *a;
char *b;
{
	char *p, *q;
	short va, vb;

	p = a;
	q = b;
	if (*p++ != '#')
		return -1;
	if (*q++ != '#')
		return -1;
	vb = 0;
	while ((char)*q >= '0' && (char)*q <= '9')
		vb = vb * 10 + *q++ - '0';
	va = 0;
	while ((char)*p >= '0' && (char)*p <= '9')
		va = va * 10 + *p++ - '0';
	if (*p == '+')
		p++;
	if (*q == '+')
		q++;
	for (;;) {
		if (*p++ != *q)
			return -1;
		if (*q++ == 0)
			break;
	}
	switch (cc) {
	case 0: return va == vb;
	case 1: return va != vb;
	case 2: return va <= vb;
	case 3: return va >= vb;
	case 4: return va < vb;
	case 5: return va > vb;
	case 6: return (unsigned short)va < (unsigned short)vb;
	case 7: return (unsigned short)va > (unsigned short)vb;
	case 8: return (unsigned short)va <= (unsigned short)vb;
	case 9: return (unsigned short)va >= (unsigned short)vb;
	}
	return -1;
}

static void
setcon(val, loc)
char *val;
char *loc;
{
	if (*val == '#' && natural(loc)) {
		strcpy(conloc, loc);
		strcpy(conval, val);
		/*
		 * In the original conloc has 20 bytes and the statistic counters follow it
		 * (ndirect, then nbrbr, 16 bit big endian): a longer name overwrites them.
		 */
		{
			int k, n = strlen(loc) + 1;

			for (k = 20; k < n && k < 24; k++) {
				int *w = k < 22 ? &ndirect : &nbrbr;
				int sh = (k & 1) ? 0 : 8;

				*w = (*w & ~(0xff << sh) & 0xffff) | ((unsigned char)loc[k] << sh);
			}
		}
	}
}

static void
setcc(s)
char *s;
{
	register char *p, *q;

	if (!natural(s))
		ccloc[0] = 0;
	else
		for (p = s, q = ccloc; (*q++ = *p++) != 0;)
			;
}

/* a location that stays the same while registers change */
static int
natural(s)
char *s;
{
	char *p;

	if (*s == '@')
		return 0;
	for (p = s; *p; p++)
		;
	if (p[-1] == ')') {
		if (p[-2] != 'p')
			return 0;
		if (p[-3] != 'f')
			return 0;
	}
	return 1;
}

/* --------------------------------------------------------------- */

/* order conditional jumps that skip over a jump so the shorter one is taken */
int
jumpsw()
{
	struct node *p, *p1;
	int n, count, d1, d2, subop;
	struct node *t;
	int tl;

	count = 0;
	n = 0;
	for (p = first.forw; p; p = p->forw)
		p->refc = ++n;
	for (p = first.forw; p; p = p1) {
		p1 = p->forw;
		if (p->op == CJP && p1->op == JP && p->ref && p1->ref) {
			d1 = absval(p1->refc - p1->ref->refc);
			d2 = absval(p->refc - p->ref->refc);
			if (d2 > d1 && p->ref != p1->ref) {
				subop = p->subop;
				p->subop = revbr[subop];
				t = p1->ref;
				p1->ref = p->ref;
				p->ref = t;
				tl = p1->labno;
				p1->labno = p->labno;
				p->labno = tl;
				nrevbr++;
				count++;
			}
		}
	}
	return count;
}

/* decrement-and-branch-back loops become djnz */
void
shortjump()
{
	struct node *p, *p1, *q;
	char *s;

	for (p = &first; (p1 = p->forw) != 0; p = p1) {
		dualop(p);
		if (p->op == DEC && equstr(RT1, "#1") &&
		    isreg(p->code) >= 0 &&
		    p1->op == CJP && p1->subop == 1 &&
		    p->refc >= p1->ref->refc) {
			q = p1->ref;
			while (ilen(q) == 0)
				q = q->forw;
			if (q->op == CJP || q->op == TST)
				continue;
			if (toofar(p1))
				continue;
			p->labno = p1->labno;
			p->op = SOB;
			for (s = p->code; *s && *s != ','; s++)
				;
			*s = 0;
			p1->forw->back = p;
			p->forw = p1->forw;
			nsob++;
		}
	}
}

static int
toofar(p)
struct node *p;
{
	struct node *q;
	int sum;

	sum = 0;
	for (q = p->ref; q; q = q->forw) {
		if (q == p)
			break;
		sum += ilen(q);
	}
	return sum >= 256;
}

/* size in bytes of the instruction in node p */
int
ilen(p)
struct node *p;
{
	int op;

	op = p->op;
	switch (op) {
	case CJP:
	case JP:
		if (p->code == 0)
#ifdef SPOPT
			return 6;
#else
			return 4;
#endif
		return adrlen(p->code, p->subop);
	case LABEL: case DLABEL: case OBRACE:
	case CODE: case DATA: case WORD: case EVEN:
	case LONG: case COMMENT: case CBRACE: case PSEC: case KCOMMENT:
		return 0;
	case SRA: case SLA: case SDA: case SRL: case SLL: case SDL:
		return 4;
	case RET:
		return 2;
	case LD:
		if (p->subop == SLDK)
			return 2;
		dualop(p);
		if (isreg(RT2) < 0) {
			if (isreg(RT1) >= 0)
				return adrlen(RT2, p->subop);
			return adrlen(RT2, p->subop) + (p->subop == SLONG ? 4 : 2);
		}
		if (p->subop == SBYTE && RT1[0] == '#')
			return 2;
		return adrlen(RT1, p->subop);
	case BIT: case SET: case RES:
		dualop(p);
		if (isreg(RT1) >= 0)
			return 4;
		return adrlen(RT2, p->subop);
	case CP:
		dualop(p);
		if (isreg(RT2) >= 0)
			return adrlen(RT1, p->subop);
		return adrlen(RT2, p->subop) + 2;
	case LDM:
		dualop(p);
		if (isreg(RT2) < 0)
			return adrlen(RT2, p->subop) + 2;
		return adrlen(RT1, p->subop) + 2;
	case INC: case DEC: case POP: case RL: case RR:
		dualop(p);
		return adrlen(RT2, p->subop);
	}
	dualop(p);
	return adrlen(RT1, p->subop);
}

static int
adrlen(s, op)
char *s;
int op;
{
	if (*s == 0)
		return 2;
	if (isreg(s) >= 0)
		return 2;
	if (*s == '@')
		return 2;
#ifdef SPOPT
	if (*s == '#')
		return op == SLONG ? 6 : 4;
	{
		char *s0 = s;

		for (; *s; s++) {
			if (*s == '|')
				return 4;
			if (*s == '(') {	/* x(rrN): a register in the index part is a short address */
				int r;

				*s = 0;
				r = isreg(s0);
				*s = '(';
				return r >= 0 ? 4 : 6;
			}
		}
	}
	return 6;
#else
	if (*s == '#' && op == SLONG)
		return 6;
	return 4;
#endif
}

/* --------------------------------------------------------------- */

int isused();

/*
 * rmove: walk the code of the function and keep track of what the
 * registers contain; remove redundant loads, merge instructions into
 * push/pop, replace shifts by cheaper sequences and so on.
 */
void
rmove()
{
	struct node *p, *p1, *n;
	int t, r11, r12, r10, r, op;
	char b1[24], b2[24];
	char *s;

	flag136a = 0;
	p = first.forw;
	while (p) {
		p1 = p->forw;
		op = p->op;
		if ((unsigned)(op - 1) > 0x33)
			goto reset;
		switch (op) {
		case CJP:
			goto do_cjp;
		case JP:
			redunbr(p);
			goto reset;
		case LD:
			goto do_ld;
		case CLR: case COM: case NEG: case EXTS:
			goto do_clr;
		case INC:
			goto do_inc;
		case DEC:
			goto do_dec;
		case TST:
			goto do_tst;
		case SRA:
			goto do_sra;
		case SLA: case SLL:
			goto do_sla;
		case SRL:
			goto do_srl;
		case CP: case BIT:
			goto do_cp;
		case ADD: case SUB: case MUL: case DIV: case SDA: case SDL:
			goto do_arith;
		case AND: case OR:
			goto do_and;
		case CALL:
			flag136a = 1;
			cleargroup(0x32, 0);
			goto reset;
		case PUSH:
			goto do_push;
		case POP:
			goto do_pop;
		case LDA:
			goto do_lda;
		case KCOMMENT:
			goto next;
		default:
			goto reset;
		}

	do_ld:
		dualop(p);
		t = dest(p);
		r12 = findslot(RT1, t);
		if (r12 >= 0 && r12 == isreg(RT2) && p1->op != CJP &&
		    p1->op != EXTS)
			goto del;
		if (equstr(RT1, RT2))
			goto del;
		if (equstr(RT1, "#0") && p->subop != SLDK) {
			if (isreg(RT2) >= 0) {
				if (p->subop != SBYTE)
					p->subop = SLDK;
				goto ld_chk;
			}
			p->op = CLR;
			strcpy(p->code, RT2);
			singop(p);
			goto clr_tail;
		}
		if (p->subop == SLDK) {
			dualop(p);
			r10 = isreg(RT2);
			if (r10 & 1) {
				r10--;
				makereg(r10, buf1, TL);
				setreg(buf1, TL);
			} else
				setreg(RT2, TL);
		}
		repladdr(p, 0);
	ld_chk:
		r11 = isreg(RT2);
#ifdef SPOPT
		if (r11 >= 0 && !isused(p, RT2) &&
		    (r11 != 8 || p->back->op != CALL))
			goto del;
#else
		if (r11 >= 0 && !isused(p, RT2))
			goto del;
#endif
		r12 = isreg(RT1);
		t = dest(p);
		setreg(RT2, t);
		if (r12 >= 0) {
			if (r11 >= 0) {
				savereg(r11, regs[r12 + t], t);
				savereg(r12, RT2, t);
				if (t == TL) {
					savereg(r11, regs[r12 + TW], TW);
					savereg(r11 + 1, regs[r12 + 0x11], TW);
					makereg(r11, b1, TW);
					savereg(r12, b1, TW);
					makereg(r11 + 1, b1, TW);
					savereg(r12 + 1, b1, TW);
				}
			} else
				savereg(r12, RT2, t);
		} else if (r11 >= 0)
			savereg(r11, RT1, t);
		else
			setcon(RT1, RT2);
		ccinval(RT1);
		goto next;

	del:
		p->back->forw = p1;
		p1->back = p->back;
		redunm++;
		nchange++;
		goto next;

	do_clr:
		singop(p);
		if (p->op == EXTS && p->subop == SLONG) {
			t = dest(p);
			setreg(RT1, t);
			setreg(RT1, TQ);
			goto clr_cc;
		}
	clr_tail:
		t = dest(p);
		setreg(RT1, t);
	clr_cc:
		if (p->op != CLR) {
			setcc(RT1);
			goto next;
		}
		r12 = isreg(RT1);
		if (r12 < 0) {
			setcon("#0", RT1);
			goto next;
		}
		t = dest(p);
		savereg(r12, "#0", t);
		goto next;

	do_inc:
		dualop(p);
		n = p->back;
		r12 = isreg(RT2);
		if (r12 < 0 || n == 0 || n->op != LD)
			goto incfail;
		if (!((equstr(RT1, "#2") && n->subop == 0) ||
		      (equstr(RT1, "#4") && n->subop == SLONG)))
			goto incfail;
		dualop(n);
#ifdef SPOPT
		if (RT1[0] == '@' && r12 != isreg(RT2) &&
		    isreg(RT1 + 1) == r12 - 1) {	/* rr14 is the stack pointer r15 */
#else
		if (RT1[0] == '@' && r12 != isreg(RT2) &&
		    r12 == isreg(RT1 + 1)) {
#endif
			n->forw = p->forw;
			p->forw->back = n;
			p = n;
			n->op = POP;
			npushpop++;
			nchange++;
		}
		/* falls through */
	redual:
		dualop(p);
		goto disp;

	do_dec:
		dualop(p);
		r12 = isreg(RT2);
		if (r12 < 0 || p1 == 0 || p1->op != LD)
			goto incfail;
		if (!((equstr(RT1, "#2") && p1->subop == 0) ||
		      (equstr(RT1, "#4") && p1->subop == SLONG)))
			goto incfail;
		dualop(p1);
#ifdef SPOPT
		if (RT2[0] == '@' && isreg(RT2 + 1) == r12 - 1) {
#else
		if (RT2[0] == '@' && r12 == isreg(RT2 + 1)) {
#endif
			if (RT1[0] == '#' && p1->subop == SLONG)
				goto redual;
			p->back->forw = p1;
			p1->back = p->back;
			p = p1;
			p1->op = PUSH;
			npushpop++;
			nchange++;
		}
		goto redual;

	incfail:
#ifdef SPOPT
		goto reset;		/* spopt forgets everything at an inc/dec that is not a push/pop */
#else
		goto disp;
#endif

	do_tst:
		singop(p);
		repladdr(p, 0);
		ccinval(RT1);
		if (p->back->subop != p->subop)
			goto next;
		if (!equstr(RT1, ccloc))
			goto next;
		p->back->forw = p1;
		p1->back = p->back;
		p = p->back;
		nrtst++;
		nchange++;
		goto next;

	do_cp:
		dualop(p);
		ccinval(RT1);
		ccinval(RT2);
		if (p->op == CP) {
			if (equstr(RT2, "#0"))
				goto cp_chk;
			if (equstr(RT1, "#0")) {
				strcpy(RT1, RT2);
				goto cp_chk;
			}
			goto cp_end;
		cp_chk:
			if (p1->op != CJP)
				goto cp_end;
			if (p1->subop != 1 && p1->subop != 0)
				goto cp_end;
			if (p1->forw->op == CJP)
				goto cp_end;
			p->op = TST;
			RT2[0] = 0;
			p->code = copy(1, RT1, 0);
			nchange++;
			nsaddr++;
		}
	cp_end:
		if (p->op == TST)
			repladdr(p, 0);
		else {
			dualop(p);
			repladdr(p, 1);
		}
		goto cc_clear;

	do_and:
		dualop(p);
		r10 = isreg(RT2);
		if (r10 & 1) {
			r10--;
			makereg(r10, buf1, TL);
			setreg(buf1, TL);
		} else
			setreg(RT2, TL);
		goto arith1;
	do_arith:
	arith1:
		dualop(p);
		if (p->op == AND && equstr(RT1, "#0") && p->subop == 0) {
			p->op = LD;
			p->subop = SLDK;
			goto do_clr;
		}
		if (p->op == OR && equstr(RT1, "#0"))
			goto noop;
		if (p->op != AND)
			goto disp;
		if (equstr(RT1, "#-1") || equstr(RT1, "#%(8)177777") ||
		    equstr(RT1, "#65535"))
			goto noop;
		goto disp;
	noop:
		if (p1->op == CJP)
			goto disp;
		p->back->forw = p1;
		p1->back = p->back;
		goto next;

	do_srl:
		dualop(p);
		if (p->subop == SLONG)
			goto disp;
		if (equstr(RT1, "#1")) {
			p->op = RES;
			sprintf(p->code, "%s,#0", RT2);
			n = (struct node *)alloc(14);
			n->op = RR;
			strcat(RT2, ",#1");
			n->code = copy(1, RT2, 0);
			goto shins;
		}
		goto sh8;

	do_sra:
		dualop(p);
	sh8:
		if (p->subop != 0)
			goto disp;
		if (!equstr(RT1, "#8"))
			goto disp;
		r12 = isreg(RT2);
		if (r12 < 0 || r12 >= 8)
			goto disp;
		n = (struct node *)alloc(14);
		if (p->op == SRA) {
			n->code = copy(1, "rX", 0);
			n->op = EXTS;
			strcpy(n->code, RT2);
		} else {
			n->code = copy(1, "rhX", 0);
			n->op = CLR;
			sprintf(n->code, "rh%d", r12);
		}
		p->op = LD;
		p->subop = SBYTE;
		p->code = copy(1, "rlX,rhX", 0);
		sprintf(p->code, "rl%d,rh%d", r12, r12);
	shins:
		n->subop = p->subop;
		p1->back = n;
		n->forw = p->forw;
		p->forw = n;
		n->back = p;
		nshft++;
		nchange++;
		goto redual;

	do_sla:
		dualop(p);
		if (p->subop == SLONG)
			goto disp;
		if (equstr(RT1, "#1")) {
			p->op = ADD;
			p->code = copy(1, "rXX,rXX", 0);
			sprintf(p->code, "%s,%s", RT2, RT2);
			nshft++;
			nchange++;
			t = dest(p);
			setreg(RT2, t);
			setcc(RT2);
			goto next;
		}
		if (p->subop != 0)
			goto disp;
		if (!equstr(RT1, "#8"))
			goto disp;
		r12 = isreg(RT2);
		if (r12 < 0 || r12 >= 8)
			goto disp;
		p->op = LD;
		p->subop = SBYTE;
		p->code = copy(1, "rhX,rlX", 0);
		sprintf(p->code, "rh%d,rl%d", r12, r12);
		n = (struct node *)alloc(14);
		n->op = CLR;
		n->code = copy(1, "rlxx", 0);
		sprintf(n->code, "rl%d", r12);
		goto shins;

	do_push:
		n = p->back;
		dualop(p);
		if (n->op == LD && n->subop == p->subop &&
		    isreg(RT1) >= 0 && !isused(p, RT1)) {
			strcpy(b1, RT1);
			strcpy(b2, RT2);
			dualop(n);
			if (equstr(RT2, b1)) {
				if (p->subop == SLONG && RT1[0] == '#')
					goto push_end;
				strcat(b2, ",");
				p->code = copy(2, b2, RT1);
				t = dest(p);
				r = isreg(b1);
				killreg(r, t);
				n->back->forw = p;
				p->back = n->back;
				npushdirect++;
				nchange++;
			}
		push_end:
			dualop(p);
		}
		if (isreg(RT1 + 1) < 0)
			goto disp;
		goto kill_all;

	do_pop:
		dualop(p);
		if (p1->op == LD && p1->subop == p->subop) {
			strcpy(b1, RT1);
			strcpy(b2, RT2);
			if (isreg(RT1 + 1) >= 0 && !isused(p1, RT2)) {
				dualop(p1);
				if (equstr(RT1, b2)) {
					strcat(RT2, ",");
					p->code = copy(2, RT2, b1);
					t = dest(p);
					r = isreg(b2);
					killreg(r, t);
					p->forw = p1->forw;
					p1->forw->back = p;
					npushdirect++;
					nchange++;
				}
				dualop(p);
			}
		}
		if (isreg(RT1 + 1) >= 0)
			goto kill_all;
		goto disp;
	kill_all:
		cleargroup(0x32, 1);
		goto disp;

	do_lda:
		dualop(p);
		if (p1->op != LD || p1->subop != 0)
			goto lda_end;
		strcpy(b1, RT1);
		strcpy(b2, RT2);
		dualop(p1);
		r = isreg(RT2);
		if (r < 0) {
			if (RT2[0] != '@')
				goto lda_end;
			if (isreg(RT1) < 0)
				goto lda_end;
			if (strncmp(RT2 + 1, b2, strlen(b2)) != 0)
				goto lda_end;
			if (isused(p1, b2))
				goto lda_end;
			p->op = LD;
			strcat(b1, ",");
			p->code = copy(2, b1, RT1);
			goto lda_done;
		}
		if (isused(p1, b2))
			goto lda_end;
		if (!equstr(RT1, b2)) {
			if (RT1[0] != '@')
				goto del;
			if (strncmp(RT1 + 1, b2, strlen(b2)) != 0)
				goto del;
		}
		if (RT1[0] == '@')
			p->op = LD;
		strcat(RT2, ",");
		p->code = copy(2, RT2, b1);
	lda_done:
		p->forw = p1->forw;
		p1->forw->back = p;
		nlda++;
		nchange++;
	lda_end:
		dualop(p);
		t = dest(p);
		setreg(RT2, t);
		goto next;

	do_cjp:
		n = p->back;
		if (n->op != TST && n->op != CP)
			goto cc_clear;
		if (n->op == TST) {
			singop(n);
			savereg(0x31, "#0", 0);
		} else
			dualop(n);
		t = dest(n);
		{
			char *a, *b;

			a = findcon(0x30, t);
			b = findcon(0x31, t);
			r12 = compare(p->subop, b, a);
		}
		if (r12 == 0) {
			if (p1->op == CJP || p1->op == EXTS) {
				p->back->forw = p1;
				p1->back = p->back;
			} else {
				p->back->back->forw = p1;
				p1->back = p->back->back;
			}
			decref(p->ref);
			p = p->back->forw;
			nchange++;
			goto cc_clear;
		}
		if (r12 > 0) {
			p->op = JP;
			p->subop = 0;
			p->back->back->forw = p;
			p->back = p->back->back;
			p = p->back;
			nchange++;
		}
		goto cc_clear;

	disp:
		/* common tail for instructions that change a register */
		switch (p->op) {
		case INC: case DEC: case SRA: case SLA: case SRL: case SLL:
			if (equstr(RT2, conloc))
				conloc[0] = 0;
			break;
		default:
			repladdr(p, 0);
		}
		ccinval(RT1);
		if (p->op == DIV || p->op == MUL) {
			if (p->subop == SLONG) {
				t = dest(p);
				setreg(RT2, t);
				setreg(RT2, TQ);
			} else {
				t = dest(p) + 0x10;
				setreg(RT2, t);
			}
			ccloc[0] = 0;
			goto next;
		}
		t = dest(p);
		setreg(RT2, t);
		setcc(RT2);
		goto next;

	cc_clear:
		ccloc[0] = 0;
		goto next;

	reset:
		clearreg();
	next:
		p = p->forw;
	}
}
