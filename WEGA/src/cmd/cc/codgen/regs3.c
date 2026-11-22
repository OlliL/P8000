/*
 * regs3.c - register variables, temporaries, what the registers hold, the commands F R S T
 */
#include "mfile3"

#ifdef OLD_D2FA
int d2facopy;	/* OLD_D2FA: the assignment matched now copies a variable */
int d2faltype;	/* OLD_D2FA: fixname of a name matched alone */
#endif
extern void memname();
extern void tmpdump();
extern struct tmpent tmptab[];
static char sccsid[] = "@[$]regs3.c\t2.5  10/06/83 10:55:19 Copr. 1981 87wega3.2.";


#define SUBST(from) \
	if ((p->in.left->in.op == REG || p->in.left->in.op == OREG) && p->in.left->tn.rval == from) \
		p->in.left->tn.rval = r; \
	if ((p->in.right->in.op == REG || p->in.right->in.op == OREG) && p->in.right->tn.rval == from) \
		p->in.right->tn.rval = r

/* r4 and r5 hold values that are still needed: move them to other registers */
#define SAVEREGS \
	n = rfind(4, RCPAIR); \
	if (n && n->busy > 0) { \
		r = getreg(RCPAIR, -1, RJAM, p->in.sname); \
		if (r < 0) \
			cerror("regfile: case A4UW can't free rr4 on %s\n", opst[p->in.op]); \
		rmode++; \
		savereg[nsave] = r; \
		saveorig[nsave] = 4; \
		savecls[nsave] = 3; \
		nsave++; \
		rfind(r, RCPAIR)->busy = 1; \
		rmove(r, 4, LONG); \
		SUBST(4); \
	} else { \
		n = rfind(4, RCWORD); \
		if (n && n->busy > 0) { \
			r = getreg(RCWORD, -1, RJAM, p->in.sname); \
			if (r < 0) \
				cerror("regfile: case A4UW can't free r4 on %s\n", opst[p->in.op]); \
			savereg[nsave] = r; \
			saveorig[nsave] = 4; \
			savecls[nsave] = 2; \
			nsave++; \
			rfind(r, RCWORD)->busy = 1; \
			rmove(r, 4, INT); \
			SUBST(4); \
		} \
		n = rfind(5, RCWORD); \
		if (n && n->busy > 0) { \
			r = getreg(RCWORD, -1, RJAM, p->in.sname); \
			if (r < 0) \
				cerror("regfile: case A4UW can't free r5 on %s\n", opst[p->in.op]); \
			if (r == 4) { \
				r = getreg(RCWORD, -1, RJAM, p->in.sname); \
				if (r < 0) \
					cerror("regfile: case A4UW can't free r5 on %s\n", opst[p->in.op]); \
				rfree(4, INT); \
			} \
			savereg[nsave] = r; \
			saveorig[nsave] = 5; \
			savecls[nsave] = 2; \
			nsave++; \
			rfind(r, RCWORD)->busy = 1; \
			rmove(r, 5, INT); \
			SUBST(5); \
		} \
	} \
	rmode = m; \
	rlmode = l

#ifdef OLD_D3P
/* the types of the size of a pointer; a cast between two of them leaves a name in its register (regforsym) */
#ifdef SEG
/* segmented (vc): pointer, long, unsigned long, and one of the two is a pointer (not long against unsigned long:
   cas iputl; gopt tabfree (long)p) */
#define D3PWORD(t)	((t) == LONG || (t) == ULONG || ISPTR(t))
#define D3PCAST(a, b)	(D3PWORD(a) && D3PWORD(b) && (ISPTR(a) || ISPTR(b)))
#else
/* v0: int, unsigned and pointers, any two (int against unsigned: cparse sputc) */
#define D3PWORD(t)	((t) == INT || (t) == UNSIGNED || ISPTR(t))
#define D3PCAST(a, b)	(D3PWORD(a) && D3PWORD(b))
#endif
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

#ifndef OLD_DNR
	if (rflag) {
		if ((i = p->tn.rval) >= 0 && (j = findtemp(i)) >= 0) {
			p->in.op = REG;
			p->tn.lval = 0;
			p->tn.rval = regvar[j].rreg;
			return;
		}
	}
