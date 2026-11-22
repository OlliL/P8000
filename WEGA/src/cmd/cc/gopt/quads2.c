/*
 * quads2.c - gopt: the quadruples (tree rewriting before them, the list of quads, the tables of constants). Written from /lib/gopt
 * (no source known); names guessed, the callees of other modules by the address (fnXXXX).
 */
static char sccsid[] = "@[$]quads2.c\t1.8  03/04/83 16:00:29 Copr. 1981 87wega3.2.";
#include "mfile2.h"

int ntl = 0, ctmpoff = 0, nctmp = 0, nctmpuses = 0, nest = 0;

extern void ncopy(), tfree(), fwalk();

extern char *symtab0;
extern struct quad *addquad();
extern void cerror();
extern struct { int a, b; } ctmp[];
extern struct { int rv; int d[4]; } fconsttbl[];

static void fold1(), fold2(), asgop2(), asgsimp();
struct qa *operand();
int tlookup(), constlook(), fconstlook();

extern struct quad *qfree, *qtail, *qhead;
extern struct { int p0; int p2; int x4; int p6, p8; } *betatbl;
void normop(), denorm();
extern int tmpno;
NODE *mknode();
extern void dotree();
extern NODE *mknode(), *bcon(), *mktemp2();
extern int qop[], nest;
extern void tfree();
static void fold1(), fold2(), asgop2();
/* forward declarations (the functions are in the order of the original) */
void cmwalk();
int newtmpno();
struct quad * labquad();
struct quad * jmpquad();

/*
 * the quads of a tree (names guessed, written from /lib/gopt): the operands of the node become operand structures p1, p2, p3
 * (p3: the result), the quad of the operator is added; the trees of the operands are freed
 */
