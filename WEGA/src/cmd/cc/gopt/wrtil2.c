/*
 * wrtil2.c - gopt: the quads are made into trees again and written as the intermediate language. Written from /lib/gopt (no source
 * known); names guessed, the callees of other modules by the address (fnXXXX).
 */
static char sccsid[] = "@[$]wrtil2.c\t1.7  03/04/83 16:06:00 Copr. 1981 87wega3.2.";
#include "mfile2.h"

extern struct { int rv; int d[4]; } fconsttbl[];
extern struct { int p0; int p2; int x4; int p6, p8; } *betatbl;

static NODE *pend = 0;
static int flab = 9000;
static int tmpoff = 0;

static int treeeq();
static void wrtree();

extern void tfree();
extern int qdop[];
extern NODE *mkleaf();
extern void addrof(), asgfold(), cleartrees();
extern struct quad *qhead;
extern struct { int next; int val; } *edges;
extern int **blks;
extern void prtcounts();
extern void cleartrees(), mktree();
/* forward declarations (the functions are in the order of the original) */
int tlfind();
void wrline();

/* the quads are written as trees in the intermediate language (names guessed, written from /lib/gopt) */
void
writeil()
{
	struct quad *q;
	NODE *p, *u, *v;
	int i;
	long pos;
	struct sym *regsym[12];

	if (qflag > 1) {
		printf("// writeil : quads to be converted to trees - \n");
		prquads();
		prtcounts();
	}
	for (i = 0; i <= 11; i++)
		regsym[i] = 0;
	tmpoff = 0;
	for (q = qhead; q; q = q->next) {
		lineno = q->pos.line;
		switch (q->op) {
		case QSTART:
			printf("{%d\t%ld\t", nlabsave, frame0);
			pos = ftell(stdout);
			printf("0000\t%d\t\n", nparm);
			wrline(curproc);
			tfree(curproc);
			continue;
		case QEND:
			for (i = 1; i <= 11; i++)
				if (regsym[i] && ((SFISGLOBAL(regsym[i]->sflags)) || regsym[i]->sclass == STATIC))
					printf("S%d\t\n", SFSYMNO(regsym[i]->sflags));
			printf("}\n");
			fseek(stdout, pos, 0);
			printf("%04.4d", tmpoff);
			fseek(stdout, 0L, 2);
			continue;
		case QBLOCK:
			cleartrees();
			printf("[%ld\t%d\t%d\t\n", constbl[q->a.w4], q->b.w4, q->c.w4);
			continue;
		case QEBLOCK:
			printf("]\n");
			continue;
		case QBBLOCK:
			cleartrees();
			printf("#%d\t", q->a.w0);
			for (i = q->a.w4; i >= 0; i = edges[i].next) {
				printf("%d\t", edges[i].val);
				deledge(&q->b.w4, edges[i].val);
			}
			printf("\0");
			for (i = q->a.w4; i >= 0; i = edges[i].next)
				if (*(int *)((char *)blks[edges[i].val] + 0x16) < 0)
					printf("%d\t", edges[i].val);
			printf("\n");
			continue;
		case QLABEL:
			cleartrees();
			printf("L%d\t\n", q->a.w4);
			continue;
		case QJP:
			cleartrees();
			printf("B%d\t\n", q->a.w4);
			continue;
		case QCTABLE:
			printf("C %d\t", q->a.w4);
			q = q->next;
			while (q->op == QCASEL) {
				printf("%d\t", q->a.w4);
				q = q->next;
			}
			printf("\n");
			continue;
		case QSPR:
			pend = 0;
			while (q = q->next, q->op != QEPR) {
				if (q->op != QPARV)
					mktree(q);
				else {
					if (q->a.b2 == OABTEMP) {
						q->a.b2 = OATEMP;
						q->a.w4 = betatbl[q->a.w4].x4;
					}
					if (pend) {
						p = talloc();
						p->in.rall = 0;
						p->in.op = CM;
						p->in.left = pend;
						p->in.right = mkleaf(&q->a);
						if (q->f0) {
							v = talloc();
							v->in.rall = 0;
							v->in.op = STARG;
							v->in.type = q->a.w0;
							v->in.left = p->in.right;
							v->stn.stsize = q->b.w4;
							v->stn.stalign = q->c.w4;
							p->in.right = v;
						}
						pend = p;
					} else {
						pend = mkleaf(&q->a);
						if (q->f0) {
							if (qflag > 4)
								printf("// structure parameter... generating STARG node\n");
							p = talloc();
							p->in.rall = 0;
							p->in.op = STARG;
							p->in.type = q->a.w0;
							p->in.left = pend;
							p->stn.stsize = q->b.w4;
							p->stn.stalign = q->c.w4;
							pend = p;
						}
					}
				}
			}
			continue;
		case QSAVE:
			if (regsym[q->a.w4] == 0)
				continue;
			printf("S%d\t\n", SFSYMNO(regsym[q->a.w4]->sflags));
			regsym[q->a.w4] = 0;
			continue;
		case QREGISTER:
			if (q->a.w4 > 7)
				q->a.w4 -= 7;
			if (q->b.b2 == OANAME) {
				printf("R%d\t%d\t\n", q->a.w4, SFSYMNO(symtab[q->b.w4].sflags));
				regsym[q->a.w4] = symtab + q->b.w4;
			} else if (q->b.b2 == OASTEMP && betatbl[q->b.w4].p6 < 0) {
				if (qflag > 2)
					printf("// assigning S%d to offset %d\n", q->a.w4, tmpoff);
				printf("R%d\t%d\t%d\t\n", q->a.w4, -tmpoff, q->b.w0);
				betatbl[q->b.w4].p6 = tmpoff;
				tmpoff += szty(q->b.w0) * 2;
			} else if (q->b.b2 == OASTEMP)
				printf("R%d\t%d\t%d\t\n", q->a.w4, -betatbl[q->b.w4].p6, q->b.w0);
			else {
				q->b.b2 = OATEMP;
				q->b.w4 = betatbl[q->b.w4].x4;
				u = mkleaf(&q->b);
				*(ctpos + q->b.w4) += 1;
				printf("R%d\t%d\t%d\t\n", q->a.w4, -u->tn.temp, q->b.w0);
			}
			continue;
		case QRESTORE:
			if (q->b.b2 == OANAME)
				printf("T%d\t\n", SFSYMNO(symtab[q->b.w4].sflags));
			else if (q->b.b2 == OASTEMP && betatbl[q->b.w4].p6 < 0) {
				if (qflag > 2)
					printf("// assigning S%d to offset %d\n", q->a.w4, tmpoff);
				printf("T%d\t\n", -tmpoff);
				betatbl[q->b.w4].p6 = tmpoff;
				tmpoff += szty(q->b.w0) * 2;
			} else if (q->b.b2 == OASTEMP)
				printf("T%d\t\n", -betatbl[q->b.w4].p6);
			else {
				q->b.b2 = OATEMP;
				q->b.w4 = betatbl[q->b.w4].x4;
				u = mkleaf(&q->b);
				*(ctpos + q->b.w4) += 1;
				printf("T%d\t\n", -u->tn.temp);
			}
			continue;
		default:
			mktree(q);
			continue;
		}
	}
}

