/*
 * regs3.c - register variables, temporaries, what the registers hold, the commands F R S T
 */
#include "cg.h"
#include "cg2.h"

extern char ascii[];
extern int cerror();
extern NODE *clocal();
extern void fcmd();
extern int findtemp();
extern void fixfld();
extern void ncopy();
extern void fixname();
extern void fixtemp();
extern NODE *foldadd();
extern struct sym *fpsym1;
extern struct sym *fpsym2;
extern long frame0;
extern NODE *getlr();
extern int getreg();
extern struct sym *mapn();
extern void mapregs();
extern int maxreg;
extern void memname();
extern int minreg;
extern NODE node[];
extern int nsave;
extern void nxwork();
extern char *opst[];
extern void oregfix();
extern int rclass();
extern void rcmd();
extern int regforsym();
extern void regreset();
extern void regtabinit();
extern struct rv regvar[];
extern struct rs *rfind();
extern int rflag;
extern void rfree();
extern void rfreeall();
extern int rlmode;
extern int rmode;
extern void rmove();
extern void rsreturn();
extern struct rs *rsuse;
extern int savecls[];
extern int saveorig[];
extern int savereg[];
extern void scmd();
extern void symdrop();
extern int symreg();
extern struct sym *symtab0;
extern void symuse();
extern void tabclear();
extern int tabfind();
extern void tcmd();
extern void tmpcleanup();
extern void tmpdump();
extern struct tmpent tmptab[];
extern int usable();
static char sccsid[] = "@[$]regs3.c\t2.5  10/06/83 10:55:19 Copr. 1981 87wega3.2.";

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

#define SUBST(from) \
	if ((p->in.left->in.op == REG || p->in.left->in.op == OREG) && p->in.left->tn.rval == from) \
		p->in.left->tn.rval = r; \
	if ((p->in.right->in.op == REG || p->in.right->in.op == OREG) && p->in.right->tn.rval == from) \
		p->in.right->tn.rval = r

/* r4 and r5 hold values that are still needed: move them to other registers */
#define SAVEREGS \
	n = rfind(4, 3); \
	if (n && n->busy > 0) { \
		r = getreg(3, -1, 0x1000, p->in.sname); \
		if (r < 0) \
			cerror("regfile: case A4UW can't free rr4 on %s\n", opst[p->in.op]); \
		rmode++; \
		savereg[nsave] = r; \
		saveorig[nsave] = 4; \
		savecls[nsave] = 3; \
		nsave++; \
		rfind(r, 3)->busy = 1; \
		rmove(r, 4, 5); \
		SUBST(4); \
	} else { \
		n = rfind(4, 2); \
		if (n && n->busy > 0) { \
			r = getreg(2, -1, 0x1000, p->in.sname); \
			if (r < 0) \
				cerror("regfile: case A4UW can't free r4 on %s\n", opst[p->in.op]); \
			savereg[nsave] = r; \
			saveorig[nsave] = 4; \
			savecls[nsave] = 2; \
			nsave++; \
			rfind(r, 2)->busy = 1; \
			rmove(r, 4, 4); \
			SUBST(4); \
		} \
		n = rfind(5, 2); \
		if (n && n->busy > 0) { \
			r = getreg(2, -1, 0x1000, p->in.sname); \
			if (r < 0) \
				cerror("regfile: case A4UW can't free r5 on %s\n", opst[p->in.op]); \
			if (r == 4) { \
				r = getreg(2, -1, 0x1000, p->in.sname); \
				if (r < 0) \
					cerror("regfile: case A4UW can't free r5 on %s\n", opst[p->in.op]); \
				rfree(4, 4); \
			} \
			savereg[nsave] = r; \
			saveorig[nsave] = 5; \
			savecls[nsave] = 2; \
			nsave++; \
			rfind(r, 2)->busy = 1; \
			rmove(r, 5, 4); \
			SUBST(5); \
		} \
	} \
	rmode = m; \
	rlmode = l

#define INCREF(t)	((((t) & ~0xF) << 2) | 0x10 | ((t) & 0xF))

#define DECREF(t)	((((t) >> 2) & ~0xF) | ((t) & 0xF))

#ifdef OLD_D3P
#define D3PWORD(t)	((t) == 4 || (t) == 14 || ((t) & 0x30) == 0x10)
#endif





