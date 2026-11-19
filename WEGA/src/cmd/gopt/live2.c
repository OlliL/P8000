/*
 * live2.c - gopt: the live variables. Written from /lib/gopt (no source known); names guessed.
 */
#include "gopt.h"

static char sccsid[] = "@[$]live2.c\t1.9  01/07/83 13:42:01 Copr. 1981 87wega3.2.";

extern struct sym *symtab;
extern char ascii[];
extern int cerror();
extern char *taballoc();
extern int s4d84, s715e, nlv, lvidx;
struct lv { unsigned kind:1, alias:1, var:14; };	/* an entry of the table of the live variables (guessed names) */
extern struct lv *lvtab;
struct bt { int x0; int x1; int x[2]; unsigned f15:1, f14:1, bopt:1, rest:13; };	/* an entry of betatbl: 10 bytes (guessed) */
extern struct bt *betatbl;
struct aln { int x0; struct aln *next; struct aln *link; };
extern struct aln *checkalias();
extern int almap();
extern int *altab;
extern int nptr, nadr;
extern struct quad **blks;
struct bl { struct bl *next; int blk; };
struct loop { struct bl *bls; int hdr; struct loop *link; };	/* see dom2.c */
extern struct loop *loops;
extern int *idfn;
extern int blkno;
extern struct quad *qhead;
extern int ndefs;
extern int tstlbit();
#define BOPT(j)	((*(int *)((char *)betatbl + ((j) * 10 + 8)) >> 13) & 1)
struct lrec { unsigned *f0, *f4, *f8, *fc, *f10, *f14, *f18; };	/* the bit vectors of a block: 28 bytes, stored in the header quad at 6 (guessed) */
extern int lvwords, dfwords, s4d86;
struct rn { struct rn *next; int idx; };	/* a node of the reach map: 6 bytes (guessed) */
extern struct rn *reachmap;
extern struct quad **defs;
extern int ndefs;
extern int *srchdef();
#define MASK(p)	((struct quad *)((long)(p) & ~1L))
extern void clrvec();
extern char *livevecs, *reachvecs;
extern struct quad **defs;
extern struct quad *qhead;
extern void prquad(), prqflags();

/* the flags of the quad q (debug; name guessed) */
void
prqflags(q)
	struct quad *q;
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
	if (q->c.b2 == 4 && ((symtab[q->c.w4].sflags >> 2) & 1) && q->c.fl)
		printf(" reg_candidate");
	printf("\n");
}

/* all quads with their flags (debug; name guessed) */
void
prqall()
{
	struct quad *q;

	for (q = qhead; q != 0; q = q->next) {
		prquad(q);
		prqflags(q);
	}
}

