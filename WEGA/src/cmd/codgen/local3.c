/*
 * local3.c - machine dependent code: entry and exit of a function, the escape codes "Z" of the templates, operands, calls
 */
#include "cg.h"
#include "cg2.h"

extern void acon();
extern void adrput();
extern int argreg;
extern int argstop;
extern char ascii[];
extern void branch();
extern NODE *canon();
extern void canon2();
extern int cbflag;
extern void cbgen();
extern int cerror();
extern void conput();
extern void d3kjump();
extern void deflab();
extern int dope[];
extern void efcode();
extern void endline();
extern int entlab;
extern void expand();
extern struct hoptab fhoptab[];
extern int fldoff;
extern int fldshf;
extern int fldsz;
extern int fltused;
extern char fname[];
extern struct sym *fpsym1;
extern struct sym *fpsym2;
extern long frame0;
extern long freetemp();
extern void eobl2();
extern void walkf();
extern void genargs();
extern int gencall();
extern long geninc();
extern int genscall();
extern int getlab();
extern NODE *getlr();
extern int getreg();
extern void hopcode();
extern void icright();
extern struct hoptab ihoptab[];
extern void insput();
extern int ispow2();
extern int jlab;
extern int lastchance();
extern void lineout();
extern int shltype();
extern int ltypeop();
extern int mainp2();
extern int match();
extern long maxoff;
extern int maxreg;
extern int minreg;
extern void mycanon();
extern int negrel[];
extern int nextcook();
extern NODE node[];
extern int nosuffix;
extern int notoff();
extern int nparmtab;
extern int nsave;
extern char *opst[];
extern void order();
extern void oregput();
extern long parmax;
extern struct { struct sym *sym; int reg; } parmtab[];
extern int plainicon();
extern void prlong();
extern void psuffix();
extern void rbusy();
extern int rclass();
extern int retlab;
extern int revrel[];
extern struct rs *rfind();
extern void rfree();
extern void rfreeall();
extern void recl2();
extern char *rhnames[];
extern int rlmode;
extern char *rlnames[];
extern int rmode;
extern void rmove();
extern char *rnames[];
extern struct rs *rsfree;
extern void rsfreeall();
extern void rsreturn();
extern int rstatus[];
extern struct rs *rsuse;
extern int rwtab[][2];
extern int savecls[];
extern int saveorig[];
extern int savereg[];
extern int shincr();
extern int shtemp();
extern int shumul();
extern int special();
extern int structret;
extern void structretcode();
extern struct sym *symtab0;
extern int szty();
extern NODE *tcopy();
extern void tfree();
extern long tmpoff;
extern int tshape();
extern void tsuffix();
extern int ttype();
extern int upperlval;
extern void upput();
extern void zzzcode();
static char sccsid[] = "@[$]local3.c\t2.4  08/24/83 11:48:31 Copr. 1981 87wega3.2.";
char *rhnames[] = { "rh0", "rh1", "rh2", "rh3", "rh4", "rh5", "rh6", "rh7" };
char *rlnames[] = { "rl0", "rl1", "rl2", "rl3", "rl4", "rl5", "rl6", "rl7" };
int rstatus[] = {
	SAREG | STAREG, SAREG | STAREG, SAREG | STAREG, SAREG | STAREG,
	SAREG | STAREG, SAREG | STAREG, SAREG | STAREG, SAREG | STAREG,
	SAREG, SAREG, SAREG, SAREG, SAREG, SAREG, SAREG, SAREG,
	SBREG | STBREG, SBREG | STBREG, SBREG | STBREG, SBREG | STBREG,
	SBREG, SBREG, SBREG, SBREG,
};
int rmode = 0;
int rlmode = 0;
int nosuffix = 0;

#define INCREF(t)	((((t) & ~0xF) << 2) | 0x10 | ((t) & 0xF))

#define DECREF(t)	((((unsigned)(t) >> 2) & ~0xF) | ((t) & 0xF))

#define DIVFLAG	0x80		/* dope flag of the division operators: the dividend gets extended */

/* the test condition codes for ZN, by relation */

#define MAXRCOUNT 5000

#define FORREW	0x4000

#define MDONE	0x1001

#define FREE_	100

static void fpinc();
static void fpdec();
static void regrestore();
static void regsave();
static void epilog();
static void regargs();
static void prolog();

/* the type suffix of the type of the node */
void
psuffix(p)
	register NODE *p;
{
	tsuffix(p->type);
}

void
tsuffix(t)
	register int t;
{
	if (nosuffix)
		return;
	switch (t) {
	case 6:
		putchar('s');
		break;
	case 7:
		putchar('d');
		break;
	case 5:
	case 15:
		putchar('l');
		rmode++;
		break;
	case 2:
	case 12:
		putchar('b');
		rlmode++;
		break;
	case 3:
	case 4:
	case 13:
	case 14:
		break;
	default:
#ifdef SEG
		if ((t & 0x30) == 0x10) {
			putchar('l');
			rmode++;
		} else
			cerror("zzzcode - bad type");
#else
		if ((t & 0x30) != 0x10)
			cerror("zzzcode - bad type");
#endif
	}
}

char fldd[] = "fldd";
char flds[] = "flds";

void
lineout(line, file)
	int line;
	char *file;
{
	printf("\t.line\t%d\t%s\t\n", (int)line, file);
}

/* the end of the text of a function */
void
eobl2(a, b)
	register int a, b;
{
	printf("\n");
	if (fltused) {
		fltused = 0;
		printf("\t.extern\t__fltuse\n");
	}
	printf("\t~L1 := %u\n", a);
	printf("\t~L2 := %u\n", b);
	printf("\n\t}\t/* %s */\n\n", fname);
#ifdef SEG
	printf("\t.psec data\n");
#endif
	printf("\t.data\n\n");
	endline();
}

/* the end of the function: entry and exit code with the frame sizes now known */
void
efcode()
{
	register int a, b, tot;
	register long sz;
	long w;

	sz = (parmax + maxoff) / 8;
	if (sz % 2)
		sz = (sz / 2 + 1) << 1;
#ifdef SEG
	a = 13 - minreg;
#else
	a = 14 - minreg;
#endif
	b = 23 - maxreg;
	w = a * 2 + sz;
	tot = b * 8 + w;
	epilog(a, sz, b, w, tot);
	prolog(a, sz, tot);
}