static void rnassign();
static void rnnullp();
static void rnnullcall();
static void rnnullall();
static void rnnulll();
static void rnnullw();
static void rnnull();

/* the table of the temporaries: tmptab[], an entry with a negative key ends the used part */
void
regtabinit()
{
	tmptab[0].off = -1;
}

int
tabfind(key)
	int key;
{
	register int i;

	for (i = 0; ; ) {
		if (tmptab[i].off < 0 || key == tmptab[i].off)
			break;
		if (++i > 0x4f)
			break;
	}
	return i;
}

void
tabclear(key)
	int key;
{
	register int i;

	i = tabfind(key);
	if (tmptab[i].off != key)
		cerror("Switch temp not found in table");
	else
		tmptab[i].done = 0;
}

/* a TEMP node becomes a register or an OREG on the frame */
void
fixtemp(p)
	register NODE *p;
{
	register int i;
	register struct rs *n;
	register int cls, j;

	if (rflag) {
		if ((i = p->tn.rval) >= 0 && (j = findtemp(i)) >= 0) {
			p->in.op = REG;
			p->tn.lval = 0;
			p->tn.rval = regvar[j].rreg;
			return;
		}
	}
	i = tabfind(p->tn.rval);
	if (tmptab[i].off != p->tn.rval) {
		if (i == 0x50)
			cerror("TEMP table overflow\n");
		tmptab[i].off = p->tn.rval;
		if (p->tn.lval == -1) {
			tmptab[i].flags |= 0x8000;
			cls = rclass(p->in.type, -1);
			tmptab[i].reg = j = getreg(cls, -1, 0x1000, 0);
			tmptab[i].cls = cls;
			n = rfind(j, cls);
			n->kind = 6;
			n->busy = 0;
		} else {
			tmptab[i].flags &= ~0x8000;
			tmptab[i].reg = (int)((long)(unsigned)p->stn.stsize + frame0);
		}
		if (i + 1 != 0x50)
			tmptab[i + 1].off = -1;
	}
	tmptab[i].done = p->tn.lval;
	if ((tmptab[i].flags >> 15 & 1) == 1) {
		p->in.op = REG;
		p->tn.lval = 0;
		p->tn.rval = tmptab[i].reg;
	} else {
		p->in.op = OREG;
		p->tn.lval = (long)(unsigned)tmptab[i].reg;
		p->tn.rval = 15;
		p->stn.stsize = 0;
#ifdef SEG
		p->in.sname = symtab0;
#else
		p->in.sname = 0;
#endif
	}
}

/* drop the finished temporaries: the last entry takes the place of a finished one */
void
tmpcleanup()
{
	register int i;
	int j;
	struct rs *n;

	for (i = 0; i <= 0x4f && tmptab[i].off >= 0; i++) {
		if (tmptab[i].done)
			continue;
		if ((tmptab[i].flags >> 15 & 1) == 1) {
			n = rfind(tmptab[i].reg, tmptab[i].cls);
			if (n) {
				n->kind = 8;
				n->busy = 0;
			}
		}
		for (j = i + 1; j <= 0x4f; j++) {
			if (tmptab[j].off < 0) {
				if (i + 1 == j) {
					tmptab[i].off = -1;
					return;
				}
				j--;
				tmptab[i] = tmptab[j];
				tmptab[j].off = -1;
				i--;
				break;
			}
		}
	}
}

void
tmpdump()
{
	register int i;

	printf("\tTEMP\tREFREM\tOFST/REG\n");
	for (i = 0; tmptab[i].off >= 0; ) {
		printf("\t%d\t%d\t%d\n", tmptab[i].off, tmptab[i].done, tmptab[i].reg);
		if (++i >= 0x4f)
			break;
	}
}