/* a quad is added to the tree (names guessed, written from /lib/gopt) */
void
mktree(q)
	struct quad *q;
{
	NODE *p, *n;
	int u;
	NODE *l, *r, *t;
	int fl;

	if (qflag > 1) {
		printf("// mktree: adding quad to tree\n");
		prquad(q);
	}
	p = talloc();
	p->in.rall = 0;
	p->in.op = qdop[q->op];
	fl = 0;
l7e:
	if (p->in.op == 0x7e) {
		p->in.op = STAR;
		if (q->c.b2 == OABTEMP && q->a.b2 == OABTEMP)
			fl = (betatbl[q->a.w4].p8 >> 10) & 1;
	}
	if (q->op == QCOPY && q->a.b2 == OANAME && q->c.b2 == OABTEMP && ISPTR(q->a.w0))
		betatbl[q->c.w4].p8 |= 1 << 10;
	if (q->op == QCALL) {
		q->c = q->b;
		q->b.b2 = OANONE;
	}
	if (q->a.b2 == OABTEMP) {
		q->a.b2 = OATEMP;
		q->a.w4 = betatbl[q->a.w4].x4;
	} else if (q->a.b2 == OASTEMP && betatbl[q->a.w4].p6 < 0) {
		if (qflag > 2)
			printf("// assigning S%d to offset %d\n", q->a.w4, tmpoff);
		betatbl[q->a.w4].p6 = tmpoff;
		tmpoff += szty(q->a.w0) * 2;
	}
	if (q->b.b2 == OABTEMP) {
		q->b.b2 = OATEMP;
		q->b.w4 = betatbl[q->b.w4].x4;
	} else if (q->b.b2 == OASTEMP && betatbl[q->b.w4].p6 < 0) {
		if (qflag > 2)
			printf("// assigning S%d to offset %d\n", q->b.w4, tmpoff);
		betatbl[q->b.w4].p6 = tmpoff;
		tmpoff += szty(q->b.w0) * 2;
	}
	if (q->c.b2 == OABTEMP) {
		q->c.b2 = OATEMP;
		q->c.w4 = betatbl[q->c.w4].x4;
	} else if (q->c.b2 == OASTEMP && betatbl[q->c.w4].p6 < 0) {
		if (qflag > 2)
			printf("// assigning S%d to offset %d\n", q->c.w4, tmpoff);
		betatbl[q->c.w4].p6 = tmpoff;
		tmpoff += szty(q->c.w0) * 2;
	}
	if (p->in.op != DESC) {
		l = mkleaf(&q->a);
		r = q->b.b2 ? mkleaf(&q->b) : 0;
		p->in.left = l;
		if (p->in.op == STAR)
			p->in.left->tn.w6 = fl;
		p->in.right = r;
	}
	p->in.type = q->c.w0;
	switch (p->in.op) {
	case DESC:
		p->tn.lval = q->a.w4;
		p->tn.rval = q->b.w4;
		break;
	case CALL:
		if (pend == 0)
			p->in.op = p->in.op + 2;
		p->in.right = pend;
		pend = 0;
		addrof(p);
		break;
	case EQ:
		if (BTYPE(q->a.w0) == FLOAT || BTYPE(q->a.w0) == DOUBLE)
			p->in.op = 0x5a;
		break;
	case NE:
		if (BTYPE(q->a.w0) == FLOAT || BTYPE(q->a.w0) == DOUBLE)
			p->in.op = 0x5b;
		break;
	case LT:
		if ((q->a.w0 <= ULONG && q->a.w0 >= UCHAR) || ISPTR(q->a.w0) || ISPTR(q->b.w0))
			p->in.op = 0x57;
		else if (BTYPE(q->a.w0) == FLOAT || BTYPE(q->a.w0) == DOUBLE)
			p->in.op = 0x5d;
		break;
	case LE:
		if ((q->a.w0 <= ULONG && q->a.w0 >= UCHAR) || ISPTR(q->a.w0) || ISPTR(q->b.w0))
			p->in.op = 0x56;
		else if (BTYPE(q->a.w0) == FLOAT || BTYPE(q->a.w0) == DOUBLE)
			p->in.op = 0x5c;
		break;
	case GT:
		if ((q->a.w0 <= ULONG && q->a.w0 >= UCHAR) || ISPTR(q->a.w0) || ISPTR(q->b.w0))
			p->in.op = 0x59;
		else if (BTYPE(q->a.w0) == FLOAT || BTYPE(q->a.w0) == DOUBLE)
			p->in.op = 0x5f;
		break;
	case GE:
		if ((q->a.w0 <= ULONG && q->a.w0 >= UCHAR) || ISPTR(q->a.w0) || ISPTR(q->b.w0))
			p->in.op = 0x58;
		else if (BTYPE(q->a.w0) == FLOAT || BTYPE(q->a.w0) == DOUBLE)
			p->in.op = 0x5f;
		break;
	case MINUS:
		if (q->a.b2 == OAICON && q->a.w4 == 0) {
			p->in.op = 0xa;
			p->in.left->in.op = FREE;
			p->in.left = p->in.right;
		}
		break;
	case SWTCH:
		n = talloc();
		n->in.rall = 0;
		n->in.op = CM;
		n->in.left = p->in.right;
		n->in.right = mkleaf(&q->c);
		p->in.right = n;
		break;
	case STASG:
		p->stn.stsize = q->b.w0;
		p->stn.stalign = q->b.w4;
		p->in.right = p->in.left;
		p->in.left = mkleaf(&q->c);
		break;
	case ASSIGN:
		p->in.right = p->in.left;
		p->in.left = mkleaf(&q->c);
		asgfold(&p);
		break;
	case CBRANCH:
		cleartrees();
		break;
	}
	if (!asgop(p->in.op) && (q->c.b2 == OASTEMP || q->c.b2 == OATEMP && p->in.op != SWTCH && ctpos[q->c.w4]-- > 2)) {
		t = talloc();
		t->in.rall = 0;
		t->in.op = ASSIGN;
		t->in.type = p->in.type;
		t->in.right = p;
		p = mkleaf(&q->c);
		if (q->c.b2 == OATEMP)
			*(ctpos + q->c.w4) += 1;
		else
			*(ctneg + q->c.w4) += 1;
		t->in.left = p;
		asgfold(&t);
		wrline(t);
		tfree(t->in.right);
		t->in.op = FREE;
		return;
	}
	if (q->c.b2 == OATEMP && !asgop(p->in.op) && p->in.op != SWTCH) {
		p->in.rall = q->c.w4;
		return;
	}
	if (q->op == QCOPY) {
		if (q->a.b2 == OANAME && q->c.b2 == OATEMP)
			p->tn.w6 = 1;
	} else if (asgop(p->in.op) && q->c.b2 != OASTEMP && needustar(p)) {
		t = talloc();
		t->in.rall = 0;
		t->in.op = STAR;
		t->in.left = p->in.left;
		t->in.type = DECREF(p->in.left->in.type);
		p->in.type = t->in.type;
		p->in.left = t;
	}
	if (p->in.op == STASG) {
		t = talloc();
		t->in.rall = 0;
		t->in.left = p;
		t->in.op = STAR;
		t->in.type = p->in.type;
		wrline(t);
		t->in.op = FREE;
	} else
		wrline(p);
	if (p->in.left->in.op != TEMP || !asgop(p->in.op))
		tfree(p);
	else {
		tfree(p->in.right);
		p->in.op = FREE;
	}
}

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

	if (p->in.left->in.op == TEMP)
		return 1;
	for (q = p->in.left; q->in.op == STAR; q = q->in.left)
		;
	if (qflag > 4)
		printf("// needustar: q->su = %d\n", q->tn.w6);
	if (q->in.op == TEMP && q->tn.w6 == 0 || optype(q->in.op) != LTYPE && q->in.op != FLD)
		return 1;
	return 0;
}

