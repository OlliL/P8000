/*
 * allo3.c - allocation of the registers a table entry needs, and the rewrite of the tree afterwards
 * (codgen / scodgen)
 *
 * Reconstructed from /lib/codgen: rbusy 0x66bc, freeparm 0x6792, allo 0x67e8, freetemp 0x6ba6, rclass 0x6bf4, allocreg 0x6c4e,
 * freereg 0x6d5a, usable 0x6e78, rfreenode 0x6efa, rfree 0x6f76, prcookie 0x6fca (debug), reclaim 0x7058,
 * tfree 0x873c, fwalk 0x87ec, tcopy 0x8a24, rfind 0x96e6, rfreeall 0x9696.
 * Not done: nxwork (0x98e4, the action selected by the field nx of the table entry), tmpcleanup (0x9308),
 * rmove (0x3cf2).
 */
#include "cg.h"

extern int cerror();
extern char *calloc();
extern struct node *getlr();
extern int tshape();
extern int szty();
extern int getreg();
extern void rsreturn();
extern void nxwork();	/* 0x98e4 */
extern void tmpcleanup();			/* 0x9308 */
extern void rmove();	/* 0x3cf2 */
void rfreeall();


long tmpoff;		/* 0x69d4: the next free temporary (bits) */
long maxoff;		/* 0x8fcc: the largest frame offset so far */
long paroff;		/* 0x8e0c: next free place for the parameters of a call (bits) */
long parmax;		/* 0x69d8 */
long baseoff;		/* 0x0050 */
long maxtemp;		/* 0x0054 */
int tflag;		/* 0x35d0: set by the template macro T */

struct node resc[4];	/* the work registers RESC1 .. RESC3 (0x8f24); the contents stay from call to call; a 4th entry because allo reads one past the last */

#define RLEFT	01
#define RRIGHT	02
#define RESC1	04
#define RESC2	010
#define RESC3	020
#define RESCC	04000
#define RESC	02000		/* rewrite & 0x400: the second register of a long result */
#define RNULL	0
#define RNOP	010000
#define FOREFF	01

/* ---- register classes ------------------------------------------------------------------------------- */

int
rclass(type, reg)
	int type;
	int reg;
{
	if (type == 6 || type == 7)
		return reg > 0x13 ? 8 : 4;
	if (szty(type) == 2)
		return reg > 7 ? 7 : 3;
	return reg > 7 ? 6 : 2;
}

/* the status node of the register reg of class cls on the use list */
struct rs *
rfind(reg, cls)
	int reg;
	int cls;
{
	struct rs *n;

	for (n = rsuse; n; n = n->next)
		if (n->reg == reg && n->cls == cls)
			return n;
	return 0;
}

/* count the register of an operand as busy; sym and lval are only passed on */
void
rbusy(reg, type, sym, lval)
	int reg;
	int type;
	struct sym *sym;
	long lval;
{
	struct rs *n, *prev;
	int cls = rclass(type, reg), l;

	if (cls == 6 || cls == 7 || cls == 8)
		return;
	for (prev = n = rsuse; n; prev = n, n = n->next)
		if (n->cls == cls && n->reg == reg) {
			n->busy++;
			return;
		}
	if (cls != 2)
		cerror("rbusy: register to be made busy not found on rslist");
	l = reg & 1 ? reg - 1 : reg;		/* the long register that holds the word register */
	for (prev = n = rsuse; n; prev = n, n = n->next)
		if (n->cls == 3 && n->reg == l) {
			rsreturn(3, l);
			if (getreg(2, reg, 0x1000, 0) != reg)
				cerror("rbusy: register %d not free", reg);
			/* the original counts the node in front of the one it found */
			prev->busy++;
			return;
		}
	cerror("rbusy: register to be made busy not found on rslist");
}

/* the register is no longer needed by one more operand */
void
rfree(reg, type)
	int reg;
	int type;
{
	struct rs *n;
	int cls = rclass(type, reg);

	if (reg == 15)
		return;
	for (n = rsuse; n; n = n->next)
		if (n->cls == cls && n->reg == reg) {
			if (n->busy > 0)
				n->busy--;
			return;
		}
}

/* free the registers an operand node holds */
void
rfreenode(p)
	struct node *p;
{
	int r = p->rval;