/* a field: the address of the member is added to the address of the structure */
void
fixfld(p)
	register NODE *p;
{
	register NODE *a, *m;
	register int ismember;
#if defined(OLD_D5S) || defined(OLD_D5G)
	int subsr = p->in.op == 120;
#endif

	if (p->in.op == FLD) {
		m = p->in.left;
		ismember = 1;
	} else {
		p->in.op = PLUS;
		a = p->in.left;
		m = a->in.left;
		ismember = 0;
	}
#ifdef OLD_D5S
	/* v0: an element of a local array is (index + offset) + fp, the frame pointer added last */
	if (subsr && a->in.op == PLUS && a->in.left->in.op == REG && a->in.left->tn.rval == 15 && a->in.right->in.op == ICON &&
	    p->in.right->in.op != ICON) {
		m = a->in.left;
		a->in.left = p->in.right;
		a->in.type = p->in.right->in.type;
		p->in.left = a;
		p->in.right = m;
		return;
	}
#endif
#if defined(OLD_D5S) && !defined(OLD_D3KC)
	/* v0: so also an element of an array that is a member: (index + offset) + the rest, which comes last */
	if (subsr && a->in.op == PLUS && a->in.right->in.op == ICON && !a->in.right->in.sname && p->in.right->in.op != ICON) {
		m = a->in.left;
		a->in.left = p->in.right;
		a->in.type = p->in.right->in.type;
		p->in.left = m;
		p->in.right = a;
		return;
	}
#endif
#if defined(OLD_D5S) && !defined(OLD_D3KC)
	/* v0: a member of an element of a global array: (index*size + member + index) + address, the address last */
	if (subsr && a->in.op == PLUS && a->in.left->in.op == UAND && a->in.right->in.op == PLUS && p->in.right->in.op != ICON) {
		m = a->in.left;
		a->in.left = a->in.right;
		a->in.right = p->in.right;
		a->in.type = p->in.right->in.type;
		p->in.left = a;
		p->in.right = m;
		return;
	}
#endif
#ifdef OLD_D5G
	/* v0p: an element of a global array of arrays is (index + index) + address, the address added last */
	if (subsr && a->in.op == PLUS && (a->in.left->in.op == UAND || (a->in.left->in.op == ICON && a->in.left->in.sname)) &&
	    p->in.right->in.op != ICON) {
		m = a->in.left;
		a->in.left = p->in.right;
		a->in.type = p->in.right->in.type;
		p->in.left = a;
		p->in.right = m;
		return;
	}
#endif
	if (p->in.op != FLD && a->in.op != UAND)
		return;
	if (m->in.op != NAME || m->tn.rval < 0 || (m->in.sname->sflags >> 2 & 1) != 0)
		return;
	m = clocal(m);
	if (m->in.op != STAR)
		return;
	if (!ismember) {
		m->in.op = FREE;
		a->in.op = FREE;
		p->in.type = INCREF(DECREF((unsigned)m->in.type));
		p->in.left = m->in.left;
	} else
		p->in.left = m;
}

#ifdef OLD_D23
/* a member or an index 0 adds nothing: p + 0 is p */
static int
d23drop(p)
	register NODE *p;
{
	register NODE *r, *n;
	int t;

	n = p->in.right;
	if ((p->in.op != PLUS && p->in.op != STREF) || n->in.op != ICON || n->in.sname != 0 || n->tn.lval != 0)
		return 0;
	t = p->in.type;
	r = p->in.left;
	n->in.op = FREE;
	ncopy(p, r);
	p->in.type = t;
	r->in.op = FREE;
	return 1;
}

#endif
/* a callback of the walk before the code is generated: members and addresses of names */
void
mapregs(p)
	register NODE *p;
{
	register NODE *n, *m, *r;

	switch (p->in.op) {
	case 120:
		fixfld(p);
#ifdef OLD_D23
		d23drop(p);
#endif
		break;
	case STREF:
		fixfld(p);
		n = p->in.right;
		if (p->in.left->in.op == UAND)
			p->in.left->in.type = p->in.type;
		if (n->in.op == NAME && (n->in.sname->sclass == 8 || n->in.sname->sclass == 11)) {
			n->in.op = ICON;
			n->tn.lval = n->in.sname->soffset / 8;
			n->tn.rval = 0;
			n->in.sname = 0;
			n->in.type = 4;
		}
#ifdef OLD_D5T
		/* an element of a local array (OLD_D5S), its member: the member goes to the offset, fp stays last */
		r = p->in.left;
		if (r->in.op == PLUS && r->in.right->in.op == REG && r->in.right->tn.rval == 15 && r->in.left->in.op == PLUS &&
		    r->in.left->in.right->in.op == ICON && n->in.op == ICON && n->in.sname == 0) {
			r->in.left->in.right->tn.lval += n->tn.lval;
			n->in.op = FREE;
			p->in.op = PLUS;
			p->in.left = r->in.left;
			p->in.right = r->in.right;
			r->in.op = FREE;
			break;
		}
#endif
#ifdef OLD_D23
		if (d23drop(p))
			break;
#endif
		n = foldadd(p);
		if (n != p) {
			p->in.type = n->in.type;
			p->in.op = n->in.op;
			p->in.sname = n->in.sname;
			p->tn.lval = n->tn.lval;
			p->tn.rval = n->tn.rval;
			n->in.op = FREE;
		}
		break;
	case UAND:
		m = p->in.left;
		if (m->in.op == NAME && (m->in.sname->sflags >> 2 & 1) == 0 && m->tn.rval >= 0) {
			m = clocal(m);
			if (m->in.op == STAR) {
				m->in.op = FREE;
				p->in.op = FREE;
				r = m->in.left;
				p->in.type = r->in.type;
				p->in.op = r->in.op;
				p->in.sname = r->in.sname;
				p->tn.lval = r->tn.lval;
				p->tn.rval = r->tn.rval;
				r->in.op = FREE;
			}
		}
		break;
	}
}

