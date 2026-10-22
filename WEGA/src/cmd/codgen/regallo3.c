/*
 * regallo3.c - the register status lists (codgen / scodgen)
 *
 * Reconstructed from /lib/codgen: rsinit 0x740c, rsbuild 0x7458, newnode 0x75e0, rsinsert 0x7650, roverlap 0x7790,
 * regjam 0x7916, getreg 0x7c10, rsreturn 0x7f50, nullsyms 0x811e, rsrelease 0x847a. Not done yet: rsfreeall 0x815e,
 * rspush 0x81c4, rsmerge 0x81fe, rsrestart 0x8502, rsdump 0x750c (debug).
 *
 * The registers are kept on two lists of 14 byte nodes: rsfree (the free registers, ordered by class) and rsuse
 * (the ones that are in use). A register class is: 1 byte register, 2 word register, 3 long register, 4 float
 * register, 6 word register for variables (r14), 7 long register for variables, 8 float register for variables;
 * the node of a long register is split into two word nodes when one of its halves is wanted, and merged again
 * when both are free.
 */
#include "cg.h"

extern int cerror();

struct rs {
	char reg;	/* register number (byte registers 0..15, word registers 0..15, long registers even) */
	char cls;
	char kind;	/* 8: holds a symbol */
	char busy;	/* use count */
	long	off;		/* frame offset of the symbol it holds */
	unsigned flags;		/* bit 15: ... */
	struct rs *next;
	struct sym *sym;
};

#define NPOOL	87
static struct rs pool[NPOOL];
struct rs *rsfree, *rsuse, *rspool;
struct rs *rsexcl;		/* 0xa79a: nodes taken away from rsuse by rsmerge */
int rsdepth;			/* 0xa7aa: saved lists */
struct rs *rssaved[3];
int rsmark[3];
int rsflag1, rsflag2;		/* 0xa2d6, 0xa7ac: not used by the functions below */

/* the order of the word registers when a node is put on the free list: r1 .. r7, then r0 */
static int rweight[8] = { 8, 1, 2, 3, 4, 5, 6, 7 };

void
rsinit()
{
	int i;

	rsflag1 = rsflag2 = 0;
	rsdepth = 0;
	rspool = &pool[0];
	for (i = 0; i < NPOOL; i++) {
		pool[i].next = i + 1 < NPOOL ? &pool[i + 1] : 0;
		pool[i].cls = 0x0A;
		pool[i].reg = -1;
		pool[i].busy = 0;
	}
	rsuse = 0;
	rsfree = 0;
}

/* give the nodes of a list back to the pool */
void
rsrelease(l)
	struct rs *l;
{
	struct rs *last;

	if (l == 0)
		cerror("freelist: null list");
	for (last = l; last->next; last = last->next)
		;
	last->next = rspool;
	rspool = l;
}

/* the free list at the start of a function: rr2 rr4 rr6 rr0, f0-f3, r14, rr8 rr10 rr12, f4-f7 */
void
rsbuild()
{
	struct rs *n, *last = 0;
	int r;

	if (rsfree)
		rsrelease(rsfree);
	rsfree = n = rspool;
	for (r = 2; r <= 6; r += 2) {
		n->cls = 3;
		n->reg = r;
		n = n->next;
	}
	n->cls = 3;
	n->reg = 0;
	n = n->next;
	for (r = 16; r <= 19; r++) {
		n->cls = 4;
		n->reg = r;
		last = n;
		n = n->next;
	}
	n->cls = 6;
	n->reg = 14;
	n = n->next;
	for (r = 8; r <= 12; r += 2) {
		n->cls = 7;
		n->reg = r;
		n = n->next;
	}
	for (r = 20; r <= 23; r++) {
		n->cls = 8;
		n->reg = r;
		last = n;
		n = n->next;
	}
	last->next = 0;
	rspool = n;
}

/*
 * Split the node n into two nodes of class k: a long node into two words, a word node into two bytes,
 * a long variable node into two word variable nodes. The new node gets the lower register.
 */
struct rs *
newnode(n, k)
	struct rs *n;
	int k;
{
	struct rs *m;

	if (rspool == 0)
		cerror("splitreg: no nodes left on elist");
	m = rspool;
	rspool = m->next;
	switch (n->cls) {
	case 2: case 6:
		n->reg *= 2;			/* word register r: bytes 2r and 2r+1 */
		/* FALLTHROUGH */
	case 3: case 7:
		m->reg = n->reg;
		n->reg++;
		m->cls = k;
		n->cls = k;
		return m;
	}
	cerror("splitreg: illegal type");
	return m;
}