/* the exit of the function: the label, the registers back, ret */
static void
epilog(a, sz, b, w, tot)
	register int a;
	register long sz;
	register int b;
	long w;
	register int tot;
{
	if (!structret)
		deflab(retlab);
	else
		structretcode();
	regrestore(a, sz, b, w, tot);
	fpinc(tot);
	printf("\tret\n");
}

/* load back the registers saved in the frame: a word registers, b float registers */
static void
regrestore(a, sz, b, w, tot)
	register int a;
	register long sz;
	register int b;
	long w;
	register int tot;
{
	int k, off;

	if (!tot)
		return;
#ifdef SEG
	if (a > 1)
		printf("\tldm\t%s,_stkseg+%d(fp),#%d\n", rnames[minreg + 1], (int)sz, a);
	else if (a == 1)
		printf("\tld\t%s,_stkseg+%d(fp)\n", rnames[minreg + 1], (int)sz);
#else
	if (a > 1) {
		if (sz != 0)
			printf("\tldm\t%s,%d(fp),#%d\n", rnames[minreg + 1], (int)sz, a);
		else
#ifdef OLD_D18
			printf("\tldm\t%s,0(fp),#%d\n", rnames[minreg + 1], a);
#else
			printf("\tldm\t%s,@fp,#%d\n", rnames[minreg + 1], a);
#endif
	} else if (a == 1) {
		if (sz != 0)
			printf("\tld\t%s,%d(fp)\n", rnames[minreg + 1], (int)sz);
		else
#ifdef OLD_D18
			printf("\tld\t%s,0(fp)\n", rnames[minreg + 1]);
#else
			printf("\tld\t%s,@fp\n", rnames[minreg + 1]);
#endif
	}
#endif
	if (b >= 1) {
		k = maxreg;
		off = (int)w;
		for (; k < 0x17; k++, off += 8)
#ifdef SEG
			printf("\tfldd\t%s,_stkseg+%d(fp)\n", rnames[k + 1], off);
#else
			printf("\tfldd\t%s,%d(fp)\n", rnames[k + 1], off);
#endif
	}
}

/* ~L2 grows or shrinks the frame pointer */
static void
fpinc(tot)
	register unsigned tot;
{
	if (tot) {
		if (tot <= 0x10)
			printf("\tinc\tfp,#~L2\n");
		else
			printf("\tadd\tfp,#~L2\n");
	}
}

/* the entry of the function */
static void
prolog(a, sz, b, w, tot)
	register int a;
	register long sz;
	register int b;
	long w;
	register int tot;
{
	deflab(entlab);
	fpdec(tot);
	regsave(a, sz, b, w, tot);
	regargs(tot);
	branch(jlab);
	eobl2((int)(maxoff / 8), tot);
}

static void
fpdec(tot)
	register unsigned tot;
{
	if (tot) {
		if (tot <= 0x10)
			printf("\tdec\tfp,#~L2\n");
		else
			printf("\tsub\tfp,#~L2\n");
	}
}

static void
regsave(a, sz, b, w, tot)
	register int a;
	register long sz;
	register int b;
	long w;
	register int tot;
{
	int k, off;

	if (!tot)
		return;
#ifdef SEG
	if (a > 1)
		printf("\tldm\t_stkseg+%d(fp),%s,#%d\n", (int)sz, rnames[minreg + 1], a);
	else if (a == 1)
		printf("\tld\t_stkseg+%d(fp),%s\n", (int)sz, rnames[minreg + 1]);
#else
	if (a > 1) {
		if (sz != 0)
			printf("\tldm\t%d(fp),%s,#%d\n", (int)sz, rnames[minreg + 1], a);
		else
#ifdef OLD_D18
			printf("\tldm\t0(fp),%s,#%d\n", rnames[minreg + 1], a);
#else
			printf("\tldm\t@fp,%s,#%d\n", rnames[minreg + 1], a);
#endif
	} else if (a == 1) {
		if (sz != 0)
			printf("\tld\t%d(fp),%s\n", (int)sz, rnames[minreg + 1]);
		else
#ifdef OLD_D18
			printf("\tld\t0(fp),%s\n", rnames[minreg + 1]);
#else
			printf("\tld\t@fp,%s\n", rnames[minreg + 1]);
#endif
	}
#endif
	if (b >= 1) {
		k = maxreg;
		off = (int)w;
		for (; k < 0x17; k++, off += 8)
#ifdef SEG
			printf("\tfldd\t_stkseg+%d(fp),%s\n", off, rnames[k + 1]);
#else
			printf("\tfldd\t%d(fp),%s\n", off, rnames[k + 1]);
#endif
	}
}