/* the registers that hold the symbol are no longer valid */
void
symuse(s)
	register struct sym *s;
{
	register struct rs *n, *next;

	for (n = rsuse; n; n = next) {
		next = n->next;
		if (s && s == n->sym) {
			if ((s == fpsym1 || s == fpsym2) && n->off != s->soffset)
				continue;
			if (n->busy <= 0) {
				n->off = 0;
				n->sym = 0;
			}
		}
	}
}

/* idle registers go back to the free list (flag 0: only the ones that hold no symbol) */
void
rfreeall(flag)
	int flag;
{
	register struct rs *n;
	register struct rs *next;

	for (n = rsuse; n; n = next) {
		next = n->next;
		if (n->busy > 0 || n->kind == 6)
			continue;
#ifdef OLD_D3K
#ifdef OLD_D3KC
		if (flag == 2 && n->cls == 2 && n->sym && n->sym->sclass == 9)
#else
		if (flag == 2 && n->cls <= 3 && n->sym && n->sym->sclass == 9)
#endif
			continue;
#endif
		if (n->sym == 0 || flag != 0)
			rsreturn(n->cls, n->reg);
	}
}

/* the status node of the register reg of class cls on the use list */
struct rs *
rfind(reg, cls)
	int reg;
	int cls;
{
	register struct rs *n;

	for (n = rsuse; n; n = n->next)
		if (n->reg == reg && n->cls == cls)
			return n;
	return 0;
}

/* p1 now holds what p2 holds: copy the symbol and offset in the register file */
static void
rnassign(p1, p2)
	register NODE *p1, *p2;
{
	register struct rs *a, *b;

	if (p1->in.op != REG)
		return;
	a = rfind(p1->tn.rval, rclass(p1->in.type, p1->tn.rval));
	if (a == 0)
		return;
	switch (a->cls) {
	case 5:
	case 6:
	case 7:
	case 8:
		return;
	}
	if (a->kind == 6)
		return;
	if (p2->in.op == REG) {
		b = rfind(p2->tn.rval, rclass(p2->in.type, p2->tn.rval));
		if (b) {
			a->sym = b->sym;
			a->off = b->off;
		} else {
			a->sym = 0;
			a->off = 0;
		}
	} else {
		a->sym = p2->in.sname;
		a->off = p2->tn.lval;
	}
	a->flags &= ~0x8000;
	a->busy = 0;
}

/* the register of the REG node p no longer holds anything known */
static void
rnnullp(p)
	register NODE *p;
{
	register struct rs *n;

	n = rfind(p->tn.rval, rclass(p->in.type, p->tn.rval));
	if (n) {
		n->sym = 0;
		n->busy = 0;
		n->off = 0;
		n->flags &= ~0x8000;
		n->kind = 8;
	}
}

/* a call destroys r4 .. r7, rr4, rr6 */
static void
rnnullcall()
{
	rnnull(4, 4);
	rnnull(5, 4);
	rnnull(6, 4);
	rnnull(7, 4);
	rnnull(4, 5);
	rnnull(6, 5);
}

static void
rnnullall()
{
	register int i;

	for (i = 0; i <= 7; i++)
		rnnull(i, 4);
	for (i = 0; i <= 6; i += 2)
		;
	rnnull(i, 5);
}

