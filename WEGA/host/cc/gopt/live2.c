/*
 * live2.c - gopt: the live variables. Written from /lib/gopt (no source known); names guessed.
 */
static char sccsid[] __attribute__((unused)) = "@[$]live2.c\t1.9  01/07/83 13:42:01 Copr. 1981 87wega3.2.";
#include "mfile2.h"

extern int lvmark, nlv, lvidx;
extern struct lv *lvtab;
extern struct bt *betatbl;
extern struct rn *reachmap;
#define MASK(p)	((struct quad *)((uintptr_t)(p) & ~(uintptr_t)1))
extern struct edge *edges;
extern unsigned *tmpv;
extern int lvfirst, dffirst;
extern char *livevecs, *livevecs;

/* the flags of the quad q (debug; name guessed) */
void prqflags(struct quad *q)
{
	printf("\t\t\t\t\t\t\t");
	if (q->f6)
		printf(" gl1");
	if (q->f5)
		printf(" gl2");
	if (q->f4)
		printf(" gr");
	if (q->f7)
		printf(" k");
	if (q->c.b2 == OANAME && (SFISGLOBAL(symtab[q->c.w4].sflags)) && q->c.fl)
		printf(" reg_candidate");
	printf("\n");
}

/* all quads with their flags (debug; name guessed) */
void prqall(void)
{
	struct quad *q;

	for (q = qhead; q != 0; q = q->next) {
		prquad(q);
		prqflags(q);
	}
}

/* the variable v (kind 1: a variable, 8: beta, else a temporary; alias 1: aliased) is entered in the table of the live variables; 1 when it was in it, lvidx its index (name from the string) */
int islive(int v, int kind, int alias)
{
	int i;

	if (ldebug > 2)
		printf("islive(%d,isvar=%s,isalias=%s)\n", v, kind == 1 ? "ISVAR" : kind == 8 ? "BETA" : "ISTMP", alias == 1 ? "ALIASED" : "UNALIASED");
	for (i = 0; i < nlv; i++)
		if (lvtab[i].var == v && lvtab[i].kind == kind && lvtab[i].alias == alias)
			goto found;
	if (lvmark) {
		if (ldebug > 1) {
			if (kind == 1)
				printf("\t%d\t\t%s\n", i, ascii + symtab[v].nameoff);
			else
				printf("\t%d,\t\t%d\n", i, v);
		}
		tabzalloc((int)sizeof(struct lv), 2, 2);
		lvtab[i].var = v;
		lvtab[i].kind = kind;
		lvtab[i].alias = alias;
		nlv++;
		return 0;
	}
	cerror("islive: variable not found");
found:
	lvidx = i;
	return 1;
}

/* the operand which (1: the result, 2: the operand a, 3: the operand b) of the quad q: its temporary is entered once in the table of the live variables (name from the strings) */
int settmptable(struct quad *q, int which)
{
	int t = 0;	/* host: = 0 (the original: what the frame holds) */

	switch (which) {
	case 1:
		if (ldebug > 5)
			printf("settmptable(RSLT)\n");
		t = q->c.w4;
		break;
	case 2:
		if (ldebug > 5)
			printf("settmptable(OP1)\n");
		t = q->a.w4;
		break;
	case 3:
		if (ldebug > 5)
			printf("settmptable(OP2)\n");
		t = q->b.w4;
		break;
	}
	if (ldebug > 3)
		printf("settmptable(), i=%d, betatbl[i].bopt=%d\n", t, bbit(t, 13));
	if (!bbit(t, 13)) {
		betatbl[t].bopt = 1;
		if (!islive(t, 0, 0) && nlv > 480)
			return 0;
	}
	return 1;
}

/* the operand a of the quad q: its alias list is entered in the table of the live variables (cnt counts); 0 when the table is full (name from the string) */
int insrtlive(struct quad *q, int *cnt)
{
	struct aln *a;
	int v;

	if (ldebug > 3) {
		printf("insrtlive(), opndtype=OP1, quad:\n");
		prquad(q);
	}
	a = checkalias(q, 2);
	if (a == 0)
		goto r1;
	if (SFISGLOBAL(symtab[q->a.w4].sflags)) {
		if (q->op == QSTORE && almap(&q->a, QSTORE) == 0)
r1:
			return 1;
		if (q->op == QIDR && almap(&q->a, QIDR) == nptr)
			return 1;
		if (q->op == QADDR && almap(&q->a, QADDR) == nadr)
			return 1;
	}
	(*cnt)++;
	v = altab[a->x0];
	if (islive(v, 1, 1))
		return 1;
	if (nlv > 480)
		return 0;
	goto r1;
}