/*
 * Put the node n at its place in the free list again. mode 1: the node was just freed or changed, its
 * place is behind the nodes of a lower class (word registers: ordered by rweight); mode 2: a node that was
 * made by merging.
 */
void
rsinsert(n, mode)
	struct rs *n;
	int mode;
{
	struct rs *a, *b, *p, *prev;
	int key;

	if (mode == 1) {
		a = n == rsfree ? n->next : rsfree;
		b = a;
		while (a && n->cls > a->cls) {
			b = a;
			a = a->next;
		}
		if (a == 0)
			return;
		if (n == a && n->cls != 2)
			return;
		if (n->cls == 2) {
			key = rweight[n->reg];
			while (a && n->cls == a->cls && key >= rweight[a->reg]) {
				b = a;
				a = a->next;
			}
		}
	} else if (mode == 2) {
		if (n->next == 0)
			return;
		if (n->next->cls > n->cls)
			return;
		b = n;
		a = n->next;
		while (a && n->cls > a->cls) {
			b = a;
			a = a->next;
		}
		if (n->cls == 3) {
			key = rweight[n->reg];
			while (a && n->cls == a->cls && key > rweight[a->reg]) {
				b = a;
				a = a->next;
			}
		}
	} else
		return;
	if (b == n)
		return;
	/* take n out of the list and put it between b and a */
	prev = p = rsfree;
	while (p != n) {
		prev = p;
		p = p->next;
	}
	if (p == prev)
		rsfree = p->next;
	else
		prev->next = p->next;
	if (b == a) {
		n->next = rsfree;
		rsfree = n;
	} else {
		b->next = n;
		n->next = a;
	}
}

/* does register reg of class cls overlap the register of the node n? */
int
roverlap(cls, reg, n)
	int cls;
	int reg;
	struct rs *n;
{
	switch (cls) {
	case 1:
		if (n->cls == 1)
			return reg == n->reg;
		if (n->cls == 3 && reg / 2 - 1 == n->reg)
			return 1;
		return reg / 2 == n->reg;
	case 2:
		if (n->cls == 1)
			return reg == n->reg / 2;
		if (n->cls == 2)
			return reg == n->reg;
		if (n->cls == 3)
			return reg == n->reg || reg - 1 == n->reg;
		return 0;
	case 3:
		if (n->cls == 1)
			return n->reg / 2 == reg || n->reg / 2 == reg + 1;
		if (n->cls == 2)
			return reg == n->reg || reg + 1 == n->reg;
		if (n->cls == 3)
			return reg == n->reg;
		break;
	case 6:
		if (n->cls == 6)
			return reg == n->reg;
		if (n->cls == 7)
			return reg == n->reg || reg - 1 == n->reg;
		return 0;
	case 7:
		if (n->cls == 6)
			return reg == n->reg || reg + 1 == n->reg;
		if (n->cls == 7)
			return reg == n->reg;
		if (n->cls == 1)
			return n->reg / 2 == reg || n->reg / 2 == reg + 1;
		if (n->cls == 2)
			return reg == n->reg || reg + 1 == n->reg;
		if (n->cls == 3)
			return reg == n->reg;
		break;
	}
	cerror("roverlap: illegal base type");
	return 0;
}

/* a node of the use list takes over the symbol sym */
static int
take(n, sym)
	struct rs *n;
	struct sym *sym;
{
	n->flags &= ~0x8000;
	n->sym = sym;
	n->off = sym ? sym->soffset : 0;
	n->kind = 8;
	return n->reg;
}

extern int getreg();
void rsreturn();

/* take a free node out of rsfree and put it on the front of rsuse */
static void
touse(n, prev)
	struct rs *n;
	struct rs *prev;
{
	struct rs *old = rsuse;

	rsuse = n;
	if (prev == n)
		rsfree = n->next;
	else
		prev->next = n->next;
	n->next = old;
}

/*
 * Force a register for a variable out of a larger node: look at the used list for a free
 * node that can be split (regjam). Returns the register, or -1.
 */
int
regjam(cls, reg, flag, sym)
	int cls;
	int reg;
	int flag;
	struct sym *sym;
{
	struct rs *n, *m, *w;
	int found = 0;