static void
rnnulll(r)
	register int r;
{
	rnnull(r, 5);
	rnnull(r, 4);
	rnnull(r + 1, 4);
}

static void
rnnullw(r)
	int r;
{
	rnnull(r, 4);
}

static void
rnnull(reg, type)
	int reg, type;
{
	register struct rs *n;

	n = rfind(reg, rclass(type, reg));
	if (n) {
		n->sym = 0;
		n->busy = 0;
		n->off = 0;
		n->flags &= ~0x8000;
		n->kind = 8;
	}
}

void
nxwork(p, nx)
	register NODE *p;
	register int nx;
{
	register struct rs *n;
	register int r, m, l;

	switch (nx) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 11:
	case 12:
	case 13:
	case 16:
	case 17:
	case 23:
		break;
	case 8:
	case 9:
		rnassign(getlr(p, 'L'), getlr(p, 'R'));
		break;
	case 10:
		rnassign(getlr(p, 'R'), getlr(p, '1'));
		break;
	case 14:
		rnassign(getlr(p, '1'), getlr(p, 'R'));
		break;
	case 15:
		rnassign(getlr(p, '1'), getlr(p, 'L'));
		break;
	case 18:
		rnnullw(getlr(p, '1')->tn.rval);
		break;
	case 19:
		rnnulll(getlr(p, '1')->tn.rval);
		break;
	case 20:
		rnnullp(getlr(p, '1'));
		break;
	case 21:
		rnnullw(getlr(p, '1')->tn.rval);
		rnnullw(getlr(p, '2')->tn.rval);
		break;
	case 22:
		rnnulll(getlr(p, '1')->tn.rval);
		rnnulll(getlr(p, '2')->tn.rval);
		break;
	case 24:
		rnnullw(getlr(p, 'L')->tn.rval);
		break;
	case 25:
		rnnulll(getlr(p, 'L')->tn.rval);
		break;
	case 26:
		rnnullp(getlr(p, 'L'));
		break;
	case 27:
		rnnullp(getlr(p, 'R'));
		break;
	case 28:
		rnnullp(getlr(p, 'L'));
		rnnullp(getlr(p, 'R'));
		break;
	case 29:
		rnnullw(getlr(p, 'R')->tn.rval);
		rnnullw(getlr(p, 'L')->tn.rval);
		break;
	case 30:
		rnnullw(getlr(p, '1')->tn.rval);
		rnnullw(getlr(p, 'L')->tn.rval);
		break;
	case 31:
		rnnullp(getlr(p, '1'));
		rnnullp(getlr(p, 'L'));
		break;
	case 32:
		rnnullall();
		break;
	case 33:
		nsave = 0;
		if (!usable(p, 'R', 4)) {
			m = rmode;
			l = rlmode;
			rmode = 0;
			rlmode = 0;
			SAVEREGS;
		}
		rnnullcall();
		break;
	case 34:
		m = rmode;
		l = rlmode;
		rmode = 0;
		rlmode = 0;
		nsave = 0;
		SAVEREGS;
		rnnullcall();
		break;
	default:
		cerror("regfile: illegal register update case\n");
	}
}

#ifdef OLD_D2C
/* the register no longer holds its symbol (the tree has taken it over) */
void
symdrop(reg)
	register int reg;
{
	register struct rs *n;

	for (n = rsuse; n; n = n->next)
		if (n->reg == reg) {
			n->off = 0;
			n->sym = 0;
		}
}
#endif

/* the register that holds the symbol (and offset), marked busy; -1 if none */
int
symreg(s, off, type)
	register struct sym *s;
	long off;
	register int type;
{
	register struct rs *n;
	register struct sym *f1, *f2;

	f1 = fpsym1;
	f2 = fpsym2;
	for (n = rsuse; n; n = n->next) {
		if (s == f1 || s == f2) {
			if (n->off == off)
				goto found;
			continue;
		}
		goto chk;
	hit:
		if (s == 0)
			;
	found:
		n->busy = 1;
		return n->reg;
	chk:
		if (s == n->sym)
			goto hit;
	}
	return -1;
}

