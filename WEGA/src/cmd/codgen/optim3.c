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

	o = p->in.op;
	ty = optype(o);
	if (ty == LTYPE)
		return p;
	if (ty == BITYPE && p->in.right->in.op == SCONV) {
		p = convelim(p, 'r');
		o = p->in.op;
		ty = optype(o);
	}
	if (p->in.left->in.op == SCONV) {
		p = convelim(p, 'l');
		o = p->in.op;
		ty = optype(o);
	}
	if (p->in.op == SCONV) {
		p = convelim(p, 'p');
		o = p->in.op;
		ty = optype(o);
	}
	if (ty == BITYPE)
		p->in.right = optim(p->in.right);
	p->in.left = optim(p->in.left);
#ifdef OLD_D24
	/* the conversion of a long to a pointer masks the address (template 040a): the mask 0x7f00ffff in front of it is dropped */
	if (o == SCONV && (p->in.type & 0x30) == 0x10 && p->in.left->in.op == AND && p->in.left->in.right->in.op == ICON &&
	    p->in.left->in.right->in.sname == 0 && p->in.left->in.right->tn.lval == 0x7F00FFFFL) {
		NODE *a;

		a = p->in.left;
		p->in.left = a->in.left;
		a->in.right->in.op = FREE;
		a->in.op = FREE;
	}