	for (n = rsuse; n; n = n->next) {
		if (n->cls != cls)
			continue;
		if (reg != -1 && n->reg != reg)
			continue;
		if (n->busy > 0) {
			if (reg == n->reg)
				goto fail;
			continue;
		}
		n->sym = sym;
		n->off = sym ? sym->soffset : 0;
		n->flags &= ~0x8000;
		n->kind = 8;
		n->busy = 1;
		return n->reg;
	}
	if (cls == 1 || cls == 2 || cls == 6) {
		for (n = rsuse; n; n = n->next) {
			if (!(n->cls <= 3 || (n->cls == 7 && cls == 6)) || n->busy > 0)
				continue;
			switch (cls) {
			case 1:
				if (reg != -1 && (n->cls == 3 || n->cls == 2) && !roverlap(1, reg, n))
					continue;
				if (n->cls == 3) {
					w = newnode(n, 2);
					if (reg != -1 && reg / 2 == n->reg) {
						n->reg--;
						w->reg++;
					}
					w->next = rsfree;
					rsfree = w;
					rsinsert(w, 1);
				} else if (n->cls != 2) {
					cerror("regjam: illegal type");
				}
				m = newnode(n, 1);
				if (reg == m->reg) {
					n->reg--;
					m->reg++;
				}
				break;
			case 2:
				if (n->cls != 3)
					continue;
				if (reg != -1 && !roverlap(2, reg, n))
					continue;
				m = newnode(n, 2);
				if (reg == m->reg) {
					n->reg--;
					m->reg++;
				}
				break;
			case 6:
				if (n->cls != 7)
					continue;
				if (reg != -1 && !roverlap(6, reg, n))
					continue;
				m = newnode(n, 6);
				if (reg == m->reg) {
					n->reg--;
					m->reg++;
				}
				break;
			default:
				cerror("regjam: illegal type");
				continue;
			}
			m->next = rsfree;
			rsfree = m;
			rsinsert(m, 1);
			n->busy = 1;
			n->sym = sym;
			n->off = sym ? sym->soffset : 0;
			n->flags &= ~0x8000;
			n->kind = 8;
			return n->reg;
		}
	}
	if (cls == 3 || cls == 7) {
		/* free the smaller registers that lie inside the wanted long register */
		for (n = rsuse; n; n = w) {
			w = n->next;
			if (n->busy > 0)
				continue;
			if (cls == 3 && n->cls != 1 && n->cls != 2)
				continue;
			if (cls == 7 && n->cls != 5 && n->cls != 6)
				continue;
			if (reg != -1 && !roverlap(cls, reg, n))
				continue;
			found = 1;
			rsreturn(n->cls, n->reg);
		}
		if (found)
			return getreg(cls, reg, 0x1000, sym);
	}
fail:
	if (reg != -1)
		return getreg(cls, -1, 0x1000, sym);
	return -1;
}

/*
 * Get a register of the class cls for the symbol sym (or a temporary when sym is 0); reg = -1: any.
 * flag 0x1000: if no register is free, take one from the used list (regjam). Returns the register
 * number, or -1.
 */
int
getreg(cls, reg, flag, sym)
	int cls;
	int reg;
	int flag;
	struct sym *sym;
{
	struct rs *n, *prev, *w, *m, *t;

