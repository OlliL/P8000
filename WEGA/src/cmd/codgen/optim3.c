/*
 * optim3.c - the tree optimizer
 */
#include "cg.h"
#include "cg2.h"

extern int bsize();
extern int cerror();
extern int conval();
extern NODE *convelim();
extern int dope[];
extern int ispow2();
extern void ncopy();
extern NODE node[];
extern NODE *optim();
extern int plainicon();
extern int revrel[];
extern NODE *tcopy();
extern void tfree();
extern int treeeq();
extern void uerror();
static char sccsid[] = "@[$]optim3.c\t2.4  03/04/83 15:46:52 Copr. 1981 87wega3.2.";
static char optsccs[] = "@[$]optim\t2.5  10/06/83 10:17:28 Copr. 1981 87wega3.2.";

#define SCONV	110

#define NONAME	0


/* is p a constant without a name */
#define nncon(p)	plainicon(p)

static int isint();

/* local optimizations: collect constants, constants to the right, towers of operators to the left */
NODE *
optim(p)
	register NODE *p;
{
	register int o, ty;
	register NODE *a, *b;
	int x;
	NODE *sp;
	int i;

	o = p->op;
	ty = optype(o);
	if (ty == LTYPE)
		return p;
	if (ty == BITYPE && p->right->op == SCONV) {
		p = convelim(p, 'r');
		o = p->op;
		ty = optype(o);
	}
	if (p->left->op == SCONV) {
		p = convelim(p, 'l');
		o = p->op;
		ty = optype(o);
	}
	if (p->op == SCONV) {
		p = convelim(p, 'p');
		o = p->op;
		ty = optype(o);
	}
	if (ty == BITYPE)
		p->right = optim(p->right);
	p->left = optim(p->left);
	switch (o) {
	case UAND:
		if (p->left->op != NAME)
			cerror("& error");
		p->left->op = ICON;
	setuleft:
		p->left->type = p->type;
		p->op = FREE;
		return p->left;
	case STAR:
		if (p->left->op != ICON)
			break;
		p->left->op = NAME;
		goto setuleft;
	case MINUS:
		if (p->left->op == PLUS && p->right->op == PLUS && treeeq(p->left->left, p->right->left) &&
		    p->left->right->type == p->type && p->right->right->type == p->type) {
			a = tcopy(p->left->right);
			b = tcopy(p->right->right);
			goto joined;
		}
		goto minus1;
	minus2:
		if (p->right->op == MINUS && treeeq(p->left->left, p->right->left) &&
		    p->left->right->type == p->type && p->right->right->type == p->type) {
			b = tcopy(p->left->right);
			a = tcopy(p->right->right);
		joined:
			tfree(p->left);
			tfree(p->right);
			p->left = a;
			p->right = b;
		minus1:
			if (p->left->op == MINUS)
				goto minus2;
		}
		if (treeeq(p->left, p->right) && p->type != 6 && p->type != 7) {
			tfree(p->left);
			tfree(p->right);
			p->op = ICON;
			p->sname = 0;
			p->rval = 0;
			p->lval = 0;
			return p;
		}
		if (!nncon(p->right))
			break;
		p->right->lval = -p->right->lval;
		p->op = PLUS;
		o = PLUS;
	case MUL:
	case PLUS:
	case AND:
	case OR:
	case ER:
		if (nncon(p->left) || (p->left->op == ICON && p->right->op != ICON &&
		    !(p->right->op == REG && p->right->rval == 15) && (p->type & 0x30) != 0x10)) {
			sp = p->left;
			p->left = p->right;
			p->right = sp;
		}
#ifndef OLD_D23
		if ((o == MINUS || o == PLUS) && nncon(p->right) && p->right->lval == 0)
			goto zapright;
#endif
		if (p->right->op == o && (p->type & 0x30) != 0x10 && (p->right->left->type & 0x30) != 0x10) {
			NODE *t1, *t2, *t3;

			t1 = p->left;
			sp = p->right;
			t2 = sp->left;
			t3 = sp->right;
			p->left = sp;
			sp->left = t1;
			sp->right = t2;
			p->right = t3;
		}
		if (o == PLUS && p->left->op == MINUS && p->right->op == ICON && p->left->right->op == ICON &&
		    conval(p->right, MINUS, p->left->right)) {
		zapleft:
			p->left->right->op = FREE;
			p->left->op = FREE;
			p->left = p->left->left;
		}
		if (p->right->op == ICON && p->left->op == o && p->left->right->op == ICON &&
		    conval(p->right, o, p->left->right))
			goto zapleft;
		if ((p->type & 0x30) == 0x10 && p->left->op == ICON && p->right->op == o &&
		    p->right->right->op == ICON && conval(p->left, o, p->right->right)) {
			p->right->right->op = FREE;
			p->right->op = FREE;
			p->right = p->right->left;
		neg:
			if (o == PLUS && nncon(p->right) && p->right->lval < 0) {
				p->right->lval = -p->right->lval;
				p->op = MINUS;
				o = MINUS;
			}
			break;
		}
		if (p->left->op == ICON && p->right->op == ICON && conval(p->left, o, p->right)) {
		zapright:
			p->right->op = FREE;
			goto setuleft;
		}
		goto neg;
	case DIV:
	case ASG DIV:
	case MOD:
	case ASG MOD:
		a = p->right;
		if (!((unsigned)p->type <= 15 && (unsigned)p->type >= 12))
			break;
		if (!nncon(a))
			break;
		if (ispow2(a->lval) <= 0)
			break;
		if (o == MOD || o == ASG MOD) {
			p->op = o == ASG MOD ? ASG AND : AND;
			a->lval -= 1;
			a->type = p->type = p->left->type;
		} else {
			p->op = o == ASG DIV ? ASG RS : RS;
			a->lval = ispow2(a->lval);
		}
		break;
	case ASG PLUS:
	case ASG MINUS:
		if (nncon(p->right) && p->right->lval == 0)
			goto zapright;
		break;
	case EQ:
	case NE:
	case LE:
	case LT:
	case GE:
	case GT:
	case ULE:
	case ULT:
	case UGE:
	case UGT:
		if (p->left->op != ICON)
			break;
		sp = p->left;
		p->left = p->right;
		p->right = sp;
		p->op = revrel[p->op - EQ];
		if (p->right->op != ICON)
			break;
		if (!conval(p->left, p->op, p->right))
			break;
		sp = p->left;
		tfree(p->right);
		*p = *sp;
		tfree(sp);
		break;
	}
	return p;
}

