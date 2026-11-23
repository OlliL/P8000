/*
 * allo3.c - allocation of the registers and temporaries of a template
 */
#include "mfile3"

#ifdef OLD_D3H
extern int d3hdepth;
#endif
extern void rwprint();
static char sccsid[] = "@[$]allo3.c\t2.4  08/24/83 11:29:34 Copr. 1981 87wega3.2.";


static int d2qfree();
#ifdef OLD_D2QI
static int d2qidle();
#endif
static int d2ofree();

/* count the register of an operand as busy; sym and lval are only passed on */
void
rbusy(reg, type, sym, lv)
	int reg;
	int type;
	struct sym *sym;
	long lv;
{
	register int cls = rclass(type, reg);
	register struct rs *n, *prev;
	int l;

	if (cls == RCVWORD || cls == RCVPAIR || cls == RCVFLOAT)
		return;
	for (n = rsuse; n; n = n->next)
		if (n->cls == cls && n->reg == reg)
			goto found;
	if (cls == RCWORD) {
		l = reg & 1 ? reg - 1 : reg;		/* the long register that holds the word register */
		for (prev = n = rsuse; n; prev = n, n = n->next)
			if (n->cls == RCPAIR && n->reg == l) {
				rsreturn(RCPAIR, l);
				if (getreg(RCWORD, reg, RJAM, 0) != reg)
					cerror("rbusy: register %d not free", reg);
				/* the original counts the node in front of the one it found */
				n = prev;
				goto found;
			}
	}
#ifdef OLD_D2TC
	/* vc: a free word register (the low half of a result pair already given back: reclaim of gopt operand,
	   p->in.sname - symtab) is taken */
	if (cls == RCWORD && reg >= 0 && reg < R8 && getreg(RCWORD, reg, RJAM, sym) == reg)
		return;
#endif
	cerror("rbusy: register to be made busy not found on rslist");
found:
	n->busy++;
}

/* ---- temporaries ------------------------------------------------------------------------------------ */

/* k words of temporary space for parameters (the area behind the parameters of a call) */
long
freeparm(k)
	register int k;
{
	register long t = tmpoff;

	tmpoff += k << 4;
#ifdef OLD_D14Q
	if (tmpoff > maxoff)
		maxoff = tmpoff;
#endif
	if (tmpoff > maxoff)
		cerror("paroff exceeds parmax, %d, %ld", (int)tmpoff / SZCHAR, maxoff / SZCHAR);
	return t;
}

#ifdef OLD_D2XP
static int d2xresc;	/* allo: the registers it takes are the result (RESC1) */
#endif

/*
 * Get the resources of the table entry q for the tree p: the registers (needs bits 0-3: A registers,
 * 4-7: B registers), temporaries in the frame (8-9) and parameter temporaries (10-11), in resc[]. Returns 0
 * when a register is not available.
 */
int
allo(p, q)
	NODE *p;
	struct optab *q;
{
	register int need, k, k0;
	register int spare;
	register int b;
	register struct rs *s;
	int cls;

	need = q->needs;
	b = need;
	k = 0;
#ifdef OLD_D2XP
	d2xresc = q->rewrite & RESC1;
#endif