	if (cls >= 5 && cls <= 8) {		/* register variables: the register is named */
		for (n = rsuse; n; n = n->next) {
			if (reg != n->reg)
				continue;
			if (sym == n->sym && cls == n->cls) {
				n->busy++;
				return n->reg;
			}
			if (n->busy <= 0) {
				n->flags &= ~0x8000;
				n->sym = sym;
				n->off = sym ? sym->soffset : 0;
				n->kind = 8;
				n->busy = 1;
				return n->reg;
			}
			cerror("bad getreg for register variable");
		}
	}
	for (n = rsuse; n; n = n->next) {	/* the symbol is still in a register */
		if (sym != n->sym || !sym || cls != n->cls || n->busy > 0)
			continue;
		if (reg != -1 && reg != n->reg)
			continue;
		if ((sym == fpsym1 || sym == fpsym2) && n->off != sym->soffset)
			continue;
		n->busy = 1;
		return n->reg;
	}
	prev = n = rsfree;
	for (; n; prev = n, n = n->next) {	/* a free node of the class */
		if (cls < n->cls)
			break;
		if (cls != n->cls)
			continue;
		if (reg != -1 && reg != n->reg)
			continue;
		take(n, sym);
		touse(n, prev);
		n->busy = 1;
		return n->reg;
	}
	if (cls == 1 || cls == 2 || cls == 6) {	/* split a larger node */
		prev = n = rsfree;
		while (n) {
			if (!(n->cls <= 3 || (n->cls == 7 && cls == 6))) {
				prev = n;
				n = n->next;
				continue;
			}
			switch (cls) {
			case 1:
				if (reg != -1 && n->cls == 3 && !roverlap(1, reg, n)) {
					prev = n;
					n = n->next;
					continue;
				}
				if (reg != -1 && n->cls == 2 && !roverlap(1, reg, n)) {
					prev = n;
					n = n->next;
					continue;
				}
				if (n->cls == 3) {
					w = newnode(n, 2);
					if (reg != -1 && reg / 2 != w->reg) {
						n->reg--;
						w->reg++;
					}
					w->next = n;
					if (prev == n)
						rsfree = w;
					else
						prev->next = w;
					rsinsert(n, 1);
					n = w;
				} else if (n->cls != 2) {
					prev = n;
					n = n->next;
					continue;
				}
				m = newnode(n, 1);
				if (reg != -1 && reg != m->reg) {
					n->reg--;
					m->reg++;
				}
				break;
			case 2:
				if (n->cls != 3 || (reg != -1 && !roverlap(2, reg, n))) {
					prev = n;
					n = n->next;
					continue;
				}
				m = newnode(n, 2);
				if (reg != -1 && reg != m->reg) {
					n->reg--;
					m->reg++;
				}
				break;
			case 6:
				if (n->cls != 7 || (reg != -1 && !roverlap(6, reg, n))) {
					prev = n;
					n = n->next;
					continue;
				}
				m = newnode(n, 6);
				if (reg != -1 && reg != m->reg) {
					n->reg--;
					m->reg++;
				}
				break;
			default:
				cerror("getreg: illegal type default case");
				continue;
			}
			rsinsert(n, 1);
			t = m;
			take(t, sym);
			w = rsuse;
			rsuse = t;
			t->next = w;
			t->busy = 1;
			return t->reg;
		}
	}
	if (flag == 0x1000)
		return regjam(cls, reg, flag, sym);
	return -1;
}

/*
 * Give the register (cls, reg) back (cls 9: any class): take it out of rsuse and put it on rsfree in its
 * place; a node whose partner (reg ^ 1) is free too is merged into one node of the next class.
 */
void
rsreturn(cls, reg)
	int cls;
	int reg;
{
	struct rs *n, *prev, *a, *b, *it, *itprev;
	int cl, key, partner;

	if (rsuse == 0)
		cerror("deletereg allocated register status list empty");
	prev = n = rsuse;
	for (; n; prev = n, n = n->next)
		if ((n->cls == cls || cls == 9) && n->reg == reg)
			break;
	if (n == 0)
		return;
	cl = n->cls;
	if (prev == n)
		rsuse = n->next;
	else
		prev->next = n->next;
	n->next = 0;
	a = rsfree;
	if (a == 0) {
		rsfree = n;
		return;
	}
	b = a;
	while (a && cl > a->cls) {
		b = a;
		a = a->next;
	}
	if (a == 0) {
		b->next = n;
		return;
	}
	if (cl == a->cls && (cl == 1 || cl == 2 || cl == 6)) {
		partner = reg ^ 1;
		itprev = it = a;
		while (it && it->cls == cl) {
			if (it->reg == partner) {
				it->reg = partner & ~1;
				if (cl == 1) {
					it->cls = 2;
					it->reg = it->reg / 2;
				} else if (cl == 2)
					it->cls = 3;
				else
					it->cls = 7;
				n->next = rspool;
				rspool = n;
				rsinsert(it, 2);
				return;
			}
			itprev = it;
			it = it->next;
		}
		n->next = it;
		itprev->next = n;
		rsinsert(n, 1);
		return;
	}
	key = rweight[reg & 7];
	if (cl == 3)
		while (a && cl == a->cls && key > rweight[a->reg & 7]) {
			b = a;
			a = a->next;
		}
	if (b == a) {
		n->next = a;
		rsfree = n;
	} else {
		b->next = n;
		n->next = a;
	}
}

/* the registers in use no longer hold symbols */
void
nullsyms(mode)
	int mode;
{
	struct rs *n;

	for (n = rsuse; n; n = n->next) {
		if (mode != 1) {
			cerror("nullist: illegal case");
			continue;
		}
		if (n->cls <= 4)
			n->sym = 0;
	}
}

/* the other register of a pair */
int
pairmate(r)
	int r;
{
	return r & 1 ? r - 1 : r + 1;
}
