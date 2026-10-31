/*
 * regallo3.c - the register status lists (codgen / scodgen)
 *
 * Reconstructed from /lib/codgen: rsinit 0x740c, rsbuild 0x7458, newnode 0x75e0, rsinsert 0x7650, roverlap 0x7790,
 * regjam 0x7916, getreg 0x7c10, rsreturn 0x7f50, nullsyms 0x811e, rsrelease 0x847a. rsfreeall 0x815e,
 * rspush 0x81c4, rsmerge 0x81fe, rsmatch 0x83ec, rsrelall 0x84b6, rsrestart 0x8502, rsdump 0x750c (debug).
 * rspush, rsmatch, rsrelall, rsrestart and rsdump are never called in codgen.
 *
 * The registers are kept on two lists of 14 byte nodes: rsfree (the free registers, ordered by class) and rsuse
 * (the ones that are in use). A register class is: 1 byte register, 2 word register, 3 long register, 4 float
 * register, 6 word register for variables (r14), 7 long register for variables, 8 float register for variables;
 * the node of a long register is split into two word nodes when one of its halves is wanted, and merged again
 * when both are free.
 */
#include "cg.h"

extern int cerror();
extern int roverlap();
extern int getreg();
extern void rsreturn();


#define NPOOL	87
static struct rs pool[NPOOL];
struct rs *rsexcl;		/* 0xa79a: nodes taken away from rsuse by rsmerge */
struct rs *rsfree, *rsuse, *rspool;	/* 0xa154, 0xa166, 0xa79c */
int rsdepth;			/* 0xa7aa: saved lists */
struct { struct rs *list; int mark; } rsstk[3];	/* the saved lists and their marks, interleaved */
int rsflag1;			/* 0xa2d6: number of entries of rstab1 */
int rstab1[3];			/* 0xa2d0 */
int rsflag2;			/* 0xa7ac: number of entries of rstab2 */
int rstab2[3];			/* 0xa7ae */

/* the order of the word registers when a node is put on the free list: r1 .. r7, then r0 */
#ifdef OLD_D2W
static int rweight[8] = { 3, 1, 2, 3, 4, 5, 6, 7 };	/* the old codgen v0: r0 is taken early (pairs rr2, rr0 first) */
#else
static int rweight[8] = { 8, 1, 2, 3, 4, 5, 6, 7 };
#endif

void
rsinit()
{
	register struct rs *n;
	register struct rs *nx;
	register struct rs *last;

	rsflag1 = 0;
	rsflag2 = 0;
	rsdepth = 0;
	n = &pool[0];
	rspool = n;
	nx = n + 1;
	while (n < &pool[NPOOL]) {
		n->next = nx;
		nx++;
		last = n;
		n->cls = 0x0A;
		n->reg = -1;
		n->busy = 0;
		n = n->next;
	}
	last->next = 0;
	rsuse = 0;
	rsfree = 0;
}

/* give the nodes of a list back to the pool */
void
rsrelease(l)
	register struct rs *l;
{
	register struct rs *n;
	register struct rs *last;

	if (l == 0)
		cerror("freelist: null list");
	for (last = n = l; n; last = n, n = n->next)
		;
	last->next = rspool;
	rspool = l;
}