	if (p->op == REG) {
		rfree(r, p->type);
		return;
	}
	if (p->op != OREG)
		return;
	if (r >= 0x100) {			/* base and index register */
		rfree((r >> 8) - 1, 0x14);
		if (r & 0xFF)
			rfree((r & 0xFF) - 1, 4);
		return;
	}
	if (p->lval != 0 || p->sname != 0 || p->stsize < 0)
		rfree(r, 4);
	else
		rfree(r, 0x14);
}

/* ---- temporaries ------------------------------------------------------------------------------------ */

/* k words of temporary space for parameters (the area behind the parameters of a call) */
long
freeparm(k)
	int k;
{
	long t = tmpoff;

	tmpoff += (long)k << 4;
	if (tmpoff > maxoff)
		cerror("paroff exceeds parmax, %d, %ld", (int)(tmpoff & 0xFFFF) / 8, maxoff / 8);
	return t;
}

/* k words of temporary space in the frame */
long
freetemp(k)
	int k;
{
	long t = paroff;

	paroff += (long)k << 4;
	if (paroff > parmax)
		parmax = paroff;
	if (paroff - baseoff > maxtemp)
		maxtemp = paroff - baseoff;
	return t;
}

/* ---- getting registers ------------------------------------------------------------------------------- */

/* the register class for a result of type of a node: float registers (16) for float, else word registers (2) */
static int
rcookie(p)
	struct node *p;
{
	return p->type == 7 || p->type == 6 ? 0x10 : 2;
}

/* does the operand on side of p already live in register r (or is its long register r - 1)? */
int
usable(p, side, r)
	struct node *p;
	int side;
	int r;
{
	struct node *q = getlr(p, side);
	int rv;

	if (q->op == STAR)
		q = q->left;
	rv = q->rval;
	if (q->op == OREG)
		return rv == r;
	if (q->op != REG)
		return 0;
	if (rv == r && (r != 0 || (q->type & 0x30) != 0x10))
		return 1;
	return szty(q->type) == 2 && rv + 1 == r;
}

/* a register that the operand of the result can reuse */
int
freereg(p, r, cookie)
	struct node *p;
	int r;
	int cookie;
{
	int reg, cls;

	if (r == -1) {
		for (reg = 0; reg <= 7; reg++)
			if (usable(p, 'L', reg) > 0) {
				rfree(reg, p->type);
				cls = rclass(p->type, reg);
				return getreg(cls, reg, 0x1000, p->sname);
			}
		return -1;
	}
	if (usable(p, 'L', r) <= 0)
		return -1;
	rfree(r, p->type);
	cls = rclass(p->type, r);
	return getreg(cls, r, 0x1000, p->sname);
}

/* a register for the result of p; cookie says what the caller wants */
int
allocreg(p, cookie)
	struct node *p;
	int cookie;
{
	int want, cls, r;

	if (dope[p->op] & 0x400)
		want = rcookie(p);
	else if (!(p->rall & 0x1000) && (p->rall & 0x2000))
		want = -1;
	else
		want = p->rall & ~0x1000;
	if (p->sname == fpsym1)
		fpsym1->soffset = p->lval;
	else if (p->sname == fpsym2)
		fpsym2->soffset = p->lval;
	if (((cookie & 0xF) != 0 && szty(p->type) == 2) || (p->op == STASG && p->stsize == 4) ||
	    (dope[p->op] & 0x80))
		cls = 3;
	else
		cls = rclass(p->type, want);
	r = getreg(cls, want, 0x1000, p->sname);
	if (r == -1) {
		r = freereg(p, want, cookie);
		if (r == -1)
			cerror("register jam");
	}
	return r;
}

/* number of words a temporary for the result of p needs */
static int
tempwords(p, nfield)
	struct node *p;
	int nfield;
{
	int o = p->op;

	if (o == STCALL || o == STARG || o == USTCALL || o == STASG)
		return ((p->stsize << 3) + 15) >> 4;
	if (p->type == 5 || p->type == 15)
		return 2;
	if ((p->type & 0x30) == 0x10)
		return 1;
	if (p->type == 7)
		return 4;
	if (p->type == 6)
		return 2;
	return nfield;
}