/* the variable v (kind 1: a variable, 8: beta, else a temporary; alias 1: aliased) is entered in the table of the live variables; 1 when it was in it, lvidx its index (name from the string) */
int
islive(v, kind, alias)
	int v, kind, alias;
{
	int i;

	if (s4d84 > 2)
		printf("islive(%d,isvar=%s,isalias=%s)\n", v, kind == 1 ? "ISVAR" : kind == 8 ? "BETA" : "ISTMP", alias == 1 ? "ALIASED" : "UNALIASED");
	for (i = 0; i < nlv; i++)
		if (lvtab[i].var == v && lvtab[i].kind == kind && lvtab[i].alias == alias)
			goto found;
	if (s715e) {
		if (s4d84 > 1) {
			if (kind == 1)
				printf("\t%d\t\t%s\n", i, ascii + symtab[v].nameoff);
			else
				printf("\t%d,\t\t%d\n", i, v);
		}
		taballoc(2, 2);
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
int
settmptable(q, which)
	struct quad *q;
	int which;
{
	int t;

	switch (which) {
	case 1:
		if (s4d84 > 5)
			printf("settmptable(RSLT)\n");
		t = q->c.w4;
		break;
	case 2:
		if (s4d84 > 5)
			printf("settmptable(OP1)\n");
		t = q->a.w4;
		break;
	case 3:
		if (s4d84 > 5)
			printf("settmptable(OP2)\n");
		t = q->b.w4;
		break;
	}
	if (s4d84 > 3)
		printf("settmptable(), i=%d, betatbl[i].bopt=%d\n", t, BOPT(t));
	if (!BOPT(t)) {
		betatbl[t].bopt = 1;
		if (islive(t, 0, 0))
			return 1;
		if (nlv > 480)
			return 0;
	}
	return 1;
}

/* the operand a of the quad q: its alias list is entered in the table of the live variables (cnt counts); 0 when the table is full (name from the string) */
int
insrtlive(q, cnt)
	struct quad *q;
	int *cnt;
{
	struct aln *a;
	int v;

	if (s4d84 > 3) {
		printf("insrtlive(), opndtype=OP1, quad:\n");
		prquad(q);
	}
	a = checkalias(q, 2);
	if (a == 0)
		return 1;
	if ((symtab[q->a.w4].sflags >> 2) & 1) {
		if (q->op == 0x2f && almap(&q->a, 0x2f) == 0)
			return 1;
		if (q->op == 0x18 && almap(&q->a, 0x18) == nptr)
			return 1;
		if (q->op == 3 && almap(&q->a, 3) == nadr)
			return 1;
	}
	(*cnt)++;
	v = altab[a->x0];
	if (islive(v, 1, 1))
		return 1;
	if (nlv > 480)
		return 0;
	return 1;
}

/* the operand which (1: the result, 2: the operand a, 3: the operand b) of the quad q: its symbol is entered once in the table of the live variables (name from the strings) */
int
setsymtable(q, which)
	struct quad *q;
	int which;
{
	int t;
	struct quad tq;
	int cnt;

	tq.a.b2 = 4;
	switch (which) {
	case 1:
		if (s4d84 > 3)
			printf("setsymtable(RSLT)\n");
		t = q->c.w4;
		if (q->c.b2 == 8)
			tq.a.w4 = t = betatbl[q->c.w4].x1;
		else
			tq.a = q->c;
		break;
	case 2:
		if (s4d84 > 3)
			printf("setsymtable(OP1)\n");
		t = q->a.w4;
		if (q->a.b2 == 8)
			tq.a.w4 = t = betatbl[q->a.w4].x1;
		else
			tq.a = q->a;
		break;
	case 3:
		if (s4d84 > 3)
			printf("setsymtable(OP2)\n");
		t = q->b.w4;
		if (q->b.b2 == 8)
			tq.a.w4 = t = betatbl[q->b.w4].x1;
		else
			tq.a = q->b;
		break;
	}
	if (s4d84 > 3)
		printf("setsymtable(), %s, optimize=%d\n", ascii + symtab[t].nameoff, symtab[t].sflags & 1);
	if (symtab[t].sflags & 1)
		return 1;
	symtab[t].sflags |= 1;
	tq.b.b2 = 0;
	tq.c.b2 = 0;
	tq.op = 0x2f;
	cnt = 0;
	if (!insrtlive(&tq, &cnt))
		return 0;
	tq.op = 0x18;
	if (!insrtlive(&tq, &cnt))
		return 0;
	tq.op = 3;
	if (!insrtlive(&tq, &cnt))
		return 0;
	if (cnt < 3 && !islive(t, 1, 0) && nlv > 480)
		return 0;
	return 1;
}

/* the symbols of the operand flag (1: the result, 2: the operand a, 3: the operand b) of the quad q are entered (name from the string); 0 when the table is full */
int
setsym(q, flag)
	struct quad *q;
	int flag;
{
	int x;

	switch (flag) {
	case 1:
		if (s4d84 > 3)
			printf("setsym(GENRBIT)\n");
		if (q->c.b2 == 4 || q->c.b2 == 8)
			if (!setsymtable(q, 1))
				return 0;
		if (q->c.b2 == 6 || q->c.b2 == 8)
			if (!settmptable(q, 1))
				return 0;
		return 1;
	case 2:
		if (s4d84 > 3)
			printf("setsym(GENL1BIT)\n");
		if (q->a.b2 == 4 || q->a.b2 == 8)
			if (!setsymtable(q, 2))
				return 0;
		if (q->a.b2 == 6 || q->a.b2 == 8)
			if (!settmptable(q, 2))
				return 0;
		return 1;
	case 3:
		if (s4d84 > 3)
			printf("setsym(GENL2BIT)\n");
		if (q->b.b2 == 4 || q->b.b2 == 8)
			if (!setsymtable(q, 3))
				return 0;
		if (q->b.b2 == 6 || q->b.b2 == 8)
			if (!settmptable(q, 3))
				return 0;
		return 1;
	default:
		cerror("setsym: illegal flag");
		return 1;
	}
}

/* the symbols of the quads of the block b are entered (name from the string); 0 when the table is full */
int
markblkvar(b)
	int b;
{
	struct quad *q;

	if (s4d84 > 2)
		printf("markblkvar(blkno=%d)\n", b);
	q = blks[b]->next;
	if (s4d84 > 2)
		prquad(q);
	for (; q != 0; q = q->next) {
		if (q->op == 0x3b)
			break;
		if (s4d84 > 2)
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
	if (s4d84 > 2)
		printf("markblkvar: returning TRUE\n");
	return 1;
}

/* the symbols of all blocks that are in the dominator numbering (name guessed) */
int
markall()
{
	int i;

	for (i = 0; i < blkno; i++)
		if (idfn[i] >= 0)
			if (!markblkvar(i))
				return 0;
}

/* the symbols of the blocks of the loops (all blocks when there is no loop); name from the string */
int
marklpvar()
{
	struct loop *p;
	struct bl *q;

	if (s4d84 > 1) {
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
int
issym(q, flag)
	struct quad *q;
	int flag;
{
	int r, j;

	r = 0;
	switch (flag) {
	case 1:
	case 4:
	case 5:
		j = q->c.w4;
		if (q->c.b2 == 4) {
			if (symtab[j].sflags & 1)
				r = 4;
		} else if (q->c.b2 == 6) {
			if (BOPT(j))
				r = 6;
		} else if (q->c.b2 == 8) {
			if (q->op == 0x2f) {
				if (symtab[betatbl[j].x1].sflags & 1)
					r = 4;
			} else if (BOPT(j))
				r = 6;
		}
		break;
	case 2:
		j = q->a.w4;
		if (q->a.b2 == 4) {
			if (symtab[j].sflags & 1)
				r = 4;
		} else if (q->a.b2 == 6) {
			if (BOPT(j))
				r = 6;
		} else if (q->a.b2 == 8) {
			if (q->op == 0x18 || q->op == 0x1e) {
				if (symtab[betatbl[j].x1].sflags & 1)
					r = 4;
			} else if (BOPT(j))
				r = 6;
		}
		break;
	case 3:
		j = q->b.w4;
		if (q->b.b2 == 4) {
			if (symtab[j].sflags & 1)
				r = 4;
		} else if (q->b.b2 == 6 || q->b.b2 == 8) {
			if (BOPT(j))
				r = 6;
		}
		break;
	default:
		cerror("setsym: illegal flag");
	}
	if (s4d84 > 3) {
		printf("// issym : q = %X, bit = %d, i=%d, j=%d\n", q, flag, r, j);
		prquad(q);
	}
	return r;
}

/* the quad q is entered in the table of the definitions (name guessed) */
void
adddef(q)
	struct quad *q;
{
	if (s4d84 > 1) {
		printf("defno=%d\n", ndefs);
		prquad(q);
	}
	taballoc(4, 3);
	defs[ndefs] = q;
	ndefs++;
}

/* the definitions that reach: the quads that set a marked symbol are entered (name from the string); 0 when the table is full */
int
countreachdefs()
{
	struct quad *q;
	int x, y;

	if (s4d84 > 2)
		printf("countreachdefs()\n");
	for (q = qhead; q != 0; q = q->next) {
		if (q->op == 0x3b || q->op == 7)
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
void
setlbit(v, i)
	unsigned *v;
	int i;
{
	v[i / 16] |= 1 << (i % 16);
}

void
clrlbit(v, i)
	unsigned *v;
	int i;
{
	v[i / 16] &= ~(1 << (i % 16));
}

int
tstlbit(v, i)
	unsigned *v;
	int i;
{
	if ((v[i / 16] >> (i % 16)) & 1)
		return 1;
	return 0;
}

/* a bit vector (debug): which 1 kill, 2 gen, 3 in, 4 out; what 1 live (nlv bits), 2 reach (ndefs bits); name guessed */
void
prvec(v, which, what)
	unsigned *v;
	int which, what;
{
	int i, n;

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
void
allocvec(q, kind, field)
	struct quad *q;
	int kind, field;
{
	struct lrec *p;

	if (kind == 1) {
		if (s4d84 > 1)
			printf("allocvec(LIVE,field=%d)\n", field);
		if (field == 5) {
			p = (struct lrec *)taballoc(28, 2);
			*(struct lrec **)&q->c = p;
			p->f0 = (unsigned *)taballoc(lvwords * 2, 2);
			p->f4 = (unsigned *)taballoc(lvwords * 2, 2);
			if (s4d86) {
				printf("//inlive set, p = %X, inlive = %X\n", p, p->f4);
				prquad(q);
			}
		} else if (field == 7) {
			p = *(struct lrec **)&q->c;
			p->f14 = (unsigned *)taballoc(lvwords * 2, 2);
			p->f10 = (unsigned *)taballoc(lvwords * 2, 2);
		}
	} else if (kind == 2) {
		p = *(struct lrec **)&q->c;
		if (field == 6)
			p->f8 = (unsigned *)taballoc(dfwords * 2, 3);
		else if (field == 7) {
			p->f14 = (unsigned *)taballoc(dfwords * 2, 3);
			p->f10 = (unsigned *)taballoc(dfwords * 2, 3);
			p->f18 = (unsigned *)taballoc(dfwords * 2, 3);
			clrvec(p->f14, dfwords);
			clrvec(p->f10, dfwords);
		}
	}
}

/* the live vectors of all blocks that are in the dominator numbering (name from the string) */
void
alloclivevecs()
{
	int i;
	struct quad *q;

	if (s4d84 > 2)
		printf("alloclivevecs()\n");
	for (i = 0; i < blkno; i++) {
		q = blks[i];
		if (idfn[i] >= 0)
			allocvec(q, 1, 5);
	}
	livevecs = taballoc(0, 2);
	for (i = 0; i < blkno; i++) {
		q = blks[i];
		if (idfn[i] >= 0)
			allocvec(q, 1, 7);
	}
}

/* the bit position of the symbol of the operand which (1: c, 2: a, 3: b) of the quad q in the live vectors; kind 4: a variable, 6: a temporary; name from the strings */
int
bitpos(q, which, kind)
	struct quad *q;
	int which, kind;
{
	struct aln *pa;
	int t;

	switch (which) {
	case 1:
		if (s4d84 > 3)
			printf("bitpos(RSLT)\n");
		t = q->c.w4;
		if (kind == 4 && q->c.b2 == 8)
			t = betatbl[t].x1;
		break;
	case 2:
		if (s4d84 > 3)
			printf("bitpos(OP1)\n");
		t = q->a.w4;
		if (kind == 4 && q->a.b2 == 8)
			t = betatbl[t].x1;
		break;
	case 3:
		if (s4d84 > 3)
			printf("bitpos(OP2)\n");
		if (kind == 4 && q->b.b2 == 8)
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
	if (s4d84 > 2)
		printf("bitpos=%d\n", lvidx);
	return lvidx;
}

/* the gen and kill vectors of every block from its quads (name from the string) */
void
initlive()
{
	struct quad *q;
	int i, k;
	struct lrec *p;
	int bp;

	if (s4d84 > 2)
		printf("initlive()\n");
	alloclivevecs();
	if (s4d84)
		printf("\n");
	for (i = 0; i < blkno; i++) {
		q = blks[i];
		if (s4d84 > 3)
			prquad(q);
		if (idfn[i] < 0)
			continue;
		if (s4d84 > 3)
			prquad(q);
		p = *(struct lrec **)&q->c;
		for (q = q->next; q != 0; q = q->next) {
			if (q->op == 0x3b)
				break;
			if (s4d84 > 3) {
				printf("initlive: processing quad:\n");
				prquad(q);
			}
			if (q->f6) {
				k = issym(q, 2);
				if (k) {
					bp = bitpos(q, 2, k);
					setlbit(p->f14, bp);
					if (k == 4 && q->a.b2 == 8 && BOPT(q->a.w4)) {
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
					if (k == 4 && q->c.b2 == 8 && BOPT(q->c.w4)) {
						bp = bitpos(q, 1, 6);
						setlbit(p->f10, bp);
					}
				}
			}
		}
		if (s4d84 > 1) {
			printf("block=%d\n", i);
			prvec(p->f10, 1, 1);
			prvec(p->f14, 2, 1);
			printf("\n");
		}
	}
}

/* the reach vectors of all blocks that are in the dominator numbering (name from the string) */
void
allocreachvecs()
{
	int i;
	struct quad *q;

	if (s4d84 > 2)
		printf("allocreachvecs()\n");
	for (i = 0; i < blkno; i++) {
		q = blks[i];
		if (idfn[i] >= 0)
			allocvec(q, 2, 6);
	}
	reachvecs = taballoc(0, 3);
	for (i = 0; i < blkno; i++) {
		q = blks[i];
		if (idfn[i] >= 0)
			allocvec(q, 2, 7);
	}
}

/* 1 when the operand pw of the quad p and the operand qw of the quad q are the same operand (name from the strings) */
int
sameopnd(p, q, pw, qw)
	struct quad *p, *q;
	int pw, qw;
{
	struct aln *a1;
	int t1, t2, v1, v2;
	char *pn, *qn;

	if (s4d84 > 5) {
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
	if (s4d84 > 5)
		printf("sameopnd, returning FALSE\n");
	return 0;
}

/* the number of words of the bit vectors: kind 1 for the live vectors, else for the reach vectors (name guessed) */
void
setwords(kind)
	int kind;
{
	if (kind == 1)
		lvwords = nlv / 16 + 1;
	else
		dfwords = ndefs / 16 + 1;
}

/* the vector v of n words is cleared */
void
clrvec(v, n)
	unsigned *v;
	int n;
{
	int i;

	for (i = 0; i < n; i++)
		*v++ = 0;
}

/* a := b (n words) */
void
copyvec(a, b, n)
	unsigned *a, *b;
	int n;
{
	int i;

	for (i = 0; i < n; i++)
		*a++ = *b++;
}

/* 1 when the vectors a and b (n words) are equal */
int
eqvec(a, b, n)
	unsigned *a, *b;
	int n;
{
	int i;

	for (i = 0; i < n; i++)
		if (*a++ != *b++)
			return 0;
	return 1;
}

/* a := b and not c (n words) */
void
andnotvec(a, b, c, n)
	unsigned *a, *b, *c;
	int n;
{
	int i;

	for (i = 0; i < n; i++)
		*a++ = *b++ & ~*c++;
}

/* a := a or b (n words) */
void
orvec(a, b, n)
	unsigned *a, *b;
	int n;
{
	int i;

	for (i = 0; i < n; i++)
		*a++ |= *b++;
}

/* the definitions are linked: the ones with the same operand form a list (the low bit of the pointer in defs marks a done one); name from the string */
void
linkreachdefs()
{
	int i, j;
	int *t;
	struct rn *r, *prev;
	struct quad *p, *q;

	if (s4d84 > 3)
		printf("linkreachdefs(), reachdefno=%d\n", ndefs);
	r = reachmap = (struct rn *)taballoc(6, 3);
	r->next = 0;
	for (i = 0; i < ndefs; i++) {
		if (!((long)defs[i] & 1)) {
			r->idx = i;
			if (s4d84 > 3)
				printf("\nprocessing def no %d, linking defs: \n", i);
			for (j = i; j < ndefs; j++) {
				if ((long)defs[j] & 1)
					continue;
				if (s4d84 > 5) {
					printf("// before calling sameopnd, i = %d, reachmap[i] = ", i, defs[i]);
					prquad(defs[i]);
				}
				if (!sameopnd(MASK(defs[i]), defs[j], 1, 1))
					continue;
				p = MASK(defs[i]);
				q = defs[j];
				if (p != q && (p->c.b2 == 8 || q->c.b2 == 8))
					if (p->op != 0x2f || q->op != 0x2f)
						continue;
				if (s4d84 > 3)
					printf(" %d", j);
				t = (int *)taballoc(2, 3);
				*t = j;
				defs[j] = (struct quad *)(((long)defs[j] | 1L) & 0x7F00FFFFL);
				if (s4d84 > 3)
					printf("r=%X, *r=%d\n", t, j);
			}
		}
		defs[i] = MASK(defs[i]);
		t = (int *)taballoc(2, 3);
		*t = -1;
		prev = r;
		r = (struct rn *)taballoc(6, 3);
		prev->next = r;
		if (s4d84 > 3)
			printf("r=%X, *r=%d, p=%X, q=%X\n", t, *t, r, prev);
		r->next = 0;
	}
	if (s4d84 > 3)
		printf("\n");
}

/* the entry of the definition of the quad q in the reach map (mode 1: the first of the list; else the entry of q itself); name from the strings */
int *
srchdef(q, mode)
	struct quad *q;
	int mode;
{
	struct rn *p;
	int *pi;
	struct quad *d;

	if (s4d84 > 3) {
		printf("srchdef(), quad=\n");
		prquad(q);
	}
	for (p = reachmap; p != 0; p = p->next) {
		if (s4d84 > 3)
			printf("srchdef(): p is %lx\n", p);
		if (!sameopnd(q, defs[p->idx], 1, 1))
			continue;
		d = defs[p->idx];
		if (q != d && (q->c.b2 == 8 || d->c.b2 == 8))
			if (q->op != 0x2f || d->op != 0x2f)
				continue;
		if (s4d84 > 3) {
			printf("found:\n");
			prquad(defs[p->idx]);
		}
		if (mode == 1)
			return &p->idx;
		if (s4d84 > 3)
			printf("p=%X, *pi=%d, &pi=%X\n", p, p->idx, &p->idx);
		for (pi = &p->idx; *++pi != -1; )
			if (defs[*pi] == q)
				break;
		if (defs[*pi] == q)
			return pi;
		if (s4d84)
			printf("*pi=%d, pi=%X", *pi, pi);
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