void
genquads(p)
	NODE *p;
{
	struct qa s1, s2, s3;
	struct qa *p1, *p2, *p3;
	struct qa s4;
	struct quad *q;
	int kind;
	NODE rcopy, lcopy;

	p1 = &s1;
	p2 = &s2;
	p3 = &s3;
	if (eflag > 4)
		fwalk(p, pr, 0);
	if (qflag > 4)
		printf("// genquads op =%d\n", p->in.op);
	nest++;
	kind = optype(p->in.op);
	if (kind == LTYPE)
		goto done;
	if (p->in.op == 0x46 || p->in.op == 0x67 || p->in.op == 0x48 || p->in.op == 0x69)
		fold1(p);
	else if (p->in.op == 0x3a || p->in.op == 0x65)
		fold2(p);
	else if (asgop(p->in.op))
		asgop2(p);
	genquads(p->in.left);
	if (kind == BITYPE && p->in.op != 0x7a) {
		if (p->in.op == 0x46 || p->in.op == 0x67) {
			if (p->in.right->in.op == CM)
				cmwalk(p->in.right);
			else {
				genquads(p->in.right);
				addquad(QSPR, 0L, 0L, 0L);
				if (p->in.right->in.op == STARG) {
					*p1 = *operand(p->in.right->in.left);
					p2->b2 = OAICON;
					p2->w4 = p->in.right->stn.stsize;
					p3->b2 = OAICON;
					p3->w4 = p->in.right->stn.stalign;
					q = addquad(QPARV, p1, p2, p3);
					q->f0 = 1;
				} else {
					*p1 = *operand(p->in.right);
					addquad(QPARV, p1, 0L, 0L);
				}
			}
			addquad(QEPR, 0L, 0L, 0L);
			tfree(p->in.right);
		} else
			genquads(p->in.right);
	}
	p3->w0 = p->in.type;
rl1:
	p3->b2 = OANONE;
	p3->w4 = 0;
	*p1 = *operand(p->in.left);
	if (kind == BITYPE && p->in.op != 0x7a)
		*p2 = *operand(p->in.right);
	else
		p2 = 0;
	switch (p->in.op) {
	case 0x46: case 0x48: case 0x67: case 0x69:
		p2 = &s2;
		p2->b2 = OANAME;
		p2->w4 = 0;
		if (nest != 1) {
			p3->b2 = OATEMP;
			p3->w4 = nexttmp++;
		}
		break;
	case 0x3a:
		*p3 = *p1;
		*p1 = *p2;
		p2 = 0;
		break;
	case 0x38:
		*p1 = *p2;
		p1 = 0;
		break;
	case 0xa:
		p2 = &s2;
		*p2 = *p1;
rl2:
		p1->b2 = OAICON;
		p1->w4 = 0;
		p->in.op = 8;
		p3->b2 = OATEMP;
		p3->w4 = nexttmp++;
		break;
	case 0x4e:
		if (p1->b2 != OATEMP) {
			p3->b2 = OATEMP;
			p3->w4 = nexttmp++;
			q = addquad(QCOPY, p1, 0L, p3);
			if (nest != 1)
				q->f0 = 1;
			s4 = *p3;
		} else
			s4 = *p1;
		p3->b2 = OATEMP;
		p3->w4 = nexttmp++;
		addquad(QADD, p1, p2, p3);
		*p2 = *p3;
		*p3 = *p1;
		if (p3->b2 == OATEMP)
			p3->w4 -= 1;
		*p1 = *p2;
		addquad(QSTORE, p1, 0L, p3);
		*p3 = s4;
		goto tail;
	case 0x4f:
		if (p1->b2 != OATEMP) {
			p3->b2 = OATEMP;
			p3->w4 = nexttmp++;
			q = addquad(QCOPY, p1, 0L, p3);
			if (nest != 1)
				q->f0 = 1;
			s4 = *p3;
		} else
			s4 = *p1;
		p3->b2 = OATEMP;
		p3->w4 = nexttmp++;
		addquad(QSUB, p1, p2, p3);
		*p2 = *p3;
		*p3 = *p1;
		if (p3->b2 == OATEMP)
			p3->w4 -= 1;
		*p1 = *p2;
		addquad(QSTORE, p1, 0L, p3);
		*p3 = s4;
		goto tail;
	case 0x65:
		*p3 = *p1;
		p3->w0 = p->in.type;
		*p1 = *p2;
rl3:
		p2->b2 = OAICON;
		p2->w0 = p->stn.stsize;
rl4:
		p2->w4 = p->stn.stalign;
		break;
	case 0x66:
		goto done;
	case 0x72:
	case 0x73:
		break;
	case 0x7a:
		p2 = &s2;
		*p2 = *operand(p->in.right->in.left);
		*p3 = *operand(p->in.right->in.right);
		break;
	default:
		p3->b2 = OATEMP;
		p3->w4 = nexttmp++;
		if (asgop(p->in.op)) {
			addquad(qop[p->in.op - 1], p1, p2, p3);
			*p2 = *p3;
			*p3 = *p1;
			*p1 = *p2;
			p2 = 0;
			if ((p3->w0 & 0x30) != 0x10 && p3->b2 == OATEMP)
				p3->w4 -= 1;
		}
	}
	if (qop[p->in.op] == 7)
		addquad(qop[p->in.op], p1, p3, p2);
	else
		addquad(qop[p->in.op], p1, p2, p3);
	if (p->in.op == 0x7a) {
		p->in.right->in.right->in.op = FREE;
		p->in.right->in.left->in.op = FREE;
	}
tail:
	lcopy = *p->in.left;
	p->in.left->in.op = FREE;
	if (kind == BITYPE) {
		rcopy = *p->in.right;
		p->in.right->in.op = FREE;
	}
	if (p3 && p3->b2 == OATEMP) {
		if (p->in.op == 0x3a)
			*p = rcopy;
		else if (asgop(p->in.op) && p->in.op != 0x4e && p->in.op != 0x4f && p->in.op != 0x24 && p->in.op != 0x75 && p->in.op != 0x65) {
			if (p1->b2 == OATEMP) {
				p->in.op = 0x7d;
				p->tn.rval = p1->w4;
			} else
				*p = lcopy;
		} else {
			p->in.op = 0x7d;
			p->tn.rval = p3->w4;
		}
	} else if (asgop(p->in.op) || p->in.op == 0x4e || p->in.op == 0x4f) {
		p->in.op = p3->b2 == OATEMP ? 0x79 : 2;
		p->tn.rval = p3->w4;
		p->in.type = p3->w0;
		p->in.sname = symtab + p->tn.rval;
	}
done:
	nest--;
}