/* the operand which (1: the result, 2: the operand a, 3: the operand b) of the quad q: its symbol is entered once in the table of the live variables (name from the strings) */
int setsymtable(struct quad *q, int which)
{
	int t = 0;	/* host: = 0 (the original: what the frame holds) */
	struct quad tq;
	int cnt;

	tq.a.b2 = OANAME;
	switch (which) {
	case 1:
		if (ldebug > 3)
			printf("setsymtable(RSLT)\n");
		t = q->c.w4;
		if (q->c.b2 == OABTEMP)
			tq.a.w4 = (t = betatbl[q->c.w4].x1, t);
		else
			tq.a = q->c;
		break;
	case 2:
		if (ldebug > 3)
			printf("setsymtable(OP1)\n");
		t = q->a.w4;
		if (q->a.b2 == OABTEMP)
			tq.a.w4 = (t = betatbl[q->a.w4].x1, t);
		else
			tq.a = q->a;
		break;
	case 3:
		if (ldebug > 3)
			printf("setsymtable(OP2)\n");
		t = q->b.w4;
		if (q->b.b2 == OABTEMP)
			tq.a.w4 = (t = betatbl[q->b.w4].x1, t);
		else
			tq.a = q->b;
		break;
	}
	if (ldebug > 3)
		printf("setsymtable(), %s, optimize=%d\n", ascii + symtab[t].nameoff, symtab[t].sflags & SFOPT);
	if (!(symtab[t].sflags & SFOPT)) {
		symtab[t].sflags |= SFOPT;
		tq.b.b2 = OANONE;
		tq.c.b2 = OANONE;
		tq.op = QSTORE;	/* byte 4 of the quad */
		cnt = 0;
		if (!insrtlive(&tq, &cnt))
			return 0;
		tq.op = QIDR;	/* byte 4 of the quad */
		if (!insrtlive(&tq, &cnt))
			return 0;
		tq.op = QADDR;	/* byte 4 of the quad */
		if (!insrtlive(&tq, &cnt))
			return 0;
		if (cnt < 3 && !islive(t, 1, 0) && nlv > 480)
			return 0;
	}
	return 1;
}

/* the symbols of the operand flag (1: the result, 2: the operand a, 3: the operand b) of the quad q are entered (name from the string); 0 when the table is full */
int setsym(struct quad *q, int flag)
{

	switch (flag) {
	default:
		goto bad;
r1:
		return 1;
	case 1:
		if (ldebug > 3)
			printf("setsym(GENRBIT)\n");
		if (((q->c.b2 == OANAME || q->c.b2 == OABTEMP) && !setsymtable(q, 1))
		    || ((q->c.b2 == OASTEMP || q->c.b2 == OABTEMP) && !settmptable(q, 1)))
r0:
			return 0;
		return 1;
	case 2:
		if (ldebug > 3)
			printf("setsym(GENL1BIT)\n");
		if (((q->a.b2 == OANAME || q->a.b2 == OABTEMP) && !setsymtable(q, 2))
		    || ((q->a.b2 == OASTEMP || q->a.b2 == OABTEMP) && !settmptable(q, 2)))
			return 0;
		return 1;
	case 3:
		if (ldebug > 3)
			printf("setsym(GENL2BIT)\n");
		if (!(((q->b.b2 == OANAME || q->b.b2 == OABTEMP) && !setsymtable(q, 3))
		    || ((q->b.b2 == OASTEMP || q->b.b2 == OABTEMP) && !settmptable(q, 3))))
			goto r1;
		goto r0;
	}
bad:
	cerror("setsym: illegal flag");
	goto r1;
}

/* the symbols of the quads of the block b are entered (name from the string); 0 when the table is full */
int markblkvar(int b)
{
	struct quad *q;

	if (ldebug > 2)
		printf("markblkvar(blkno=%d)\n", b);
	q = blks[b]->next;
	if (ldebug > 2)
		prquad(q);
	for (; q != 0; q = q->next) {
		if (q->op == QBBLOCK)
			break;
		if (ldebug > 2)
			prquad(q);
		if (q->f4 || q->f7)
			if (!setsym(q, 1))
				return 0;
		if (q->f6)
			if (!setsym(q, 2))
				return 0;
		if (q->f5)
			if (!setsym(q, 3))
				return 0;
	}
	if (ldebug > 2)
		printf("markblkvar: returning TRUE\n");
	return 1;
}

/* the symbols of all blocks that are in the dominator numbering (name guessed) */
int markall(void)
{
	int i;

	for (i = 0; i < blkno; i++)
		if (idfn[i] >= 0)
			if (!markblkvar(i))
				return 0;
	return 1;	/* host: the original has no return; marklpvar() does not use the result */
}