/* the registers that arrive as arguments but live in the frame go to their place */
static void
regargs(tot)
	register int tot;
{
	register int lo, hi, t;
	int i, unused, cls, reg;
	struct sym *s;
	struct rs *n;

	lo = 7;
	hi = 2;
	t = tot;
	for (i = nparmtab - 1; i >= 0; i--) {
		s = parmtab[i].sym;
		reg = parmtab[i].reg;
		cls = rclass(s->stype, reg);
#ifdef OLD_D22
		if ((s->stype == 6 || s->stype == 7) && reg < 8) {	/* v0p: a double parameter in rq4 */
			if (lo > reg)
				lo = reg;
			if (hi < reg + 3)
				hi = reg + 3;
			if (s->soffset / 8 < (unsigned long)(unsigned)t)
				t = (int)(s->soffset / 8);
			continue;
		}
#endif
		n = rfind(reg, cls);
#ifdef OLD_D3J
		if ((!n || n->sym != s || (n->flags & 0x4000)) && cls < 4) {
#else
		if ((!n || n->sym != s) && cls < 4) {
#endif
			int k;

			if (lo > reg)
				lo = reg;
			if (cls == 3)
				k = 1;
			else
				k = 0;
			if (hi < reg + k)
				hi = reg + k;
			if (s->soffset / 8 < (unsigned long)(unsigned)t) {
				t = (int)(s->soffset / 8);
				if (t & 1)
					t--;
			}
		} else if (cls == 4) {
			char *mn;

			if (s->stype == 6)
				mn = flds;
			else
				mn = fldd;
#ifdef SEG
			printf("\t%s\t|_stkseg+~L1+%ld|(fp),%s\n", mn, s->soffset / 8, rnames[reg]);
#else
			if (maxoff + s->soffset)
				printf("\t%s\t~L1+%ld(fp),%s\n", mn, s->soffset / 8, rnames[reg]);
			else
#ifdef OLD_D18
				printf("\t%s\t0(fp),%s\n", mn, rnames[reg]);
#else
				printf("\t%s\t@fp,%s\n", mn, rnames[reg]);
#endif
#endif
		}
	}
	if (hi < lo)
		return;
#ifdef SEG
	if (hi == lo)
		printf("\tld\t|_stkseg+~L1+%u|(fp),r%d\n", t, lo);
	else if (!(lo & 1) && hi - lo == 1)
		printf("\tldl\t|_stkseg+~L1+%u|(fp),rr%d\n", t, lo);
	else
		printf("\tldm\t|_stkseg+~L1+%u|(fp),r%d,#%d\n", t, lo, hi - lo + 1);
#else
	if (hi == lo) {
		if ((long)(unsigned)t + maxoff)
			printf("\tld\t~L1+%u(fp),r%d\n", t, lo);
		else
#ifdef OLD_D18
			printf("\tld\t0(fp),r%d\n", lo);
#else
			printf("\tld\t@fp,r%d\n", lo);
#endif
	} else if (!(lo & 1) && hi - lo == 1) {
		if ((long)(unsigned)t + maxoff)
			printf("\tldl\t~L1+%u(fp),rr%d\n", t, lo);
		else
#ifdef OLD_D18
			printf("\tldl\t0(fp),rr%d\n", lo);
#else
			printf("\tldl\t@fp,rr%d\n", lo);
#endif
	} else {
		if ((long)(unsigned)t + maxoff)
			printf("\tldm\t~L1+%u(fp),r%d,#%d\n", t, lo, hi - lo + 1);
		else
#ifdef OLD_D18
			printf("\tldm\t0(fp),r%d,#%d\n", lo, hi - lo + 1);
#else
			printf("\tldm\t@fp,r%d,#%d\n", lo, hi - lo + 1);
#endif
	}
#endif
}

#ifdef SEG
#include "hop_scodgen.c"
#else
#include "hop_codgen.c"
#endif

void
hopcode(p)
	register NODE *p;
{
	register int o = p->op;
	register struct hoptab *q;
	register int t = p->type;

	q = (t == 7 || t == 6) ? fhoptab : ihoptab;
	for (; q->op >= 0; q++)
		if (q->op == o && ttype(t, q->tword)) {
			printf("%s", q->str);
			return;
		}
	cerror("no hoptab for %s", opst[o]);
}

/* print inc/dec or add/sub for the operator, depending on the size of the constant n; returns the constant to print */
long
geninc(op, n)
	register int op;
	register long n;
{
	switch (op) {
	case PLUS:
	case ASG PLUS:
	case INCR:
		if (n < 0) {
			n = -n;
			goto dec;
		}
	inc:
		if (n != 0 && n <= 16)
			printf("inc");
		else
			printf("add");
		break;
	case MINUS:
	case ASG MINUS:
	case DECR:
		if (n < 0) {
			n = -n;
			goto inc;
		}
	dec:
		if (n != 0 && n <= 16)
			printf("dec");
		else
			printf("sub");
		break;
	default:
		cerror("geninc: bad op: %s\n", opst[op]);
	}
	return n;
}


int unused9 = 0;
static char *tcctab[] = {
	"\ttcc\teq,A1\n",
	"\ttcc\tne,A1\n",
	"\ttcc\tle,A1\n",
	"\ttcc\tlt,A1\n",
	"\ttcc\tge,A1\n",
	"\ttcc\tgt,A1\n",
	"\ttcc\tule,A1\n",
	"\ttcc\tult,A1\n",
	"\ttcc\tuge,A1\n",
	"\ttcc\tugt,A1\n",
	"\ttcc\tfeq,A1\n",
	"\ttcc\tfneu,A1\n",
	"\ttcc\tfle,A1\n",
	"\ttcc\tflt,A1\n",
	"\ttcc\tfge,A1\n",
	"\ttcc\tfgt,A1\n",
};

void
zzzcode(p, c)
	register NODE *p;
	char c;
{
	register int n;
	register NODE *z;
	register NODE *l;
	register NODE *q, *r;
	register NODE *u;
	int x;
	struct rs *rp;

	switch (c) {
	case '<':
	case '>':
		/* the type suffix of the left or the right operand */
		l = getlr(p, c == '<' ? 'L' : 'R');
	case 'B':
		/* the type suffix of the node, none when a float operation works on registers only */
		if (p->type == 6 || p->type == 7) {
			q = getlr(p, 'L');
			r = getlr(p, 'R');
			if (q->op == REG && r->op == REG)
				return;
		}
		psuffix(c == 'B' ? p : l);
		return;
	case 'R':
		/* "sgl," or "dbl," */
		q = getlr(p, 'L');
		if (q->type == 6)
			printf("sgl,");
		else if (q->type == 7)
			printf("dbl,");
		return;
	case 'f':
		if (p->op == FNE || p->op == FEQ)
			return;
		putchar('x');
		return;
	case 'U':
		printf(".word\t");
		return;
	case 'V':
		printf(".byte\t");
		return;
	case 'W':
		printf(".long\t");
		return;
	case 'F':
	case 'I':
		cbgen(p->op, p->label, c);
		return;
	case '2':
		/* log2 of the constant */
		printf("%d", ispow2(p->right->lval));
		return;
	case '~':
		printf("%d", ispow2(~p->right->lval));
		return;
	case 'i':
		/* inc/dec/add/sub or the mnemonic of the operator, with the type suffix */
		if (!tshape(z = getlr(p, 'R'), SCON))
			cerror("bad Zi");
		if (plainicon(z))
			z->lval = geninc(p->op, z->lval);
		else
			hopcode(p);
		psuffix(p);
		return;
	case 't':
		nosuffix++;
		return;
	case 'C':
		if (p->type == 2)
			p->type = 4;
		else if (p->type == 12)
			p->type = 14;
		else
			cerror("bad ZC");
		return;
	case 'c':
		/* float <-> double: the operand has to be in a B register */
		if (p->type == 6)
			p->type = 7;
		else
			p->type = 6;
		if (!tshape(p->left, STBREG))
			order(p->left, STBREG);
		return;
	case 'N':
		/* a comparison used as a value: 0 or 1 */
		switch ((int)p->lval) {
		case EQ:
		case NE:
		case LE:
		case LT:
		case GE:
		case GT:
		case ULE:
		case ULT:
		case UGE:
		case UGT:
		case FEQ:
		case FNE:
		case FLE:
		case FLT:
		case FGE:
		case FGT:
			deflab(p->label);
			expand(p, FOREFF, "\tldk\tA1,#0\n");
			expand(p, FOREFF, tcctab[negrel[p->lval - EQ] - EQ]);
			return;
		}
		expand(p, FOREFF, "\tldk\tA1,#1\n");
		cbgen(0, n = getlab(), 'I');
		deflab(p->label);
		expand(p, FOREFF, "\tldk\tA1,#0\n");
		deflab(n);
		return;
	case 'H':
	case 'h':
		/* the low byte register of a word register */
		p = getlr(p, c == 'H' ? 'L' : 'R');
		if (p->rval > 7 || p->rval < 0)
			cerror("bad %c call: register = %s\n", c, rnames[p->rval]);
		printf(rhnames[p->rval]);
		return;
	case 'l':
	case 'r':
		/* the upper half of a long operand */
		upperlval = 1;
		adrput(getlr(p, c));
		upperlval = 0;
		return;
	case 'Q':
		if (p->type == 5)
			printf("\textsl\trq4\n");
		else
			printf("\tsubl\trr4,rr4\n");
		return;
	case '0':
		/* give the registers back that were moved around a call (nxwork 33 and 34) */
		while (nsave) {
			int m, k;

			nsave--;
			x = savecls[nsave] == 3 ? 5 : 4;
			m = rmode;
			k = rlmode;
			if (x == 5)
				rmode = 1;
			else
				rmode = 0;
			rlmode = 0;
			rmove(saveorig[nsave], savereg[nsave], x);
			rp = rfind(saveorig[nsave], savecls[nsave]);
			if (!rp)
				cerror("zzzcode : can't find %d on rslist\n", saveorig[nsave]);
			rp->busy = 1;
			rp = rfind(savereg[nsave], savecls[nsave]);
			if (!rp)
				cerror("zzzcode : can't find %d on rslist\n", savereg[nsave]);
			rp->busy = 0;
			if ((p->left->op == REG || p->left->op == OREG) && p->left->rval == savereg[nsave])
				p->left->rval = saveorig[nsave];
			if ((p->right->op == REG || p->right->op == OREG) && p->right->rval == savereg[nsave])
				p->right->rval = saveorig[nsave];
			rmode = m;
			rlmode = k;
		}
		return;
	case 'u':
#ifdef SEG
		q = p->left;
		if (q->op == OREG && q->lval == 0 && q->sname == 0 && q->stsize >= 0) {
			printf("\tinc\t%s,#2\n", rnames[q->rval + 1]);
			expand(p, FOREFF, "\tZtZi\tAL,AR\n");
			printf("\tdec\t%s,#2\n", rnames[q->rval + 1]);
			return;
		}
#endif
		expand(p, FOREFF, "\tZtZi\tUL,AR\n");
		return;
	case 'K':
		/* store into a field: bit instructions, a byte, a word or the general mask and shift */
		switch (fldsz) {
		case 1:
		case 2:
			if (p->right->lval & 1)
				expand(p, FOREFF, "\tset\tAL,");
			else
				expand(p, FOREFF, "\tres\tAL,");
			printf("#%d\n", fldshf);
			if (fldsz == 1)
				return;
			if (p->right->lval & 2)
				expand(p, FOREFF, "\tset\tAL,");
			else
				expand(p, FOREFF, "\tres\tAL,");
			printf("#%d\n", fldshf + 1);
			return;
		case 8:
			if (fldoff % 8)
				break;
			if (fldoff) {
#ifdef SEG
				q = p->left->left;
				if (q->lval != 0)
					q->lval++;
				else
					printf("\tinc\t%s,#1\n", rnames[q->rval + 1]);
#else
				p->left->left->lval += 1;
#endif
			}
			if (p->right->lval & 0xFFFF)
				expand(p, FOREFF, "\tldb\tWAL,AR\n");
			else
				expand(p, FOREFF, "\tclrb\tWAL\n");
			return;
		case 16:
			if (p->right->lval)
				expand(p, FOREFF, "\tld\tAL,AR\n");
			else
				expand(p, FOREFF, "\tclr\tAL\n");
			return;
		}
		expand(p, FOREFF, "\tld\tA1,AL\n\tand\tA1,#N\n");
		if (p->right->lval) {
			expand(p, FOREFF, "\tor\tA1,");
			printf("#%d\n", ((int)p->right->lval & ~(-1 << fldsz)) << fldshf);
		}
		expand(p, FOREFF, "\tld\tAL,A1\n");
		return;
	case 'L':
		/* the address of the left operand into the register A1 */
		if ((u = getlr(p, 'L'))->op == NAME) {
			expand(p, FOREFF, "\tldZB\tA1,#AL\n");
			return;
		}
		if (u->rval >= 0x100 || u->lval != 0 || u->sname != 0 || (u->op == OREG && u->stsize < 0)) {
#ifdef SEG
			expand(p, FOREFF, "\tlda\tVA1X,AL\n");
			expand(p, FOREFF, "\tand\tA1,#%7f00\n");
#else
			expand(p, FOREFF, "\tlda\tA1,AL\n");
#endif
			return;
		}
		expand(p, FOREFF, "\tldZB\tA1");
#ifdef SEG
		printf(",r%s\n", rnames[u->rval]);
#else
		printf(",%s\n", rnames[u->rval]);
#endif
		return;
	case 'M':
		/* load the operand into U1 and extend it */
		{
		int t;

		u = p->left;
		t = u->type;
		if (t == 12)
			expand(p, FOREFF, "\tldk\tU1,#0\n");
		if (u->op != REG || getlr(p, '1')->rval + 1 != u->rval)
			expand(u, FOREFF, "\tldZB\tU1,AL\n");
		if (t == 2)
			expand(p, FOREFF, "\textsb\tU1\n");
		if (!(dope[p->op] & DIVFLAG))
			return;
		if ((unsigned)t <= 15 && (unsigned)t >= 12)
			expand(p, FOREFF, "\tldk\tA1,#0\n");
		else
			expand(p, FOREFF, "\texts\tVA1\n");
		return;
		}
	case 'S':
	case 's':
		/* the structure assignment: a word, a long or a block move */
		{
		NODE *r2;
		int n2;
		char *name;
		int rn;
		NODE *rg, *tmp;
		struct rs *rp2;
		int y;
		int k2;
		int cls;

		if (p->op != STASG)
			cerror("zzzcode: bad ZS");
		u = p->left;
		r2 = p->right;
		n2 = p->stsize;
		rg = getlr(p, '1');
		rn = rg->rval;
		name = rnames[rn];
		k2 = szty(rg->type) == 2 ? 5 : 4;
		cls = rclass(rg->type, -1);
		rp2 = rfind(rn, cls);
		rp2->busy = 1;
		if (n2 <= 4) {
			tmp = 0;
			if (r2->op == ICON) {
				tmp = tcopy(r2);
				r2->op = NAME;
				r2->type = DECREF(r2->type);
			} else if (!tshape(r2, STAREG | SAREG))
				order(r2, STAREG | SAREG);
			if (r2->op == REG) {
				if (notoff(r2->type, r2->rval))
					order(r2, STAREG);
				tmp = tcopy(r2);
				r2->op = OREG;
				r2->sname = 0;
				r2->stsize = 0;
			}
			printf("\tld");
			if (n2 == 4) {
				printf("l\tr%s,", name);
				rmode++;
			} else
				printf("\t%s,", name);
			expand(p, FOREFF, "AR\n\tld");
			if (n2 == 4) {
				putchar('l');
				rmode++;
			}
			expand(p, FOREFF, "\tAL,");
			printf(n2 != 4 ? "%s\n" : "r%s\n", name);
			rfree(rn, k2);
			if (tmp) {
				walkf(r2, recl2);
				tfree(r2);
				p->right = tmp;
			}
		} else {
			tmp = 0;
			if (!tshape(r2, STAREG))
				order(r2, STAREG);
			if (notoff(r2->type, r2->rval))
				order(r2, STAREG);
			if (u->op == OREG && u->lval == 0 && u->sname == 0 && u->rval < 0x100) {
				tmp = tcopy(u);
				u->op = REG;
				if ((u->type & 0x30) != 0x10)
					u->type = INCREF(u->type);
			}
			if (!tshape(u, STAREG)) {
				if (!tmp)
					tmp = tcopy(u);
				order(u, STAREG);
			}
			if (notoff(u->type, u->rval))
				order(u, STAREG);
			n2 >>= 1;
			if (n2 < 16)
				printf("\tldk\t%s,#%d\n", name, n2);
			else
				printf("\tld\t%s,#%d\n", name, n2);
#ifdef SEG
			expand(p, FOREFF, "\tldir\t@rAL,@rAR,");
#else
			expand(p, FOREFF, "\tldir\t@AL,@AR,");
#endif
			printf("%s\n", name);
			rfree(rn, k2);
			if (c == 's') {
				if (!tmp || (tmp && (tmp->op == REG || tmp->op == OREG) && tmp->rval == u->rval))
#ifdef SEG
					printf("\tsub\t%s,#%d\n", rnames[u->rval + 1], n2 * 2);
#else
					printf("\tsub\t%s,#%d\n", rnames[u->rval], n2 * 2);
#endif
			} else
#ifdef SEG
				printf("\tsub\t%s,#%d\n", rnames[r2->rval + 1], n2 * 2);
#else
				printf("\tsub\t%s,#%d\n", rnames[r2->rval], n2 * 2);
#endif
			if (tmp) {
				if (c == 's') {
					walkf(u, recl2);
					tfree(u);
					p->left = tmp;
				} else {
					walkf(tmp, recl2);
					tfree(tmp);
				}
			}
		}
		if (c == 's')
			p->type = p->left->type;
		return;
		}
	case 'A':
		/* an argument that is a structure: up to 4 bytes through a register, else ldm */
		{
		int n3, m3, k3;

		if (p->op != STARG)
			cerror("zzzcode: bad ZA");
		u = p->left;
		n3 = p->stsize;
		if (n3 <= 0 || n3 > 0x6b5d)
			cerror("structure size out of range: %ld", (long)n3);
		if (n3 <= 4) {
			if (u->op == ICON)
				u->op = NAME;
			if (!tshape(u, SNAME | STAREG | SAREG))
				order(u, SNAME | STAREG | SAREG);
			if (u->op == REG) {
				if (notoff(u->type, u->rval))
					order(u, STAREG);
				u->op = OREG;
				u->sname = 0;
				u->stsize = 0;
			}
			printf("\tld");
			if (n3 == 4) {
				printf("l\tr");
				rmode++;
			} else
				putchar('\t');
			m3 = p->rall & 0xEFFF;
			printf("%s,", rnames[m3]);
			expand(p, FOREFF, "AL\n");
			k3 = getreg(n3 == 4 ? 3 : 2, m3, 0x1000, p->sname);
			if (m3 == k3) {
				rbusy(m3, n3 == 4 ? 5 : 4, p->sname, p->lval);
				return;
			}
			rfree(k3, n3 == 4 ? 5 : 4);
			rsreturn(n3 == 4 ? 3 : 2, k3);
			return;
		}
		if (!tshape(u, STAREG))
			order(u, STAREG);
		if (notoff(u->type, u->rval))
			order(u, STAREG);
		n3 >>= 1;
		printf("\tldm\t%s,", rnames[p->rall & 0xEFFF]);
#ifdef SEG
		expand(p, FOREFF, "@VAL");
#else
		expand(p, FOREFF, "@AL");
#endif
		printf(",#%d\n", n3);
		return;
		}
	case 'T':
		/* the second word of a long operand, for the conversion to a shorter type */
		u = (NODE *)p->type;
		p = p->left;
		n = p->type;
		switch (p->op) {
		case NAME:
		case OREG:
#ifdef SEG
			if (p->rval != 15) {
				order(p, 6);
				goto longreg;
			}
#endif
			if (p->sname->sclass == 3 && p->op == NAME && !(p->sname->sflags >> 2 & 1) && p->rval >= 0) {
				if (szty(n) == 2) {
					if ((int)u == 2 || (int)u == 12)
						p->rval += 3;
					else {
						if ((int)u == 3 || (int)u == 13)
							;
						p->rval += 2;
					}
				} else
					p->rval += 1;
				return;
			}
			if (szty(n) == 2) {
				if ((int)u == 2 || (int)u == 12)
					p->lval += 3;
				else {
					if ((int)u == 3 || (int)u == 13)
						;
					p->lval += 2;
				}
			} else
				p->lval += 1;
			return;
		case REG:
#ifdef SEG
		longreg:
#endif
			if (szty(n) != 2)
				return;
			rfree(p->rval, p->type);
			rfreeall(0);
			p->rval++;
			p->type = (int)u;
			if (p->rval <= 7 && getreg(2, p->rval, 0x1000, 0) != p->rval)
				cerror("zzzcode: case 'T'  reg not found");
			return;
		default:
			cerror("Illegal ZT type conversion");
			return;
		}
	case 'P':
		/* the argument registers r4 .. r7 on the use list are free again after a call */
		{
		struct rs *a, *b;

		for (a = rsuse; a; a = b) {
			b = a->next;
			switch (a->reg) {
			case 4:
			case 5:
			case 6:
			case 7:
				rsreturn(9, a->reg);
			}
		}
		return;
		}
	default:
		cerror("illegal zzzcode: %c", c);
	}
}

/*
 * rmove: copy register `from` to register `to` (assembler text) and let `to` take over what `from` holds
 * (symbol, offset, kind); when `from` is on no list `to` holds nothing known afterwards (0x3cf2)
 */
int rwtab[][2] = {
	{ 0x14, 0x14 }, { 0xe, 0xaca }, { 0x1000, 0x1000 }, { 0x2000, 0x2000 }, { 0x4, 0x240 }, { 0, 0 }
};

void
rmove(to, from, type)
	register int to, from;
	int type;
{
	register struct rs *t, *f;

	for (t = rsuse; t; t = t->next)
		if (t->reg == to)
			goto tfound;
	for (t = rsfree; t; t = t->next)
		if (t->reg == to)
			goto tfound;
	cerror("rmove: destination reg not found");
tfound:
	for (f = rsuse; f; f = f->next)
		if (f->reg == from)
			goto ffound;
	for (f = rsfree; f; f = f->next)
		if (f->reg == from)
			goto ffound;
	t->sym = 0;
	t->off = 0;
	t->kind = 8;
	goto copied;
ffound:
	t->sym = f->sym;
	t->off = f->off;
	t->kind = f->kind;
copied:
	if (type == 6 || type == 7)
		printf("/* rmove */\n\tfld");
	else
		printf("/* rmove */\n\tld");
	tsuffix(type);
	if (rmode) {
		printf("\tr%s,r%s\n", rnames[to], rnames[from]);
		rmode = 0;
	} else if (rlmode) {
		printf("\t%s,%s\n", rlnames[to], rlnames[from]);
		rlmode = 0;
	} else
		printf("\t%s,%s\n", rnames[to], rnames[from]);
}

/* an operator that is a leaf, or (with p) a STAR over a leaf that shumul accepts */
int
shltype(op, p)
	register int op;
	register NODE *p;
{
	if (op == NAME || op == REG || op == ICON || op == OREG)
		return 1;
	return op == STAR && p != 0 && shumul(p->left);
}

/* the operator is a leaf that can be addressed: NAME, REG, ICON, OREG */
int
ltypeop(p)
	register NODE *p;
{
	register int o;

	o = p->op;
	return o == NAME || o == REG || o == ICON || o == OREG;
}

/* STAR over p: a register variable or a constant (operand that needs no register) */
int
shtemp(p)
	register NODE *p;
{
	if (p->op == STAR)
		p = p->left;
	return (p->op == REG || p->op == OREG) ? !(rstatus[p->rval] & 0x14) : (p->op == NAME || p->op == ICON);
}

/* does an increment of incr bytes on a pointer of type t step by the size of the object it points to? */
int
shincr(t, incr)
	register int t;
	register long incr;
{
	if ((t & 0x30) != 0x10)
		return 0;
	t = ((unsigned)t >> 2 & ~0xF) | (t & 0xF);
	if ((t & 0x30) == 0x10)
		return incr == 2;		/* a pointer to a pointer (16 bit) */
	switch (t) {
	case 2: case 12: return incr == 1;
	case 3: case 4: case 13: case 14: return incr == 2;
	case 6: return incr == 4;
	case 7: return incr == 8;
	}
	return 0;
}

/* STAR over p: which address shapes does it fit? STARNM for a name, STARREG for a post increment through a register */
int
shumul(p)
	register NODE *p;
{
	register int o = p->op;

	if (o == NAME || o == OREG || o == ICON)
		return STARNM;
	if ((o == 0x4E || o == 9) && p->left->op == REG && p->right->op == ICON && p->right->sname == 0 &&
	    shincr(p->left->type, p->right->lval))
		return STARREG;
	return 0;
}

/* print a long mask of the M and N templates */
void
prlong(v)
	long v;
{
	printf("%ld", v);
}

/* a constant or a register as an operand part */
void
conput(p)
	register NODE *p;
{
	switch (p->op) {
	case ICON:
		acon(p);
		break;
	case REG:
		if (rlmode)
			printf("%s", rlnames[p->rval]);
		else {
			if (rmode)
				printf("r");
			printf("%s", rnames[p->rval]);
		}
		break;
	case OREG:
		if (p->lval >= 0x100)
			cerror("bad oreg in conput");
		else if (p->lval != 0 || p->sname != 0 || (p->op == OREG && p->stsize < 0))
			oregput(p);
		else
#ifdef SEG
			printf("@r%s", rnames[p->rval]);
#else
			printf("@%s", rnames[p->rval]);
#endif
		break;
	default:
		cerror("illegal conput");
	}
}

void
insput(p)
	NODE *p;
{
	cerror("insput");
}

/* offset(register) of an OREG */
void
oregput(p)
	register NODE *p;
{
	int bar = 0;		/* only the segmented version uses it */

#ifdef SEG
	if (p->rval == 15) {
		if ((p->sname == symtab0 && (unsigned)(int)p->lval <= 0xFF) ||
		    (p->sname == fpsym1 && (unsigned)(int)(p->lval + maxoff) <= 0xFF)) {
			bar++;
			putchar('|');
		}
		acon(p);
		if (bar)
			putchar('|');
		printf("(fp)");
	} else {
		acon(p);
		printf("(%s)", rnames[p->rval]);
	}
#else
	if (p->rval == 15) {
		acon(p);
		printf("(fp)");
	} else {
		acon(p);
		printf("(%s)", rnames[p->rval]);
	}
#endif
}

/* the second word of an operand */
void
upput(p)
	register NODE *p;
{
	register long save;
	register int r;

	if (p->op == FLD)
		p = p->left;
	save = p->lval;
	switch (p->op) {
	case NAME:
		p->lval += 2;
		if (p->rval < 0)
			goto ac;
		r = p->rval;
		p->rval += 2;
		acon(p);
		p->rval = r;
		break;
	case ICON:
		p->lval &= 0xFFFF;
		putchar('#');
		goto ac;
	case REG:
		if (rlmode) {
			printf("%s", rlnames[p->rval + 1]);
			return;
		}
		printf("%s", rnames[p->rval + 1]);
		break;
	case OREG:
		if (p->rval >= 0x100)
			cerror("illegal OREG upput");
#ifdef SEG
		if (p->lval != 0 || p->sname != 0 || (p->op == OREG && p->stsize < 0)) {
			p->lval += 2;
			oregput(p);
		} else {
			p->lval = 2;
			r = p->rval;
			p->rval = (r + 1) << 8;
			adrput(p);
			p->rval = r;
		}
#else
		p->lval += 2;
		oregput(p);
#endif
		break;
	default:
		cerror("illegal upper address");
		break;
	ac:
		acon(p);
	}
	p->lval = save;
}

/* an operand */
void
adrput(p)
	register NODE *p;
{
	long save;
	register int r;

	if (p->op == FLD)
		p = p->left;
	switch (p->op) {
	case NAME:
		acon(p);
		break;
	case ICON:
		if (upperlval) {			/* the high word of a long constant */
			save = p->lval;
			p->lval = (p->lval >> 16) & 0xFFFF;
			putchar('#');
			acon(p);
			p->lval = save;
		} else {
			putchar('#');
			acon(p);
		}
		break;
	case REG:
		if (rlmode)
			printf("%s", rlnames[p->rval]);
		else {
			if (rmode)
				printf("r");
			printf("%s", rnames[p->rval]);
		}
		break;
	case OREG:
		r = p->rval;
		if (r >= 0x100) {			/* base and index register */
			if (r & 0xFF)
#ifdef SEG
				printf("r%s(%s)", rnames[(r >> 8) - 1], rnames[(r & 0xFF) - 1]);
#else
				printf("r%s(%s)", rnames[(r >> 8) - 1], rnames[(r & 0xFF) - 1]);
#endif
			else {
				printf("r%s(#", rnames[(r >> 8) - 1]);
				acon(p);
				putchar(')');
			}
		} else if (p->lval == 0 && p->sname == 0 && p->stsize == 0)
#ifdef SEG
			printf("@r%s", rnames[r]);
#else
			printf("@%s", rnames[r]);
#endif
		else
			oregput(p);
		break;
	default:
		cerror("illegal address: %d (%s)", p->op, opst[p->op]);
	}
}

/* a name or constant: label, name, +offset */
void
acon(p)
	register NODE *p;
{
#ifdef SEG
	struct sym *s;
#endif
	char c;

	if (p->op == ICON && p->rval < 0)
		printf("L%d", -p->rval);
	else if (p->op == OREG && p->stsize < 0)
		printf("L%d", -p->stsize);
	else if (p->op == NAME && p->rval < 0)
		printf("L%d", -p->rval);
	else {
		if (p->sname == 0) {
			printf("%ld", p->lval);
			return;
		}
#ifdef SEG
		s = p->sname;
		c = ascii[s->nameoff];
		if (s == symtab0 || c == '~') {		/* an address in the stack segment: _stkseg+~L1+offset */
			printf("_stkseg");
			if (c == ascii[fpsym1->nameoff] || ascii[fpsym2->nameoff] == c)
				printf("+%s", ascii + s->nameoff);
		} else if (p->op == NAME && s->sclass == 3 && !(s->sflags >> 2 & 1)) {	/* a local static: its label */
			putchar('L');
			printf("%ld", s->soffset);
			p->lval = p->rval;
		} else {
			putchar('_');
			printf("%s", ascii + s->nameoff);
		}
#else
		if (p->sname && ascii[p->sname->nameoff] != '~') {
			if (p->op == NAME && p->sname->sclass == 3 && !(p->sname->sflags >> 2 & 1)) {	/* a local static: its label */
				putchar('L');
				printf("%ld", p->sname->soffset);
				p->lval = p->rval;
			} else {
				putchar('_');
				printf("%s", ascii + p->sname->nameoff);
			}
		} else if ((c = ascii[p->sname->nameoff]) == ascii[fpsym1->nameoff] || ascii[fpsym2->nameoff] == c)
			printf("%s", ascii + p->sname->nameoff);		/* ~L1, ~L2 */
#endif
	}
	if (p->lval) {
		putchar('+');
		printf("%ld", p->lval);
	}
}

int
genscall(p, cookie)
	NODE *p;
	int cookie;
{
	return gencall(p, cookie);
}

/* generate a call: the function address (an expression) is made addressable, the arguments are placed, then the call itself */
int
gencall(p, cookie)
	register NODE *p;
	int cookie;
{
	register NODE *f;
	register int fop;
	register int indirect;
#ifdef SEG
	int r;
#endif
	long save;

	f = p->left;
	fop = f->op;
	indirect = 0;
	argstop = 0;
	if (fop != ICON) {
		if (p->right) {
			indirect = 1;
			order(f, INTEMP);
		} else if (fop != REG || f->rval == 0) {
			order(f, 6);
			if (f->rval == 0)
				order(f, 6);
			rfree(f->rval, 0x14);
		}
	}
	if (p->right) {
		genargs(p->right);
	}
#ifdef SEG
	if (indirect) {
		if (argstop == 0 && argreg >= 3) {
			/* the address is still in reach of a free register pair */
			f->rall = 0x1002;
			order(f, 6);
			rfree(f->rval, 0x14);
			indirect = 0;
		} else {
			/* rr8 holds the address during the call; its old contents wait in a temporary */
			save = freetemp(2) / 8;
			printf("\tldl\t_stkseg+~L1+%ld(fp),rr8\n", save);
			printf("\tldl\trr8,_stkseg+~L1+%ld(fp)\n", f->lval);
			f->rval = 8;
			f->lval = 0;
			f->sname = 0;
		}
	}
#else
	if (indirect) {
		if (maxoff + f->lval)
			printf("\tld\tr1,~L1+%ld(fp)\n", f->lval);
		else
#ifdef OLD_D18
			printf("\tld\tr1,0(fp)\n");
#else
			printf("\tld\tr1,@fp\n");
#endif
		f->rval = 1;
		f->lval = 0;
		f->sname = 0;
	}
#endif
	if (f->op == REG) {
		f->op = OREG;
		f->stsize = 0;
	}
	p->op = UCALL;
	rsfreeall(1);
	tmpoff = 0;
#ifdef SEG
	r = match(p, 0x16);
	if (indirect)
		printf("\tldl\trr8,_stkseg+~L1+%ld(fp)\n", save);
	return r != MDONE;
#else
#ifdef OLD_D22
	/* v0p: a double result comes back in rq4 */
	if (match(p, 0x16) != MDONE)
		return 1;
	if ((p->type == 6 || p->type == 7) && p->op == REG)
		printf("\tfldd\t%s,r4\n", rnames[p->rval]);
	return 0;
#else
	return match(p, 0x16) != MDONE;
#endif
#endif
}

/* the table of the conditional jumps by relation */
static char *jprtab[] = {
	"\tjpr\teq,L%d\n",
	"\tjpr\tne,L%d\n",
	"\tjpr\tle,L%d\n",
	"\tjpr\tlt,L%d\n",
	"\tjpr\tge,L%d\n",
	"\tjpr\tgt,L%d\n",
	"\tjpr\tule,L%d\n",
	"\tjpr\tult,L%d\n",
	"\tjpr\tuge,L%d\n",
	"\tjpr\tugt,L%d\n",
	"\tjpr\tfeq,L%d\n",
	"\tjpr\tfneu,L%d\n",
	"\tjpr\tfle,L%d\n",
	"\tjpr\tflt,L%d\n",
	"\tjpr\tfge,L%d\n",
	"\tjpr\tfgt,L%d\n",
};

/* the relation with the operands exchanged (EQ stays EQ, LE becomes GE ...) */

/* a jump to the label lab when the relation o holds (o 0: always); mode 'F': the operands are exchanged */
void
cbgen(o, lab, mode)
	register int o;
	register int lab;
	register int mode;
{
	register int spare1, spare2;

#ifdef OLD_D3K
	d3kjump(lab);
#endif
	if (o == 0) {
		printf("\tjpr\tL%d\n", lab);
		return;
	}
	if (o > FGT) {
		cerror("bad conditional branch: %s", opst[o]);
		return;
	}
	if (mode == 'F')
		o = revrel[o - EQ];
	printf(jprtab[o - EQ], lab);
	cbflag = 0;
}

/* the next cookie to try when no table entry matched */
int
nextcook(p, cookie)
	register NODE *p;
	register int cookie;
{
	if (cookie == FORREW)
		return 0;
	if (!(cookie & 0x14))
		return 0x14;
	if (!(cookie & 0x1000) && asgop(p->op))
		return 0x101E;
	return FORREW;
}

/* the last chance: a name or an OREG may go to a register by the rewrite tables */
int
lastchance(p, cookie)
	register NODE *p;
	register int cookie;
{
	if (cookie == 0x2000 && (p->op == OREG || p->op == NAME))
		return 1;
	return 0;
}

/* a constant operand of an AND goes to the right */
void
icright(p)
	register NODE *p;
{
	register NODE *t;

	switch (p->op) {
	case AND:
		if (p->left->op == ICON) {
			t = p->left;
			p->left = p->right;
			p->right = t;
		}
	}
}

/* canon, then the constants of the ANDs to the right */
void
canon2(p)
	register NODE *p;
{
	p = canon(p);
	walkf(p, icright);
}

/* the SPECIAL shapes: small constants and powers of two */
int
special(p, shape)
	register NODE *p;
	int shape;
{
	if (p->op != ICON)
		return 0;
	if (p->sname) {
		if (shape != SPECIAL + 8)
			return 0;
		return 1;
	}
	if (p->rval < 0)
		return 0;
	switch (shape) {
	case SPECIAL + 1:
		return p->lval >= 0 && p->lval <= 15;
	case SPECIAL + 2:
		return p->lval >= -16 && p->lval <= 16;
	case SPECIAL + 3:
		return p->lval >= -256 && p->lval <= 255;
	case SPECIAL + 5:
		if (!plainicon(p))
			return 0;
		/* FALLTHROUGH */
	case SPECIAL + 4:
		return p->lval >= -32768 && p->lval <= 32767;
	case SPECIAL + 6:
		return ispow2(p->lval) >= 0;
	case SPECIAL + 7:
		return ispow2(~p->lval) >= 0;
	case SPECIAL + 8:
		return 0;
	}
	cerror("bad special shape: SPECIAL | %%%0.2x", shape - SPECIAL);
}


/* a comparison with a register on the right goes with the operands exchanged */
static int rev[] = { EQ, NE, GE, GT, LE, LT, UGE, UGT, ULE, ULT };

void
mycanon(p)
	register NODE *p;
{
	register NODE *t;

	switch (p->op) {
	case EQ: case NE: case GE: case GT: case LE: case LT: case UGE: case UGT: case ULE: case ULT:
		break;
	default:
		return;
	}
	if (p->right->op != REG)
		return;
	if (p->left->op == REG)
		return;
	if (p->left->op == ICON && p->left->lval == 0)
		return;
	t = p->left;
	p->left = p->right;
	p->right = t;
	p->op = rev[p->op - EQ];
}

int
main(argc, argv)
	int argc;
	char **argv;
{
	return mainp2(argc, argv);
}
