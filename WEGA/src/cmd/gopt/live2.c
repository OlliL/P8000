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
		printf("settmptable(), i=%d, betatbl[i].bopt=%d\n", t, betatbl[t].bopt);
	while (!betatbl[t].bopt) {
		betatbl[t].bopt = 1;
		if (islive(t, 0, 0))
			return 1;
		if (nlv > 480)
			return 0;
		return 1;
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