/*
 * Get the resources of the table entry q for the tree p: the registers (needs bits 0-3: A registers,
 * 4-7: B registers), temporaries in the frame (8-9) and parameter temporaries (10-11), in resc[]. Returns 0
 * when a register is not available.
 */
int
allo(p, q)
	struct node *p;
	struct optab *q;
{
	int need = q->needs, a = need, b = need, k = 0, k0, cls, n;
	struct rs *s;

	while (a & 3) {				/* A registers */
		resc[k].op = REG;
		resc[k].type = p->type;
		resc[k].rval = allocreg(p, a & 0xF);
		resc[k].lval = 0;
		resc[k].stsize = 0;
		a--;
		k++;
	}
	k0 = k;
	while (b & 3) {				/* the registers stay busy only for the code of the entry */
		if (((b & 0xF) != 0 && szty(p->type) == 2) || (p->op == STASG && p->stsize == 4) ||
		    (dope[p->op] & 0x80))
			cls = 3;
		else
			cls = rclass(p->type, resc[k0].rval);
		b--;
		k0--;
		s = rfind(resc[k0].rval, cls);
		if (s)
			s->busy = 0;
	}
	a = need;
	while (a & 0x30) {			/* B registers */
		resc[k].op = REG;
		resc[k].type = p->type;
		resc[k].rval = allocreg(p, a & 0xF0);
		resc[k].lval = 0;
		resc[k].stsize = 0;
		a -= 16;
		k++;
	}
	k0 = k;
	b = need;
	while (b & 0x30) {
		cls = rclass(p->type, resc[k0].rval);
		b -= 16;
		k0--;
		s = rfind(resc[k0].rval, cls);
		if (s)
			s->busy = 0;
	}
	if (need & 0x300) {			/* a temporary in the frame */
		resc[k].op = OREG;
		resc[k].type = p->type;
		resc[k].rval = 15;
		resc[k].stsize = 0;
		n = tempwords(p, (need & 0x300) >> 8);
		resc[k].lval = freetemp(n);
		resc[k].sname = fpsym1;
		resc[k].lval >>= 3;
		k++;
	}
	if (need & 0xC00) {			/* a temporary for parameters */
		resc[k].op = OREG;
		resc[k].type = p->type;
		resc[k].rval = 15;
		resc[k].stsize = 0;
		n = tempwords(p, (need & 0xC00) >> 10);
		resc[k].lval = freeparm(n);
		resc[k].lval >>= 3;
		resc[k].sname = 0;
		k++;
	}
	for (n = 0; n < k; n++)
		if (resc[n].rval < 0)
			return 0;
	nxwork(p, q->nx);
	return 1;
}

/* ---- trees --------------------------------------------------------------------------------------------- */

