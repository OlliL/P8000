/*
 * tr3.c - building and converting trees (codgen / scodgen)
 *
 * Reconstructed from /lib/codgen: clocal() 0xab48, makeblock() 0xaece, foldadd() 0xaf14, buildstar() 0xafe4,
 * bcon() 0xb0a0, member() 0xb0d4. The classic PCC clocal(): names become the address computation of their class.
 * Names are guesses.
 */
#include "cg.h"

#define SCONV	110
#define INCREF(t)	((((t) & ~0xF) << 2) | 0x10 | ((t) & 0xF))
#define DECREF(t)	((((t) >> 2) & ~0xF) | ((t) & 0xF))

extern int cerror(), uerror();
extern int tbytes(), szty();
extern int dope[];
extern struct node *talloc();
extern struct sym *fpsym1, *fpsym2;
struct node *makeblock(), *clocal(), *bcon(), *member(), *buildstar();

/* a new node */
struct node *
makeblock(op, l, r, type)
	int op;
	struct node *l, *r;
	int type;
{
	register struct node *n;

	n = talloc();
	n->op = op;
	n->left = l;
	n->right = r;
	n->type = type;
	n->su = 0;
	n->fsu = 0;
	n->rall = 0;
	return n;
}

/* an integer constant of the given number of bits, as bytes */
struct node *
bcon(bits)
	long bits;
{
	register struct node *n;

	n = makeblock(ICON, 0, 0, 4);
	n->sname = 0;
	n->lval = bits / 8;
	return n;
}

#ifdef SEG
/* PLUS(frame register, offset): the offset goes first and takes the pointer type, the register becomes an int */
static void
chk(p)
	register struct node *p;
{
	register struct node *r, *c;

	r = p->left;
	c = p->right;
	c->type = r->type;
	p->left = c;
	if (r->op != REG)
		cerror("badly formed offset");
	p->right = r;
	r->type = 4;
}
#else
/* a debugging hook that does nothing */
static void
chk(p)
	struct node *p;
{
}
#endif

/* the node p after the conversions: names to addresses of their class, conversions of constants */
struct node *
clocal(p)
	struct node *p;
{
	register struct node *l, *r;
	struct sym *s;
	struct node *fp, *n, *q;
	long val;
	int o, pf, lf, pt, lt, t, ob;

	l = p->left;
	r = p->right;
	o = p->op;
	if (o == NAME) {
		s = p->sname;
		switch (s->sclass) {
		case 1:
		case 9:
			fp = makeblock(REG, 0, 0, 0x18);
			fp->lval = s->soffset / 8;
			fp->rval = 15;
			n = makeblock(STREF, fp, p, 0);
			p = member(n);
			q = p->left;
			if (q->op == PLUS) {
				if (q->right->op == ICON) {
					if (s->sclass == 9 && (s->sflags >> 3 & 1))
						q->right->sname = fpsym2;
					else
						q->right->sname = fpsym1;
				}
				chk(q);
			}
			break;
		case 3:
		case 6:
			if (s->sflags >> 2 & 1)
				return p;
			p->lval = 0;
			p->rval = -(int)s->soffset;
			break;
		case 4:
			p->op = REG;
			p->lval = 0;
			p->rval = s->sbyte5;
			break;
		default:
			cerror("lcl: NAME op has illegal class\n");
		}
		l = p->left;
		r = p->right;
	} else if (o == SCONV) {
		pf = p->type == 6 || p->type == 7;
		lf = l->type == 6 || l->type == 7;
		if (pf || lf)
			goto done;
		while (l->op == SCONV) {
			if (tbytes(l->type) <= tbytes(p->type))
				break;
			if (l->left->type == 7 || l->left->type == 6)
				break;
			l->op = FREE;
			l = l->left;
			p->left = l;
		}
		pt = p->type;
		lt = l->type;
		if (l->op == ICON) {
			val = l->lval;
#ifdef SEG
			if (pt == 4 && (lt & 0x30) == 0x10) {
				l->lval = (long)(int)val;
				goto done;
			}
			if (pt == 14 && (lt & 0x30) == 0x10) {
				l->lval = val & 0xFFFFL;
				goto done;
			}
#endif
			switch (pt) {
			case 2:
				l->lval = (long)(char)val;
				break;
			case 4:
				l->lval = (long)(int)val;
				break;
			case 12:
				l->lval = val & 0xFF;
				break;
			case 14:
				l->lval = val & 0xFFFFL;
				break;
			}
			l->type = pt;
		} else {
			if (l->op == MUL) {
				if (szty(pt) - szty(lt) != 1)
					if (tbytes(lt) != tbytes(pt))
						goto done;
			} else if (tbytes(lt) != tbytes(pt))
				goto done;
			l->type = pt;
		}
		if (tbytes(pt) != tbytes(lt)) {
#ifdef SEG
			goto done;
#else
			p->op = FREE;
			return l;
#endif
		}
#ifdef SEG
		if ((pt & 0x30) == 0x10)
			goto done;
#endif
		l->type = p->type;
		l->rval = p->rval;
		l->lval = p->lval;
		l->sname = p->sname;
		return l;
	}
done:
	l = p->left;
	r = p->right;
	if ((dope[p->op] & 0x10) && r->op == ICON && l->op == SCONV &&
	    (l->left->type == 2 || l->left->type == 12) && r->lval >= -256 && r->lval <= 255) {
		l->op = FREE;
		p->left = l->left;
		l = p->left;
		r->type = l->type;
		p->type = l->type;
	}
	return p;
}

