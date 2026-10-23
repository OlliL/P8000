/*
 * optim3.c - local optimizations of the trees (codgen / scodgen)
 *
 * Reconstructed from /lib/codgen: optim() 0xb178, conval() 0xb774, treeeq() 0xb9ea, convelim() 0xba7c, isint() 0xbba2.
 * The classic PCC optim(): constants are collected, operand order is made canonical, redundant conversions are removed.
 * Names are guesses.
 */
#include "cg.h"

#define SCONV	110
#define NONAME	0

extern int cerror(), uerror();
extern int bsize(), ispow2_();
extern int ispow2();
extern int plainicon();
extern void tfree(), ncopy();
extern struct node *tcopy();
extern int dope[];

static int rev[] = { EQ, NE, GE, GT, LE, LT, UGE, UGT, ULE, ULT };

/* is p a constant without a name */
#define nncon(p)	plainicon(p)

struct node *optim();
int conval(), treeeq();
struct node *convelim();

/* the type is not a float */
static int
isint(t)
	int t;
{
	return t == 6 || t == 7 ? 0 : 1;
}

/* remove a conversion that changes nothing: c is 'r' (right operand), 'l' (left operand) or 'p' (the node itself) */
struct node *
convelim(p, c)
	register struct node *p;
	int c;
{
	register struct node *q, *n;
	int s;

	if (c == 'r') {
		q = p->right;
		if (q->type != q->left->type) {
			s = bsize(q->left->type);
			if (s == bsize(q->type)) {
				if (isint(q->left->type) ^ isint(q->type))
					goto test;
			} else
				goto test;
		}
	remove:
		n = tcopy(q->left);
		tfree(q);
		p->right = n;
		return p;
	test:
		if (q->type == 7 && q->left->type == 6)
			goto remove;
		return p;
	}
	if (c == 'l') {
		q = p->left;
		if (q->type != q->left->type) {
			s = bsize(q->left->type);
			if (s == bsize(q->type)) {
				if (isint(q->left->type) ^ isint(q->type))
					goto ltest;
			} else
				goto ltest;
		}
	lremove:
		n = tcopy(q->left);
		tfree(q);
		p->left = n;
		return p;
	ltest:
		if (q->type == 7 && q->left->type == 6)
			goto lremove;
		return p;
	}
	if (p->type == 0) {
		n = tcopy(p->left);
		tfree(p);
		p = n;
	}
	return p;
}

/* are the two trees the same */
int
treeeq(p, q)
	register struct node *p, *q;
{
	register int o;

	o = p->op;
	if (o != q->op)
		return 0;
	switch (dope[o] & 0xE) {
	case LTYPE:
		if (o == NAME)
			return p->sname == q->sname && p->lval == q->lval;
		return p->lval == q->lval && p->rval == q->rval;
	case UTYPE:
		return treeeq(p->left, q->left);
	case BITYPE:
		return treeeq(p->right, q->right) && treeeq(p->left, q->left);
	}
}

/* fold the constant q into p by the operator o; 0 if that is not possible */
int
conval(p, o, q)
	register struct node *p;
	int o;
	register struct node *q;
{
	long val;
	int unsgn;
	int sh;

	val = q->lval;
	unsgn = (p->type >= 12 && p->type <= 15) || (q->type >= 12 && q->type <= 15);
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
		if ((p->type & 0x30) == 0x10) {		/* the difference of pointer constants keeps only its low byte */
			p->lval = (p->lval - val) & 0xFFL;
			break;
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
		sh = q->lval;
		p->lval <<= sh;
		break;
	case RS:
		sh = q->lval;
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
	case EQ:
		p->lval = p->lval == val;
		break;
	case NE:
		p->lval = p->lval != val;
		break;
	case LE:
		p->lval = p->lval <= val;
		break;
	case LT:
		p->lval = p->lval < val;
		break;
	case GE:
		p->lval = p->lval >= val;
		break;
	case GT:
		p->lval = p->lval > val;
		break;
	case ULE:
		p->lval = p->lval - val <= 0;
		break;
	case ULT:
		p->lval = p->lval - val < 0;
		break;
	case UGE:
		p->lval = p->lval - val >= 0;
		break;
	case UGT:
		p->lval = p->lval - val > 0;
		break;
	default:
		return 0;
	}
	return 1;
}

