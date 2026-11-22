/*
 * allo3.c - allocation of the registers and temporaries of a template
 */
#include "mfile3"

static char sccsid[] __attribute__((unused)) = "@[$]allo3.c\t2.4  08/24/83 11:29:34 Copr. 1981 87wega3.2.";


/* count the register of an operand as busy; sym and lval are only passed on */
void
rbusy(int reg, int type, struct sym *sym, long lv)
{
	(void)sym;
	(void)lv;
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
	cerror("rbusy: register to be made busy not found on rslist");
found:
	n->busy++;
}

/* ---- temporaries ------------------------------------------------------------------------------------ */

/* k words of temporary space for parameters (the area behind the parameters of a call) */
long
freeparm(int k)
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

/*
 * Get the resources of the table entry q for the tree p: the registers (needs bits 0-3: A registers,
 * 4-7: B registers), temporaries in the frame (8-9) and parameter temporaries (10-11), in resc[]. Returns 0
 * when a register is not available.
 */
int
allo(NODE *p, struct optab *q)
{
	register int need, k, k0;
	register int b;
	register struct rs *s;
	int cls;

	need = q->needs;
	b = need;
	k = 0;

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
		if (s)		/* the original does not test: it writes through a null pointer (data byte 3, inside sccsid of reader3.c) */
			s->busy = 0;
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
		if (s)		/* the original does not test: it writes through a null pointer (data byte 3, inside sccsid of reader3.c) */
			s->busy = 0;
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
freetemp(int k)
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
rclass(int type, int reg)
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
				c = 7;
			else
				c = 3;
		} else if (reg > R7)
			c = 6;
		else
			c = 2;
	} else if (reg > F3)
		c = 8;
	else
		c = 4;
	return c;
}

#ifdef OLD_D2QF
/* the word register r is busy once and its partner r + 1 is free */
static int
d2qfree(int r)
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
#endif

#if defined(OLD_D2O) || defined(OLD_D2MO)
/* the odd word register r is busy once and its partner r - 1 is free, or idle and then given back */
static int
d2ofree(int r)
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

/* a register for the result of p; cookie says what the caller wants */
int
allocreg(NODE *p, int cookie)
{
	register int want, r, cls;

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
	r = cls == RCWORD && (p->in.op == UNARY MUL || p->in.op == OREG) ? freereg(p, want, cookie) : -1;	/* a load through a pointer: its register */
#ifdef OLD_D2V
	if (r != -1 && want == -1 && r < R8 && !d2vfirst(r))
		r = -1;		/* v0: only when no lighter register is free; r stays busy for the operand */
#endif
#ifdef OLD_D2D
	if (r == -1 && cls == RCPAIR && want == -1 && (p->in.op == DIV || p->in.op == MOD || p->in.op == ASG DIV || p->in.op == ASG MOD)) {
		register NODE *l = getlr(p, 'L');	/* a word operand in an odd register: its pair */

		if (l->in.op == REG && (l->tn.rval & 1) && l->tn.rval < R8 && szty(l->in.type) == 1) {
			rfree(l->tn.rval, l->in.type);
			r = getreg(RCPAIR, l->tn.rval - 1, RJAM, p->in.sname);
			if (r != l->tn.rval - 1) {
				if (r != -1)
					rfree(r, p->in.type);
				r = -1;
				getreg(RCWORD, l->tn.rval, RJAM, l->in.sname);
			}
		}
	}
#endif
#ifdef OLD_D2N
	if (r == -1 && cls == RCWORD && (p->in.op == NAME || (p->in.op == OREG && p->tn.rval == FP)))
		r = getreg(cls, want, RJAM, (struct sym *)0);	/* v0: a loaded name is not remembered */
#endif
#ifdef OLD_D2C
	if (r == -1 && p->in.op == REG)
		r = getreg(cls, want, RJAM, (struct sym *)0);	/* v0: the copy of a name in a register holds no name */
#endif
#ifdef OLD_D2X
	if (r == -1 && cls == RCPAIR && want == -1 && optype(p->in.op) == BITYPE && p->in.op != LS && p->in.op != RS &&
	    p->in.op != ASG LS && p->in.op != ASG RS && p->in.left->in.op == REG && szty(p->in.left->in.type) == 2)
		r = freereg(p, want, cookie);	/* v0: a long result into the pair of its left operand */
#endif
#ifdef OLD_D2M
	if (r == -1 && cls == RCPAIR && want == -1 && p->in.op == MUL && p->in.left->in.op == REG &&
	    szty(p->in.left->in.type) == 1 && p->in.left->tn.rval < R8 && !(p->in.left->tn.rval & 1) && d2qfree(p->in.left->tn.rval)) {
		rfree(p->in.left->tn.rval, INT);	/* v0: a word multiplied in the pair of its even register */
		rsreturn(RCWORD, p->in.left->tn.rval);
		r = getreg(RCPAIR, p->in.left->tn.rval, 0, p->in.sname);
		if (r == -1)
			rbusy(p->in.left->tn.rval, 4, p->in.left->in.sname, p->in.left->tn.lval);
	}
#ifdef OLD_D2MO
	/* an odd one in the pair below it when the even half is free or idle */
	if (r == -1 && cls == RCPAIR && want == -1 && p->in.op == MUL && p->in.left->in.op == REG &&
	    szty(p->in.left->in.type) == 1 && p->in.left->tn.rval < R8 && (p->in.left->tn.rval & 1) && d2ofree(p->in.left->tn.rval)) {
		rfree(p->in.left->tn.rval, INT);
		rsreturn(RCWORD, p->in.left->tn.rval);
		r = getreg(RCPAIR, p->in.left->tn.rval - 1, 0, p->in.sname);
		if (r == -1)
			rbusy(p->in.left->tn.rval, 4, p->in.left->in.sname, p->in.left->tn.lval);
	}
#endif
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
		    d2qfree(q->tn.rval)) {
#endif
			rfree(q->tn.rval, INT);
			rsreturn(RCWORD, q->tn.rval);		/* merged with its free partner */
			r = getreg(RCPAIR, q->tn.rval, 0, p->in.sname);
			if (r == -1)
				rbusy(q->tn.rval, 4, q->in.sname, q->tn.lval);
		}
