/*
 * debug2.c - gopt: the names of the quad operators and the printing of the quads (debug output). Written from /lib/gopt (no source known);
 * names guessed.
 */
static char sccsid[] __attribute__((unused)) = "@[$]debug2.c\t1.3  02/07/83 15:51:07 Copr. 1981 87wega3.2.";
#include "mfile2.h"

char *qname[128];
int qop[128];
int qdop[128];

/* the quad operators: quad op, name, operator of the trees (the third field is the index of qop[]) */
struct dopest { int dopeop; char opst[8]; int dopeval; };	/* 12 bytes, as indope of common */

static struct dopest qtab[] = {
	{ QADD, "ADD", PLUS },
	{ QADDFLD, "ADDFLD", STREF },
	{ QADDR, "ADDR", UNARY AND },
	{ QAND, "AND", AND },
	{ QANDL, "ANDL", ANDAND },
	{ QBLOCK, "BLOCK", 0 },
	{ QCALL, "CALL", UNARY CALL },
	{ QCALL, "CALL", STCALL },
	{ QCALL, "CALL", USTCALL },
	{ QCALL, "CALL", CALL },
	{ QCASE, "CASE", SWTCH },
	{ QCTABLE, "CTABLE", 0 },
	{ QCASEL, "CASEL", 0 },
	{ QCEND, "CEND", 0 },
	{ QCOMPL, "COMPL", COMPL },
	{ QCONV, "CONV", SCONV },
	{ QCOPY, "COPY", ASSIGN },
	{ QDIV, "DIV", DIV },
	{ QEBLOCK, "EBLOCK", 0 },
	{ QELOOP, "ELOOP", 0 },
	{ QEND, "END", 0 },
	{ QENTRY, "ENTRY", 0 },
	{ QEQ, "EQ", FEQ },
	{ QEQ, "EQ", EQ },
	{ QFIELD, "FIELD", FLD },
	{ QFREE, "FREE", 0 },
	{ QGE, "GE", FGE },
	{ QGE, "GE", UGE },
	{ QGE, "GE", GE },
	{ QGT, "GT", FGT },
	{ QGT, "GT", UGT },
	{ QGT, "GT", GT },
	{ QIDR, "IDR", UNARY MUL },
	{ QINDEX, "INDEX", SUBSR },
	{ QJP, "JP", 0 },
	{ QJPC, "JPC", CBRANCH },
	{ QLABEL, "LABEL", 0 },
	{ QLE, "LE", FLE },
	{ QLE, "LE", ULE },
	{ QLE, "LE", LE },
	{ QLOAD, "LOAD", 126 },
	{ QLOOP, "LOOP", 0 },
	{ QLT, "LT", FLT },
	{ QLT, "LT", ULT },
	{ QLT, "LT", LT },
	{ QMOVE, "MOVE", STASG },
	{ QMULT, "MULT", MUL },
	{ QNE, "NE", FNE },
	{ QNE, "NE", NE },
	{ QNOT, "NOT", NOT },
	{ QOR, "OR", OR },
	{ QORL, "ORL", OROR },
	{ QREM, "REM", MOD },
	{ QREGISTER, "REGISTER", 0 },
	{ QRESTORE, "RESTORE", 0 },
	{ QRET, "RET", RET },
	{ QSAVE, "SAVE", 0 },
	{ QSHIFTL, "SHIFTL", LS },
	{ QSHIFTR, "SHIFTR", RS },
	{ QSTART, "START", 0 },
	{ QSTORE, "STORE", ASG MINUS },
	{ QSTORE, "STORE", ASG PLUS },
	{ QSTORE, "STORE", ASG MUL },
	{ QSTORE, "STORE", ASG LS },
	{ QSTORE, "STORE", ASG RS },
	{ QSTORE, "STORE", ASG MOD },
	{ QSTORE, "STORE", ASG DIV },
	{ QSTORE, "STORE", ASG OR },
	{ QSTORE, "STORE", ASG AND },
	{ QSTORE, "STORE", ASG ER },
	{ QSTORE, "STORE", ASSIGN },
	{ QSUB, "SUB", MINUS },
	{ QXOR, "XOR", ER },
	{ QSPR, "SPR", 0 },
	{ QPARR, "PARR", 0 },
	{ QPARV, "PARV", CM },
	{ QPARV, "PARV", STARG },
	{ QEPR, "EPR", 0 },
	{ QNOCALLS, "NOCALLS", 0 },
	{ QNOOPT, "NOOPT", 0 },
	{ QOPT, "OPT", 0 },
	{ QSEARCH, "SEARCH", SEARCH },
	{ QDESC, "DESC", DESC },
	{ QBBLOCK, "BBLOCK", 0 },
	{ -1, "", 0 },
};

/* qname[], qop[] and qdop[] from the table */
void mkqnames(void)
{
	register struct dopest *q;

	for (q = qtab; q->dopeop >= 0; q++) {
		qname[q->dopeop] = q->opst;
		qop[q->dopeval] = q->dopeop;
		qdop[q->dopeop] = q->dopeval;
	}
}

/* all quads of the list */
void prquads(void)
{
	struct quad *q;

	for (q = qhead; q; q = q->next)
		prquad(q);
}

void prquad(struct quad *q)
{
	if (q == 0) {
		printf("Null quad\n");
		return;
	}
	printf("// %8s\t", qname[q->op]);
	if (q->op == QBBLOCK || q->op == QBLOCK) {
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
void propnd(struct qa *a)
{
	switch (a->b2) {
	case OAICON:
		printf("%d", a->w4);
		break;
	case OALCON:
		printf("%ld", constbl[a->w4]);
		break;
	case OAFCON:
		printf("%f", fcget(a->w4));
		break;
	case OATEMP:
		printf("T%d", a->w4);
		break;
	case OANAME:
		printf("%s", ascii + symtab[a->w4].nameoff);
		break;
	case OALABEL:
		printf("L%d", a->w4);
		break;
	case OASTEMP:
		printf("S%d", a->w4);
		break;
	case OABTEMP:
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

void prstats(void)
{
	printf("number of labels removed = %d\n", nlabrm);
	printf("number of jumps removed = %d\n", njmprm);
	printf("number of dead blocks removed = %d\n", ndeadrm);
}