/* the symbols of the blocks of the loops (all blocks when there is no loop); name from the string */
int marklpvar(void)
{
	struct bl *q;
	struct loop *p;

	if (ldebug > 1) {
		printf("marklpvar()\n\n");
		printf("live bit position -- Symbol\n");
	}
	p = loops;
	if (loops != 0) {
		for (; p != 0; p = p->link) {
			if (!markblkvar(p->hdr))
				return 0;
			for (q = p->bls; q != 0; q = q->next)
				if (!markblkvar(q->blk))
					return 0;
		}
	} else
		markall();
	return 1;
}

/* the kind of the symbol of the operand flag of the quad q: 4 when its symbol is marked, 6 when its temporary is, else 0 (flag 1, 4, 5: the result c, 2: a, 3: b; name from the string) */
int issym(struct quad *q, int flag)
{
	int r, j;

	r = 0;
	switch (flag) {
	case 1:
	case 4:
	case 5:
		j = q->c.w4;
		if (q->c.b2 == OANAME) {
			if (symtab[j].sflags & SFOPT)
				r = 4;
		} else if (q->c.b2 == OASTEMP) {
			if (bbit(j, 13))
				r = 6;
		} else if (q->c.b2 == OABTEMP) {
			if (q->op == QSTORE) {
				if (symtab[betatbl[j].x1].sflags & SFOPT)
					r = 4;
			} else if (bbit(j, 13))
				r = 6;
		}
		break;
	case 2:
		j = q->a.w4;
		if (q->a.b2 == OANAME) {
			if (symtab[j].sflags & SFOPT)
				r = 4;
		} else if (q->a.b2 == OASTEMP) {
			if (bbit(j, 13))
				r = 6;
		} else if (q->a.b2 == OABTEMP) {
			if (q->op == QIDR || q->op == QLOAD) {
				if (symtab[betatbl[j].x1].sflags & SFOPT)
					r = 4;
			} else if (bbit(j, 13))
				r = 6;
		}
		break;
	case 3:
		j = q->b.w4;
		if (q->b.b2 == OANAME) {
			if (symtab[j].sflags & SFOPT)
				r = 4;
		} else if (q->b.b2 == OASTEMP || q->b.b2 == OABTEMP) {
			if (bbit(j, 13))
				r = 6;
		}
		break;
	default:
		cerror("setsym: illegal flag");
	}
	if (ldebug > 3) {
		printf("// issym : q = %X, bit = %d, i=%d, j=%d\n", (unsigned)(uintptr_t)(q), flag, r, j);
		prquad(q);
	}
	return r;
}

/* the quad q is entered in the table of the definitions (name guessed) */
void adddef(struct quad *q)
{
	if (ldebug > 1) {
		printf("defno=%d\n", ndefs);
		prquad(q);
	}
	tabzalloc((int)sizeof(struct quad *), 4, 3);
	defs[ndefs] = q;
	ndefs++;
}

/* the definitions that reach: the quads that set a marked symbol are entered (name from the string); 0 when the table is full */
int countreachdefs(void)
{
	struct quad *q;

	if (ldebug > 2)
		printf("countreachdefs()\n");
	for (q = qhead; q != 0; q = q->next) {
		if (q->op == QBBLOCK || q->op == QCALL)
			continue;
		if (!q->f4)
			continue;
		if (!issym(q, 1))
			continue;
		adddef(q);
		if (ndefs > 480)
			return 0;
	}
	return 1;
}

/* bit i of the bit vector v is set (names of the three bit functions guessed) */
void setlbit(unsigned *v, int i)
{
	v[i / 16] |= 1 << (i % 16);
}

void clrlbit(unsigned *v, int i)
{
	v[i / 16] &= ~(1 << (i % 16));
}

int tstlbit(unsigned *v, int i)
{
	if ((v[i / 16] >> (i % 16)) & 1)
		return 1;
	return 0;
	i = 0;	/* dead: its store at the entry is left (the original) */
}

/* a bit vector (debug): which 1 kill, 2 gen, 3 in, 4 out; what 1 live (nlv bits), 2 reach (ndefs bits); name guessed */
void prvec(unsigned *v, int which, int what)
{
	int i, n = 0;	/* host: = 0 (the original: what the frame holds) */

	if (which == 2)
		printf("%s ", "gen ");
	else if (which == 1)
		printf("%s ", "kill");
	else if (which == 3)
		printf("%s ", "in  ");
	else if (which == 4)
		printf("%s ", "out ");
	if (what == 1) {
		printf(" (live) = ");
		n = nlv;
	} else if (what == 2) {
		printf(" (reach) = ");
		n = ndefs;
	}
	for (i = 0; i < n; i++) {
		putchar(' ');
		if (tstlbit(v, i))
			putchar('1');
		else
			putchar('0');
	}
	putchar('\n');
}