/* the register that can serve for the symbol; -1 if none */
int
regforsym(reg, s, type, off)
	register int reg;
	register struct sym *s;
	register int type;
	long off;
{
	register int r, cls;

	if (s->sclass == 4) {
		cls = rclass(type, reg);
		r = getreg(cls, reg, 0x1000, s);
		if (r == s->sbyte5)
			return r;
		rsreturn(cls, r);
		return -1;
	}
	if (s == fpsym1 || s == fpsym2)
		return symreg(s, off, type);
#ifdef OLD_D3P
	/* v0: also under a cast between int, unsigned and pointers (the word types) */
	if (type != s->stype && !(D3PWORD(type) && D3PWORD(s->stype)))
		return -1;
#else
	if (type != s->stype)
		return -1;
#endif
#ifndef OLD_D3P
	if (reg != -1 && reg != s->sbyte5)
		return -1;	/* (OLD_D3P: the register that holds it also when another one is wanted, a copy follows) */
#endif
	return symreg(s, 0L, type);
}

/* a name that lives in a register becomes REG, otherwise an OREG */
void
fixname(p)
	register NODE *p;
{
	register int r;

	r = (p->in.rall & MUSTDO) ? p->in.rall & ~MUSTDO : -1;
	r = regforsym(r, p->in.sname, p->in.type, p->in.sname->soffset / 8);
	if (r >= 0) {
		p->in.op = REG;
		p->tn.rval = r;
		p->tn.lval = 0;
	} else
		oregfix(p);
}

/* a NAME of an automatic variable or a parameter becomes an OREG off the frame pointer */
void
oregfix(p)
	register NODE *p;
{
	register int spare;

	if ((p->in.sname->sflags >> 2 & 1) == 1)
		return;
	if (p->in.op == NAME && p->in.sname->sclass == 3)
		return;
	if (p->in.op == NAME && p->tn.rval < 0)
		return;
	p->in.op = OREG;
	p->tn.rval = 15;
	p->tn.lval = p->in.sname->soffset / 8;
	p->stn.stsize = 0;
	p->in.sname = p->in.sname->sclass == 9 && ((p->in.sname->sflags >> 3 & 1) == 1) ? fpsym2 : fpsym1;
}

/* command F: a register variable or a temporary is released */
void
fcmd(n)
	int n;
{
	register int i;
	register int spare;	/* unused: the original reserves this register */
	register struct sym *s;

	if (n > 0) {
		s = mapn(n);
		i = s->sbyte5;
	} else {
		i = findtemp(-n);
		if (i == -1)
			cerror("Can't find temp to be freed");
	}
	regvar[i].sym = 0;
	regvar[i].rtag = -1;
	regvar[i].rreg = -1;
	regvar[i].rtype = -1;
	if (n > 0)
		s->sclass = regvar[i].rcls;
	regvar[i].rcls = -1;
}

