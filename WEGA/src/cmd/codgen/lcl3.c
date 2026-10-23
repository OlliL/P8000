/*
 * lcl3.c - the commands F R S T of the intermediate file, register variables, temporaries, function head, switch
 *
 * Reconstructed from /lib/codgen: parminit() 0x8c74, hdrout() 0x8c0c, endline() 0x8c22, structretcode() 0x8f7e,
 * fbegin() 0x8fda, regtabinit() 0x910e, tabfind() 0x9116, tabclear() 0x9152, fixtemp() 0x918e, fixfld() 0x944a,
 * mapregs() 0x9512, symuse() 0x964c, symreg() 0x9fdc, regforsym() 0xa03a, fixname() 0xa0c6, oregfix() 0xa12a,
 * fcmd() 0xa1ba, regreset() 0xa252, rcmd() 0xa2f4, memname() 0xa734, scmd() 0xa4aa, tcmd() 0xa5de, findtemp() 0xa7ac,
 * gswitch() 0xa7e0. Names are guesses. The register variables of the function are in regvar[]: the first 7
 * entries are word registers, the next ones float registers.
 */
#include "cg.h"

#define INCREF(t)	((((t) & ~0xF) << 2) | 0x10 | ((t) & 0xF))
#define DECREF(t)	((((t) >> 2) & ~0xF) | ((t) & 0xF))

extern int cerror(), uerror();
extern int deflab(), getlab();
extern int getreg(), rclass(), mapsym(), argsize(), rallkind(), szty();
extern void rsreturn(), rfree(), rmove(), reclaim(), delay(), tfree();
extern struct rs *rfind();
extern struct sym *mapn();
void oregfix();
extern struct node *clocal(), *foldadd(), *member();
extern void mapnm();
extern char *rnames[];
extern char ascii[];
extern int rflag, pflag, lflag;
extern int minreg, maxreg, ifline;
extern long maxoff;
extern char ifname[];
extern struct rs *rsuse;
extern struct sym *fpsym1, *fpsym2, *symtab0;
extern struct tmpent { int off; char done; char cls; unsigned flags; int reg; } tmptab[];

struct rv { struct sym *sym; int rtag; int rreg; int rtype; int rcls; };
int nparmtab;			/* 0x8f98 */
struct { struct sym *sym; int reg; } parmtab[10];	/* 0x8f5c */
struct rv regvar[12];		/* 0x8fe8: symbol, tag of the symbol or of the temporary, register, type, saved class */
int structret;			/* 0xa15a: the function returns a structure */
int structlab;		/* 0xa7b8 */
long structsz;		/* 0xa7ba */
int retlab;			/* 0x8fca */
int entlab, jlab;		/* 0xa0e2, 0xa14c */
int mcountlab;			/* 0xa7b6 */
char fname[64];			/* 0xa0e4 */
extern long frame0;			/* 0x69ce */

/* the header of the assembler text */
void
hdrout()
{
#ifdef SEG
	printf("\t.seg\n\t.psec data\n\t.data\n\n");
	printf("\n\tfp\t\t:= r15;\n");
	printf("\tsp\t\t:= rr14;\n");
#else
	printf("\n\tfp\t\t:= r15;\n");
	printf("\tsp\t\t:= r15;\n");
#endif
}

/* with -l: the line directive for a source file with the extension .c */
void
endline()
{
	register int n;
	register char *cp;

	if (lflag) {
		n = strlen(ifname);
		cp = &ifname[n - 1];
		if (*cp == '"')
			cp--;
		if (*cp-- == 'c' && *cp == '.')
			printf("\t.line\t%d\t%s\t\n", ifline, ifname);
	}
}

void
endrun()
{
	endline();
}