static void
fwalk(p, f)
	struct node *p;
	void (*f)();
{
	int kind;

	if (p == 0)
		return;
	kind = dope[p->op] & 0xE;
	if (kind != LTYPE)
		fwalk(p->left, f);
	if (kind == BITYPE)
		fwalk(p->right, f);
	f(p);
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

struct node *
talloc()
{
	struct node *p;

	p = (struct node *)calloc(1, sizeof *p);

	if (p == 0)
		cerror("out of tree space; simplify expression");
	return p;
}

/* a copy of the tree; the registers it uses count as busy once more */
struct node *
tcopy(p)
	struct node *p;
{
	struct node *q = talloc();
	int r = p->rval, kind;

	*q = *p;
	if (p->op == REG)
		rbusy(r, p->type, p->sname, p->lval);
	else if (p->op == OREG) {
		if (r >= 0x100) {
			rbusy((r & 0xFF) - 1, 0x14, p->sname, p->lval);
			if (r & 0xFF)
				rbusy((r >> 8) - 1, 4, p->sname, p->lval);
		} else if (p->lval != 0 || p->sname != 0 || p->stsize < 0)
			rbusy(r, 4, p->sname, p->lval);
		else
			rbusy(r, 0x14, p->sname, p->lval);
	}
	kind = dope[q->op] & 0xE;
	if (kind == BITYPE)
		q->right = tcopy(p->right);
	if (kind != LTYPE)
		q->left = tcopy(p->left);
	return q;
}

/* ---- reclaim ------------------------------------------------------------------------------------------- */

/* cookie masks and the shapes of the node that serves them */
static int rwtab[][2] = {
	{ 0x14, 0x14 }, { 0xe, 0xaca }, { 0x1000, 0x1000 }, { 0x2000, 0x2000 }, { 0x4, 0x240 }, { 0, 0 }
};

/*
 * After the code of an entry was printed: free the registers of the tree and make the result (the node the
 * rewrite names: RLEFT, RRIGHT, RESC1..3) the new tree p.
 */
void
reclaim(p, rewrite, cookie)
	struct node *p;
	int rewrite;
	int cookie;
{
	struct node *a[6], *c = 0, *r;
	int o = p->op, i, k, want = 0;

	if (rewrite == RNOP)
		goto done;
	if (o == FREE && rewrite == RNULL)
		goto done;
	fwalk(p, rfreenode);
	if (rewrite == 0 || (cookie & FOREFF)) {
		tfree(p);
		goto done;
	}
	if ((cookie & 0x20) && (rewrite & RESCC)) {	/* the condition codes */
		tfree(p);
		p->op = CCODES;
		p->lval = o;
		p->rval = 0;
		goto done;
	}
	i = 0;
	if (rewrite & RLEFT)
		a[i++] = getlr(p, 'L');
	if (rewrite & RRIGHT)
		a[i++] = getlr(p, 'R');
	if (rewrite & RESC1)
		a[i++] = &resc[0];
	if (rewrite & RESC2)
		a[i++] = &resc[1];
	if (rewrite & RESC3)
		a[i++] = &resc[2];
	if (i == 0)
		cerror("illegal reclaim");
	a[i] = 0;
	for (k = 0; rwtab[k][0]; k++) {
		if (!(cookie & rwtab[k][0]))
			continue;
		for (i = 0; (c = a[i]) != 0; i++)
			if (tshape(c, rwtab[k][1]))
				goto found;
	}
	cerror("cannot reclaim");
found:
	if (o == STARG)
		p = p->left;
	c->type = p->type;
	r = tcopy(c);
	tfree(p);
	p->op = r->op;
	p->lval = r->lval;
	p->rval = r->rval;
	p->sname = r->sname;
	if (r->op == OREG)
		p->stsize = r->stsize;
	r->op = FREE;
	if (p->op != REG)
		goto done;
	if (!tflag) {
		if (p->type == 3)
			p->type = 4;
		else if (p->type == 13)
			p->type = 14;
	}
	if (rewrite & RESC) {			/* the second register of a long result */
		want = p->rval + 1;
		if (!(p->rall & 0x1000))
			p->rall = (p->rall & 0x2000) | want;
		rfree(p->rval, p->type);
		if (getreg(2, want, 0x1000, p->sname) != want)
			cerror("reclaim: cannot find register free");
		p->rval = want;
		if (p->type == 5)
			p->type = 4;
		else
			p->type = 14;		/* sic: the original has no test for ULONG here */
		if (p->rall & 0x1000)
			want = p->rall & ~0x1000;
	} else if (o == 0x3E || o == 0x3F) {	/* a long result of a shift or divide goes to rr4 */
		if (p->type == 5 || p->type == 15) {
			rfree(p->rval, p->type);
			if (getreg(3, 4, 0x1000, p->sname) != 4)
				cerror("reclaim: cannot find register free");
			p->rval = 4;
			if (!(p->rall & 0x1000))
				p->rall = (p->rall & 0x2000) | 4;
		}
	}
	if (!(p->rall & 0x1000))
		goto done;
	if (!(rewrite & RESC)) {
		want = p->rall & ~0x1000;
		if (want & 0x2000)
			goto done;
	}
	if (want == p->rval)
		goto done;
	rfree(p->rval, p->type);
	if (getreg(rclass(p->type, want), want, 0x1000, p->sname) != want)
		cerror("reclaim dest reg not free");
	rmove(want, p->rval, p->type);
	p->rval = want;
done:
	tmpcleanup();
	rfreeall(0);
}

/* idle registers go back to the free list (flag 0: only the ones that hold no symbol) */
void
rfreeall(flag)
	int flag;
{
	struct rs *n, *next;

	for (n = rsuse; n; n = next) {
		next = n->next;
		if (n->busy > 0 || n->kind == 6)
			continue;
		if (n->sym == 0 || flag != 0)
			rsreturn(n->cls, n->reg);
	}
}
