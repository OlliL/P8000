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