/* U& of a NAME under a node: drop the U& */
static void
fold1(p)
	NODE *p;
{
	NODE *q;

	if (p->in.left->in.op == UAND && p->in.left->in.left->in.op == NAME) {
		q = p->in.left;
		p->in.left = q->in.left;
		q->in.op = FREE;
	}
}

/* the stars on the left side are dropped; a node of an address calculation is dropped */
static void
fold2(p)
	NODE *p;
{
	NODE *q;

	for (q = p->in.left; q->in.op == STAR; q = q->in.left)
		q->in.op = 126;
	if (q->in.op == SUBSR || q->in.op == STREF || q->in.op == PLUS || q->in.op == MINUS) {
		q = p->in.left;
		p->in.left = q->in.left;
		q->in.op = FREE;
	}
}

/* x op= y: x = x op y, the left side copied */
static void
asgop2(p)
	NODE *p;
{
	NODE *t;
	int op;
	NODE *q;

	op = p->in.op;
	if (op == 7 || op == 9 || op == 0x12 || op == 0xf) {
		q = t = talloc();
		ncopy(q, p);
		p->in.op = ASSIGN;
		t->in.op = t->in.op - 1;
		t->in.left = tcopy(p->in.left);
		fwalk(t->in.left, asgsimp);
		p->in.right = t;
		fold2(p);
	}
}

/* the right side of an assignment operator goes into the node */
static void
asgsimp(p)
	NODE *p;
{
	NODE *q;

	if (asgop(p->in.op)) {
		q = p->in.left;
		tfree(p->in.right);
		p->in.op = FREE;
		*p = *q;
	}
}

/* the arguments of a call: a comma list is walked, each argument becomes a quad */
void
cmwalk(p)
	NODE *p;
{
	struct quad *q;
	struct qa s1, s2;
	struct qa *r, *p1, *p2;

	p1 = &s1;
	p2 = &s2;
	if (p->in.op == CM) {
		cmwalk(p->in.right);
		cmwalk(p->in.left);
		if (p->in.left->in.op != CM) {
			addquad(QSPR, 0L, 0L, 0L);
			if (p->in.left->in.op == STARG) {
				r = operand(p->in.left->in.left);
				p1->b2 = OAICON;
				p1->w4 = p->in.left->stn.stsize;
				p2->b2 = OAICON;
				p2->w4 = p->in.left->stn.stalign;
				q = addquad(QPARV, r, p1, p2);
				q->f0 = 1;
			} else {
				r = operand(p->in.left);
				addquad(QPARV, r, 0L, 0L);
			}
		}
		if (p->in.right->in.op == STARG) {
			r = operand(p->in.right->in.left);
			p1->b2 = OAICON;
			p1->w4 = p->in.right->stn.stsize;
			p2->b2 = OAICON;
			p2->w4 = p->in.right->stn.stalign;
			q = addquad(QPARV, r, p1, p2);
			q->f0 = 1;
		} else {
			r = operand(p->in.right);
			addquad(QPARV, r, 0L, 0L);
		}
		if (0)
			goto e1;
	} else {
e1:
		if (optype(p->in.op) != LTYPE)
			genquads(p);
	}
}