/* the register variables are given up at the end of a function (flag: put the classes back) */
void
regreset(flag)
	int flag;
{
	register int i;
	register struct sym *s;

#ifdef SEG
	for (i = 0; i < 11; i++) {
#else
	for (i = 0; i < NREGVAR; i++) {
#endif
		s = regvar[i].sym;
		if (flag && s)
			s->sclass = regvar[i].rcls;
		regvar[i].sym = 0;
		regvar[i].rtag = -1;
		regvar[i].rreg = -1;
		regvar[i].rtype = -1;
	}
#ifdef SEG
	for (i = 8; i <= 13; i++)
#else
	for (i = 8; i <= 14; i++)
#endif
		if (rsuse)
			rsreturn(9, i);
	for (i = 20; i <= 23; i++)
		if (rsuse)
			rsreturn(9, i);
}

/* command R: a variable (b > 0: by its tag, else a temporary) is placed in register a */
void
rcmd(a, b, type)
	int a, b, type;
{
	register int i, r, got, tag, cls;
	register struct sym *s;
	register struct sym *old;
	int saved;
	struct rs *n;

	if (b > 0) {
		s = mapn(b);
		type = s->stype;
		tag = -1;
	} else {
		s = 0;
		tag = -b;
	}
	if (type == 6 || type == 7) {
#ifdef SEG
		i = a + 6;
#else
		i = a + 7;
#endif
		old = regvar[i].sym;
		regvar[i].sym = s;
		regvar[i].rtag = tag;
		regvar[i].rreg = r = a + 19;
		regvar[i].rtype = type;
		saved = regvar[i].rcls;
		regvar[i].rcls = s->sclass;
		if (b > 0)
			s->sbyte5 = r;
		if (r - 1 < maxreg)
			maxreg = r - 1;
	} else {
		old = regvar[a].sym;
		regvar[a].sym = s;
		regvar[a].rtag = tag;
		regvar[a].rreg = r = a + 7;
		regvar[a].rtype = type;
		saved = regvar[a].rcls;
		regvar[a].rcls = s->sclass;
		if (b > 0)
			s->sbyte5 = r;
		if (r - 1 < minreg)
			minreg = r - 1;
	}
	cls = rclass(type, r);
	got = getreg(cls, r, 0x1000, s);
	if (got != r)
		cerror("rvplace: getreg got wrong register");
	n = rfind(got, cls);
	n->busy = 0;
	if (b > 0)
		s->sclass = 4;
	if (old && old != s)
		old->sclass = saved;
}

/* command S: a register variable or a temporary is stored in its memory place */
void
scmd(n)
	int n;
{
	register struct sym *s;
	register long off;
	register int j, t, reg;
	int tmp, i;

	if (n > 0) {
		s = mapn(n);
		t = s->stype;
		off = s->soffset;
		reg = s->sbyte5;
		i = reg - 7;
		if (regvar[i].rcls != 3)
			off = off / 8;
		tmp = s->sclass;
		s->sclass = regvar[i].rcls;
	} else {
		j = findtemp(-n);
		if (j == -1)
			cerror("Can't find temp to be freed");
		t = regvar[j].rtype;
		s = 0;
		off = -n;
		reg = regvar[j].rreg;
	}
	printf("/* rvstore */\n");
#ifdef SEG
	if ((t & 0x30) == 0x10)
		t = 5;
#endif
	switch (t) {
	case 5:
		printf("\tldl\t");
		memname(s, off);
		printf(",rr%d\n", reg);
		break;
	case 6:
		printf("\tflds\t");
		memname(s, off);
		printf(",f%d\n", reg - 16);
		break;
	case 7:
		printf("\tfldd\t");
		memname(s, off);
		printf(",f%d\n", reg - 16);
		break;
	default:
		printf("\tld\t");
		memname(s, off);
		printf(",r%d\n", reg);
	}
	if (s)
		s->sclass = tmp;
}

/* command T: a register variable or a temporary is loaded from its memory place */
void
tcmd(n)
	int n;
{
	register struct sym *s;
	register long off;
	register int t, j, reg;

	if (n > 0) {
		s = mapn(n);
		t = s->stype;
		off = s->soffset;
		if (s->sclass != 3)
			off = off / 8;
		reg = s->sbyte5;
	} else {
		j = findtemp(-n);
		if (j == -1)
			cerror("Can't find temp to be restored");
		t = regvar[j].rtype;
		s = 0;
		off = -n;
		reg = regvar[j].rreg;
	}
	printf("/* rvrestore */\n");
#ifdef SEG
	if ((t & 0x30) == 0x10)
		t = 5;
#endif
	switch (t) {
	case 5:
		printf("\tldl\t");
		printf("rr%d,", reg);
		memname(s, off);
		putchar('\n');
		break;
	case 6:
		printf("\tflds\t");
		printf("f%d,", reg - 16);
		memname(s, off);
		putchar('\n');
		break;
	case 7:
		printf("\tfldd\t");
		printf("f%d,", reg - 16);
		memname(s, off);
		putchar('\n');
		break;
	default:
		printf("\tld\t");
		printf("r%d,", reg);
		memname(s, off);
		putchar('\n');
	}
}

/* the operand of a store or load of a register variable: a name or an address on the frame */
void
memname(s, off)
	register struct sym *s;
	long off;
{
	int x;

	if (s && (s->sflags >> 2 & 1))
		printf("_%s", ascii + s->nameoff);
	else if (s && s->sclass == 3) {
		putchar('L');
		printf("%ld", off);
	} else {
#ifdef SEG
		printf("_stkseg+");
#endif
		printf("~L1+%ld(fp)", off);
	}
}

int
findtemp(tag)
	int tag;
{
	register int i;

	for (i = 0; i < NREGVAR; i++)
		if (regvar[i].rtag == tag)
			return i;
	return -1;
}