/* the bit vectors of the header quad q: kind 1 the live vectors (field 5 or 7), kind 2 the reach vectors (field 6 or 7); name from the string */
void allocvec(struct quad *q, int kind, int field)
{
	struct lrec *p;

	if (kind == 1) {
		if (ldebug > 2)
			printf("allocvec(LIVE,field=%d)\n", field);
		if (field == 5) {
			p = (struct lrec *)tabzalloc((int)sizeof(struct brec), 28, 2);
			brecinit(p);
			q->rec = p;
			p->f0 = (unsigned *)tabzalloc(lvwords * (int)sizeof(unsigned), lvwords * 2, 2);
			p->f4 = (unsigned *)tabzalloc(lvwords * (int)sizeof(unsigned), lvwords * 2, 2);
			if (pdebug) {
				printf("//inlive set, p = %X, inlive = %X\n", (unsigned)(uintptr_t)(p), (unsigned)(uintptr_t)(p->f4));
				prquad(q);
			}
		} else if (field == 7) {
			p = q->rec;
			p->f14 = (unsigned *)tabzalloc(lvwords * (int)sizeof(unsigned), lvwords * 2, 2);
			p->f10 = (unsigned *)tabzalloc(lvwords * (int)sizeof(unsigned), lvwords * 2, 2);
		}
	} else if (kind == 2) {
		p = q->rec;
		if (field == 6)
			p->f8 = (unsigned *)tabzalloc(dfwords * (int)sizeof(unsigned), dfwords * 2, 3);
		else if (field == 7) {
			p->f14 = (unsigned *)tabzalloc(dfwords * (int)sizeof(unsigned), dfwords * 2, 3);
			p->f10 = (unsigned *)tabzalloc(dfwords * (int)sizeof(unsigned), dfwords * 2, 3);
			p->f18 = (unsigned *)tabzalloc(dfwords * (int)sizeof(unsigned), dfwords * 2, 3);
			clrvec(p->f14, dfwords);
			clrvec(p->f10, dfwords);
		}
	}
}

/* the live vectors of all blocks that are in the dominator numbering (name from the string) */
void alloclivevecs(void)
{
	int i;
	struct quad *q;

	if (ldebug > 2)
		printf("alloclivevecs()\n");
	for (i = 0; i < blkno; i++) {
		q = blks[i];
		if (idfn[i] >= 0)
			allocvec(q, 1, 5);
	}
	livevecs = tabzalloc(0, 0, 2);
	for (i = 0; i < blkno; i++) {
		q = blks[i];
		if (idfn[i] >= 0)
			allocvec(q, 1, 7);
	}
}

/* the bit position of the symbol of the operand which (1: c, 2: a, 3: b) of the quad q in the live vectors; kind 4: a variable, 6: a temporary; name from the strings */
int bitpos(struct quad *q, int which, int kind)
{
	struct aln *pa;
	int t = 0;	/* host: = 0 (the original: what the frame holds) */

	switch (which) {
	case 1:
		if (ldebug > 3)
			printf("bitpos(RSLT)\n");
		t = q->c.w4;
		if (kind == 4 && q->c.b2 == OABTEMP)
			t = betatbl[t].x1;
		break;
	case 2:
		if (ldebug > 3)
			printf("bitpos(OP1)\n");
		t = q->a.w4;
		if (kind == 4 && q->a.b2 == OABTEMP)
			t = betatbl[t].x1;
		break;
	case 3:
		if (ldebug > 3)
			printf("bitpos(OP2)\n");
		if (kind == 4 && q->b.b2 == OABTEMP)
			t = betatbl[t].x1;
		t = q->b.w4;
		break;
	}
	if (kind == 4) {
		pa = checkalias(q, which);
		if (pa != 0) {
			t = altab[pa->x0];
			if (!islive(t, 1, 1))
				cerror("live var not found");
		} else if (!islive(t, 1, 0))
			cerror("live var not found");
	} else if (kind == 6) {
		if (!islive(t, 0, 0))
			cerror("live var not found");
	} else
		cerror("bitpos: illegal symbol");
	if (ldebug > 2)
		printf("bitpos=%d\n", lvidx);
	return lvidx;
}

