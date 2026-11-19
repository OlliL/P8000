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
struct bt { int x[4]; unsigned f15:1, f14:1, bopt:1, rest:13; };	/* an entry of betatbl: 10 bytes (guessed) */
extern struct bt *betatbl;
struct aln { int x0; struct aln *next; struct aln *link; };
extern struct aln *checkalias();
extern int almap();
extern int *altab;
extern int nptr, nadr;
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
