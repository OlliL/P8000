/*
 * wrtil2.c - gopt: the quads are made into trees again and written as the intermediate language. Written from /lib/gopt (no source
 * known); names guessed, the callees of other modules by the address (fnXXXX).
 */
#include "gopt.h"

static char sccsid[] = "@[$]wrtil2.c\t1.7  03/04/83 16:06:00 Copr. 1981 87wega3.2.";

#define INCREF(t)	((((t) & ~0xF) << 2) | 0x10 | ((t) & 0xF))

extern NODE *talloc();
extern int qflag, ntl;
extern int tl[];
extern char ctpos[], ctneg[];
extern int ifline;
extern char ifname[];
extern int *itab;
extern long constbl[];
extern struct { int rv; int d[4]; } fconsttbl[];
extern struct { int p0; int p2; int x4; int p6, p8; } *ptbl9c;
extern FILE *dfile;
extern NODE node[];
extern struct sym *symtab;
extern int szty();
extern long ftell();

static int flab = 9000;
static int tmpoff = 0;
static int ftot = 0;
static int fflag = 1;
extern int nitab;

extern int cerror();
static int treeeq();
static void wrtree();

/* a name that is not a pointer: its address is taken */
void
addrof(p)
	NODE *p;
{
	NODE *q;

	if (p->in.left->in.op == NAME && (p->in.left->in.type & 0x30) != 0x10) {
		q = talloc();
		q->in.rall = 0;
		q->in.op = UAND;
		q->in.left = p->in.left;
		q->in.type = INCREF(p->in.left->in.type);
		p->in.left = q;
	}
}

/* does the tree below p need a star */
int
needustar(p)
	NODE *p;
{
	NODE *q;

	if (p->in.left->in.op != TEMP) {
		for (q = p->in.left; q->in.op == STAR; q = q->in.left)
			;
		if (qflag > 4)
			printf("// needustar: q->su = %d\n", q->tn.w6);
		if (q->in.op == TEMP && q->tn.w6 == 0)
			return 1;
		return !(optype(q->in.op) == LTYPE || q->in.op == FLD);
	}
	return 1;
}

/* is the operand in the list of the temporaries that have to be kept */
int
tlfind(a)
	struct qa *a;
{
	int i;

	for (i = 0; i < ntl; i++)
		if (a->w4 == tl[i])
			return 1;
	return 0;
}

/* the line and the tree */
void
wrline(p)
	NODE *p;
{
	if (p->in.op != FREE) {
		putchar('.');
		printf("%d\t%s\n", ifline, ifname);
		wrtree(p);
	}
}

/* are the two trees equal */
static int
treeeq(p, q)
	register NODE *p, *q;
{
	register int op;

	op = p->in.op;
	if (op != q->in.op)
		return 0;
	switch (optype(op)) {
	case LTYPE:
		return p->tn.rval == q->tn.rval && (p->in.op == TEMP || p->tn.lval == q->tn.lval);
	case BITYPE:
		if (!treeeq(p->in.right, q->in.right))
			return 0;
	case UTYPE:
		return treeeq(p->in.left, q->in.left);
	}
}

/* x = x op y becomes x op= y */
void
asgfold(pp)
	NODE **pp;
{
	int op;
	NODE *l, *rl, *p;

	p = *pp;
	op = p->in.right->in.op;
	l = p->in.left;
	if (op == 6 || op == 8 || op == 0xb || op == 0x3c) {
		rl = p->in.right->in.left;
		if (treeeq(l, rl)) {
			p->in.op = FREE;
			p->in.left->in.op = FREE;
			p = p->in.right;
			p->in.op = p->in.op + 1;
			*pp = p;
		}
	}
}

/* the counts of the temporaries (debug) */
void
prtcounts()
{
	int i;

	printf("// TEMP COUNTS: temp number, temp count, stmp count\n\n");
	for (i = 0; i < 1000; i++)
		if (ctpos[i] || ctneg[i])
			printf("// (%d)\t%d\t%d\n", i, ctpos[i], ctneg[i]);
}

int
itfind(a)
	struct qa *a;
{
	int i;

	for (i = 1; i < nitab; i++)
		if (itab[i] == a->w4)
			return i;
	return 0;
}

/* the leaf node of an operand of a quad */
NODE *
mkleaf(a)
	struct qa *a;
{
	int i;
	NODE *q, *r;
	int *d;
	long pos1, pos2, pos3;