/* the address of a structure member: the base pointer plus the offset of the member, then the indirection */
struct node *
member(p)
	struct node *p;
{
	register struct node *base;
	struct sym *m;
	struct node *off, *n;
	int nt;

	m = p->right->sname;
	p->right->op = FREE;
	p->op = FREE;
	base = p->left;
	nt = INCREF(m->stype);
	base->type = nt;
	off = bcon(m->soffset);
	n = makeblock(PLUS, base, off, nt);
	base = clocal(n);
	if ((base->type & 0x30) != 0x10)
		base->type += 16;
	return buildstar(STAR, base, 0);
}

/* a constant added to or subtracted from an address that has one already: folded */
struct node *
foldadd(p)
	register struct node *p;
{
	register struct node *l, *r, *q;
	int lo, x;

	l = p->left;
	r = p->right;
	lo = l->op;
	q = lo == PLUS || lo == MINUS ? l->left : l;
	if (q->op == ICON && r->op == ICON) {
		x = (int)q->lval;
		switch (p->op) {
		case PLUS:
			x += (int)r->lval;
			break;
		case MINUS:
			x -= (int)r->lval;
			break;
		default:
			cerror("bad op in convert");
		}
		q->lval = (long)x;
		p->op = FREE;
		r->op = FREE;
		l->type = p->type;
		return l;
	}
	if (lo == PLUS || lo == MINUS) {
		if (lo == MINUS)
			p->op = p->op == MINUS ? PLUS : MINUS;
		l->right = buildstar(p->op, l->right, r);
		p->op = FREE;
		p = l;
	}
	return p;
}

/* a unary tree of the operator op over l (and r): * and & cancel each other, the type is the one below or above */
struct node *
buildstar(op, l, r)
	int op;
	register struct node *l, *r;
{
	register struct node *n;

	n = makeblock(op, l, r, 4);
	if (n->op == STAR) {
		if (l->op == UAND) {
			l->op = FREE;
			n->op = FREE;
			n = l->left;
		}
		if ((l->type & 0x30) != 0x10)
			uerror("illegal indirection");
		n->type = DECREF(l->type);
		n->su = l->su;
		n->fsu = l->fsu;
		n->rall = l->rall;
	} else if (n->op == UAND && l->op == STAR) {
		l->op = FREE;
		n->op = FREE;
		n = l->left;
		n->type = INCREF(l->type);
		n->su = l->su;
		n->fsu = l->fsu;
		n->rall = l->rall;
	}
	return clocal(n);
}
