/*
 * wrt3.c - text of the head of a function, its parameters
 */
#include "cg.h"
#include "cg2.h"

extern int argsize();
extern char ascii[];
extern int cerror();
extern void endline();
extern void endrun();
extern int entlab;
extern void fbegin();
extern char fname[];
extern int getlab();
extern int getreg();
extern void hdrout();
extern int ifline;
extern char ifname[];
extern int jlab;
extern int lflag;
extern void mapsym();
extern int mcountlab;
extern NODE node[];
extern int nparmtab;
extern void parminit();
extern struct { struct sym *sym; int reg; } parmtab[];
extern int pflag;
extern int callreg();
extern int rclass();
extern struct rv regvar[];
extern int retlab;
extern int rflag;
extern void rfree();
extern char *rnames[];
extern struct rs *rsuse;
extern int structlab;
extern int structret;
extern void structretcode();
extern long structsz;
static char sccsid[] = "@[$]wrt3.c\t2.4  08/24/83 14:01:53 Copr. 1981 87wega3.2.";

#define INCREF(t)	((((t) & ~0xF) << 2) | 0x10 | ((t) & 0xF))

#define DECREF(t)	((((t) >> 2) & ~0xF) | ((t) & 0xF))

#ifdef OLD_D3P
#define D3PWORD(t)	((t) == 4 || (t) == 14 || ((t) & 0x30) == 0x10)
#endif

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

void
endrun()
{
	endline();
}

/* with -l: the line directive for a source file with the extension .c */
void
endline()
{
	register int n;
	register char *cp;

	if (lflag) {
		n = strlen(ifname);
		if (*(cp = &ifname[n - 1]) == '"')
			cp--;
		if (*cp-- == 'c' && *cp == '.')
			printf("\t.line\t%d\t%s\t\n", ifline, ifname);
	}
}

/* a formal parameter of the function: into its register, or noted for the entry code */
void
parminit(p)
	register NODE *p;
{
	register int n, r, cls, t2, sz;
	register struct rs *q;
	struct sym *s;

	if (p->in.op != NAME)
		return;
	mapsym(p);
	sz = argsize(p);
	s = p->in.sname;
	n = s->sbyte5;
	r = s->stype;
#ifdef SEG
	if (r == 6 || r == 7)
		n -= 6;
#else
	if (r == 6 || r == 7)
		n -= 5;
#endif
	else
		n -= 7;
	if (rflag && s->sclass == 4 && regvar[n].sym != s)
		s->sclass = 9;
	if (p->tn.lval < 0 && s->sclass != 4 && s->sclass != 8)
		s->sbyte5 = -p->tn.lval;
	r = s->sbyte5;
	if (p->in.type == 8 || p->in.type == 9) {
		if (sz == 1)
			t2 = 4;
		else if (sz == 2)
			t2 = 5;
		else
			t2 = p->in.type;
	} else
		t2 = p->in.type;
	cls = rclass(t2, r);
#ifdef OLD_D22
	if (s->sclass == 9 && p->tn.lval < 0 && (t2 == 6 || t2 == 7) && r < 8) {	/* v0p: a double parameter in rq4 */
		if (nparmtab > 9)
			cerror("rinit: formal paramter array overflow");
		parmtab[nparmtab].reg = s->sbyte5;
		parmtab[nparmtab].sym = s;
		nparmtab++;
		parmtab[nparmtab].sym = 0;
		s->sflags &= ~8;
		return;
	}
#endif
	if ((s->sclass == 9 && p->tn.lval < 0) || s->sclass == 4) {
		for (q = rsuse; q; q = q->next)
			if (q->reg == r) {
				q->busy = 0;
				goto got;
			}
		getreg(cls, r, 0x1000, s);
		rfree(r, t2);
	got:
		if (s->sclass == 4) {
			if (s->stype == 7 || (s->stype == 6 && p->tn.lval < 0))
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
			if (p->tn.lval < 0) {
				if (sz == 2)
					putchar('r');
				printf("%s\n", rnames[-p->tn.lval]);
				return;
			}
#ifdef SEG
				printf("_stkseg+~L2+%ld(fp)\n", p->tn.lval);
#else
				printf("~L2+%ld(fp)\n", p->tn.lval);
#endif
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
	if (s->sclass == 9 && p->tn.lval >= 0)
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
		printf("\tldir\t@rr4,@r%s,r1\n", rnames[callreg(0)]);
		printf("\tldl\tr%s,#L%d\n", rnames[callreg(0)], structlab);
#else
		printf("\tld\tr4,#L%d\n", structlab);
		printf("\tld\tr1,#%ld\n", structsz);
		printf("\tldir\t@r4,@%s,r1\n", rnames[callreg(0)]);
		printf("\tld\t%s,#L%d\n", rnames[callreg(0)], structlab);
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
	char b[8];

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
	t = s->stype;
	t = DECREF(t);
	structret = t == 8 || t == 9;
	if (structret) {
		structlab = s->sx6;
		structsz = s->ssize / 16;
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