/* the operand of a quad for a leaf */
struct qa *
operand(p)
	NODE *p;
{
	static struct qa opnd;
	struct qa a, b, c;
	int i;

	if (0)
		p = 0;	/* dead: its store of p at the entry is left (the original) */
	switch (p->in.op) {
	case NAME:
		if (p->tn.rval >= 0) {
			opnd.b2 = OANAME;
			opnd.w4 = p->in.sname - symtab;
		nmt:
			opnd.w0 = p->in.type;
			break;
		}
		opnd.b2 = OALABEL;
		opnd.w4 = -p->tn.rval;
		goto nmt;
	case ICON:
		if ((p->tn.lval > 0 ? p->tn.lval : -p->tn.lval) < 0x7fff) {
			opnd.b2 = OAICON;
			opnd.w4 = p->tn.lval;
		} else {
			opnd.b2 = OALCON;
			opnd.w4 = constlook(p->tn.lval);
		}
		opnd.w0 = p->in.type;
		break;
	case FCON:
		opnd.b2 = OAFCON;
		opnd.w0 = p->in.type;
		opnd.w4 = fconstlook(p);
		break;
	case TEMP:
		opnd.b2 = OASTEMP;
		if (p->in.rall)
			opnd.w4 = p->tn.w6;
		else if ((i = tlookup(p->tn.rval)) >= 0)
			opnd.w4 = ctmp[i].b;
		else {
			opnd.w4 = p->tn.rval + ctmpoff;
			if (nctmp >= 100)
				cerror("exceeded ctmp_no limit\n");
			ctmp[nctmp].a = p->tn.rval;
			ctmp[nctmp].b = opnd.w4;
			nctmp++;
		}
		opnd.w0 = p->in.type;
		if (p->tn.lval < 0)
			tl[ntl++] = opnd.w4;
		if (maxtmp < opnd.w4)
			maxtmp = opnd.w4;
		nctmpuses++;
		break;
	case 0x7d:
		opnd.b2 = OATEMP;
		opnd.w0 = p->in.type;
		opnd.w4 = p->tn.rval;
		break;
	case 0x7b:
		a.b2 = OAICON;
		a.w4 = (int)p->in.left;
		a.w0 = 4;
		b.b2 = OAICON;
		b.w4 = p->tn.rval;
		b.w0 = 4;
		c.b2 = OATEMP;
		c.w4 = nexttmp++;
		c.w0 = p->in.type;
		addquad(QDESC, &a, &b, &c);
		opnd = c;
		break;
	}
	return &opnd;
}

/* the index of x in the table of the temporaries, or -1 */
int
tlookup(x)
	int x;
{
	int i;

	for (i = 0; i < nctmp; i++)
		if (ctmp[i].a == x)
			return i;
	return -1;
}

/* the index of x in the table of the long constants (entered when not there) */
int
constlook(x)
	long x;
{
	int i;

	for (i = 0; i < nconst; i++)
		if (constbl[i] == x)
			break;
	if (i == nconst)
		constbl[nconst++] = x;
	if (nconst >= 512)
		cerror("out of space in constbl\n");
	return i;
}

/* the index of the floating constant of p in the table (read from the data file when not there) */
int
fconstlook(p)
	NODE *p;
{
	int i;
	int *q;

	for (i = 0; i < nfconst; i++)
		if (fconsttbl[i].rv == p->tn.rval)
			break;
	if (i == nfconst) {
		fconsttbl[i].rv = p->tn.rval;
		q = fconsttbl[nfconst].d;
		nfconst++;
		fseek(dfile, p->in.left, 0);
		fscanf(dfile, "\t.word\t%%%x\n\t.word\t%%%x\n\t.word\t%%%x\n\t.word\t%%%x\n", q, q + 1, q + 2, q + 3);
	}
	if (nfconst >= 200)
		cerror("out of space in fconstbl\n");
	return i;
}

/* the index of the double constant x in the table (entered when not there; name guessed) */
int
dconstlook(x)
	double x;
{
	int i;

	for (i = 0; i < nfconst; i++)
		if (*(double *)fconsttbl[i].d == x)
			break;
	if (i == nfconst) {
		fconsttbl[i].rv = 0;
		*(double *)fconsttbl[nfconst].d = x;
		nfconst++;
	}
	if (nfconst >= 200)
		cerror("out of space in fconstbl\n");
	return i;
}

/* a new quad at the end of the list (op, the operands a, b, c; 0: none) */
struct quad *
addquad(op, a, b, c)
	int op;
	struct qa *a, *b, *c;
{
	struct quad *q;