#endif
	switch (o) {
	case UAND:
		if (p->in.left->in.op != NAME)
			cerror("& error");
		p->in.left->in.op = ICON;
	setuleft:
		p->in.left->in.type = p->in.type;
		p->in.op = FREE;
		return p->in.left;
	case STAR:
		if (p->in.left->in.op != ICON)
			break;
		p->in.left->in.op = NAME;
		goto setuleft;
	case MINUS:
		if (p->in.left->in.op == PLUS && p->in.right->in.op == PLUS && treeeq(p->in.left->in.left, p->in.right->in.left) &&
		    p->in.left->in.right->in.type == p->in.type && p->in.right->in.right->in.type == p->in.type) {
			a = tcopy(p->in.left->in.right);
			b = tcopy(p->in.right->in.right);
			goto joined;
		}
		goto minus1;
	minus2:
		if (p->in.right->in.op == MINUS && treeeq(p->in.left->in.left, p->in.right->in.left) &&
		    p->in.left->in.right->in.type == p->in.type && p->in.right->in.right->in.type == p->in.type) {
			b = tcopy(p->in.left->in.right);
			a = tcopy(p->in.right->in.right);
		joined:
			tfree(p->in.left);
			tfree(p->in.right);
			p->in.left = a;
			p->in.right = b;
		minus1:
			if (p->in.left->in.op == MINUS)
				goto minus2;
		}
		if (treeeq(p->in.left, p->in.right) && p->in.type != 6 && p->in.type != 7) {
			tfree(p->in.left);
			tfree(p->in.right);
			p->in.op = ICON;
			p->in.sname = 0;
			p->tn.rval = 0;
			p->tn.lval = 0;
			return p;
		}
		if (!nncon(p->in.right))
			break;
		p->in.right->tn.lval = -p->in.right->tn.lval;
		p->in.op = PLUS;
		o = PLUS;
	case MUL:
	case PLUS:
	case AND:
	case OR:
	case ER:
		if (nncon(p->in.left) || (p->in.left->in.op == ICON && p->in.right->in.op != ICON &&
		    !(p->in.right->in.op == REG && p->in.right->tn.rval == 15) && (p->in.type & 0x30) != 0x10)) {
			sp = p->in.left;
			p->in.left = p->in.right;
			p->in.right = sp;
		}
#if defined(OLD_D13) && defined(SEG)
		/* vc: int + constant address: the address goes to the left (ldl rr2,#addr; add r3,i); the shipped codgen
		   makes an int register the target of the pointer and jams */
		if (o == PLUS && (p->in.type & 0x30) == 0x10 && p->in.right->in.op == ICON && (p->in.right->in.type & 0x30) == 0x10 &&
		    (p->in.left->in.type & 0x30) != 0x10) {
			sp = p->in.left;
			p->in.left = p->in.right;
			p->in.right = sp;
		}
#endif
#ifndef OLD_D23
		if ((o == MINUS || o == PLUS) && nncon(p->in.right) && p->in.right->tn.lval == 0)
			goto zapright;
#endif
		if (p->in.right->in.op == o && (p->in.type & 0x30) != 0x10 && (p->in.right->in.left->in.type & 0x30) != 0x10) {
			NODE *t1, *t2, *t3;

			t1 = p->in.left;
			sp = p->in.right;
			t2 = sp->in.left;
			t3 = sp->in.right;
			p->in.left = sp;
			sp->in.left = t1;
			sp->in.right = t2;
			p->in.right = t3;
		}
		if (o == PLUS && p->in.left->in.op == MINUS && p->in.right->in.op == ICON && p->in.left->in.right->in.op == ICON &&
		    conval(p->in.right, MINUS, p->in.left->in.right)) {
		zapleft:
			p->in.left->in.right->in.op = FREE;
			p->in.left->in.op = FREE;
			p->in.left = p->in.left->in.left;
		}
		if (p->in.right->in.op == ICON && p->in.left->in.op == o && p->in.left->in.right->in.op == ICON &&
		    conval(p->in.right, o, p->in.left->in.right))
			goto zapleft;
		if ((p->in.type & 0x30) == 0x10 && p->in.left->in.op == ICON && p->in.right->in.op == o &&
		    p->in.right->in.right->in.op == ICON && conval(p->in.left, o, p->in.right->in.right)) {
			p->in.right->in.right->in.op = FREE;
			p->in.right->in.op = FREE;
			p->in.right = p->in.right->in.left;
		neg:
			if (o == PLUS && nncon(p->in.right) && p->in.right->tn.lval < 0) {
				p->in.right->tn.lval = -p->in.right->tn.lval;
				p->in.op = MINUS;
				o = MINUS;
			}
			break;
		}
		if (p->in.left->in.op == ICON && p->in.right->in.op == ICON && conval(p->in.left, o, p->in.right)) {
		zapright:
			p->in.right->in.op = FREE;
			goto setuleft;
		}
		goto neg;
	case DIV:
	case ASG DIV:
	case MOD:
	case ASG MOD:
		a = p->in.right;
		if (!((unsigned)p->in.type <= 15 && (unsigned)p->in.type >= 12))
			break;
		if (!nncon(a))
			break;
		if (ispow2(a->tn.lval) <= 0)
			break;
		if (o == MOD || o == ASG MOD) {
			p->in.op = o == ASG MOD ? ASG AND : AND;
			a->tn.lval -= 1;
			a->in.type = p->in.type = p->in.left->in.type;
		} else {
			p->in.op = o == ASG DIV ? ASG RS : RS;
			a->tn.lval = ispow2(a->tn.lval);
		}
		break;
	case ASG PLUS:
	case ASG MINUS:
		if (nncon(p->in.right) && p->in.right->tn.lval == 0)
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
		if (p->in.left->in.op != ICON)
			break;
		sp = p->in.left;
		p->in.left = p->in.right;
		p->in.right = sp;
		p->in.op = revrel[p->in.op - EQ];
		if (p->in.right->in.op != ICON)
			break;
		if (!conval(p->in.left, p->in.op, p->in.right))
			break;
		sp = p->in.left;
		tfree(p->in.right);
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

	val = q->tn.lval;
	unsgn = ((unsigned)p->in.type <= 15 && (unsigned)p->in.type >= 12) || ((unsigned)q->in.type <= 15 && (unsigned)q->in.type >= 12);
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
		if ((p->in.type & 0x30) == 0x10) {
			p->tn.lval = ((val & 0x7F00FFFFL) + p->tn.lval) & 0x7F00FFFFL;
			return 1;
		}
#endif
		p->tn.lval += val;
		if (nncon(p)) {
			p->in.type = q->in.type;
			p->tn.rval = q->tn.rval;
			p->in.sname = q->in.sname ? q->in.sname : p->in.sname;
		}
		break;
	case MINUS:
#ifdef SEG
		if ((p->in.type & 0x30) == 0x10) {		/* the difference of pointer constants: its offset */
			p->tn.lval = (p->tn.lval - val) & 0xFFL;
			return 1;
		}
#endif
		p->tn.lval -= val;
		break;
	case MUL:
		p->tn.lval *= val;
		break;
	case DIV:
		if (val == 0) {
			uerror("division by 0");
			return 1;
		}
		p->tn.lval /= val;
		break;
	case MOD:
		if (val == 0) {
			uerror("division by 0");
			return 1;
		}
		p->tn.lval %= val;
		break;
	case AND:
		p->tn.lval &= val;
		break;
	case OR:
		p->tn.lval |= val;
		break;
	case ER:
		p->tn.lval ^= val;
		break;
	case LS:
		sh = val;
		p->tn.lval <<= sh;
		break;
	case RS:
		sh = val;
		p->tn.lval >>= sh;
		break;
	case UMINUS:
		p->tn.lval = -p->tn.lval;
		break;
	case COMPL:
		p->tn.lval = ~p->tn.lval;
		break;
	case NOT:
		p->tn.lval = !p->tn.lval;
		break;
	case LT:
		p->tn.lval = p->tn.lval < val;
		break;
	case LE:
		p->tn.lval = p->tn.lval <= val;
		break;
	case GT:
		p->tn.lval = p->tn.lval > val;
		break;
	case GE:
		p->tn.lval = p->tn.lval >= val;
		break;
	case ULT:
		p->tn.lval = p->tn.lval - val < 0;
		break;
	case ULE:
		p->tn.lval = p->tn.lval - val <= 0;
		break;
	case UGE:
		p->tn.lval = p->tn.lval - val >= 0;
		break;
	case UGT:
		p->tn.lval = p->tn.lval - val > 0;
		break;
	case EQ:
		p->tn.lval = p->tn.lval == val;
		break;
	case NE:
		p->tn.lval = p->tn.lval != val;
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

	o = p->in.op;
	if (o != q->in.op)
		return 0;
	switch optype(o) {
	case LTYPE:
		if (o == NAME)
			return p->in.sname == q->in.sname && p->tn.lval == q->tn.lval;
		return p->tn.lval == q->tn.lval && p->tn.rval == q->tn.rval;
	case BITYPE:
		if (!treeeq(p->in.right, q->in.right))
			return 0;
	case UTYPE:
		return treeeq(p->in.left, q->in.left);
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
		q = p->in.right;
#ifdef OLD_D24
		if ((q->in.type & 0x30) != (q->in.left->in.type & 0x30) && ((q->in.type & 0x30) == 0x10 || (q->in.left->in.type & 0x30) == 0x10))
			return p;
#endif
		if (q->in.type != q->in.left->in.type) {
			s = bsize(q->in.left->in.type);
			if (s != bsize(q->in.type))
				goto test;
			if (!(isint(q->in.type) ^ isint(q->in.left->in.type)))
				goto remove;
		test:
			if (q->in.type != 7 || q->in.left->in.type != 6)
				return p;
		}
	remove:
		n = tcopy(q->in.left);
		tfree(q);
		p->in.right = n;
		return p;
	}
	if (c == 'l') {
		q = p->in.left;
#ifdef OLD_D24
		if ((q->in.type & 0x30) != (q->in.left->in.type & 0x30) && ((q->in.type & 0x30) == 0x10 || (q->in.left->in.type & 0x30) == 0x10))
			return p;
#endif
		if (q->in.type != q->in.left->in.type) {
			s = bsize(q->in.left->in.type);
			if (s != bsize(q->in.type))
				goto ltest;
			if (!(isint(q->in.type) ^ isint(q->in.left->in.type)))
				goto lremove;
		ltest:
			if (q->in.type != 7 || q->in.left->in.type != 6)
				return p;
		}
	lremove:
		n = tcopy(q->in.left);
		tfree(q);
		p->in.left = n;
		return p;
	}
	if (p->in.type == 0) {
		n = tcopy(p->in.left);
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