/* a formal parameter of the function: into its register, or noted for the entry code */
void
parminit(p)
	register struct node *p;
{
	register struct sym *s;
	register int sz, n, t, r, t2;
	int cls;
	struct rs *q;

	if (p->op != NAME)
		return;
	mapsym(p);
	sz = argsize(p);
	s = p->sname;
	n = s->sbyte5;
	t = s->stype;
#ifdef SEG
	if (t == 6 || t == 7)
		n -= 6;
#else
	if (t == 6 || t == 7)
		n -= 5;
#endif
	else
		n -= 7;
	if (rflag && s->sclass == 4 && regvar[n].sym != s)
		s->sclass = 9;
	if (p->lval < 0 && s->sclass != 4 && s->sclass != 8)
		s->sbyte5 = -(int)p->lval;
	r = s->sbyte5;
	if (p->type == 8 || p->type == 9) {
		if (sz == 1)
			t2 = 4;
		else if (sz == 2)
			t2 = 5;
		else
			t2 = p->type;
	} else
		t2 = p->type;
	cls = rclass(t2, r);
	if ((s->sclass == 9 && p->lval < 0) || s->sclass == 4) {
		for (q = rsuse; q; q = q->next)
			if (q->reg == r) {
				q->busy = 0;
				goto got;
			}
		getreg(cls, r, 0x1000, s);
		rfree(r, t2);
	got:
		if (s->sclass == 4) {
			if (s->stype == 7 || (s->stype == 6 && p->lval < 0))
				printf("\tfldd\t%s,", rnames[r]);
			else if (s->stype == 6)
				printf("\tflds\t%s,", rnames[r]);
			else {
				if (sz == 1)
					printf("\tld\t");
				else if (sz == 2)
					printf("\tldl\tr");
				printf("%s,", rnames[r]);
			}
			if (p->lval >= 0)
#ifdef SEG
				printf("_stkseg+~L2+%ld(fp)\n", p->lval);
#else
				printf("~L2+%ld(fp)\n", p->lval);
#endif
			else {
				if (sz == 2)
					putchar('r');
				printf("%s\n", rnames[-(int)p->lval]);
			}
			return;
		}
		if (s->stype == 8) {
			if (sz == 1)
#ifdef SEG
				printf("\tld\t|_stkseg+~L1+%ld|(fp),%s\n", s->soffset / 8, rnames[r]);
#else
				printf("\tld\t~L1+%ld(fp),%s\n", s->soffset / 8, rnames[r]);
#endif
			else if (sz == 2)
#ifdef SEG
				printf("\tldl\t|_stkseg+~L1+%ld|(fp),r%s\n", s->soffset / 8, rnames[r]);
#else
				printf("\tldl\t~L1+%ld(fp),r%s\n", s->soffset / 8, rnames[r]);
#endif
			else
				cerror("structure argument too long");
			s->sflags &= ~8;
			return;
		}
		if (nparmtab > 9)
			cerror("rinit: formal paramter array overflow");
		parmtab[nparmtab].reg = s->sbyte5;
		parmtab[nparmtab].sym = s;
		nparmtab++;
		parmtab[nparmtab].sym = 0;
		s->sflags &= ~8;
		return;
	}
	if (s->sclass == 9 && p->lval >= 0)
		s->sflags |= 8;
	else if (s->sclass != 9)
		cerror("illegal class in PROC tree");
}

/* the exit of a function that returns a structure: copy it to the place the caller gave */
void
structretcode()
{
	if (structret) {
#ifdef SEG
		printf("\tldl\trr4,#L%d\n", structlab);
		printf("\tld\tr1,#%ld\n", structsz);
		printf("\tldir\t@rr4,@r%s,r1\n", rnames[rallkind(0)]);
		printf("\tldl\tr%s,#L%d\n", rnames[rallkind(0)], structlab);
#else
		printf("\tld\tr4,#L%d\n", structlab);
		printf("\tld\tr1,#%ld\n", structsz);
		printf("\tldir\t@r4,@%s,r1\n", rnames[rallkind(0)]);
		printf("\tld\t%s,#L%d\n", rnames[rallkind(0)], structlab);
#endif
	}
}

/* the head of a function: the label and the jumps to the entry code */
void
fbegin(s)
	struct sym *s;
{
	register int spare1;		/* unused: the original reserves these registers */
	register int t;
	register int spare2;
	register char *cp, *np;

