/*
 * efcode3.c - the entry and the exit code of a function, and the end of its text (codgen / scodgen)
 *
 * Reconstructed from /lib/codgen: lineout() 0x2880, funcend() 0x289a, efcode() 0x28f6, epilog() 0x2990, regrestore() 0x29de,
 * fpinc() 0x2aa2, prolog() 0x2aca, fpdec() 0x2b20, regsave() 0x2b48, regargs() 0x2c0e. Names are guesses.
 * The frame has two parts, the saved registers (~L2) and the automatic variables (~L1).
 */
#include "cg.h"

extern int cerror();
extern int deflab(), branch();
extern struct rs *rfind();
extern int rclass();
extern char *rnames[];
extern long maxoff, parmax;
extern int minreg, maxreg;
extern int fltused;
extern char fname[];		/* 0xa0e4: the name of the function */
extern int retlab, jlab, entlab, structret;	/* 0x8fca, 0xa14c, 0xa0e2, 0xa15a */
extern void structretcode(), endline();
extern struct { struct sym *sym; int reg; } parmtab[];	/* 0x8f5c */
extern int nparmtab;		/* 0x8f98 */

void
lineout(line, file)
	long line;
	char *file;
{
	printf("\t.line\t%d\t%s\t\n", (int)line, file);
}

/* the end of the text of a function */
void
funcend(a, b)
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

/* ~L2 grows or shrinks the frame pointer */
static void
fpinc(tot)
	unsigned tot;
{
	if (tot) {
		if (tot > 0x10)
			printf("\tadd\tfp,#~L2\n");
		else
			printf("\tinc\tfp,#~L2\n");
	}
}

static void
fpdec(tot)
	unsigned tot;
{
	if (tot) {
		if (tot > 0x10)
			printf("\tsub\tfp,#~L2\n");
		else
			printf("\tdec\tfp,#~L2\n");
	}
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
			printf("\tldm\t%s,@fp,#%d\n", rnames[minreg + 1], a);
	} else if (a == 1) {
		if (sz != 0)
			printf("\tld\t%s,%d(fp)\n", rnames[minreg + 1], (int)sz);
		else
			printf("\tld\t%s,@fp\n", rnames[minreg + 1]);
	}
#endif
	if (b >= 1) {
		off = (int)w;
		for (k = maxreg; k < 0x17; k++, off += 8)
#ifdef SEG
			printf("\tfldd\t%s,_stkseg+%d(fp)\n", rnames[k + 1], off);
#else
			printf("\tfldd\t%s,%d(fp)\n", rnames[k + 1], off);
#endif
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
			printf("\tldm\t@fp,%s,#%d\n", rnames[minreg + 1], a);
	} else if (a == 1) {
		if (sz != 0)
			printf("\tld\t%d(fp),%s\n", (int)sz, rnames[minreg + 1]);
		else
			printf("\tld\t@fp,%s\n", rnames[minreg + 1]);
	}
#endif
	if (b >= 1) {
		off = (int)w;
		for (k = maxreg; k < 0x17; k++, off += 8)
#ifdef SEG
			printf("\tfldd\t_stkseg+%d(fp),%s\n", off, rnames[k + 1]);
#else
			printf("\tfldd\t%d(fp),%s\n", off, rnames[k + 1]);
#endif
	}
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

/* the registers that arrive as arguments but live in the frame go to their place */
static void
regargs(tot)
	register int tot;
{
	register int lo, hi, t;
	int i, reg, cls, k;
	struct sym *s;
	struct rs *n;
	char *mn;

	lo = 7;
	hi = 2;
	t = tot;
	for (i = nparmtab - 1; i >= 0; i--) {
		s = parmtab[i].sym;
		reg = parmtab[i].reg;
		cls = rclass(s->stype, reg);
		n = rfind(reg, cls);
		if (n && n->sym == s)
			goto fl;
		if (cls < 4) {
			if (lo > reg)
				lo = reg;
			k = cls == 3 ? 1 : 0;
			if (hi < reg + k)
				hi = reg + k;
			if ((unsigned long)(s->soffset / 8) < (unsigned long)(unsigned)t) {
				t = (int)(s->soffset / 8);
				if (t & 1)
					t--;
			}
			continue;
		}
	fl:
		if (cls == 4) {
			mn = s->stype == 6 ? "flds" : "fldd";
#ifdef SEG
			printf("\t%s\t|_stkseg+~L1+%ld|(fp),%s\n", mn, s->soffset / 8, rnames[reg]);
#else
			if (maxoff + s->soffset != 0)
				printf("\t%s\t~L1+%ld(fp),%s\n", mn, s->soffset / 8, rnames[reg]);
			else
				printf("\t%s\t@fp,%s\n", mn, rnames[reg]);
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
		if ((long)(unsigned)t + maxoff != 0)
			printf("\tld\t~L1+%u(fp),r%d\n", t, lo);
		else
			printf("\tld\t@fp,r%d\n", lo);
	} else if (!(lo & 1) && hi - lo == 1) {
		if ((long)(unsigned)t + maxoff != 0)
			printf("\tldl\t~L1+%u(fp),rr%d\n", t, lo);
		else
			printf("\tldl\t@fp,rr%d\n", lo);
	} else {
		if ((long)(unsigned)t + maxoff != 0)
			printf("\tldm\t~L1+%u(fp),r%d,#%d\n", t, lo, hi - lo + 1);
		else
			printf("\tldm\t@fp,r%d,#%d\n", lo, hi - lo + 1);
	}
#endif
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
	funcend((int)(maxoff / 8), tot);
}

/* the end of the function: entry and exit code with the frame sizes now known */
void
efcode()
{
	long sz, w;
	int a, b, tot;

	sz = (parmax + maxoff) / 8;
	if (sz % 2)
		sz = (sz / 2 + 1) * 2;
#ifdef SEG
	a = 13 - minreg;
#else
	a = 14 - minreg;
#endif
	b = 23 - maxreg;
	w = a * 2 + sz;
	tot = b * 8 + w;
	epilog(a, sz, b, w, tot);
	prolog(a, sz, tot, w, tot);
}
