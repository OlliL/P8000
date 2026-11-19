/*
 * debug2.c - gopt: the names of the quad operators and the printing of the quads (debug output). Written from /lib/gopt (no source known);
 * names guessed.
 */
static char sccsid[] = "@[$]debug2.c\t1.3  02/07/83 15:51:07 Copr. 1981 87wega3.2.";
#include "gopt.h"

extern struct quad *qhead;
extern char ascii[];
extern long constbl[];
extern struct { int rv; double d; } fconsttbl[];
extern struct sym *symtab;
extern int s4d74, s4d76, s4d78;
extern void tprint(), prquad(), propnd();

char *qname[128];
int qop[128];
int qdop[128];

/* the quad operators: quad op, name, operator of the trees (the third field is the index of qop[]) */
static struct dopest qtab[] = {
	{ 1, "ADD", 6 },
	{ 2, "ADDFLD", 69 },
	{ 3, "ADDR", 16 },
	{ 4, "AND", 14 },
	{ 5, "ANDL", 23 },
	{ 6, "BLOCK", 0 },
	{ 7, "CALL", 72 },
	{ 7, "CALL", 103 },
	{ 7, "CALL", 105 },
	{ 7, "CALL", 70 },
	{ 8, "CASE", 122 },
	{ 9, "CTABLE", 0 },
	{ 10, "CASEL", 0 },
	{ 11, "CEND", 0 },
	{ 12, "COMPL", 77 },
	{ 13, "CONV", 110 },
	{ 60, "COPY", 58 },
	{ 14, "DIV", 60 },
	{ 15, "EBLOCK", 0 },
	{ 16, "ELOOP", 0 },
	{ 17, "END", 0 },
	{ 18, "ENTRY", 0 },
	{ 19, "EQ", 90 },
	{ 19, "EQ", 80 },
	{ 20, "FIELD", 109 },
	{ 21, "FREE", 0 },
	{ 22, "GE", 94 },
	{ 22, "GE", 88 },
	{ 22, "GE", 84 },
	{ 23, "GT", 95 },
	{ 23, "GT", 89 },
	{ 23, "GT", 85 },
	{ 24, "IDR", 13 },
	{ 25, "INDEX", 120 },
	{ 26, "JP", 0 },
	{ 27, "JPC", 115 },
	{ 28, "LABEL", 0 },
	{ 29, "LE", 92 },
	{ 29, "LE", 86 },
	{ 29, "LE", 82 },
	{ 30, "LOAD", 126 },
	{ 31, "LOOP", 0 },
	{ 32, "LT", 93 },
	{ 32, "LT", 87 },
	{ 32, "LT", 83 },
	{ 33, "MOVE", 101 },
	{ 34, "MULT", 11 },
	{ 35, "NE", 91 },
	{ 35, "NE", 81 },
	{ 36, "NOT", 76 },
	{ 37, "OR", 17 },
	{ 38, "ORL", 24 },
	{ 39, "REM", 62 },
	{ 40, "REGISTER", 0 },
	{ 41, "RESTORE", 0 },
	{ 42, "RET", 114 },
	{ 43, "SAVE", 0 },
	{ 44, "SHIFTL", 64 },
	{ 45, "SHIFTR", 66 },
	{ 46, "START", 0 },
	{ 47, "STORE", 9 },
	{ 47, "STORE", 7 },
	{ 47, "STORE", 12 },
	{ 47, "STORE", 65 },
	{ 47, "STORE", 67 },
	{ 47, "STORE", 63 },
	{ 47, "STORE", 61 },
	{ 47, "STORE", 18 },
	{ 47, "STORE", 15 },
	{ 47, "STORE", 20 },
	{ 47, "STORE", 58 },
	{ 48, "SUB", 8 },
	{ 49, "XOR", 19 },
	{ 50, "SPR", 0 },
	{ 51, "PARR", 0 },
	{ 52, "PARV", 56 },
	{ 52, "PARV", 102 },
	{ 53, "EPR", 0 },
	{ 54, "NOCALLS", 0 },
	{ 55, "NOOPT", 0 },
	{ 56, "OPT", 0 },
	{ 57, "SEARCH", 124 },
	{ 58, "DESC", 123 },
	{ 59, "BBLOCK", 0 },
	{ -1, "", 0 },
};

/* qname[], qop[] and qdop[] from the table */
void
mkqnames()
{
	register struct dopest *q;

	for (q = qtab; q->dopeop >= 0; q++) {
		qname[q->dopeop] = q->opst;
		qop[q->dopeval] = q->dopeop;
		qdop[q->dopeop] = q->dopeval;
	}
}

/* all quads of the list */
void
prquads()
{
	struct quad *q;

	for (q = qhead; q; q = q->next)
		prquad(q);
}

void
prquad(q)
	struct quad *q;
{
	if (q == 0) {
		printf("Null quad\n");
		return;
	}
	printf("// %8s\t", qname[q->op]);
	if (q->op == 0x3b || q->op == 6) {
		printf("%d\n", q->a.w0);
		return;
	}
	propnd(&q->a);
	propnd(&q->b);
	propnd(&q->c);
	printf("(line %d)", q->pos.line);
	printf("\n");
}

/* an operand of a quad */
void
propnd(a)
	struct qa *a;
{
	switch (a->b2) {
	case 1:
		printf("%d", a->w4);
		break;
	case 7:
		printf("%ld", constbl[a->w4]);
		break;
	case 2:
		printf("%f", fconsttbl[a->w4].d);
		break;
	case 3:
		printf("T%d", a->w4);
		break;
	case 4:
		printf("%s", ascii + symtab[a->w4].nameoff);
		break;
	case 5:
		printf("L%d", a->w4);
		break;
	case 6:
		printf("S%d", a->w4);
		break;
	case 8:
		printf("B%d", a->w4);
		break;
	default:
		if (a->b2)
			printf("badatt");
		else
			printf("noop");
	}
	putchar('(');
	tprint(a->w0, stdout);
	printf(")\t");
}

void
prstats()
{
	printf("number of labels removed = %d\n", s4d74);
	printf("number of jumps removed = %d\n", s4d76);
	printf("number of dead blocks removed = %d\n", s4d78);
}