	if (qfree) {
		q = qfree;
		qfree = qfree->next;
	} else {
		q = (struct quad *)taballoc(30, 1);
		if (q == (struct quad *)0x7F00FFFFL)
			return 0;
	}
	normop(a);
	normop(b);
	normop(c);
	q->op = op;
	q->f7 = 0;
	q->f4 = 0;
	q->f6 = 0;
	q->f5 = 0;
	q->f0 = 0;
	if (c == 0)
		q->c.b2 = OANONE;
	else
		q->c = *c;
	q->c.fl = 0;
	if (a == 0)
		q->a.b2 = OANONE;
	else
		q->a = *a;
	if (b == 0)
		q->b.b2 = OANONE;
	else
		q->b = *b;
	q->pos = curpos;
	q->next = 0;
	if (qtail)
		qtail->next = q;
	qtail = q;
	if (q->op == QJP || q->op == QJPC || q->op == QCEND)
		lastq = q;
	if (qflag > 1) {
		printf("// in addquad -- ");
		prquad(q);
	}
	return q;
}

/* a new quad behind the quad where (0: at the head of the list) */
struct quad *
insertquad(op, a, b, c, where)
	int op;
	struct qa *a, *b, *c;
	struct quad *where;
{
	struct quad *q;

	if (qfree) {
		q = qfree;
		qfree = qfree->next;
	} else
		q = (struct quad *)taballoc(30, 1);
	if (op != 0x28 && op != 0x29 && op != 0x2b && op != 0x3b) {
		normop(a);
		normop(b);
		normop(c);
	}
	q->op = op;
	q->f0 = 0;
	q->f7 = 0;
	q->f4 = 0;
	q->f6 = 0;
	q->f5 = 0;
	if (c == 0)
		q->c.b2 = OANONE;
	else
		q->c = *c;
	q->c.fl = 0;
	if (a == 0)
		q->a.b2 = OANONE;
	else
		q->a = *a;
	if (b == 0)
		q->b.b2 = OANONE;
	else
		q->b = *b;
	q->pos = curpos;
	if (where) {
		q->next = where->next;
		where->next = q;
	} else {
		q->next = qhead;
		qhead = q;
	}
	if (qflag > 2) {
		printf("in insertquad -- ");
		prquad(q);
	}
	return q;
}

/* take a quad out of the list (to the free list); the operands are given back */
struct quad *
delquad(q)
	struct quad *q;
{
	struct quad *p, *prev;

	if (qflag) {
		printf("// removquad : quad = \n");
		prquad(q);
	}
	if (q->op != QBBLOCK) {
		denorm(&q->a);
		denorm(&q->b);
		denorm(&q->c);
	}
	for (p = qhead; p != q; p = p->next)
		prev = p;
	prev->next = q->next;
	q->next = qfree;
	qfree = q;
	return prev;
}

/* a quad behind the quad b (it leaves its place); the quad in front of its old place */
struct quad *
movequad(a, b)
	struct quad *a, *b;
{
	struct quad *p, *prev;

	for (p = qhead; p != a; p = p->next)
		prev = p;
	prev->next = a->next;
	a->next = b->next;
	b->next = a;
	return prev;
}

/* the quads up to the next 0x3b or 0x11 are taken out */
void
delrun(q)
	struct quad *q;
{
	struct quad *r;

	r = q->next;
	delquad(q);
	while (r->op != QBBLOCK) {
		if (r->op == QEND)
			return;
		q = r;
		r = q->next;
		delquad(q);
	}
}

/* the use of a temporary of an operand is counted */
void
normop(a)
	struct qa *a;
{
	int w;

	if (a == 0)
		return;
	if (a->b2 == OATEMP)
		w = a->w4;
	else if (a->b2 == OASTEMP)
		w = -a->w4;
	else if (a->b2 == OABTEMP)
		w = betatbl[a->w4].x4;
	else
		return;
	if (w > 1000 || w < -1000) {
		cerror("compiler error - out of space in temp_cnt");
		return;
	}
	if (w > 0)
		*(ctpos + w) += 1;
	else
		*(ctneg + -w) += 1;
}

void
denorm(a)
	struct qa *a;
{
	if (a->b2 == OATEMP)
		*(ctpos + a->w4) -= 1;
	else if (a->b2 == OABTEMP)
		*(ctpos + betatbl[a->w4].x4) -= 1;
	else if (a->b2 == OASTEMP)
		*(ctneg + a->w4) -= 1;
}