	endline();
	cp = fname;
	*cp++ = '_';
	for (np = ascii + s->nameoff; *cp++ = *np++; )
		;
	if (s->sclass == 5)
		printf("/*~~%s:*/\n", ascii + s->nameoff);
#ifdef SEG
	printf("\n\t.psec\n\t.code\n\n");
#else
	printf("\n\t.code\n\n");
#endif
	printf("\t%s:", fname);
	if (s->sclass == 5)
		printf(":");
	printf("\n\t{\n\n");
	t = DECREF(s->stype);
	structret = t == 8 || t == 9;
	if (structret) {
		structlab = s->sx6;
		structsz = s->ssize / 2;
	}
	retlab = getlab();
	if (pflag) {
		mcountlab = s->soffset;
#ifdef SEG
		printf("\tpushl\t@sp,rr2\n\tldl\trr2,#L%d\n", mcountlab);
		printf("\tpushl\t@sp,rr2\n");
		printf("\tcall\tmcount\n\tinc\tfp,#4\n");
		printf("\tpopl\trr2,@sp\n");
#else
		printf("\tpush\t@sp,#L%d\n", mcountlab);
		printf("\tcall\tmcount\n\tinc\tsp,#2\n");
#endif
	}
	entlab = getlab();
	jlab = getlab();
	printf("\tjpr\tL%d\n", entlab);
	printf("L%d:\n", jlab);
}

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
	register struct node *p;
{
	register int i;
	register int j, cls, reg;
	struct rs *n;

	if (rflag) {
		if (p->rval >= 0 && (j = findtemp(p->rval)) >= 0) {
			p->op = REG;
			p->lval = 0;
			p->rval = regvar[j].rreg;
			return;
		}
	}
	i = tabfind(p->rval);
	if (tmptab[i].off != p->rval) {
		if (i == 0x50)
			cerror("TEMP table overflow\n");
		tmptab[i].off = p->rval;
		if (p->lval == -1) {
			tmptab[i].flags |= 0x8000;
			cls = rclass(p->type, -1);
			reg = getreg(cls, -1, 0x1000, 0);
			tmptab[i].reg = reg;
			tmptab[i].cls = cls;
			n = rfind(reg, cls);
			n->kind = 6;
			n->busy = 0;
		} else {
			tmptab[i].flags &= ~0x8000;
			tmptab[i].reg = (int)((long)(unsigned)p->stsize + frame0);
		}
		if (i + 1 != 0x50)
			tmptab[i + 1].off = -1;
	}
	tmptab[i].done = p->lval;
	if (tmptab[i].flags >> 15 & 1) {
		p->op = REG;
		p->lval = 0;
		p->rval = tmptab[i].reg;
	} else {
		p->op = OREG;
		p->lval = (long)(unsigned)tmptab[i].reg;
		p->rval = 15;
		p->stsize = 0;
#ifdef SEG
		p->sname = symtab0;
#else
		p->sname = 0;
#endif
	}
}

/* a field: the address of the member is added to the address of the structure */
void
fixfld(p)
	register struct node *p;
{
	register struct node *m, *a;
	register int ismember;
	int t;

	if (p->op == FLD) {
		m = p->left;
		ismember = 1;
	} else {
		p->op = PLUS;
		a = p->left;
		m = a->left;
		ismember = 0;
	}
	if (p->op != FLD && a->op != UAND)
		return;
	if (m->op != NAME || m->rval < 0 || (m->sname->sflags >> 2 & 1))
		return;
	m = clocal(m);
	if (m->op != STAR)
		return;
	if (ismember)
		p->left = m;
	else {
		m->op = FREE;
		a->op = FREE;
		t = DECREF(m->type);
		p->type = INCREF(t);
		p->left = m->left;
	}
}