/* the free list at the start of a function: rr2 rr4 rr6 rr0, f0-f3, r14, rr8 rr10 rr12, f4-f7 */
void
rsbuild()
{
	register struct rs *n;
	register struct rs *last;
	register struct rs *unused;	/* the original has a third register pointer here that nothing uses */
	register int r;

	if (rsfree)
		rsrelease(rsfree);
	rsfree = n = rspool;
#ifdef OLD_D2W
	for (r = 2; r <= 6; n = n->next, r += 2) {	/* the pairs in the order of rweight: rr2 rr0 rr4 rr6 */
		n->cls = 3;
		n->reg = r == 4 ? 0 : r == 6 ? 4 : r;
	}
	n->cls = 3;
	n->reg = 6;
	n = n->next;
#else
	for (r = 2; r <= 6; n = n->next, r += 2) {
		n->cls = 3;
		n->reg = r;
	}
	n->cls = 3;
	n->reg = 0;
	n = n->next;
#endif
	for (r = 16; r <= 19; r++, n = n->next) {
		n->cls = 4;
		n->reg = r;
		last = n;
	}
#ifndef SEG
	n->cls = 6;
	n->reg = 14;
	n = n->next;
#endif
	for (r = 8; r <= 12; r += 2, n = n->next) {
		n->cls = 7;
		n->reg = r;
	}
	for (r = 20; r <= 23; r++, n = n->next) {
		n->cls = 8;
		n->reg = r;
		last = n;
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
	register int k;
	register struct rs *n;
{
	register struct rs *m;

	if (rspool == 0)
		cerror("splitreg: no nodes left on elist");
	m = rspool;
	rspool = m->next;
	switch (n->cls) {
	case 2: case 6:
		n->reg = n->reg * 2;			/* word register r: bytes 2r and 2r+1 */
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
	register int cls;
	register int reg;
	register struct rs *n;
{
	register int q;

	switch (cls) {
	case 1:
		if (n->cls == 1) {
			if (reg == n->reg)
				return 1;
			return 0;
		}
		q = reg / 2;
		if (n->cls == 3 && q - 1 == n->reg)
			return 1;
		if (q == n->reg)
			return 1;
		return 0;
	case 6:
		if (n->cls == 6) {
			if (reg == n->reg)
				return 1;
			return 0;
		}
		if (n->cls == 7) {
			if (reg == n->reg || reg - 1 == n->reg)
				return 1;
			return 0;
		}
		break;
	case 2:
		if (n->cls == 1) {
			if (reg == n->reg / 2)
				return 1;
			return 0;
		}
		if (n->cls == 2) {
			if (reg == n->reg)
				return 1;
			return 0;
		}
		if (n->cls == 3) {
			if (reg == n->reg || reg - 1 == n->reg)
				return 1;
			return 0;
		}
		break;
	case 7:
		if (n->cls == 6) {
			if (reg == n->reg || reg + 1 == n->reg)
				return 1;
			return 0;
		}
		if (n->cls == 7) {
			if (reg == n->reg)
				return 1;
			return 0;
		}
		/* falls through */
	case 3:
		if (n->cls == 1) {
			if (n->reg / 2 == reg || n->reg / 2 == reg + 1)
				return 1;
			return 0;
		}
		if (n->cls == 2) {
			if (reg == n->reg || reg + 1 == n->reg)
				return 1;
			return 0;
		}
		if (n->cls == 3) {
			if (reg == n->reg)
				return 1;
			return 0;
		}
		/* falls through */
	default:
		cerror("roverlap: illegal base type");
	}
}

extern int getreg();
void rsreturn();

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
	register int cls;
	register int reg;
	register int flag;
	register struct sym *sym;
{
	register struct rs *n;
	register struct rs *prev;
	struct rs *w;
	struct rs *m;
	struct rs *t;
	struct rs *old;

	switch (cls) {				/* register variables: the register is named */
	case 5:
	case 6:
	case 7:
	case 8:
		for (n = rsuse; n; n = n->next) {
			if (reg != n->reg)
				continue;
			if (sym == n->sym && cls == n->cls) {
				n->busy++;
				goto ret;
			}
			if (n->busy <= 0) {
				n->flags &= ~0x8000;
				n->sym = sym;
				n->off = sym ? sym->soffset : 0;
				n->kind = 8;
				goto found;
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
		goto found;
	}
	prev = n = rsfree;
	for (; n; prev = n, n = n->next) {	/* a free node of the class */
		if (cls < n->cls)
			break;
		if (cls != n->cls)
			continue;
		if (reg != -1 && reg != n->reg)
			continue;
		n->flags &= ~0x8000;
		n->sym = sym;
		n->off = sym ? sym->soffset : 0;
		n->kind = 8;
		old = rsuse;		/* take the free node n out of rsfree and put it on the front of rsuse */
		rsuse = n;
		if (prev == n)
			rsfree = n->next;
		else
			prev->next = n->next;
		n->next = old;
		goto found;
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
				if (n->cls != 3 || (reg != -1 && !roverlap(cls, reg, n))) {
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
				if (n->cls != 7 || (reg != -1 && !roverlap(cls, reg, n))) {
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
				goto take;
			}
			rsinsert(n, 1);
			t = m;
take:
			t->flags &= ~0x8000;
			t->sym = sym;
			t->off = sym ? sym->soffset : 0;
			t->kind = 8;
			w = rsuse;
			rsuse = t;
			t->next = w;
			n = t;
			goto found;
		}
	}
	return flag == 0x1000 ? regjam(cls, reg, flag, sym) : -1;
found:
	n->busy = 1;
ret:
	return n->reg;
}

/*
 * Give the register (cls, reg) back (cls 9: any class): take it out of rsuse and put it on rsfree in its
 * place; a node whose partner (reg ^ 1) is free too is merged into one node of the next class.
 */
void
rsreturn(cls, reg)
	register int cls;
	register int reg;
{
	register struct rs *n;
	register struct rs *prev;
	register struct rs *a;
	register struct rs *b;
	register int partner;
	struct rs *itprev;
	int key;

	if (rsuse == 0)
		cerror("deletereg allocated register status list empty");
	prev = n = rsuse;
	for (; n; prev = n, n = n->next)
		if ((n->cls == cls || cls == 9) && n->reg == reg)
			break;
	if (n == 0)
		return;
	cls = n->cls;
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
	while (a && cls > a->cls) {
		b = a;
		a = a->next;
	}
	if (a == 0) {
		b->next = n;
		return;
	}
	if (cls == a->cls && (cls == 1 || cls == 2 || cls == 6)) {
		partner = reg ^ 1;
		b = a;
		while (b->cls == cls) {
			if (b->reg == partner) {
				b->reg = partner & ~1;
				if (cls == 1) {
					b->cls = 2;
					b->reg = b->reg / 2;
				} else if (cls == 2)
					b->cls = 3;
				else if (cls == 6)
					b->cls = 7;
				else
					cerror("deletereg: illegal type");
				n->next = rspool;
				rspool = n;
				rsinsert(b, 2);
				return;
			}
			itprev = b;
			b = b->next;
		}
		if (b == a) {
			if (a == rsfree) {
				n->next = a;
				rsfree = n;
				return;
			}
			n->next = a->next;
			a->next = n;
			return;
		}
		n->next = b;
		itprev->next = n;
		rsinsert(n, 1);
		return;
	}
	key = rweight[reg];
	if (cls == 3)
		while (a && cls == a->cls && key > rweight[a->reg]) {
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
		if (mode != 1)
			cerror("nullist: illegal case");
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

/* debug: print a list of register nodes */
static char rcname[][8] = {
	"TBYTE", "TWORD", "TLONG", "TFLOAT", "GBYTE", "GWORD", "GLONG", "GFLOAT", "RANY", "NULL"
};

void
rsdump(l)
	register struct rs *l;
{
	register struct rs *n;

	printf("//\n//   Reg No.\tType\tSymbol\tusecount\toffset\taddr\tdtype\n");
	for (n = l; n; n = n->next) {
		printf("//\t%d", n->reg);
		printf("\t%s", rcname[n->cls - 1]);
		printf("\t%s", ascii + n->sym->nameoff);
		printf("\t%d", n->busy);
		printf("\t\t%ld", n->off);
		printf("\t%d", n->flags >> 15 & 1);
		if (n->kind == 8)
			printf("\tDNULL");
		else if (n->kind == 6)
			printf("\tDETMP");
		putchar('\n');
	}
}

/*
 * Free the nodes of rsuse that flag selects: 1 the variables of the classes up to 4 that are not
 * register variables (kind 6), 2 the nodes of the classes above 4, 3 all of them.
 */
void
rsfreeall(flag)
	int flag;
{
	register struct rs *next;
	register struct rs *n;

	for (n = rsuse; n; n = next) {
		next = n->next;
		switch (flag) {
		case 1:
			if (n->cls > 4)
				continue;
			if (n->kind == 6)
				continue;
			break;
		case 2:
			if (n->cls <= 4)
				continue;
			break;
		case 3:
			break;
		default:
			continue;
		}
		rsreturn(n->cls, n->reg);
	}
}

/* save rsuse (with a mark) and start with an empty list; never called */
void
rspush(mark)
	int mark;
{
	if (rsdepth < 3) {
		rsstk[rsdepth].list = rsuse;
		rsstk[rsdepth].mark = mark;
		rsdepth++;
		rsuse = 0;
	}
}

/* put the nodes of a saved list back: those that overlap one of rsuse or rsexcl replace or are dropped */
void
rsmerge(hp)
	struct rs **hp;
{
	struct rs *n, *prev, *p, *q;
	char gone1, gone2, drop1, drop2;

	if (rsuse == 0) {
		rsuse = *hp;
		rsexcl = 0;
		return;
	}
	gone2 = gone1 = 0;
	n = prev = *hp;
	while (n) {
		drop2 = drop1 = 0;
		q = p = rsexcl;
		while (p) {
			if (roverlap(p->cls, p->reg, n)) {
				gone1 = 1;
				if (p->cls < n->cls) {
					drop1 = 0;
					if (q == p) {
						rsexcl = p->next;
						q = rsexcl;
					} else
						q->next = p->next;
					p->next = rspool;
					rspool = p;
					p = q;
					if (q == rsexcl)
						continue;
				} else
					drop1 = 1;
			}
			q = p;
			p = p->next;
		}
		if (!drop1) {
			q = p = rsuse;
			while (p) {
				if (roverlap(p->cls, p->reg, n)) {
					gone2 = 1;
					if (p->cls >= n->cls) {
						drop2 = 1;
						if (q == p) {
							rsuse = p->next;
							q = rsuse;
						} else
							q->next = p->next;
						p->next = rsexcl;
						rsexcl = p;
					} else {
						drop2 = 0;
						if (q == p) {
							rsuse = p->next;
							q = rsuse;
						} else
							q->next = p->next;
						p->next = rspool;
						rspool = p;
					}
					p = q;
					if (q == rsuse)
						continue;
				}
				q = p;
				p = p->next;
			}
		}
		if (gone2 || gone1) {
			if (drop1 || drop2) {
				if (prev == n) {
					*hp = prev->next;
					prev = *hp;
				} else
					prev->next = n->next;
				n->next = rspool;
				rspool = n;
			} else {
				if (prev == n) {
					*hp = prev->next;
					prev = *hp;
				} else
					prev->next = n->next;
				n->next = rsexcl;
				rsexcl = n;
			}
		} else {
			if (prev == n) {
				*hp = prev->next;
				prev = *hp;
			} else
				prev->next = n->next;
			n->next = rsuse;
			rsuse = n;
		}
		n = prev;
		if (prev != *hp)
			n = n->next;
	}
}

/* merge the saved lists whose marks are all in rstab1; 0 when one is missing; never called */
int
rsmatch()
{
	register int i;
	register int j;

	if (rsflag2 <= 0)
		return 0;
	for (i = rsflag2; i > 0; i--) {
		for (j = 0; ; j++) {
			if (j >= rsdepth)
				return 0;
			if (rsstk[j].mark == rstab1[i - 1])
				break;
		}
	}
	for (i = rsflag2; i > 0; i--)
		for (j = 0; j < rsdepth; j++)
			if (rsstk[j].mark == rstab1[i - 1])
				rsmerge(&rsstk[j].list);
	return 1;
}

/* give back the saved lists named in rstab2 (the loop runs only for a negative count: dead code); never called */
void
rsrelall()
{
	register int i;
	register int j;

	for (i = rsflag1; i < 0; i--)
		for (j = 0; j < rsdepth; j++)
			if (rsstk[j].mark == rstab2[i])
				rsrelease(rsstk[j].list);
}

/* take the registers of rsuse again on a fresh free list; never called */
void
rsrestart()
{
	register struct rs *l;
	register struct rs *n;
	register struct rs *last;

	rsbuild();
	if (rsuse == 0)
		return;
	l = rsuse;
	rsuse = 0;
	for (last = n = l; n; last = n, n = n->next)
		getreg(n->cls, n->reg, 0x2000, 0);
	rsrelease(l);
}