/* fold the constant q into p by the operator o; 0 if that is not possible */
int
conval(p, o, q)
	register NODE *p;
	int o;
	register NODE *q;
{
	int sh;
	int unsgn;
	long val;

	val = q->lval;
	unsgn = ((unsigned)p->type <= 15 && (unsigned)p->type >= 12) || ((unsigned)q->type <= 15 && (unsigned)q->type >= 12);
	if (unsgn && (o == LE || o == LT || o == GE || o == GT))
		o += 4;
	if (!nncon(p) && !nncon(q))
		return 0;
	if (!nncon(q) && o != PLUS)
		return 0;
	if (!nncon(p) && o != PLUS && o != MINUS)
		return 0;
	switch (o) {
	case PLUS:
#ifdef SEG
		if ((p->type & 0x30) == 0x10) {
			p->lval = ((val & 0x7F00FFFFL) + p->lval) & 0x7F00FFFFL;
			return 1;
		}
#endif
		p->lval += val;
		if (nncon(p)) {
			p->type = q->type;
			p->rval = q->rval;
			p->sname = q->sname ? q->sname : p->sname;
		}
		break;
	case MINUS:
#ifdef SEG
		if ((p->type & 0x30) == 0x10) {		/* the difference of pointer constants: its offset */
			p->lval = (p->lval - val) & 0xFFL;
			return 1;
		}
#endif
		p->lval -= val;
		break;
	case MUL:
		p->lval *= val;
		break;
	case DIV:
		if (val == 0) {
			uerror("division by 0");
			return 1;
		}
		p->lval /= val;
		break;
	case MOD:
		if (val == 0) {
			uerror("division by 0");
			return 1;
		}
		p->lval %= val;
		break;
	case AND:
		p->lval &= val;
		break;
	case OR:
		p->lval |= val;
		break;
	case ER:
		p->lval ^= val;
		break;
	case LS:
		sh = val;
		p->lval <<= sh;
		break;
	case RS:
		sh = val;
		p->lval >>= sh;
		break;
	case UMINUS:
		p->lval = -p->lval;
		break;
	case COMPL:
		p->lval = ~p->lval;
		break;
	case NOT:
		p->lval = !p->lval;
		break;
	case LT:
		p->lval = p->lval < val;
		break;
	case LE:
		p->lval = p->lval <= val;
		break;
	case GT:
		p->lval = p->lval > val;
		break;
	case GE:
		p->lval = p->lval >= val;
		break;
	case ULT:
		p->lval = p->lval - val < 0;
		break;
	case ULE:
		p->lval = p->lval - val <= 0;
		break;
	case UGE:
		p->lval = p->lval - val >= 0;
		break;
	case UGT:
		p->lval = p->lval - val > 0;
		break;
	case EQ:
		p->lval = p->lval == val;
		break;
	case NE:
		p->lval = p->lval != val;
		break;
	default:
		return 0;
	}
	return 1;
}

/* are the two trees the same */
int
treeeq(p, q)
	register NODE *p, *q;
{
	register int o;

	o = p->op;
	if (o != q->op)
		return 0;
	switch optype(o) {
	case LTYPE:
		if (o == NAME)
			return p->sname == q->sname && p->lval == q->lval;
		return p->lval == q->lval && p->rval == q->rval;
	case BITYPE:
		if (!treeeq(p->right, q->right))
			return 0;
	case UTYPE:
		return treeeq(p->left, q->left);
	}
}

/* remove a conversion that changes nothing: c is 'r' (right operand), 'l' (left operand) or 'p' (the node itself) */
NODE *
convelim(p, c)
	register NODE *p;
	char c;
{
	register NODE *q, *n;
	int s;

	if (c == 'r') {
		q = p->right;
		if (q->type != q->left->type) {
			s = bsize(q->left->type);
			if (s != bsize(q->type))
				goto test;
			if (!(isint(q->type) ^ isint(q->left->type)))
				goto remove;
		test:
			if (q->type != 7 || q->left->type != 6)
				return p;
		}
	remove:
		n = tcopy(q->left);
		tfree(q);
		p->right = n;
		return p;
	}
	if (c == 'l') {
		q = p->left;
		if (q->type != q->left->type) {
			s = bsize(q->left->type);
			if (s != bsize(q->type))
				goto ltest;
			if (!(isint(q->type) ^ isint(q->left->type)))
				goto lremove;
		ltest:
			if (q->type != 7 || q->left->type != 6)
				return p;
		}
	lremove:
		n = tcopy(q->left);
		tfree(q);
		p->left = n;
		return p;
	}
	if (p->type == 0) {
		n = tcopy(p->left);
		tfree(p);
		p = n;
	}
	return p;
}

/* the type is not a float */
static int
isint(t)
	register int t;
{
	return t == 6 || t == 7 ? 0 : 1;
}
