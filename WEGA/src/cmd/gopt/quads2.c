/*
 * quads2.c - gopt: the quadruples (tree rewriting before them, the list of quads, the tables of constants). Written from /lib/gopt
 * (no source known); names guessed, the callees of other modules by the address (fnXXXX).
 */
#include "gopt.h"

static char sccsid[] = "@[$]quads2.c\t1.8  03/04/83 16:00:29 Copr. 1981 87wega3.2.";

extern NODE *talloc();
extern NODE *tcopy();
extern void ncopy(), tfree(), fwalk();

extern char *symtab0;
extern struct sym *symtab;
extern struct quad *addquad();
extern char *taballoc();
extern struct qa *fn1d22();
extern void cerror(), fn0b5e();
extern FILE *dfile;
extern int fscanf();
extern int nconst, nfconst, nctmp, ntl, s4d94, nctmpuses, ctmpoff, v0e;
extern long constbl[];
extern int tl[];
extern struct { int a, b; } ctmp[];
extern struct { int rv; int d[4]; } fconsttbl[];

static struct qa opnd;

static void fold1(), fold2(), asgop2(), asgsimp();
extern struct quad *delquad();
struct qa *operand();
int tlookup(), constlook(), fconstlook();

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
	int op;
	NODE *q, *t;

	op = p->in.op;
	if (op == 7 || op == 9 || op == 0x12 || op == 0xf) {
		q = t = talloc();
		ncopy(q, p);
		p->in.op = ASSIGN;
		q->in.op = q->in.op - 1;
		q->in.left = tcopy(p->in.left);
		fwalk(q->in.left, asgsimp);
		p->in.right = q;
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

/* the operand of a quad for a leaf */
struct qa *
operand(p)
	NODE *p;
{
	int i;
	long l;
	struct qa a, b, c;
	struct sym *sp;

	switch (p->in.op) {
	case NAME:
		if (p->tn.rval >= 0) {
			opnd.b2 = 4;
			sp = p->in.sname;
			opnd.w4 = sp - symtab;
		} else {
			opnd.b2 = 5;
			opnd.w4 = -p->tn.rval;
		}
		opnd.w0 = p->in.type;
		break;
	case ICON:
		if ((p->tn.lval > 0 ? p->tn.lval : -p->tn.lval) < 0x7fff) {
			opnd.b2 = 1;
			opnd.w4 = p->tn.lval;
		} else {
			opnd.b2 = 7;
			opnd.w4 = constlook(p->tn.lval);
		}
		opnd.w0 = p->in.type;
		break;
	case FCON:
		opnd.b2 = 2;
		opnd.w0 = p->in.type;
		opnd.w4 = fconstlook(p);
		break;
	case TEMP:
		opnd.b2 = 6;
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
		if (s4d94 < opnd.w4)
			s4d94 = opnd.w4;
		nctmpuses++;
		break;
	case 0x7d:
		opnd.b2 = 3;
		opnd.w0 = p->in.type;
		opnd.w4 = p->tn.rval;
		break;
	case 0x7b:
		a.b2 = 1;
		a.w4 = (int)p->in.left;
		a.w0 = 4;
		b.b2 = 1;
		b.w4 = p->tn.rval;
		b.w0 = 4;
		c.b2 = 3;
		c.w4 = v0e++;
		c.w0 = p->in.type;
		fn1d22(0x3a, &a, &b, &c);
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

/* the arguments of a call: a comma list is walked, each argument becomes a quad */
void
cmwalk(p)
	NODE *p;
{
	struct qa *q;
	struct qa s1, s2;
	struct qa *r, *p1, *p2;

	p1 = &s1;
	p2 = &s2;
	if (p->in.op == CM) {
		cmwalk(p->in.right);
		cmwalk(p->in.left);
		if (p->in.left->in.op != CM) {
			fn1d22(0x32, 0L, 0L, 0L);
			if (p->in.left->in.op == STARG) {
				r = operand(p->in.left->in.left);
				p1->b2 = 1;
				p1->w4 = p->in.left->stn.stsize;
				p2->b2 = 1;
				p2->w4 = p->in.left->stn.stalign;
				q = fn1d22(0x34, r, p1, p2);
				q->w4 |= 1;
			} else {
				r = operand(p->in.left);
				fn1d22(0x34, r, 0L, 0L);
			}
		}
		if (p->in.right->in.op == STARG) {
			r = operand(p->in.right->in.left);
			p1->b2 = 1;
			p1->w4 = p->in.right->stn.stsize;
			p2->b2 = 1;
			p2->w4 = p->in.right->stn.stalign;
			q = fn1d22(0x34, r, p1, p2);
			q->w4 |= 1;
		} else {
			r = operand(p->in.right);
			fn1d22(0x34, r, 0L, 0L);
		}
	} else if (optype(p->in.op) != LTYPE)
		fn0b5e(p);
}

extern struct quad *qfree, *qtail, *qhead, *lastq;
extern struct pos curpos;
extern int qflag;
extern struct { int p0; int p2; int x4; int p6, p8; } *ptbl9c;
extern char ctpos[], ctneg[];
extern void fn2c8c();
static void normop(), denorm();

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
		q->c.b2 = 0;
	else
		q->c = *c;
	q->c.fl = 0;
	if (a == 0)
		q->a.b2 = 0;
	else
		q->a = *a;
	if (b == 0)
		q->b.b2 = 0;
	else
		q->b = *b;
	q->pos = curpos;
	q->next = 0;
	if (qtail)
		qtail->next = q;
	qtail = q;
	if (q->op == 0x1a || q->op == 0x1b || q->op == 0xb)
		lastq = q;
	if (qflag > 1) {
		printf("// in addquad -- ");
		fn2c8c(q);
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
		q->c.b2 = 0;
	else
		q->c = *c;
	q->c.fl = 0;
	if (a == 0)
		q->a.b2 = 0;
	else
		q->a = *a;
	if (b == 0)
		q->b.b2 = 0;
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
		fn2c8c(q);
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
		fn2c8c(q);
	}
	if (q->op != 0x3b) {
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
delquads(q)
	struct quad *q;
{
	struct quad *r;

	r = q->next;
	delquad(q);
	while (r->op != 0x3b) {
		if (r->op == 0x11)
			return;
		q = r;
		r = q->next;
		delquad(q);
	}
}

/* the use of a temporary of an operand is counted */
static void
normop(a)
	struct qa *a;
{
	int w;

	if (a == 0)
		return;
	if (a->b2 == 3)
		w = a->w4;
	else if (a->b2 == 6)
		w = -a->w4;
	else if (a->b2 == 8)
		w = ptbl9c[a->w4].x4;
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

static void
denorm(a)
	struct qa *a;
{
	if (a->b2 == 3)
		*(ctpos + a->w4) -= 1;
	else if (a->b2 == 8)
		*(ctpos + ptbl9c[a->w4].x4) -= 1;
	else if (a->b2 == 6)
		*(ctneg + a->w4) -= 1;
}

extern int s4d94, nctmpuses, ctmpoff, tmpno;
extern int map();
NODE *mknode();

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
	p->tn.w6 = nctmpuses ? ++s4d94 : 0;
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
	p->b2 = 1;
	p->w0 = 4;
	p->w4 = x;
	return addquad(0x1c, p, 0L, 0L);
}

struct quad *
jmpquad(x)
	int x;
{
	struct qa s;
	struct qa *p;

	p = &s;
	p->b2 = 1;
	p->w4 = x;
	return addquad(0x1a, p, 0L, 0L);
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