/* the gen and kill vectors of every block from its quads (name from the string) */
void initlive(void)
{
	struct quad *q;
	int i, k;
	struct lrec *p;
	int bp;

	if (ldebug > 2)
		printf("initlive()\n");
	alloclivevecs();
	if (ldebug)
		printf("\n");
	for (i = 0; i < blkno; i++) {
		q = blks[i];
		if (ldebug > 3)
			prquad(q);
		if (idfn[i] < 0)
			continue;
		if (ldebug > 3)
			prquad(q);
		p = q->rec;
		for (q = q->next; q != 0; q = q->next) {
			if (q->op == QBBLOCK)
				break;
			if (ldebug > 3) {
				printf("initlive: processing quad:\n");
				prquad(q);
			}
			if (q->f6) {
				k = issym(q, 2);
				if (k) {
					bp = bitpos(q, 2, k);
					setlbit(p->f14, bp);
					if (k == 4 && q->a.b2 == OABTEMP && bbit(q->a.w4, 13)) {
						bp = bitpos(q, 2, 6);
						setlbit(p->f14, bp);
					}
				}
			}
			if (q->f5) {
				k = issym(q, 3);
				if (k) {
					bp = bitpos(q, 3, k);
					setlbit(p->f14, bp);
				}
			}
			if (q->f7) {
				k = issym(q, 4);
				if (k) {
					bp = bitpos(q, 1, k);
					setlbit(p->f10, bp);
					if (k == 4 && q->c.b2 == OABTEMP && bbit(q->c.w4, 13)) {
						bp = bitpos(q, 1, 6);
						setlbit(p->f10, bp);
					}
				}
			}
		}
		if (ldebug > 1) {
			printf("block=%d\n", i);
			prvec(p->f10, 1, 1);
			prvec(p->f14, 2, 1);
			printf("\n");
		}
	}
}

/* the reach vectors of all blocks that are in the dominator numbering (name from the string) */
void allocreachvecs(void)
{
	int i;
	struct quad *q;

	if (ldebug > 2)
		printf("allocreachvecs()\n");
	for (i = 0; i < blkno; i++) {
		q = blks[i];
		if (idfn[i] >= 0)
			allocvec(q, 2, 6);
	}
	livevecs = tabzalloc(0, 0, 3);
	for (i = 0; i < blkno; i++) {
		q = blks[i];
		if (idfn[i] >= 0)
			allocvec(q, 2, 7);
	}
}

/* 1 when the operand pw of the quad p and the operand qw of the quad q are the same operand (name from the strings) */
int sameopnd(struct quad *p, struct quad *q, int pw, int qw)
{
	struct aln *a1;
	int t1 = 0, t2 = 0, v1 = 0, v2 = 0;	/* host: = 0 (the original: what the frame holds) */
	char *pn = 0, *qn = 0;	/* host: = 0 (the original: what the frame holds) */

	if (ldebug > 5) {
		switch (pw) {
		case 1:
			pn = "RSLT";
			break;
		case 2:
			pn = "OP1";
			break;
		case 3:
			pn = "OP2";
			break;
		}
		switch (qw) {
		case 1:
			qn = "RSLT";
			break;
		case 2:
			qn = "OP1";
			break;
		case 3:
			qn = "OP2";
			break;
		}
		printf("// sameopnd(), pop=%s, qop=%s\n", pn, qn);
		printf("//\tp = ");
		prquad(p);
		printf("\n//\tq = ");
		prquad(q);
		printf("\n");
	}
	switch (pw) {
	case 2:
		t1 = p->a.b2;
		v1 = p->a.w4;
		break;
	case 3:
		t1 = p->b.b2;
		v1 = p->b.w4;
		break;
	case 1:
		t1 = p->c.b2;
		v1 = p->c.w4;
		break;
	}
	switch (qw) {
	case 2:
		t2 = q->a.b2;
		v2 = q->a.w4;
		break;
	case 3:
		t2 = q->b.b2;
		v2 = q->b.w4;
		break;
	case 1:
		t2 = q->c.b2;
		v2 = q->c.w4;
		break;
	}
	if (t2 == t1 && v1 == v2)
		return 1;
	if ((t1 == 4 || t1 == 8) && (t2 == 4 || t2 == 8)) {
		a1 = checkalias(q, qw);
		if (a1 != 0 && (long)(checkalias(p, pw) == a1))
			return 1;
	}
	if (ldebug > 5)
		printf("sameopnd, returning FALSE\n");
	return 0;
}