#endif
	i = tabfind(p->tn.rval);
	if (tmptab[i].off != p->tn.rval) {
		if (i == 0x50)
			cerror("TEMP table overflow\n");
		tmptab[i].off = p->tn.rval;
		if (p->tn.lval == -1) {
			tmptab[i].flags |= TMPINREG;
			cls = rclass(p->in.type, -1);
			tmptab[i].reg = j = getreg(cls, -1, RJAM, 0);
			tmptab[i].cls = cls;
			n = rfind(j, cls);
			n->kind = RKTEMP;
			n->busy = 0;
		} else {
			tmptab[i].flags &= ~TMPINREG;
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
		p->tn.rval = FP;
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
				n->kind = RKVAR;
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
	int subsr = p->in.op == SUBSR;
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
	if (subsr && a->in.op == PLUS && a->in.left->in.op == REG && a->in.left->tn.rval == FP && a->in.right->in.op == ICON &&
	    p->in.right->in.op != ICON) {
		m = a->in.left;
		a->in.left = p->in.right;
		a->in.type = p->in.right->in.type;
		p->in.left = a;
		p->in.right = m;
		return;
	}
#endif
#if defined(OLD_D5S) && !defined(OLD_D3KC) && !defined(SEG)
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
#if defined(OLD_D5S) && !defined(OLD_D3KC) && !defined(SEG)
	/* v0: a member of an element of a global array: (index*size + member + index) + address, the address last */
	if (subsr && a->in.op == PLUS && a->in.left->in.op == UNARY AND && a->in.right->in.op == PLUS && p->in.right->in.op != ICON) {
		m = a->in.left;
		a->in.left = a->in.right;
		a->in.right = p->in.right;
		a->in.type = p->in.right->in.type;
		p->in.left = a;
		p->in.right = m;
		return;
	}
#endif
#if defined(OLD_D5G) && defined(SEG)
	/* vc: a constant second index of a global array of arrays is folded into the address (_lctab+4(r3)) */
	if (subsr && a->in.op == PLUS && p->in.right->in.op == ICON && !p->in.right->in.sname &&
	    ((a->in.left->in.op == ICON && a->in.left->in.sname) ||
	     (a->in.left->in.op == UNARY AND && a->in.left->in.left->in.op == NAME))) {
		if (a->in.left->in.op == ICON)
			a->in.left->tn.lval += p->in.right->tn.lval;
		else
			a->in.left->in.left->tn.lval += p->in.right->tn.lval;
		m = p->in.right;
		p->in.left = a->in.left;
		p->in.right = a->in.right;
		a->in.op = FREE;
		m->in.op = FREE;
		return;
	}
#endif
#ifdef OLD_D5G
	/* v0p: an element of a global array of arrays is (index + index) + address, the address added last */
	if (subsr && a->in.op == PLUS && (a->in.left->in.op == UNARY AND || (a->in.left->in.op == ICON && a->in.left->in.sname)) &&
	    p->in.right->in.op != ICON) {
		m = a->in.left;
		a->in.left = p->in.right;
		a->in.type = p->in.right->in.type;
#ifdef SEG
		p->in.left = m;		/* the 32-bit address first, the index behind it: _a(r3) */
		p->in.right = a;
		m = a->in.left;		/* the outer index is computed first */
		a->in.left = a->in.right;
		a->in.right = m;
#else
		p->in.left = a;
		p->in.right = m;
#endif
		return;
	}
#endif
	if (p->in.op != FLD && a->in.op != UNARY AND)
		return;
	if (m->in.op != NAME || m->tn.rval < 0 || (SFISGLOBAL(m->in.sname->sflags)) != 0)
		return;
	m = clocal(m);
	if (m->in.op != UNARY MUL)
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
	case SUBSR:
		fixfld(p);
#ifdef OLD_D23
		d23drop(p);
#endif
		break;
	case STREF:
		fixfld(p);
		n = p->in.right;
		if (p->in.left->in.op == UNARY AND)
			p->in.left->in.type = p->in.type;
		if (n->in.op == NAME && (n->in.sname->sclass == MOS || n->in.sname->sclass == MOU)) {
			n->in.op = ICON;
			n->tn.lval = n->in.sname->soffset / SZCHAR;
			n->tn.rval = 0;
			n->in.sname = 0;
			n->in.type = INT;
		}
#ifdef OLD_D5T
		/* an element of a local array (OLD_D5S), its member: the member goes to the offset, fp stays last */
		r = p->in.left;
		if (r->in.op == PLUS && r->in.right->in.op == REG && r->in.right->tn.rval == FP && r->in.left->in.op == PLUS &&
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
	case UNARY AND:
		m = p->in.left;
		if (m->in.op == NAME && (SFISGLOBAL(m->in.sname->sflags)) == 0 && m->tn.rval >= 0) {
			m = clocal(m);
			if (m->in.op == UNARY MUL) {
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
		if (n->busy > 0 || n->kind == RKTEMP)
			continue;
#ifdef OLD_D3K
#ifdef OLD_D3KC
		if (flag == 2 && n->cls == RCWORD && n->sym && n->sym->sclass == PARAM)
#else
		if (flag == 2 && n->cls <= RCPAIR && n->sym && n->sym->sclass == PARAM)
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
	case RCVBYTE:
	case RCVWORD:
	case RCVPAIR:
	case RCVFLOAT:
		return;
	}
	if (a->kind == RKTEMP)
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
#ifdef OLD_D2FA
		/* OLD_D2FA: named by the assignment of a computed value (not the copy of a variable: delrun q = r) */
		if (!d2facopy)
			a->flags |= RSASG;
		else
			a->flags &= ~RSASG;
#endif
		a->sym = p2->in.sname;
		a->off = p2->tn.lval;
	}
	a->flags &= ~RSCONST;
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
		n->flags &= ~RSCONST;
		n->kind = RKVAR;
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

	for (i = R0; i <= R7; i++)
		rnnull(i, 4);
	for (i = R0; i <= R6; i += 2)
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
		n->flags &= ~RSCONST;
		n->kind = RKVAR;
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
/* OLD_D2C: a store went through the register reg (segmented: a pair and its words): it no longer holds a name (not a register variable) */
void
d2cforget(reg)
	register int reg;
{
	register struct rs *n;

	for (n = rsuse; n; n = n->next)
#ifdef SEG
		if (n->cls <= RCFLOAT && n->kind != RKTEMP && (n->reg == reg || n->reg == reg + 1)) {	/* the address is a pair */
#else
		if (n->cls <= RCFLOAT && n->kind != RKTEMP && n->reg == reg) {
#endif
			n->sym = 0;
			n->off = 0;
		}
}
#endif

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

	if (s->sclass == REGISTER) {
		cls = rclass(type, reg);
		r = getreg(cls, reg, RJAM, s);
		if (r == s->sbyte5)
			return r;
		rsreturn(cls, r);
		return -1;
	}
	if (s == fpsym1 || s == fpsym2)
		return symreg(s, off, type);
#ifdef OLD_D3P
	/* also under a cast between types of the size of a pointer (D3PCAST) */
	if (type != s->stype && !D3PCAST(type, s->stype))
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

#ifdef OLD_D2FA
int d2fanone;	/* OLD_D2FA: the statement is a condition with && or || */
#endif

/* a name that lives in a register becomes REG, otherwise an OREG */
void
fixname(p)
	register NODE *p;
{
	register int r;

	r = (p->in.rall & MUSTDO) ? p->in.rall & ~MUSTDO : -1;
#ifdef OLD_D2FA
	/* vc (gopt): an automatic pointer or long is also found in the pair an assignment named with its frame place
	   (rnassign: ~L1+off), when no register is wanted for it (an argument is loaded from the frame) */
	if ((p->in.sname->sclass == AUTO || p->in.sname->sclass == PARAM) && p->in.rall == NOPREF && !d2fanone && d2faltype) {
		register struct rs *n;
		register struct sym *f;

		f = p->in.sname->sclass == PARAM && (SFISPMEM(p->in.sname->sflags)) == 1 ? fpsym2 : fpsym1;
		for (n = rsuse; n; n = n->next)
			if (n->cls == RCPAIR && n->sym == f && n->off == p->in.sname->soffset / SZCHAR && rclass(p->in.type, n->reg) == RCPAIR &&
			    (p->in.sname->sclass == AUTO || (n->flags & RSASG))) {
				n->busy = 1;
				p->in.op = REG;
				p->tn.rval = n->reg;
				p->tn.lval = 0;
				return;
			}
	}
#endif
	r = regforsym(r, p->in.sname, p->in.type, p->in.sname->soffset / SZCHAR);
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

	if ((SFISGLOBAL(p->in.sname->sflags)) == 1)
		return;
	if (p->in.op == NAME && p->in.sname->sclass == STATIC)
		return;
	if (p->in.op == NAME && p->tn.rval < 0)
		return;
	p->in.op = OREG;
	p->tn.rval = FP;
	p->tn.lval = p->in.sname->soffset / SZCHAR;
	p->stn.stsize = 0;
	p->in.sname = p->in.sname->sclass == PARAM && ((SFISPMEM(p->in.sname->sflags)) == 1) ? fpsym2 : fpsym1;
}

#ifndef OLD_DNR	/* vc: the register records of gopt (F R S T, -O3 and -O5) are not read */
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

#endif
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
	for (i = R8; i <= R14; i++)
#endif
		if (rsuse)
			rsreturn(RCANY, i);
	for (i = F4; i <= F7; i++)
		if (rsuse)
			rsreturn(RCANY, i);
}

#ifndef OLD_DNR
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
	if (type == FLOAT || type == DOUBLE) {
		i = a + NREGVW;
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
	got = getreg(cls, r, RJAM, s);
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
		if (regvar[i].rcls != STATIC)
			off = off / SZCHAR;
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
	if (ISPTR(t))
		t = LONG;
#endif
	switch (t) {
	case LONG:
		printf("\tldl\t");
		memname(s, off);
		printf(",rr%d\n", reg);
		break;
	case FLOAT:
		printf("\tflds\t");
		memname(s, off);
		printf(",f%d\n", reg - 16);
		break;
	case DOUBLE:
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
		if (s->sclass != STATIC)
			off = off / SZCHAR;
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
	if (ISPTR(t))
		t = LONG;
#endif
	switch (t) {
	case LONG:
		printf("\tldl\t");
		printf("rr%d,", reg);
		memname(s, off);
		putchar('\n');
		break;
	case FLOAT:
		printf("\tflds\t");
		printf("f%d,", reg - 16);
		memname(s, off);
		putchar('\n');
		break;
	case DOUBLE:
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

	if (s && (SFISGLOBAL(s->sflags)))
		printf("_%s", ascii + s->nameoff);
	else if (s && s->sclass == STATIC) {
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

#endif