/*
 * the conditional operators (?: && ||) and the comma operator are made into jumps; f and t are the labels of the context
 * (0: none); 1 is returned when the tree has changed (names guessed, written from /lib/gopt)
 */
int
map(p, f, t)
	register NODE *p;
	int f, t;
{
	register int op, kind, res;
	int a, b;

	if (eflag > 4) {
		printf("// map: f = %d, t = %d, p =\n", f, t);
		fwalk(p, pr, 0);
	}
	op = p->in.op;
	kind = optype(op);
	a = 0;
	b = 0;
	if (kind == LTYPE)
		return 0;
	if (kind == UTYPE) {
		if (op == 0x4c) {
			res = map(p->in.left, t, f);
			if (p->in.left->in.op == FREE)
				p->in.op = FREE;
			if (eflag > 4) {
				printf("// map: FREE NOT\n");
				fwalk(p, pr, 0);
			}
			return res;
		}
		return map(p->in.left, f, t);
	}
	switch (op) {
	case 0x3b: {
		NODE *q;

		dotree(p->in.left);
		q = p->in.right;
		ncopy(p, p->in.right);
		q->in.op = FREE;
		return 1;
	}
	case 0x17:
	case 0x18: {
		NODE *n, *n2, *t1, *x;
		int u, lab;
		NODE *c, *y;

		if (f == 0) {
			t1 = mktemp2(4);
			t1->tn.lval = 3;
			if (logop(p->in.left->in.op)) {
				if (p->in.left->in.type == 4)
					n = mknode(0x3a, t1, p->in.left, 4);
				else {
					n = mknode(0x6e, p->in.left, 0L, 4);
					n = mknode(0x3a, t1, n, 4);
				}
			} else {
				c = bcon(0);
				n = mknode(0x51, p->in.left, c, 4);
				n->in.right->in.type = n->in.left->in.type;
				n = mknode(0x3a, t1, n, 4);
			}
			n2 = tcopy(t1);
			dotree(n);
			lab = newtmpno();
			x = mknode(0x4c, 0L, 0L, 4);
			c = bcon(lab);
			n = mknode(0x73, x, c, 0L);
			t1 = n2;
			t1->tn.lval = 2;
			if (op == 0x18) {
				y = c = talloc();
				ncopy(y, t1);
				x->in.left = c;
			} else
				ncopy(x, t1);
			n2 = tcopy(n->in.left);
			dotree(n);
			x = talloc();
			if (logop(p->in.right->in.op)) {
				if (p->in.right->in.type == 4)
					n = mknode(0x3a, x, p->in.right, 4);
				else {
					n = mknode(0x6e, p->in.right, 0L, 4);
					n = mknode(0x3a, x, n, 4);
				}
			} else {
				c = bcon(0);
				n = mknode(0x51, p->in.right, c, 4);
				n->in.right->in.type = n->in.left->in.type;
				n = mknode(0x3a, x, n, 4);
			}
			if (op == 0x18)
				ncopy(x, n2->in.left);
			else
				ncopy(x, n2);
			x->tn.lval = 1;
			n2 = tcopy(n->in.left);
			dotree(n);
			labquad(lab);
			ncopy(p, n2);
			p->in.left = 0;
			return 1;
		}
		if (op == 0x18) {
			lab = (long)t == 0 ? newtmpno() : t;
			if (p->in.left->in.op == 0x4c) {
				x = p->in.left->in.left;
				p->in.left->in.op = FREE;
			} else
				x = mknode(0x4c, p->in.left, 0L, 4);
			c = bcon(lab);
			n = mknode(0x73, x, c, 0L);
			n->in.right->tn.rval = 0;
			dotree(n);
			if ((long)f == 0) {
				x = mknode(0x4c, p->in.right, 0L, 4);
				c = bcon(t);
				n = mknode(0x73, x, c, 0L);
				n->in.right->tn.rval = 0;
				dotree(n);
			} else if ((long)t == 0) {
				c = bcon(f);
				n = mknode(0x73, p->in.right, c, 0L);
				n->in.right->tn.rval = 0;
				dotree(n);
				labquad(lab);
			} else
				cerror("map - OROR");
		} else {
			lab = (long)f == 0 ? newtmpno() : f;
			c = bcon(lab);
			n = mknode(0x73, p->in.left, c, 0L);
			n->in.right->tn.rval = 0;
			dotree(n);
			if ((long)f == 0) {
				x = mknode(0x4c, p->in.right, 0L, 4);
				c = bcon(t);
				n = mknode(0x73, x, c, 0L);
				n->in.right->tn.rval = 0;
				dotree(n);
				labquad(lab);
			} else if ((long)t == 0) {
				c = bcon(f);
				n = mknode(0x73, p->in.right, c, 0L);
				n->in.right->tn.rval = 0;
				dotree(n);
			} else
				cerror("map - ANDAND");
		}
		p->in.op = FREE;
		return 0;
	}
	case 0x15: {
		int x1, x2;
		NODE *n, *cp, *c;

		x1 = newtmpno();
		x2 = newtmpno();
		c = bcon(x1);
		n = mknode(0x73, p->in.left, c, 4);
		dotree(n);
		n = mktemp2(p->in.right->in.left->in.type);
		n->tn.lval = 2;
		n = mknode(0x3a, n, p->in.right->in.left, p->in.right->in.left->in.type);
		cp = tcopy(n->in.left);
		dotree(n);
		jmpquad(x2);
		labquad(x1);
		n = talloc();
		n = mknode(0x3a, n, p->in.right->in.right, p->in.right->in.right->in.type);
		ncopy(n->in.left, cp);
		n->in.left->tn.lval = 1;
		cp = tcopy(n->in.left);
		dotree(n);
		labquad(x2);
		p->in.right->in.op = FREE;
		ncopy(p, cp);
		p->in.left = 0;
		return 1;
	}
	case 0x73:
		a = (int)p->in.right->in.left;
		b = p->in.right->tn.rval;
		break;
	}
	return map(p->in.left, a, b) || map(p->in.right, a, b);
}