/* the definitions are linked: the ones with the same operand form a list (the low bit of the pointer in defs marks a done one); name from the string */
void linkreachdefs(void)
{
	int i, j;
	int *t;
	struct rn *r, *prev;
	struct quad *p, *q;

	if (ldebug > 3)
		printf("linkreachdefs(), reachdefno=%d\n", ndefs);
	r = reachmap = (struct rn *)tabzalloc((int)sizeof(struct rn), 6, 3);
	r->next = 0;
	for (i = 0; i < ndefs; i++) {
		if (!((uintptr_t)defs[i] & 1)) {
			r->idx = i;
			if (ldebug > 3)
				printf("\nprocessing def no %d, linking defs: \n", i);
			for (j = i; j < ndefs; j++) {
				if ((uintptr_t)defs[j] & 1)
					continue;
				if (ldebug > 5) {
					printf("// before calling sameopnd, i = %d, reachmap[i] = %X\n", i, (unsigned)(uintptr_t)(defs[i]));
					prquad(defs[i]);
				}
				if (!sameopnd(MASK(defs[i]), defs[j], 1, 1))
					continue;
				p = MASK(defs[i]);
				q = defs[j];
				if (p != q && (p->c.b2 == OABTEMP || q->c.b2 == OABTEMP))
					if (p->op != QSTORE || q->op != QSTORE)
						continue;
				if (ldebug > 3)
					printf(" %d", j);
				t = (int *)tabzalloc((int)sizeof(int), 2, 3);
				*t = j;
				defs[j] = (struct quad *)((uintptr_t)defs[j] | 1);	/* the original masks the segmented address: & 0x7F00FFFF */
				if (ldebug > 3)
					printf("r=%X, *r=%d\n", (unsigned)(uintptr_t)(t), j);
			}
		}
		defs[i] = MASK(defs[i]);
		t = (int *)tabzalloc((int)sizeof(int), 2, 3);
		*t = -1;
		prev = r;
		r = (struct rn *)tabzalloc((int)sizeof(struct rn), 6, 3);
		prev->next = r;
		if (ldebug > 3)
			printf("r=%X, *r=%d, p=%X, q=%X\n", (unsigned)(uintptr_t)(t), *t, (unsigned)(uintptr_t)(r), (unsigned)(uintptr_t)(prev));
		r->next = 0;
	}
	if (ldebug > 3)
		printf("\n");
}

/* the entry of the definition of the quad q in the reach map (mode 1: the first of the list; else the entry of q itself); name from the strings */
int *srchdef(struct quad *q, int mode)
{
	struct rn *p;
	int *pi;
	struct quad *d;

	if (ldebug > 3) {
		printf("srchdef(), quad=\n");
		prquad(q);
	}
	for (p = reachmap; p != 0; p = p->next) {
		if (ldebug > 3)
			printf("srchdef(): p is %lx\n", (unsigned long)(uintptr_t)(p));
		if (!sameopnd(q, defs[p->idx], 1, 1))
			continue;
		d = defs[p->idx];
		if (q != d && (q->c.b2 == OABTEMP || d->c.b2 == OABTEMP))
			if (q->op != QSTORE || d->op != QSTORE)
				continue;
		if (ldebug > 3) {
			printf("found:\n");
			prquad(defs[p->idx]);
		}
		if (mode == 1)
			return &p->idx;
		if (ldebug > 3)
			printf("p=%X, *pi=%d, &pi=%X\n", (unsigned)(uintptr_t)(p), p->idx, (unsigned)(uintptr_t)(&p->idx));
		for (pi = &p->idx; *++pi != -1; ) {
			if (defs[*pi] != q)
				continue;
			break;
		}
		if (defs[*pi] == q)
			return pi;
		if (ldebug)
			printf("*pi=%d, pi=%X", *pi, (unsigned)(uintptr_t)(pi));
		cerror("srchdef: def not found");
	}
	if (mode == 1) {
		p = reachmap;
		pi = &p->idx;
		do
			pi++;
		while (*pi != -1);
		pi--;
		return pi;
	}
	cerror("srchdef: variable not found");
}

/* the kill vector (field 0x10) of the block r: the definitions of the symbols that are marked (name from the string) */
void killdefs(struct lrec *r)
{
	int i;
	struct quad *q;

	for (i = 0; i < ndefs; i++) {
		q = defs[i];
		if ((q->c.b2 == OANAME && (SFISGLOBAL(symtab[q->c.w4].sflags))) ||
		    (q->c.b2 == OABTEMP && !bbit(q->c.w4, 11) && (SFISGLOBAL(symtab[betatbl[q->c.w4].x1].sflags)))) {
			setlbit(r->f10, i);
			if (ldebug)
				printf("kill, i=%d\n", i);
		}
	}
}

