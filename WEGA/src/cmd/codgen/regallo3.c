/*
 * regallo3.c - the register lists
 */
#include "cg.h"
#include "cg2.h"

extern char ascii[];
extern int cerror();
extern int d2vfirst();
extern struct sym *fpsym1;
extern struct sym *fpsym2;
extern int getreg();
extern struct rs *newnode();
extern void nullsyms();
extern int pairmate();
extern struct rs pool[];
extern void rcount_reset();
extern int regjam();
extern int roverlap();
extern void rsbuild();
extern int rsdepth;
extern void rsdump();
extern struct rs *rsexcl;
extern int rsflag1;
extern int rsflag2;
extern struct rs *rsfree;
extern void rsfreeall();
extern void rsinit();
extern void rsinsert();
extern int rsmatch();
extern void rsmerge();
extern struct rs *rspool;
extern void rspush();
extern void rsrelall();
extern void rsrelease();
extern void rsrestart();
extern void rsreturn();
extern struct { struct rs *list; int mark; } rsstk[];
extern int rstab1[];
extern int rstab2[];
extern struct rs *rsuse;
static char sccsid[] = "@[$]regallo3.c\t2.5  08/24/83 13:36:00 Copr. 1981 87wega3.2.";
static char rcname[][8] = {
	"TBYTE", "TWORD", "TLONG", "TFLOAT", "GBYTE", "GWORD", "GLONG", "GFLOAT", "RANY", "NULL"
};

#define MAXRCOUNT 5000



/* the order of the word registers when a node is put on the free list: r1 .. r7, then r0 */

#ifdef OLD_D2PT
#define D2PK(t)	1		/* v0p: OLD_D2P takes the registers of a TEMP too (the value of a switch) */
#else
#define D2PK(t)	((t)->kind != 6)
#endif

#ifdef OLD_D2L
#define D2VARS
#endif
#ifdef OLD_D2P
#define D2VARS
#endif
#ifdef OLD_D2W6
#define D2VARS
#endif

/* the other register of a pair */
int
pairmate(r)
	register int r;
{
	if (r & 1)
		r--;
	else
		r++;
	return r;
}

void
rsinit()
{
	register struct rs *n;
	register struct rs *nx;
	register struct rs *last;

	rsflag1 = 0;
	rsflag2 = 0;
	rsdepth = 0;
	rspool = nx = n = &pool[0];
	nx++;
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
	n = rsfree = rspool;
#ifdef OLD_D3J
	for (last = n; last; last = last->next)
		last->flags &= ~0x4000;
#endif
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
	for (r = 16; r <= 19; r++, last = n, n = n->next) {
		n->cls = 4;
		n->reg = r;
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
	for (r = 20; r <= 23; r++, last = n, n = n->next) {
		n->cls = 8;
		n->reg = r;
	}
	last->next = 0;
	rspool = n;
}

/* debug: print a list of register nodes */

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
	rspool = rspool->next;
	switch (n->cls) {
	case 3:
	case 7:
		m->reg = n->reg;
		n->reg++;
		n->cls = m->cls = k;
	rt:
		return m;
	case 2:
	case 6:
		m->reg = n->reg = n->reg * 2;		/* word register r: bytes 2r and 2r+1 */
		n->reg++;
		n->cls = m->cls = k;
		goto rt;
	default:
		cerror("splitreg: illegal type");
		goto rt;
	}
}

#ifdef OLD_D2W
static int rweight[8] = { 3, 1, 2, 3, 4, 5, 6, 7 };	/* the old codgen v0: r0 is taken early (pairs rr2, rr0 first) */
#else
static int rweight[8] = { 8, 1, 2, 3, 4, 5, 6, 7 };
#endif

/*
 * Put the node n at its place in the free list again. mode 1: the node was just freed or changed, its
 * place is behind the nodes of a lower class (word registers: ordered by rweight); mode 2: a node that was
 * made by merging.
 */