/* a callback of the walk before the code is generated: members and addresses of names */
void
mapregs(p)
	register struct node *p;
{
	register struct node *m, *n, *r;

	switch (p->op) {
	case 120:
		fixfld(p);
		return;
	case STREF:
		fixfld(p);
		r = p->right;
		if (p->left->op == UAND)
			p->left->type = p->type;
		if (r->op == NAME && (r->sname->sclass == 8 || r->sname->sclass == 11)) {
			r->op = ICON;
			r->lval = r->sname->soffset / 8;
			r->rval = 0;
			r->sname = 0;
			r->type = 4;
		}
		n = foldadd(p);
		if (n == p)
			return;
		p->type = n->type;
		p->op = n->op;
		p->sname = n->sname;
		p->lval = n->lval;
		p->rval = n->rval;
		n->op = FREE;
		return;
	case UAND:
		m = p->left;
		if (m->op != NAME)
			return;
		if (m->sname->sflags >> 2 & 1)
			return;
		if (m->rval < 0)
			return;
		m = clocal(m);
		if (m->op != STAR)
			return;
		m->op = FREE;
		p->op = FREE;
		n = m->left;
		p->type = n->type;
		p->op = n->op;
		p->sname = n->sname;
		p->lval = n->lval;
		p->rval = n->rval;
		n->op = FREE;
		return;
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

/* the register that holds the symbol (and offset), marked busy; -1 if none */
int
symreg(s, off)
	register struct sym *s;
	long off;
{
	register struct rs *n;
	register struct sym *f1, *f2;

	f1 = fpsym1;
	f2 = fpsym2;
	for (n = rsuse; n; n = n->next) {
		if (s == f1 || s == f2) {
			if (n->off != off)
				continue;
		} else if (s != n->sym)
			continue;
		n->busy = 1;
		return n->reg;
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
		return symreg(s, off);
	if (type != s->stype)
		return -1;
	if (reg != -1 && reg != s->sbyte5)
		return -1;
	return symreg(s, 0L);
}

/* a name that lives in a register becomes REG, otherwise an OREG */
void
fixname(p)
	register struct node *p;
{
	register int r;

	r = (p->rall & 0x1000) ? p->rall & ~0x1000 : -1;
	r = regforsym(r, p->sname, p->type, p->sname->soffset / 8);
	if (r >= 0) {
		p->op = REG;
		p->rval = r;
		p->lval = 0;
	} else
		oregfix(p);
}

/* a NAME of an automatic variable or a parameter becomes an OREG off the frame pointer */
void
oregfix(p)
	struct node *p;
{
	if ((p->sname->sflags >> 2 & 1) == 1)
		return;
	if (p->op == NAME && p->sname->sclass == 3)
		return;
	if (p->op == NAME && p->rval < 0)
		return;
	p->op = OREG;
	p->rval = 15;
	p->lval = p->sname->soffset / 8;
	p->stsize = 0;
	p->sname = p->sname->sclass == 9 && ((p->sname->sflags >> 3 & 1) == 1) ? fpsym2 : fpsym1;
}

int
findtemp(tag)
	int tag;
{
	register int i;

	for (i = 0; i < 12; i++)
		if (regvar[i].rtag == tag)
			return i;
	return -1;
}

/* command F: a register variable or a temporary is released */
void
fcmd(n)
	int n;
{
	register struct sym *s;
	register int i;

	if (n > 0) {
		s = mapn(n);
		i = s->sbyte5;
	} else {
		s = 0;
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
	for (i = 0; i < 12; i++) {
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
	register int a, b, type;
{
	register struct sym *s;
	register struct sym *old;
	register int tag, i;
	int r, cls, got, saved;
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
		r = a + 19;
		regvar[i].rreg = r;
		regvar[i].rtype = type;
		saved = regvar[i].rcls;
		regvar[i].rcls = s->sclass;
		if (b > 0)
			s->sbyte5 = r;
		if (r - 1 < maxreg)
			maxreg = r - 1;
	} else {
		i = a;
		old = regvar[i].sym;
		regvar[i].sym = s;
		regvar[i].rtag = tag;
		r = a + 7;
		regvar[i].rreg = r;
		regvar[i].rtype = type;
		saved = regvar[i].rcls;
		regvar[i].rcls = s->sclass;
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

/* the operand of a store or load of a register variable: a name or an address on the frame */
void
memname(s, off)
	register struct sym *s;
	long off;
{
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

/* command S: a register variable or a temporary is stored in its memory place */
void
scmd(n)
	register int n;
{
	register struct sym *s;
	register long off;
	register int t, reg;
	int i, j, tmp;

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
	register int n;
{
	register struct sym *s;
	register long off;
	register int t, reg;
	int j;

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
		break;
	case 6:
		printf("\tflds\t");
		printf("f%d,", reg - 16);
		break;
	case 7:
		printf("\tfldd\t");
		printf("f%d,", reg - 16);
		break;
	default:
		printf("\tld\t");
		printf("r%d,", reg);
	}
	memname(s, off);
	putchar('\n');
}

/* a free register other than avoid and r0 */
static int
newreg(avoid)
	int avoid;
{
	int r, t, t2;

	r = getreg(2, -1, 0x1000, 0);
	if (r == avoid || r == 0) {
		t = r;
		r = getreg(2, -1, 0x1000, 0);
		if (r == avoid || r == 0) {
			t2 = r;
			r = getreg(2, -1, 0x1000, 0);
			rfree(t2, 3);
		}
		rfree(t, 3);
	}
	return r;
}

#ifdef SEG
/* a free long register that does not overlap avoid, not rr0 */
static int
newlreg(avoid)
	int avoid;
{
	int r, t, t2;

	r = getreg(3, -1, 0x1000, 0);
	if (r == avoid || (avoid ^ 1) == r || r == 0) {
		t = r;
		r = getreg(3, -1, 0x1000, 0);
		if (r == avoid || (avoid ^ 1) == r || r == 0) {
			t2 = r;
			r = getreg(3, -1, 0x1000, 0);
			rfree(t2, 5);
		}
		rfree(t, 5);
	}
	return r;
}
#endif

/* the code for a switch: a jump table, or a search with cpir */
void
gswitch(p)
	register struct node *p;
{
	register struct node *l, *r;
	register struct node *x;
	register long n;
	register int dlab;
	int tab, reg, save, ra, rb, cnt, tlab;

	l = p->left;
	r = p->right;
	n = r->right->lval;
	tab = r->right->rval;
	dlab = -(int)r->left->lval;
	x = l->op == REG ? l : l->left;
	reg = x->rval;
	if (l->op != 124) {
		mapnm(l->left);
		delay(l);
		reclaim(l, 0, 0);
		printf("\tcp\t%s,#%ld\n", rnames[reg], n);
		printf("\tjpr\tugt,L%d\n", dlab);
		if (reg == 0) {
			save = reg;
			reg = getreg(2, -1, 0x1000, 0);
			rmove(reg, save, 3);
			rfree(save, 3);
		}
#ifdef SEG
		ra = newlreg(reg);
		printf("\trl\t%s,#2\n", rnames[reg]);
		printf("\tldl\tr%s,L%d(%s)\n", rnames[ra], tab, rnames[reg]);
		printf("\tjp\t@r%s\n", rnames[ra]);
#else
		printf("\trl\t%s,#1\n", rnames[reg]);
		printf("\tld\t%s,L%d(%s)\n", rnames[reg], tab, rnames[reg]);
		printf("\tjp\t@%s\n", rnames[reg]);
#endif
		return;
	}
	cnt = (int)l->right->lval;
	tlab = l->right->rval;
#ifdef SEG
	ra = newlreg(reg);
	rb = newreg(reg);
	printf("\tldl\tr%s,#L%d\n", rnames[ra], tlab);
	printf("\tld\t%s,#%d\n", rnames[rb], cnt);
	printf("\tcpir\t%s,@r%s,%s,eq\n", rnames[reg], rnames[ra], rnames[rb]);
	printf("\tjpr\tne,L%d\n", dlab);
	printf("\tsll\t%s,#2\n", rnames[rb]);
	printf("\tneg\t%s\n", rnames[rb]);
	printf("\tldl\tr%s,L%d+%d(%s)\n", rnames[ra], tab, (cnt - 1) << 2, rnames[rb]);
	printf("\tjp\t@r%s\n", rnames[ra]);
	rfree(ra, 5);
	rfree(rb, 3);
	rfree(reg, 3);
#else
	if (reg == 0) {
		save = reg;
		reg = getreg(2, -1, 0x1000, 0);
		rmove(reg, save, 3);
		rfree(save, 3);
	}
	ra = newreg(reg);
	rb = newreg(reg);
	printf("\tld\t%s,#L%d\n", rnames[ra], tlab);
	printf("\tld\t%s,#%d\n", rnames[rb], cnt);
	printf("\tcpir\t%s,@%s,%s,eq\n", rnames[reg], rnames[ra], rnames[rb]);
	printf("\tjpr\tne,L%d\n", dlab);
	printf("\tsll\t%s,#1\n", rnames[rb]);
	printf("\tneg\t%s\n", rnames[rb]);
	printf("\tld\t%s,L%d+%d(%s)\n", rnames[reg], tab, (cnt - 1) * 2, rnames[rb]);
	printf("\tjp\t@%s\n", rnames[reg]);
	rfree(ra, 3);
	rfree(rb, 3);
	rfree(reg, 3);
#endif
}