	while (need & NACOUNT) {			/* A registers */
		resc[k].in.op = REG;
		resc[k].in.type = p->in.type;
		resc[k].tn.rval = allocreg(p, need & NAMASK);
		resc[k].tn.lval = 0;
		resc[k].in.sname = 0;
		need -= NAREG;
		k++;
	}
	k0 = k;
	while (b & NACOUNT) {				/* the registers stay busy only for the code of the entry */
		if (((b & NAMASK) && szty(p->in.type) == 2) || (p->in.op == STASG && p->stn.stsize == 4) ||
		    (dope[p->in.op] & DIVFLG))
			cls = RCPAIR;
		else
			cls = rclass(p->in.type, resc[k0].tn.rval);
		b -= NAREG;
		k0--;
		s = rfind(resc[k0].tn.rval, cls);
		s->busy = 0;		/* no test: the original writes through a null pointer when the register is not found */
	}
	while (need & NBCOUNT) {			/* B registers */
		resc[k].in.op = REG;
		resc[k].in.type = p->in.type;
		resc[k].tn.rval = allocreg(p, need & NBMASK);
		resc[k].tn.lval = 0;
		resc[k].in.sname = 0;
		need -= NBREG;
		k++;
	}
	k0 = k;
	while (b & NBCOUNT) {
		cls = rclass(p->in.type, resc[k0].tn.rval);
		b -= NBREG;
		k0--;
		s = rfind(resc[k0].tn.rval, cls);
		s->busy = 0;		/* no test: the original writes through a null pointer when the register is not found */
	}
	if (need & NTMASK) {			/* a temporary in the frame */
		resc[k].in.op = OREG;
		resc[k].in.type = p->in.type;
		resc[k].tn.rval = TMPREG;
		resc[k].stn.stsize = 0;
		resc[k].tn.lval = freetemp(TEMPWORDS(p, (need & NTMASK) / NTEMP));
		resc[k].in.sname = fpsym1;
		resc[k].tn.lval = BITOOR(resc[k].tn.lval);
		k++;
	}
	if (need & NPTMASK) {			/* a temporary for parameters */
		resc[k].in.op = OREG;
		resc[k].in.type = p->in.type;
		resc[k].tn.rval = TMPREG;
		resc[k].stn.stsize = 0;
		resc[k].tn.lval = freeparm(PARMWORDS(p, (need & NPTMASK) / NPTEMP));
		resc[k].tn.lval = BITOOR(resc[k].tn.lval);
#ifdef SEG
		resc[k].in.sname = symtab0;
#else
		resc[k].in.sname = 0;
#endif
		k++;
	}
	for (k0 = 0; k0 < k; k0++)
		if (resc[k0].tn.rval < 0)
			return 0;
	nxwork(p, q->nx);
	return 1;
}

/* k words of temporary space in the frame */
long
freetemp(k)
	int k;
{
	register long t = paroff;

	paroff += k * SZINT;
	if (paroff > parmax)
		parmax = paroff;
	if (paroff - baseoff > maxtemp)
		maxtemp = paroff - baseoff;
	return t;
}

/* ---- register classes ------------------------------------------------------------------------------- */

int
rclass(type, reg)
	int type;
	int reg;
{
	register int c;

	if (type != FLOAT && type != DOUBLE) {
#ifdef SEG
		if (szty(type) == 2 || type == STRTY || type == UNIONTY)
#else
		if (szty(type) == 2)
#endif
		{
			if (reg > R7)
				c = RCVPAIR;
			else
				c = RCPAIR;
		} else if (reg > R7)
			c = RCVWORD;
		else
			c = RCWORD;
	} else if (reg > F3)
		c = RCVFLOAT;
	else
		c = RCFLOAT;
	return c;
}

#ifdef OLD_D2QF
/* the word register r is busy once and its partner r + 1 is free */
static int
d2qfree(r)
	register int r;
{
	register struct rs *n;

	for (n = rsuse; n; n = n->next)
		if (n->reg == r && n->cls == RCWORD)
			break;
	if (n == 0 || n->busy != 1)
		return 0;
	for (n = rsfree; n; n = n->next)
		if (n->reg == r + 1 && n->cls == RCWORD)
			return 1;
	return 0;
}

#ifdef OLD_D2QI
/* OLD_D2Q (v0): the odd partner may also be idle (a parameter no longer used), it is given back (as d2ofree) */
static int
d2qidle(r)
	register int r;
{
	register struct rs *n;

	if (d2qfree(r))
		return 1;
	for (n = rsuse; n; n = n->next)
		if (n->reg == r && n->cls == RCWORD)
			break;
	if (n == 0 || n->busy != 1)
		return 0;
	for (n = rsuse; n; n = n->next)
		if (n->reg == r + 1 && n->cls == RCWORD && n->busy <= 0 && n->kind != RKTEMP) {
			rsreturn(RCWORD, r + 1);
			return 1;
		}
	return 0;
}
#else
#define d2qidle	d2qfree
#endif
#endif