/* local optimizations: collect constants, constants to the right, towers of operators to the left */
struct node *
optim(p)
	register struct node *p;
{
	register int o, ty;
	struct node *sp, *t1, *t2, *t3, *r;
	int i;

	o = p->op;
	ty = dope[o] & 0xE;
	if (ty == LTYPE)
		return p;
	if (ty == BITYPE && p->right->op == SCONV) {
		p = convelim(p, 'r');
		o = p->op;
		ty = dope[o] & 0xE;
	}
	if (p->left->op == SCONV) {
		p = convelim(p, 'l');
		o = p->op;
		ty = dope[o] & 0xE;
	}
	if (p->op == SCONV) {
		p = convelim(p, 'p');
		o = p->op;
		ty = dope[o] & 0xE;
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
			t1 = tcopy(p->left->right);
			t2 = tcopy(p->right->right);
			goto joined;
		}
		if (p->left->op == MINUS && p->right->op == MINUS && treeeq(p->left->left, p->right->left) &&
		    p->left->right->type == p->type && p->right->right->type == p->type) {
			t2 = tcopy(p->left->right);
			t1 = tcopy(p->right->right);
		joined:
			tfree(p->left);
			tfree(p->right);
			p->left = t1;
			p->right = t2;
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
		if ((o == MINUS || o == PLUS) && nncon(p->right) && p->right->lval == 0)
			goto zapright;
		if (p->right->op == o && (p->type & 0x30) != 0x10 && (p->right->left->type & 0x30) != 0x10) {
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
		    conval(p->right, MINUS, p->left->right))
			goto zapleft;
		if (p->right->op == ICON && p->left->op == o && p->left->right->op == ICON &&
		    conval(p->right, o, p->left->right)) {
		zapleft:
			p->left->right->op = FREE;
			p->left->op = FREE;
			p->left = p->left->left;
		} else if ((p->type & 0x30) == 0x10 && p->left->op == ICON && p->right->op == o &&
		    p->right->right->op == ICON && conval(p->left, o, p->right->right)) {
			p->right->right->op = FREE;
			p->right->op = FREE;
			p->right = p->right->left;
		} else if (p->left->op == ICON && p->right->op == ICON && conval(p->left, o, p->right)) {
		zapright:
			p->right->op = FREE;
			goto setuleft;
		}
		if (o == PLUS && nncon(p->right) && p->right->lval < 0) {
			p->right->lval = -p->right->lval;
			p->op = MINUS;
			o = MINUS;
		}
		break;
	case ASG PLUS:
	case ASG MINUS:
		if (nncon(p->right) && p->right->lval == 0)
			goto zapright;
		break;
	case DIV:
	case ASG DIV:
	case MOD:
	case ASG MOD:
		r = p->right;
		if (p->type < 12 || p->type > 15)
			break;
		if (!nncon(r))
			break;
		if (ispow2(r->lval) <= 0)
			break;
		if (o == MOD || o == ASG MOD) {
			p->op = o == ASG MOD ? ASG AND : AND;
			r->lval -= 1;
			r->type = p->left->type;
			p->right->type = r->type;
		} else {
			p->op = o == ASG DIV ? ASG RS : RS;
			r->lval = ispow2(r->lval);
		}
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
		p->op = rev[p->op - EQ];
		if (p->right->op != ICON)
			break;
		if (!conval(p->left, p->op, p->right))
			break;
		sp = p->left;
		tfree(p->right);
		ncopy(p, sp);
		tfree(sp);
		break;
	}
	return p;
}