/* the gen and kill vectors of the reaching definitions of every block (name guessed) */
void initreach(void)
{
	struct quad *q;
	int i;
	int *pk;
	struct lrec *r;

	linkreachdefs();
	allocreachvecs();
	for (i = 0; i < blkno; i++) {
		q = blks[i];
		if (idfn[i] < 0)
			continue;
		r = q->rec;
		for (q = q->next; q != 0; q = q->next) {
			if (q->op == QBBLOCK)
				break;
			if (q->op == QCALL) {
				killdefs(r);
				continue;
			}
			if (q->f4 && issym(q, 1)) {
				pk = srchdef(q, 2);
				setlbit(r->f14, *pk);
				if (ldebug > 3)
					printf("gen, *pk =%d\n", *pk);
			}
			if (q->f7 && issym(q, 5)) {
				pk = srchdef(q, 1);
				while (*++pk != -1) {
					if (q == defs[*pk])
						continue;
					setlbit(r->f10, *pk);
					if (ldebug > 3)
						printf("kill, *pk =%d\n", *pk);
				}
			}
		}
		if (ldebug > 1) {
			printf("block=%d\n", i);
			prvec(r->f14, 2, 2);
			prvec(r->f10, 1, 2);
			printf("\n");
		}
	}
}

/* the number of words of the bit vectors: kind 1 for the live vectors, else for the reach vectors (name guessed) */
void setwords(int kind)
{
	if (kind == 1)
		lvwords = nlv / 16 + 1;
	else
		dfwords = ndefs / 16 + 1;
}

/* the vector v of n words is cleared */
void clrvec(unsigned *v, int n)
{
	int i;

	for (i = 0; i < n; i++)
		*v++ = 0;
}

/* a := b (n words) */
void copyvec(unsigned *a, unsigned *b, int n)
{
	int i;

	for (i = 0; i < n; i++)
		*a++ = *b++;
}

/* 1 when the vectors a and b (n words) are equal */
int eqvec(unsigned *a, unsigned *b, int n)
{
	int i;

	for (i = 0; i < n; i++)
		if (*a++ != *b++)
			return 0;
	return 1;
}

/* a := b and not c (n words) */
void andnotvec(unsigned *a, unsigned *b, unsigned *c, int n)
{
	int i;

	for (i = 0; i < n; i++)
		*a++ = *b++ & ~*c++;
}

/* a := a or b (n words) */
void orvec(unsigned *a, unsigned *b, int n)
{
	int i;

	for (i = 0; i < n; i++)
		*a++ |= *b++;
}

/* the vectors in, out: the copies of the gen vectors (kind 2: reach, 1: live); a work vector tmpvec (name guessed) */
void initvecs(int kind)
{
	struct quad *q;
	struct lrec *r;
	int i;

	if (kind == 2) {
		tmpv = (unsigned *)tabzalloc(dfwords * (int)sizeof(unsigned), dfwords * 2, 3);
		i = 0;
		while (i < blkno) {
			q = blks[i];
			if (idfn[i] >= 0) {
				r = q->rec;
				clrvec(r->f18, dfwords);
				copyvec(r->f8, r->f14, dfwords);
			}
			i++;
		}
	} else if (kind == 1) {
		tmpv = (unsigned *)tabzalloc(lvwords * (int)sizeof(unsigned), lvwords * 2, 2);
		for (i = 0; i < blkno; i++) {
			q = blks[i];
			if (idfn[i] >= 0) {
				r = q->rec;
				copyvec(r->f4, r->f14, lvwords);
			}
		}
	}
}

/* the confluence: tmpv is the or of the vectors of the predecessors (kind 2: reach) or of the successors (kind 1: live) of the block b; name from the string */
void confluence(int kind, int b)
{
	struct lrec *r;
	int e, pb;

	if (ldebug > 2)
		printf("confluence(block=%d)\n", b);
	if (kind == 2) {
		clrvec(tmpv, dfwords);
		e = blks[b]->a.w4;
		if (ldebug > 1)
			printf("predecessors: ");
		while (e >= 0) {
			pb = edges[e].blk;
			if (ldebug > 2)
				printf(" %d", pb);
			r = blks[pb]->rec;
			orvec(tmpv, r->f8, dfwords);
			e = edges[e].next;
		}
	} else if (kind == 1) {
		clrvec(tmpv, lvwords);
		e = blks[b]->b.w4;
		if (ldebug > 2)
			printf("successors: ");
		for (; e >= 0; e = edges[e].next) {
			pb = edges[e].blk;
			if (ldebug > 2)
				printf(" %d", pb);
			r = blks[pb]->rec;
			orvec(tmpv, r->f4, lvwords);
		}
	}
	if (ldebug > 1)
		printf("\n");
}