	switch (a->b2) {
	case 4:
		q = talloc();
		q->in.rall = 0;
		q->in.op = NAME;
		q->in.sname = symtab + a->w4;
		q->in.type = a->w0;
		q->tn.lval = 0;
		q->tn.rval = a->w4;
		break;
	case 5:
		q = talloc();
		q->in.rall = 0;
		q->in.op = NAME;
		q->in.type = a->w0;
		q->tn.rval = -a->w4;
		q->tn.lval = 0;
		break;
	case 1:
		q = talloc();
		q->in.rall = 0;
		q->in.op = ICON;
		q->in.type = a->w0;
		q->tn.lval = a->w4;
		break;
	case 7:
		q = talloc();
		q->in.rall = 0;
		q->in.op = ICON;
		q->in.type = a->w0;
		q->tn.lval = constbl[a->w4];
		break;
	case 2:
		q = talloc();
		q->in.rall = 0;
		q->in.op = FCON;
		q->in.type = a->w0;
		if (fconsttbl[a->w4].rv == 0) {
			d = fconsttbl[a->w4].d;
			fseek(dfile, 0L, 2);
			if (fflag) {
				pos1 = ftell(dfile);
				fprintf(dfile, "0000000\t\n");
				fflag = 0;
			}
			pos2 = ftell(dfile);
			fprintf(dfile, "\tINTERNAL\nL%d:\n\t\tWORD\t:= %%%x\n\t\tWORD\t:= %%%x\n\t\tWORD\t:= %%%x\n\t\tWORD\t:= %%%x\n", flab, d[0], d[1], d[2], d[3]);
			pos3 = ftell(dfile);
			fseek(dfile, pos1, 0);
			ftot += pos3 - pos2;
			fprintf(dfile, "%07.7d", ftot);
			fconsttbl[a->w4].rv = -flab;
			flab++;
		}
		q->tn.rval = fconsttbl[a->w4].rv;
		break;
	case 6:
		q = talloc();
		q->in.rall = 0;
		q->in.op = TEMP;
		q->in.type = a->w0;
		q->tn.rval = a->w4;
		q->tn.lval = --ctneg[a->w4];
		if (tlfind(a))
			q->tn.lval = -q->tn.lval;
		q->tn.temp = ptbl9c[a->w4].p6;
		break;
	case 3:
		for (r = node; r <= &node[NTREE - 1]; ++r) {
			if (r->in.rall == a->w4 && r->in.op != FREE) {
				if (qflag > 4)
					printf("matched temp %d with op %d\n", a->w4, r->in.op);
				*(ctpos + a->w4) -= 1;
				if (r->in.op == TEMP) {
					q = talloc();
					*q = *r;
					q->tn.lval = ctpos[a->w4];
					r->tn.lval = ctpos[a->w4];
					r = q;
				} else
					r->in.rall = 0;
				return r;
			}
		}
		q = talloc();
		q->in.rall = a->w4;
		q->in.op = TEMP;
		q->in.type = a->w0;
		q->tn.rval = a->w4;
		q->tn.lval = --ctpos[a->w4];
		if (qflag > 2)
			printf("// assigning T%d to offset %d\n", a->w4, tmpoff);
		q->tn.temp = tmpoff;
		tmpoff += szty(q->in.type) * 2;
		break;
	default:
		cerror("cannot recreate tree (%d, %d)", a->b2, a->w4);
	}
	return q;
}

extern void tfree();

/* the trees that hold a temporary with a count are written (assignments to the temporaries), the others are checked */
void
cleartrees()
{
	NODE *p, *q, *n1, *n2;

	for (p = node; p <= &node[NTREE - 1]; ++p) {
		if (p->in.op != FREE && p->in.op != TEMP && p->in.rall > 0 && ctpos[p->in.rall] > 0) {
			if (qflag > 2)
				printf("cleartrees: emitting tree\n");
			n1 = talloc();
			n1->in.rall = 0;
			n1->in.op = ASSIGN;
			n1->in.type = p->in.type;
			n1->in.right = p;
			n2 = talloc();
			n2->in.rall = 0;
			n2->in.op = TEMP;
			n2->in.type = n1->in.type;
			n2->tn.rval = p->in.rall;
			n2->tn.lval = ctpos[p->in.rall];
			n1->in.left = n2;
			for (q = node; q <= &node[NTREE - 1]; ++q) {
				if (q->in.op == TEMP && p->in.rall == q->in.rall) {
					n2->tn.temp = q->tn.temp;
					break;
				}
			}
			if (q > &node[NTREE - 1]) {
				if (qflag > 2)
					printf("// assigning T%d to offset %d\n", p->in.rall, tmpoff);
				n2->tn.temp = tmpoff;
				tmpoff += szty(n2->in.type) * 2;
			}
			n2->in.rall = p->in.rall;
			wrline(n1);
			tfree(n1->in.right);
			n1->in.op = FREE;
		}
	}
	for (p = node; p <= &node[NTREE - 1]; ++p)
		if (p->in.op == TEMP && p->in.rall == 0)
			p->in.op == FREE;
}

/* the tree in the form of the intermediate language */
static void
wrtree(p)
	register NODE *p;
{
	register int spare1, spare2;	/* unused */
	register int kind;

	kind = optype(p->in.op);
	printf("%d\t", p->in.op);
	if (kind == LTYPE) {
		printf("%ld", p->tn.lval);
		putchar('\t');
	}
	if (kind != BITYPE) {
		if (p->in.op == ICON)
			printf("0\t");
		else if (p->in.op == NAME && p->tn.rval >= 0)
			printf("%d\t", (p->in.sname->sflags >> 4) & 0xFFF);
		else if (p->in.op == TEMP)
			printf("%d\t", p->tn.temp);
		else
			printf("%d\t", p->tn.rval);
	}
	if (p->in.op == TEMP)
		printf("%d\t", p->tn.temp);
	printf("%0.2o\t", p->in.type);
	switch (p->in.op) {
	case STCALL:
	case USTCALL:
		printf("%ld", p->in.left->in.sname->ssize);
		printf("\t%d\t\n", p->tn.w6 << 3);
		break;
	case STASG:
	case STARG:
		printf("%d", p->stn.stsize << 3);
		printf("\t%d\t\n", p->stn.stalign << 3);
		break;
	default:
		putchar('\n');
	}
	if (kind != LTYPE)
		wrtree(p->in.left);
	if (kind == BITYPE)
		wrtree(p->in.right);
}