#ifdef OLD_D2O
		/* v0p: an odd address register takes its pair too when the even half is free or idle */
		else if ((q->in.op == OREG || q->in.op == REG) && q->tn.rval < R8 && (q->tn.rval & 1) &&
		    (want == -1 || want == q->tn.rval - 1) && d2ofree(q->tn.rval)) {
			rfree(q->tn.rval, INT);
			rsreturn(RCWORD, q->tn.rval);
			r = getreg(RCPAIR, q->tn.rval - 1, 0, p->in.sname);
			if (r == -1)
				rbusy(q->tn.rval, 4, q->in.sname, q->tn.lval);
		}
#endif
	}
#endif
	if (r == -1)
		r = getreg(cls, want, RJAM, p->in.sname);
#else
	r = getreg(cls, want, RJAM, p->in.sname);
#endif
	if (r == -1) {
		r = freereg(p, want, cookie);
		if (r == -1)
			cerror("register jam");
	}
	return r;
}

/* a register that the operand of the result can reuse */
int
freereg(NODE *p, int r, int cookie)
{
	register NODE *q;
	register int ty;
	register int reg;
	register struct sym *sym;
	char side;
	int cls;

	if ((cookie & NASL) | NBSL)	/* sic: always true (precedence in the original), see the quirks table */
		side = 'L';
	else if ((cookie & NASR) | NBSR)
		side = 'R';
	else
		return -1;
	q = getlr(p, side);
	(void)q;
	ty = p->in.type;
	sym = p->in.sname;
	if (r == -1) {
		if ((cookie & NASL) | NASR) {
			for (reg = R0; reg <= R7; reg++)
				if (usable(p, side, reg) > 0) {
				found:
					rfree(reg, ty);
					cls = rclass(ty, reg);
					return getreg(cls, reg, RJAM, sym);
				}
		} else if ((cookie & NBSL) | NBSR) {
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
usable(NODE *p, int side, int r)
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
recl2(NODE *p)
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
rfree(int reg, int type)
{
	register struct rs *n;
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
rwprint(int rw)
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
			fputs(rwnames[i], stdout);
		}
	}
}

void
reclaim(NODE *p, int rw, int cookie)
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
		else if (p->in.type == ULONG) {
		}
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