#if defined(OLD_D2R) && defined(SEG)
/* the pair r is busy once (the address of the load) */
static int
d2rpair(r)
	register int r;
{
	register struct rs *n;

	for (n = rsuse; n; n = n->next)
		if (n->reg == r && n->cls == RCPAIR)
			return n->busy == 1;
	return 0;
}
#endif

#if defined(OLD_D2O) || defined(OLD_D2MO)
/* the odd word register r is busy once and its partner r - 1 is free, or idle and then given back */
static int
d2ofree(r)
	register int r;
{
	register struct rs *n;

	for (n = rsuse; n; n = n->next)
		if (n->reg == r && n->cls == RCWORD)
			break;
	if (n == 0 || n->busy != 1)
		return 0;
	for (n = rsfree; n; n = n->next)
		if (n->reg == r - 1 && n->cls == RCWORD)
			return 1;
	for (n = rsuse; n; n = n->next)
		if (n->reg == r - 1 && n->cls == RCWORD && n->busy <= 0 && n->kind != RKTEMP) {
			rsreturn(RCWORD, r - 1);
			return 1;
		}
	return 0;
}
#endif

#if defined(OLD_D2M) || defined(OLD_D2Q) || defined(OLD_D2S)
/* the word x given back and the pair y taken for the result (with the name of p); not free: x busy again (for q) */
static int
d2pair(x, y, p, q)
	register int x;
	int y;
	NODE *p, *q;
{
	register int r;

	rfree(x, INT);
	rsreturn(RCWORD, x);
	r = getreg(RCPAIR, y, 0, p->in.sname);
	if (r == -1)
		rbusy(x, 4, q ? q->in.sname : (struct sym *)0, q ? q->tn.lval : 0L);
	return r;
}
#endif