void
rsinsert(n, mode)
	register struct rs *n;
	int mode;
{
	register struct rs *a, *b, *prev, *p;
	register int key;

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

void
rcount_reset()
{
	/* the original has this empty function at 0x778e and nobody calls it; stmt() clears nrecur */
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

/*
 * Force a register for a variable out of a larger node: look at the used list for a free
 * node that can be split (regjam). Returns the register, or -1.
 */
int
regjam(cls, reg, flag, sym)
	register int cls;
	register int reg;
	register int flag;
	register struct sym *sym;
{
	register struct rs *n;
	register struct rs *prev;

	for (prev = n = rsuse; n; prev = n, n = n->next) {
		if (cls == n->cls) {
			if ((reg == -1 || reg == n->reg) && n->busy <= 0) {
				n->sym = sym;
				n->off = sym ? sym->soffset : 0;
				n->flags &= ~0x8000;
				n->kind = 8;
				n->busy = 1;
				goto ret;
			}
			goto chk;
		split: {
		struct rs *w;

		for (prev = n = rsuse; n; prev = n, n = n->next) {
			if (!(n->cls <= 3 || (n->cls == 7 && cls == 6)) || n->busy > 0)
				continue;
			switch (cls) {
			case 1:
				if (reg != -1) {
					if (n->cls == 3) {
						if (!roverlap(1, reg, n))
							continue;
					} else if (n->cls == 2) {
						if (!roverlap(1, reg))
							continue;
					}
				}
				if (n->cls == 3) {
					w = newnode(n, 2);
					if (reg != -1 && reg / 2 == n->reg) {
						n->reg--;
						w->reg++;
					}
					w->next = rsfree;
					rsfree = w;
					rsinsert(w, 1);
				} else if (n->cls != 2)
					cerror("regjam: illegal type");
				w = newnode(n, 1);
				if (reg == w->reg)
					goto swap;
				goto link;
			case 6:
				if (n->cls != 7)
					continue;
				if (reg != -1 && !roverlap(cls, reg, n))
					continue;
				w = newnode(n, 6);
				if (reg != -1 && reg == w->reg)
					goto swap;
				goto link;
			case 2:
				if (n->cls != 3)
					continue;
				if (reg != -1 && !roverlap(cls, reg, n))
					continue;
				w = newnode(n, 2);
				if (reg != -1 && reg == w->reg) {
				swap:
					n->reg--;
					w->reg++;
				}
			link:
				w->next = rsfree;
				rsfree = w;
				rsinsert(w, 1);
				break;
			default:
				cerror("regjam: illegal type");
				break;
			}
			n->busy = 1;
			n->sym = sym;
			n->off = sym ? sym->soffset : 0;
			n->flags &= ~0x8000;
			n->kind = 8;
		ret:
			return n->reg;
		}
			}
			goto more;
		chk:
			if (reg == n->reg && n->busy > 0)
				goto fail;
		}
	}
	if (cls == 1 || cls == 2 || cls == 6)
		goto split;
more:
	if (cls == 3 || cls == 7) {
		/* free the smaller registers that lie inside the wanted long register */
		int found, t;

		found = 0;
		for (prev = n = rsuse; n; n = prev) {
			prev = n->next;
			if (n->busy > 0)
				continue;
			if ((cls == 3 && ((t = n->cls) == 1 || t == 2)) ||
			    (cls == 7 && ((t = n->cls) == 5 || t == 6))) {
				if (reg != -1 && !roverlap(cls, reg, n))
					continue;
				found = 1;
				rsreturn(n->cls, n->reg);
			}
		}
		if (found)
			return getreg(cls, reg, 0x1000, sym);
	}
fail:
	if (reg != -1)
		return getreg(cls, -1, 0x1000, sym);
	return -1;
}

#ifdef OLD_D2V
/* v0: the word register reg (that of an address) weighs no more than the one getreg(2, -1) would take */
int
d2vfirst(reg)
	int reg;
{
	register struct rs *w, *t;
	register int k;

	for (w = rsfree; w && w->cls != 2 && w->cls != 3; w = w->next)
		;
	if (w && w->cls == 2) {
		for (t = w; t && t->cls != 3; t = t->next)
			;
		if (t && rweight[w->reg] > rweight[t->reg])
			w = t;
	}
	k = w ? rweight[w->reg] : 100;
	for (t = rsuse; t; t = t->next)
		if (t->cls == 2 && t->busy <= 0 && t->reg != reg && rweight[t->reg] < k)
			k = rweight[t->reg];
	return rweight[reg] <= k;
}
#endif

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
#ifdef D2VARS
	struct rs *m;
	struct rs *t;
#endif
#ifdef OLD_D2P
	int k;
#endif

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
			goto chk;
		blk:
			n->flags &= ~0x8000;
			n->sym = sym;
			n->off = sym ? sym->soffset : 0;
			n->kind = 8;
		found:
			n->busy = 1;
		ret:
			return n->reg;
		chk:
			if (n->busy <= 0)
				goto blk;
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
#ifdef OLD_D2L
	if (cls == 2 && reg == -1) {	/* v0: an idle word that holds a name is free when it weighs less */
		for (w = rsfree; w && w->cls != 2 && w->cls != 3; w = w->next)
			;
#ifdef OLD_D2F
		if (w && w->cls == 2) {		/* the word getreg would take: the first free word or the first pair */
			for (t = w; t && t->cls != 3; t = t->next)
				;
			if (t && rweight[w->reg] > rweight[t->reg])
				w = t;
		}
#endif
		m = 0;
		for (t = rsuse; t; t = t->next)
#ifdef OLD_D2T
			/* v0: also the register of a TEMP (the value of a switch, needed again only behind a jump) */
#ifdef OLD_D2G
			if (t->cls == 2 && t->busy <= 0 && (w == 0 || rweight[t->reg] <= rweight[w->reg]) &&	/* v0: an equal weight is enough */
#else
			if (t->cls == 2 && t->busy <= 0 && (w == 0 || rweight[t->reg] < rweight[w->reg]) &&
#endif
#else
			if (t->cls == 2 && t->busy <= 0 && t->kind != 6 && (w == 0 || rweight[t->reg] < rweight[w->reg]) &&
#endif
			    (m == 0 || rweight[t->reg] < rweight[m->reg]))
				m = t;
#ifdef OLD_D2P
		for (t = rsuse; t; t = t->next)		/* an idle pair lighter than both: given back, it is split */
			if (t->cls == 3 && t->reg < 8 && t->busy <= 0 && t->kind != 6 &&
			    (w == 0 || rweight[t->reg] < rweight[w->reg]) && (m == 0 || rweight[t->reg] < rweight[m->reg])) {
				rsreturn(3, t->reg);
				return getreg(cls, reg, flag, sym);
			}
#endif
		if (m) {
			n = m;
			n->flags &= ~0x8000;
			n->sym = sym;
			n->off = sym ? sym->soffset : 0;
			n->kind = 8;
			goto found;
		}
	}
#endif
#ifdef OLD_D2P
	if (cls == 3 && reg == -1) {	/* v0: an idle pair that holds a name is free when it weighs less (OLD_D2L for pairs) */
		for (w = rsfree; w && w->cls != 3; w = w->next)
			;
		for (t = rsuse; t; t = t->next)		/* two idle words of a pair: given back, they merge */
			if (t->cls == 2 && !(t->reg & 1) && t->reg < 8 && t->busy <= 0 && D2PK(t) &&
			    (w == 0 || rweight[t->reg] < rweight[w->reg])) {
				for (m = rsuse; m; m = m->next)
					if (m->cls == 2 && m->reg == t->reg + 1 && m->busy <= 0 && D2PK(m))
						break;
				if (m == 0) {
					for (m = rsfree; m; m = m->next)
						if (m->cls == 2 && m->reg == t->reg + 1)
							break;
					if (m == 0)
						continue;
					k = t->reg;
					rsreturn(2, k);
				} else {
					k = t->reg;
					rsreturn(2, k);
					rsreturn(2, k + 1);
				}
				return getreg(cls, reg, flag, sym);
			}
#ifdef OLD_D2PO
		for (t = rsuse; t; t = t->next)		/* an idle odd word whose even half is free: given back */
			if (t->cls == 2 && (t->reg & 1) && t->reg < 8 && t->busy <= 0 && D2PK(t) &&
			    (w == 0 || rweight[t->reg - 1] < rweight[w->reg])) {
				for (m = rsfree; m; m = m->next)
					if (m->cls == 2 && m->reg == t->reg - 1)
						break;
				if (m == 0)
					continue;
				rsreturn(2, t->reg);
				return getreg(cls, reg, flag, sym);
			}
#endif
		m = 0;
		for (t = rsuse; t; t = t->next)
			if (t->cls == 3 && t->busy <= 0 && D2PK(t) && (w == 0 || rweight[t->reg] < rweight[w->reg]) &&
			    (m == 0 || rweight[t->reg] < rweight[m->reg]))
				m = t;
		if (m) {
			n = m;
			n->flags &= ~0x8000;
			n->sym = sym;
			n->off = sym ? sym->soffset : 0;
			n->kind = 8;
			goto found;
		}
	}
#endif
	prev = n = rsfree;
	for (; n; prev = n, n = n->next) {	/* a free node of the class */
		if (cls < n->cls)
			break;
		if (cls != n->cls)
			continue;
		if (reg != -1 && reg != n->reg)
			continue;
#ifdef OLD_D2F
		if (cls == 2 && reg == -1) {	/* v0: a free word only when it weighs no more than the first free pair */
			for (w = n; w && w->cls != 3; w = w->next)
				;
			if (w && rweight[n->reg] > rweight[w->reg])
				break;
		}
#endif
#ifdef OLD_D2W6
		if (cls == 3 && reg == -1 && n->reg == 4) {	/* v0p: a pair asked for: rr6 before rr4 */
			for (m = n; m && m->cls == 3; prev = m, m = m->next)
				if (m->reg == 6)
					break;
			if (m && m->cls == 3 && m->reg == 6)
				n = m;
			else
				for (prev = rsfree; prev != n && prev->next != n; prev = prev->next)
					;
		}
#endif
		n->flags &= ~0x8000;
		n->sym = sym;
		n->off = sym ? sym->soffset : 0;
		n->kind = 8;
		w = rsuse;		/* take the free node n out of rsfree and put it on the front of rsuse */
		rsuse = n;
		if (prev == n)
			rsfree = n->next;
		else
			prev->next = n->next;
		goto nxt;
	split:					/* split a larger node */
		for (prev = n = rsfree; n; prev = n, n = n->next) {
			if (!(n->cls <= 3 || (n->cls == 7 && cls == 6)))
				continue;
			switch (cls) {
			case 1:
				if (reg != -1) {
					if (n->cls == 3) {
						if (!roverlap(1, reg, n))
							continue;
					} else if (n->cls == 2) {
						if (!roverlap(1, reg, n))
							continue;
					} else
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
				} else if (n->cls != 2)
					continue;
				w = newnode(n, 1);
				if (reg != -1 && reg != w->reg)
					goto swap;
				goto link;
			case 6:
				if (n->cls != 7)
					continue;
				if (reg != -1 && !roverlap(cls, reg, n))
					continue;
				w = newnode(n, 6);
				if (reg != -1 && reg != w->reg)
					goto swap;
				goto link;
			case 2:
				if (n->cls != 3)
					continue;
				if (reg != -1 && !roverlap(cls, reg, n))
					continue;
				w = newnode(n, 2);
				if (reg != -1 && reg != w->reg) {
				swap:
					n->reg--;
					w->reg++;
				}
			link:
				rsinsert(n, 1);
				n = w;
				break;
			default:
				cerror("getreg: illegal type default case");
				break;
			}
			n->flags &= ~0x8000;
			n->sym = sym;
			n->off = sym ? sym->soffset : 0;
			n->kind = 8;
			w = rsuse;
			rsuse = n;
		nxt:
			rsuse->next = w;
			goto found;
		}
		goto fail;
	}
	if (cls == 1 || cls == 2 || cls == 6)
		goto split;
fail:
	return flag == 0x1000 ? regjam(cls, reg, flag, sym) : -1;
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

	if ((n = rsuse) == 0)
		cerror("deletereg allocated register status list empty");
	for (prev = n; n; prev = n, n = n->next) {
		if ((cls == n->cls || cls == 9) && reg == n->reg) {
			cls = n->cls;
			if (prev == n)
				rsuse = prev->next;
			else
				prev->next = n->next;
			n->next = 0;
			a = rsfree;
			if (a == 0) {
				rsfree = n;
				return;
			}
			for (b = a; a && cls > a->cls; ) {
				b = a;
				a = a->next;
			}
			if (a == 0) {
				b->next = n;
				return;
			}
			if (cls != a->cls || (cls != 1 && cls != 2 && cls != 6)) {
				key = rweight[reg];
				if (cls == 3)
					while (a && cls == a->cls && key > rweight[a->reg]) {
						b = a;
						a = a->next;
					}
				if (b == a) {
					n->next = b;
					rsfree = n;
					return;
				}
				b->next = n;
				n->next = a;
				return;
			}
			partner = reg ^ 1;
			for (b = a; cls == b->cls; ) {
				if (partner == b->reg) {
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
					if (rspool)
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
	}
}

/* the registers in use no longer hold symbols */
void
nullsyms(mode)
	int mode;
{
	register struct rs *n;

	for (n = rsuse; n; n = n->next) {
		switch (mode) {
		case 1:
			if (n->cls <= 4)
				n->sym = 0;
			break;
		default:
			cerror("nullist: illegal case");
		}
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


	for (next = n = rsuse; n; n = next) {
		next = n->next;
		switch (flag) {
		case 1:
			if (n->cls > 4)
				continue;
			if (n->kind == 6)
				continue;
			break;
#ifdef OLD_D3K
		case 4:			/* v0, at a label: as 1, but a parameter stays known in its register */
			if (n->cls > 4 || n->kind == 6)
				continue;
			if (n->busy <= 0 && n->sym && n->sym->sclass == 9)
				continue;
			break;
#endif
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
	register int spare;

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
	register struct rs *q, *p, *prev, *n;
	char gone1, gone2, drop1, drop2;

	if (rsuse == 0) {
		rsuse = *hp;
		rsexcl = 0;
		return;
	}
	gone1 = gone2 = 0;
	for (prev = n = *hp; n; ) {
		drop1 = drop2 = 0;
		q = p = rsexcl;
		while (p) {
			if (roverlap(p->cls, p->reg, n)) {
				gone1 = 1;
				if (p->cls < n->cls) {
					drop1 = 0;
					if (q == p)
						q = rsexcl = p->next;
					else
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
					if (p->cls < n->cls) {
						drop2 = 0;
						if (q == p)
							q = rsuse = p->next;
						else
							q->next = p->next;
						p->next = rspool;
						rspool = p;
					} else {
						drop2 = 1;
						if (q == p)
							q = rsuse = p->next;
						else
							q->next = p->next;
						p->next = rsexcl;
						rsexcl = p;
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
			if (!drop1 && !drop2) {
				if (prev == n)
					prev = *hp = prev->next;
				else
					prev->next = n->next;
				n->next = rsexcl;
				rsexcl = n;
			} else {
				if (prev == n)
					prev = *hp = prev->next;
				else
					prev->next = n->next;
				n->next = rspool;
				rspool = n;
			}
		} else {
			if (prev == n)
				prev = *hp = prev->next;
			else
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

	if (rsflag2 > 0)
		goto st;
	return 0;
nx:
	j++;
	goto tst;
st:
	for (i = rsflag2; i > 0; i--) {
		j = 0;
	tst:
		if (j >= rsdepth)
			return 0;
		if (rsstk[j].mark != rstab1[i - 1])
			goto nx;
	}
	for (i = rsflag2; i > 0; i--)
		for (j = 0; j < rsdepth; j++)
			if (rsstk[j].mark == rstab1[i - 1])
				rsmerge(&rsstk[j].list);
	return 1;
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
	for (n = last = l; n; last = n, n = n->next)
		;
	last->next = rspool;
	rspool = l;
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