/* the transfer function of the block r: out = gen or (in and not kill) for the reach vectors (kind 2), in = use or (out and not def) for the live vectors (kind 1); name guessed */
void transfer(int kind, struct lrec *r)
{
	if (kind == 2) {
		andnotvec(r->f8, r->f18, r->f10, dfwords);
		orvec(r->f8, r->f14, dfwords);
	} else if (kind == 1) {
		andnotvec(r->f4, r->f0, r->f10, lvwords);
		orvec(r->f4, r->f14, lvwords);
	}
}

/* the reaching definitions: the vectors are iterated until nothing changes (name guessed) */
void solvereach(void)
{
	struct quad *q;
	struct lrec *r;
	int i, change;

	initvecs(2);
	change = 1;
	while (change) {
		change = 0;
		for (i = 0; i < blkno; i++) {
			if (idfn[i] < 0)
				continue;
			q = blks[i];
			r = (q)->rec;
			confluence(2, i);
			if (!eqvec(tmpv, r->f18, dfwords)) {
				copyvec(r->f18, tmpv, dfwords);
				transfer(2, r);
				change = 1;
			}
			if (ldebug > 1) {
				printf("block=%d, change=%s\n", i, change == 1 ? "true" : "false");
				prvec(r->f18, 3, 2);
				prvec(r->f8, 4, 2);
				printf("\n");
			}
		}
	}
}

/* the live variables: the vectors are iterated backwards until nothing changes (name guessed) */
void solvelive(void)
{
	struct quad *q;
	struct lrec *r;
	int i, change;

	initvecs(1);
	change = 1;
	while (change) {
		change = 0;
		for (i = blkno; i >= 0; i--) {
			if (idfn[i] < 0)
				continue;
			q = blks[i];
			r = (q)->rec;
			confluence(1, i);
			if (!eqvec(tmpv, r->f0, lvwords)) {
				copyvec(r->f0, tmpv, lvwords);
				transfer(1, r);
				change = 1;
			}
			if (ldebug > 1) {
				printf("block=%d, change=%s\n", i, change == 1 ? "true" : "false");
				prvec(r->f4, 3, 1);
				prvec(r->f0, 4, 1);
				printf("\n");
			}
		}
	}
}

/* the marks of the symbols of the table of the live variables are cleared again (name guessed) */
void clearflags(void)
{
	int i;

	for (i = 0; i < nlv; i++) {
		if (lvtab[i].kind == 1) {
			if (ldebug > 1)
				printf("clearing flag for symbol  %s\n", ascii + symtab[lvtab[i].var].nameoff);
			symtab[lvtab[i].var].sflags &= ~SFOPT;
		} else {
			if (ldebug > 1)
				printf("clearing flag for beta sym %s\n", ascii + symtab[betatbl[lvtab[i].var].x1].nameoff);
			symtab[betatbl[lvtab[i].var].x1].sflags &= ~SFOPT;
			betatbl[lvtab[i].var].bopt = 0;
		}
	}
}

/* the sizes of the reach vectors (debug; name guessed) */
void prreachstat(void)
{
	printf("//\n//Number of defs in reaching def. analysis = %d\n", ndefs);
	printf("//Size of reaching def vector \t\t= %d (16 bit words)\n", dfwords);
}

/* the sizes of the live vectors (debug; name guessed) */
void prlivestat(void)
{
	printf("//\n//Number of vars. used in live var. analysis = %d\n", nlv);
	printf("//Size of live variable vector \t= %d (16 bit words)\n", lvwords);
}

/* the live variable analysis of the program (name guessed) */
void livevars(void)
{
	if (ldebug)
		prqall();
	lvwords = 0;
	if (lvfirst == 0)
		tabfree(2, (char *)0);
	nlv = 0;
	lvfirst = 0;
	lvtab = (struct lv *)tabzalloc(0, 0, 2);
	lvmark = 1;
	if (!marklpvar())
		cerror("out of live var space");
	lvmark = 0;
	setwords(1);
	initlive();	/* the original passes 1, initlive() has no parameter */
	solvelive();
	if (sdebug)
		prlivestat();
	tabfree(2, livevecs);
}

/* the reaching definitions of the program (name guessed) */
void reachdefs(void)
{
	ndefs = 0;
	dfwords = 0;
	if (dffirst == 0)
		tabfree(3, (char *)0);
	dffirst = 0;
	defs = (struct quad **)tabzalloc(0, 0, 3);
	if (countreachdefs()) {
		if (ndefs > 0) {
			setwords(2);
			initreach();	/* the original passes 2, initreach() has no parameter */
			solvereach();
		}
	} else
		cerror("out of reaching def space");
	if (sdebug)
		prreachstat();
}

int lvfirst = 1, dffirst = 1;