/* a register for the result of p; cookie says what the caller wants */
int
allocreg(p, cookie)
	NODE *p;
	int cookie;
{
	register int want, r, cls;
#if defined(OLD_D2R) && defined(SEG) && defined(OLD_D2V)
	int d2rw = 0;	/* r is the even half of a pair address already given back */
#endif

	if callop(p->in.op)
		want = callreg(p);
	else if ((p->in.rall & MUSTDO) || !(p->in.rall & NOPREF))
		want = p->in.rall & ~MUSTDO;
	else
		want = -1;
	if (p->in.sname == fpsym1)
		fpsym1->soffset = p->tn.lval;
	else if (p->in.sname == fpsym2)
		fpsym2->soffset = p->tn.lval;
	if (((cookie & NAMASK) && szty(p->in.type) == 2) || (p->in.op == STASG && p->stn.stsize == 4) ||
#ifdef SEG
	    p->in.type == STRTY || p->in.type == UNIONTY ||
#endif
	    (dope[p->in.op] & DIVFLG))
		cls = RCPAIR;
	else
		cls = rclass(p->in.type, want);
#ifdef OLD_D2R
#ifdef SEG
	r = -1;
	if (cls == RCWORD && (p->in.op == UNARY MUL || p->in.op == OREG)) {
		register NODE *q = getlr(p, 'L');	/* segmented: a pair address, used only here, gives its even half */
		register int pr = -1;

		if (q->in.op == UNARY MUL)
			q = q->in.left;
		if (q->in.op == OREG && R2TEST(q->tn.rval))
			pr = R2UPK1(q->tn.rval);	/* rr2(#n), and rr2(r1) with an index */
		else if (q->in.op == OREG && q->tn.rval < R8)
			pr = q->tn.rval;	/* @rr2, rr2(#n): a pair base below 0x100 */
		if ((pr == R2 || pr == R4) && (want == -1 || want == pr || want == pr + 1) && d2rpair(pr)) {	/* rr2, rr4 */
			rfree(pr, LONG);		/* LONG: the pair */
			rsreturn(RCPAIR, pr);
			r = getreg(RCWORD, want == pr + 1 ? pr + 1 : pr, 0, p->in.sname);	/* the odd half when it is wanted */
#ifdef OLD_D2V
			d2rw = 1;
#endif
		}
		/* an index in an odd word register (tab(r3)): the even half of its pair, when free */
		if (r == -1 && q->in.op == OREG && q->tn.rval < R8 && (q->tn.rval & 1) && (want == -1 || want == q->tn.rval - 1))
			r = getreg(RCWORD, q->tn.rval - 1, 0, p->in.sname);
	}
	if (r == -1)
#endif
	r = cls == RCWORD && (p->in.op == UNARY MUL || p->in.op == OREG) ? freereg(p, want, cookie) : -1;	/* a load through a pointer: its register */
#ifdef OLD_D2V
	if (r != -1 && want == -1 && r < R8 && !d2vfirst(r)) {
#if defined(OLD_D2R) && defined(SEG)
		/* the half of a pair address OLD_D2R gave back belongs to no operand: it goes back too (gopt chkfamindvar:
		   ldb rl3,@rr4 kept r4 busy, the next pair was rr6, the parameter r) */
		if (d2rw)
			rsreturn(RCWORD, r);
#endif
		r = -1;		/* v0: only when no lighter register is free; r stays busy for the operand */
	}
#endif
#ifdef OLD_D2N
	/* v0: a loaded name is not remembered: a word (a global, an auto), a pair only for a global (gopt linkp, eval) */
	if (r == -1 && (cls == RCWORD && (p->in.op == NAME || (p->in.op == OREG && p->tn.rval == FP)) || cls == RCPAIR && p->in.op == NAME))
		r = getreg(cls, want, RJAM, (struct sym *)0);
#endif
#ifdef OLD_D2C
	if (r == -1 && p->in.op == REG)
		r = getreg(cls, want, RJAM, (struct sym *)0);	/* v0: the copy of a name in a register holds no name */
#endif
#ifdef OLD_D2X
	if (r == -1 && cls == RCPAIR && want == -1 && optype(p->in.op) == BITYPE && p->in.op != LS && p->in.op != RS &&
	    p->in.op != ASG LS && p->in.op != ASG RS && p->in.left->in.op == REG && szty(p->in.left->in.type) == 2
#ifdef OLD_D2XP
	    /* vc (gopt): not the pair of a parameter (nodeno c = q + 6: ldl rr2,rr6; inc r3,#6) */
	    && !(p->in.left->in.sname && p->in.left->in.sname->sclass == PARAM)
	    /* nor for a scratch register when the result is the left operand (gopt constfold: ld U1,UR; and AL,AR; and UL,U1) */
	    && d2xresc
#endif
	    )
		r = freereg(p, want, cookie);	/* v0: a long result into the pair of its left operand */
#endif
#if defined(OLD_D2M) || defined(OLD_D2S)
	if (r == -1 && cls == RCPAIR && want == -1 && (p->in.op == MUL || p->in.op == DIV || p->in.op == SCONV) && p->in.left->in.op == REG &&
	    szty(p->in.left->in.type) == 1 && p->in.left->tn.rval < R8) {
		register NODE *l = p->in.left;
		register int lr = l->tn.rval;

#ifdef OLD_D2M
		/* v0: a word multiplied in the pair of its even register; divided too when it is a name found in its
		 * register (gopt affsize; not a computed value: codgen tagfind (lo + hi) / 2) */
		if ((p->in.op == MUL || p->in.op == DIV && l->in.sname) && !(lr & 1) && d2qfree(lr))
			r = d2pair(lr, lr, p, l);
#ifdef OLD_D2MO
		/* an odd one in the pair below it when the even half is free or idle; not a parameter (gopt canshare,
		 * setedges, testbit) */
		else if (p->in.op == MUL && (lr & 1) && !(l->in.sname && l->in.sname->sclass == PARAM) && d2ofree(lr))
			r = d2pair(lr, lr - 1, p, l);
#endif
#endif
#ifdef OLD_D2S
		/* vc: a word in an odd register widened to long (SCONV) takes the lighter of the pair getreg gives and its own pair
		 * (rweight; cas symtype: r3 into rr2; locadv: r7 stays, rr2; asmins: r1, rr2 before rr0; not a byte: opshort) */
		if (p->in.op == SCONV && l->in.type != CHAR && l->in.type != UCHAR && (lr & 1) && d2ofree(lr)) {
			r = getreg(RCPAIR, -1, 0, p->in.sname);
			if (r == -1 || rweightof(lr - 1) < rweightof(r)) {
				if (r != -1)
					rsreturn(RCPAIR, r);
				r = d2pair(lr, lr - 1, p, l);
			}
		}
#endif
	}
#endif
#ifdef OLD_D2Q
	if (r == -1 && cls == RCPAIR && (p->in.op == UNARY MUL || p->in.op == OREG)) {
		register NODE *q = getlr(p, 'L');	/* a long through a pointer: the pair of its even register */

		if (q->in.op == UNARY MUL)
			q = q->in.left;
#ifdef OLD_D2QO
		/* v0p: also when an odd word register is wanted (the low half of the value, an argument) */
		if ((q->in.op == OREG || q->in.op == REG) && q->tn.rval < R8 && !(q->tn.rval & 1) &&
		    (want == -1 || want == q->tn.rval || (want >= 0 && want < R8 && (want & 1))) && d2qfree(q->tn.rval)) {
#else
		if ((q->in.op == OREG || q->in.op == REG) && q->tn.rval < R8 && !(q->tn.rval & 1) && (want == -1 || want == q->tn.rval) &&
		    d2qidle(q->tn.rval)) {
#endif
			r = d2pair(q->tn.rval, q->tn.rval, p, q);		/* merged with its free partner */
		}
#ifdef SEG
		/* segmented: base and index (`rr4(r2)`), the index in an even word register: the pair of the index, also when
		 * another pair is wanted (an argument: a copy follows); vc: the index in an odd word register (`rr6(r3)`, alone
		 * or the low half of a product): the pair below it (gopt prcgraph) */
		else if (q->in.op == OREG && R2TEST(q->tn.rval) && (q->tn.rval & 0xFF) != 0 && (R2UPK2(q->tn.rval)) < 8) {
			register int ix = R2UPK2(q->tn.rval);

			if (!(ix & 1)) {
				if (d2qfree(ix))
					r = d2pair(ix, ix, p, (NODE *)0);
			} else if (d2rpair(ix - 1) || d2ofree(ix)) {
				if (d2rpair(ix - 1)) {		/* the index is the low half of a pair (a product) */
					rfree(ix - 1, LONG);
					rsreturn(RCPAIR, ix - 1);
				} else {
					rfree(ix, INT);
					rsreturn(RCWORD, ix);
				}
				r = getreg(RCPAIR, ix - 1, 0, p->in.sname);
				if (r == -1)
					cerror("allocreg: the pair of an index not free");
			}
		}
#endif
#ifdef OLD_D2O
		/* v0p: an odd address register takes its pair too when the even half is free or idle */
		else if ((q->in.op == OREG || q->in.op == REG) && q->tn.rval < R8 && (q->tn.rval & 1) &&
#ifdef SEG
		    /* vc: also when an odd word register is wanted (the low half of the value, an argument), as OLD_D2QO */
		    (want == -1 || want == q->tn.rval - 1 || (want >= 0 && want < R8 && (want & 1))) && d2ofree(q->tn.rval)) {
#else
		    (want == -1 || want == q->tn.rval - 1) && d2ofree(q->tn.rval)) {
#endif
			r = d2pair(q->tn.rval, q->tn.rval - 1, p, q);
		}
#endif
	}
#endif
	if (r == -1)
		r = getreg(cls, want, RJAM, p->in.sname);
#else
#ifdef OLD_D2PW
	/* OLD_D2PW (libcisam): the copy of a register that is wanted in a busy register goes into the other half of that
	   pair when the half is idle and holds a name, not a parameter (compare/libcisam/COMPILER.md) */
	if (p->in.op == REG && cls == RCWORD && want >= 0 && want < R8) {
		register struct rs *q;

		for (q = rsuse; q; q = q->next)
			if (q->cls == RCWORD && q->reg == want && q->busy > 0)
				break;
		if (q) {
			for (q = rsuse; q; q = q->next)
				if (q->cls == RCWORD && q->reg == (want ^ 1) && q->busy <= 0 && q->kind != RKTEMP && q->sym &&
				    q->sym->sclass != PARAM)
					break;
			if (q) {
				rsreturn(RCWORD, want ^ 1);
				r = getreg(cls, want ^ 1, RJAM, p->in.sname);
				if (r != -1)
					goto d2pw;
			}
		}
	}
#endif
#ifdef OLD_D2CW
	/* v1 (segmented): the copy of a word of a register variable pair (`(int)x` of a pointer in rr12) holds no name
	   (libPW move: the byte behind it takes r2 again) */
	r = p->in.op == REG && p->tn.rval >= R8 && p->tn.rval < R14 && szty(p->in.type) == 1 && p->in.sname &&
	    szty(p->in.sname->stype) == 2 ? getreg(cls, want, RJAM, (struct sym *)0) : -1;
	if (r == -1)
#endif
	r = getreg(cls, want, RJAM, p->in.sname);
#endif
	if (r == -1) {
		r = freereg(p, want, cookie);
		if (r == -1)
			cerror("register jam");
	}
#ifdef OLD_D2PW
d2pw:
#endif
#ifdef OLD_D2CT
#ifdef SEG
	/* OLD_D2CT (libcisam, segmented; needs OLD_D3H): the copy of a pair that holds a name and was given out in the last
	   condition into the pair of a wanted odd word register: the pair itself is taken, no copy
	   (compare/libcisam/COMPILER.md) */
	if (p->in.op == REG && cls == RCPAIR && want >= 0 && want < R8 && (want & 1) && p->tn.rval >= 0 && p->tn.rval < R8 && !(p->tn.rval & 1)) {
		register struct rs *q;

		for (q = rsuse; q; q = q->next)
			if (q->cls == RCPAIR && q->reg == p->tn.rval)
				break;
		if (q && q->sym && q->sym->sclass != PARAM && q->busy == 1 && q->kind != RKTEMP && (q->flags & RSD3H)) {
			if (r != -1 && r != p->tn.rval)
				rsreturn(RCPAIR, r);
			r = p->tn.rval;
			q->busy++;
		}
	}
#endif
	/* OLD_D2CT (needs OLD_D3H): the copy into the wanted register of a scratch word that holds a name (not a parameter)
	   and was given out in the last condition: the name goes with the copy, the source register is free */
	if (p->in.op == REG && cls == RCWORD && want >= 0 && r == want && p->tn.rval >= 0 && p->tn.rval < R8 && p->tn.rval != r) {
		register struct rs *q;

		for (q = rsuse; q; q = q->next)
			if (q->cls == RCWORD && q->reg == p->tn.rval)
				break;
		if (q && q->sym && q->sym->sclass != PARAM && q->busy == 1 && q->kind != RKTEMP && (q->flags & RSD3H)) {
			q->sym = 0;
			q->busy = 0;
			rsreturn(RCWORD, p->tn.rval);
		}
	}
#endif
#ifdef OLD_D3H
	if (d3hdepth > 0) {	/* OLD_D3H: given out inside a condition */
		register struct rs *q;

		for (q = rsuse; q; q = q->next)
			if (q->reg == r && q->cls == cls)
				q->flags |= RSD3H;
	}
#endif
	return r;
}

/* a register that the operand of the result can reuse */
int
freereg(p, r, cookie)
	NODE *p;
	int r;
	int cookie;
{
	register NODE *q;
	register int ty;
	register NODE *spare;
	register int reg;
	register struct sym *sym;
	char side;
	int cls;

	if (cookie & NASL | NBSL)	/* sic: always true (precedence), see the quirks table */
		side = 'L';
	else if (cookie & NASR | NBSR)
		side = 'R';
	else
		return -1;
	q = getlr(p, side);
	spare = q->in.right;
	ty = p->in.type;
	sym = p->in.sname;
	if (r == -1) {
		if (cookie & NASL | NASR) {
			for (reg = R0; reg <= R7; reg++)
				if (usable(p, side, reg) > 0) {
				found:
					rfree(reg, ty);
					cls = rclass(ty, reg);
					return getreg(cls, reg, RJAM, sym);
				}
		} else if (cookie & NBSL | NBSR) {
			for (reg = F0; ; reg++) {
				if (reg > F3)
					return -1;
				if (usable(p, side, reg) > 0)
					goto found;
			}
		}
		return -1;
	}
	if (usable(p, side, r) <= 0)
		return -1;
	rfree(r, ty);
	cls = rclass(ty, r);
	return getreg(cls, r, RJAM, sym);
}

/* ---- getting registers ------------------------------------------------------------------------------- */

/* does the operand on side of p already live in register r (or is its long register r - 1)? */
int
usable(p, side, r)
	register NODE *p;
	int side;
	int r;
{
	register int rv;

	p = getlr(p, side);
	if (p->in.op == UNARY MUL)
		p = p->in.left;
	rv = p->tn.rval;
#ifdef SEG
	if (p->in.op == OREG) {
		if (R2TEST(rv))
			return R2UPK1(rv) == r || R2UPK1(rv) + 1 == r || R2UPK2(rv) == r;
		return rv == r || (p->tn.lval == 0 && p->in.sname == 0 && !(p->in.op == OREG && p->stn.stsize < 0) && rv + 1 == r);
	}
	if (p->in.op == REG)
		return rv == r || (szty(p->in.type) == 2 && rv + 1 == r);
	return 0;
#else
	if (p->in.op == OREG)
		return rv == r;
	if (p->in.op == REG)
		return (rv == r && (r != R0 || !ISPTR(p->in.type))) || (szty(p->in.type) == 2 && rv + 1 == r);
	return 0;
#endif
}

/* free the registers an operand node holds */
void
recl2(p)
	register NODE *p;
{
	register int r = p->tn.rval;

	if (p->in.op == REG) {
		rfree(r, p->in.type);
		return;
	}
	if (p->in.op != OREG)
		return;
	if (R2TEST(r)) {			/* base and index register */
		rfree(R2UPK1(r), PTR|INT);
		if (r & 0xFF)
			rfree(R2UPK2(r), INT);
		return;
	}
	if (p->tn.lval != 0 || p->in.sname != 0 || (p->in.op == OREG && p->stn.stsize < 0))
		rfree(r, INT);
	else
		rfree(r, PTR|INT);
}

/* the register is no longer needed by one more operand */
void
rfree(reg, type)
	register int reg;
	register int type;
{
	register struct rs *n, *prev;
	register int cls = rclass(type, reg);

#ifdef SEG
	if (reg == FP || reg == R14)
#else
	if (reg == FP)
#endif
		return;
	for (n = rsuse; n; n = n->next)
		if (n->cls == cls && n->reg == reg)
			break;
	if (n && n->busy > 0)
		n->busy--;
}

/* ---- reclaim ------------------------------------------------------------------------------------------- */

/* cookie masks and the shapes of the node that serves them */

/*
 * After the code of an entry was printed: free the registers of the tree and make the result (the node the
 * rewrite names: RLEFT, RRIGHT, RESC1..3) the new tree p.
 */
void
rwprint(rw)
	int rw;
{
	register int i, flag;
	static char *rwnames[] = {
		"RLEFT",
		"RRIGHT",
		"RESC1",
		"RESC2",
		"RESC3",
		0,
	};

top:
	if (rw == RNULL) {
		printf("RNULL");
		return;
	}
	if (rw == RNOP) {
		printf("RNOP");
		return;
	}
	flag = 0;
	for (i = 0; rwnames[i]; ++i) {
		if (rw & (1 << i)) {
			if (flag)
				putchar('|');
			++flag;
			printf(rwnames[i]);
		}
	}
}

void
reclaim(p, rw, cookie)
	NODE *p;
	int rw;
	int cookie;
{
	register int want;
	register NODE *c;
	register NODE **ap;
	register int o;
	register struct respref *r;
	NODE *a[5];

	o = p->in.op;
	if (rw == RNOP)
		goto done;
	if (o == FREE && rw == RNULL)
		goto done;
	want = o;
	walkf(p, recl2);
	if (rw == RNULL || (cookie & FOREFF)) {
		tfree(p);
		goto done;
	}
	if ((cookie & FORCC) && (rw & RESCC)) {	/* the condition codes */
		tfree(p);
		p->in.op = CCODES;
		p->tn.lval = want;
		p->tn.rval = 0;
		goto done;
	}
	ap = a;
	if (rw & RLEFT)
		*ap++ = getlr(p, 'L');
	if (rw & RRIGHT)
		*ap++ = getlr(p, 'R');
	if (rw & RESC1)
		*ap++ = &resc[0];
	if (rw & RESC2)
		*ap++ = &resc[1];
	if (rw & RESC3)
		*ap++ = &resc[2];
	if (ap == a)
		cerror("illegal reclaim");
	*ap = NIL;
	for (r = respref; r->cform; ++r) {
		if (!(cookie & r->cform))
			continue;
		for (ap = a; (c = *ap) != NIL; ap++)
			if (tshape(c, r->mform))
				goto found;
	}
	cerror("cannot reclaim");
found:
	if (o == STARG)
		p = p->in.left;
	c->in.type = p->in.type;
	c = tcopy(c);
	tfree(p);
	p->in.op = c->in.op;
	p->tn.lval = c->tn.lval;
	p->tn.rval = c->tn.rval;
	p->in.sname = c->in.sname;
	if (c->in.op == OREG)
		p->stn.stsize = c->stn.stsize;
	c->in.op = FREE;
	switch (p->in.op) {
	case REG:
		goto isreg;
	case OREG:
		break;
	}
done:
	tmpcleanup();
	rfreeall(0);
	return;
isreg:
	if (!rtyflg) {
		if (p->in.type == SHORT)
			p->in.type = INT;
		else if (p->in.type == USHORT)
			p->in.type = UNSIGNED;
	}
	if (rw & RESC) {			/* the second register of a long result */
		want = p->tn.rval;
		want++;
		if (!(p->in.rall & MUSTDO))
			p->in.rall = (p->in.rall & NOPREF) | want;
		rfree(p->tn.rval, p->in.type);
		if (getreg(RCWORD, want, RJAM, p->in.sname) != want)
			cerror("reclaim: cannot find register free");
		p->tn.rval = want;
		if (p->in.type == LONG)
			p->in.type = INT;
		else if (p->in.type == ULONG)
			;
		p->in.type = UNSIGNED;		/* sic: also after the LONG case (the original jumps to this store) */
		if (p->in.rall & MUSTDO)
			want = p->in.rall & ~MUSTDO;
	} else if (o == MOD || o == ASG MOD) {	/* a long remainder goes to rr4 */
		if (p->in.type == LONG || p->in.type == ULONG) {
			rfree(p->tn.rval, p->in.type);
			if (getreg(RCPAIR, R4, RJAM, p->in.sname) != R4)
				cerror("reclaim: cannot find register free");
			p->tn.rval = R4;
			if (!(p->in.rall & MUSTDO))
				p->in.rall = (p->in.rall & NOPREF) | R4;
		}
	}
	if (!(p->in.rall & MUSTDO))
		goto done;
	if (!(rw & RESC)) {
		want = p->in.rall & ~MUSTDO;
		if (want & NOPREF)
			goto done;
	}
	if (want == p->tn.rval)
		goto done;
	rfree(p->tn.rval, p->in.type);
	if (getreg(rclass(p->in.type, want), want, RJAM, p->in.sname) != want)
		cerror("reclaim dest reg not free");
	rmove(want, p->tn.rval, p->in.type);
	p->tn.rval = want;
	goto done;
}