/* the leaf node of an operand of a quad */
NODE *
mkleaf(a)
	struct qa *a;
{
	static int ftot = 0;
	static int fflag = 1;
	NODE *q, *r;
	int *d;
	long pos1, pos2, pos3;

	if (0)
		a = 0;	/* dead: its store of a at the entry is left (the original) */
	switch (a->b2) {
	case OANAME:
		q = talloc();
		q->in.rall = 0;
		q->in.op = NAME;
		q->in.sname = symtab + a->w4;
		q->in.type = a->w0;
		q->tn.lval = 0;
		q->tn.rval = a->w4;
		break;
	case OALABEL:
		q = talloc();
		q->in.rall = 0;
		q->in.op = NAME;
		q->in.type = a->w0;
		q->tn.rval = -a->w4;
		q->tn.lval = 0;
		break;
	case OAICON:
		q = talloc();
		q->in.rall = 0;
		q->in.op = ICON;
		q->in.type = a->w0;
		q->tn.lval = a->w4;
		break;
	case OALCON:
		q = talloc();
		q->in.rall = 0;
		q->in.op = ICON;
		q->in.type = a->w0;
		q->tn.lval = constbl[a->w4];
		break;
	case OAFCON:
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
		q->tn.lval = 0;
		break;
	case OASTEMP:
		q = talloc();
		q->in.rall = 0;
		q->in.op = TEMP;
		q->in.type = a->w0;
		q->tn.rval = a->w4;
		q->tn.lval = --ctneg[a->w4];
		if (tlfind(a))
			q->tn.lval = -q->tn.lval;
		q->tn.temp = betatbl[a->w4].p6;
		break;
	case OATEMP:
		for (r = node; r <= &node[TREESZ - 1]; ++r) {
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

/* the trees that hold a temporary with a count are written (assignments to the temporaries), the others are checked */
void
cleartrees()
{
	NODE *p, *q, *n1, *n2;

	for (p = node; p <= &node[TREESZ - 1]; ++p) {
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
			for (q = node; q <= &node[TREESZ - 1]; ++q) {
				if (q->in.op == TEMP && p->in.rall == q->in.rall) {
					n2->tn.temp = q->tn.temp;
					break;
				}
			}
			if (q > &node[TREESZ - 1]) {
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
	for (p = node; p <= &node[TREESZ - 1]; ++p)
		if (p->in.op == TEMP && p->in.rall == 0)
			p->in.op == FREE;
}

/* the line and the tree */
void
wrline(p)
	NODE *p;
{
	if (p->in.op != FREE) {
		putchar('.');
		printf("%d\t%s\n", lineno, ftitle);
		wrtree(p);
	}
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
			printf("%d\t", SFSYMNO(p->in.sname->sflags));
		else if (p->in.op == TEMP)
			printf("%d\t", p->tn.temp);
#ifdef SHIPPED
		/* rval overlays in.right: the original prints the segment word of the node it points to (node[] is in its
		   segment 8: 0x8800); the shipped link puts node[] elsewhere (KNOWLEDGE.md, quirks) */
		else if (kind != LTYPE && p->in.right >= node && p->in.right < &node[TREESZ])
			printf("%d\t", -30720);
#endif
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