/* a new node (names guessed) */
NODE *
mknode(op, l, r, type)
	int op;
	register NODE *l, *r;
	int type;
{
	register NODE *p;

	p = talloc();
	p->tn.rval = 0;
	p->in.rall = 0;
	p->tn.w6 = 0;
	p->in.sname = 0;
	p->tn.temp = 0;
	p->in.op = op;
	p->in.left = l;
	p->in.right = r;
	p->in.type = type;
	return p;
}

/* an integer constant */
NODE *
bcon(v)
	int v;
{
	register NODE *p;

	p = mknode(ICON, 0L, 0L, 4);
	p->tn.lval = v;
	p->tn.rval = 0;
	return p;
}

/* a temporary */
NODE *
mktemp2(type)
	int type;
{
	register NODE *p;

	p = mknode(TEMP, 0L, 0L, type);
	p->tn.w6 = nctmpuses ? ++maxtmp : 0;
	p->in.rall = -1;
	nctmpuses++;
	ctmpoff++;
	return p;
}

int
newtmpno()
{
	return tmpno++;
}

/* the quad 0x1c (a label) for the number x */
struct quad *
labquad(x)
	int x;
{
	struct qa s;
	struct qa *p;

	p = &s;
	p->b2 = OAICON;
	p->w0 = 4;
	p->w4 = x;
	return addquad(QLABEL, p, 0L, 0L);
}

struct quad *
jmpquad(x)
	int x;
{
	struct qa s;
	struct qa *p;

	p = &s;
	p->b2 = OAICON;
	p->w4 = x;
	return addquad(QJP, p, 0L, 0L);
}

/* the tree mapped again and again; a comma node without a left side disappears */
void
mapall(p)
	NODE *p;
{
	while (map(p, 0, 0))
		;
	if (p->in.op == 0x73 && p->in.left->in.op == FREE) {
		p->in.right->in.op = FREE;
		p->in.op = FREE;
	}
}

int tmpno = 25000;
