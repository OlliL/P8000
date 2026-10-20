static char sccsid[] = "@[$]expr.c\t1.2 07/18/83 13:22:11 - 87wega3.2";

/*
 * expr - expressions
 *
 * An expression is a tree of nodes: a constant (type 3), a name (7, the index of a
 * symbol) or an operation (type 25: c1 the type of the result, c2 the operator, l1 and
 * l2 the operands; the operators are 0x1A +, 0x1B -, 0x1C *, 0x1D /, 0x1E ^<, 0x1F ^>,
 * 0x20 ^&, 0x21 ^|, 0x22 ^X, 0x23 | |, 0x24 ^O, 0x25 ^S).  The functions of the parser
 * fold what is constant and leave the rest to pass 2 (exprval).  The type of an
 * expression (exprtype): 0 not known yet, 1 relocatable, 3 constant, 4 external, 5
 * short segmented address, 6 and 7 undefined or invalid.
 */
#include "cas.h"

extern NODE *opsub(), *opadd();
extern int shortop;			/* a short address was seen in pass 2 (assemble.c) */

/* the type of an expression */
unsigned char
exprtype(np)
NODE *np;
{
	struct sym *sp;

	switch (np->n_type) {
	case 3:
		return (3);
	case 7:
		sp = stabp(np->n_u.pw.w);
		if (sp->s_type == 0)
			return (7);
		if (sp->s_type != 1)
			return (6);
		if ((sp->s_7 & 6) == 0)
			return (4);
		return (1);
	case 0x19:
		return (np->n_u.cc.c1);
	default:
		error(2, "Cannot determine expression type\n");
	}
}

/* the value of a simple expression as an int: a name, or a name plus a constant */
evalint(np)
NODE *np;
{
	switch (np->n_type) {
	case 7:
		return ((int)symval(np->n_u.pw.w));
	case 0x19:
		return ((int)(symval(((NODE *)np->n_u.cc.l1)->n_u.pw.w) + ((NODE *)np->n_u.cc.l2)->n_u.l));
	default:
		error(2, "Must be simple expression\n");
	}
}

/* the relocation word of an expression (pass 2) */
long
exprreloc(np)
NODE *np;
{
	register int idx;
	register long v;

	switch (np->n_u.cc.c1) {
	case 1:
	case 4:
		idx = ((NODE *)np->n_u.cc.l1)->n_u.pw.w;
		return (symreloc(idx));
	case 5:
		switch (np->n_u.cc.c2) {
		case 0x23:
		case 0x24:
		case 0x25:
			break;
		default:
			return (0L);
		}
		if (((NODE *)np->n_u.cc.l1)->n_type == 0x19)
			v = exprreloc(np->n_u.cc.l1);
		else
			v = symreloc(((NODE *)np->n_u.cc.l1)->n_u.pw.w);
		shortop++;
		switch (np->n_u.cc.c2) {
		case 0x23:
			if ((v & 8) == 0)
				v |= 0x20;
			else
				v |= 2;
			break;
		case 0x24:
			if ((v & 8) == 0)
				;
			break;
		case 0x25:
			if ((v & 8) == 0)
				v |= 0x10;
			else
				v |= 1;
			break;
		}
		return (v);
	default:
		return (0L);
	}
}

/* the value of an expression (pass 2) */
long
exprval(np)
NODE *np;
{
	long v;
	int idx;

	switch (np->n_u.cc.c1) {
	case 1:
	case 4:
		idx = ((NODE *)np->n_u.cc.l1)->n_u.pw.w;
		if (np->n_u.cc.c1 == 4) {
			if (np->n_u.cc.c2 == 0x1A)
				v = ((NODE *)np->n_u.cc.l2)->n_u.l;
			else
				v = 0 - ((NODE *)np->n_u.cc.l2)->n_u.l;
		} else {
			v = symval(idx);
			if (np->n_u.cc.c2 == 0x1A)
				v += ((NODE *)np->n_u.cc.l2)->n_u.l;
			else
				v -= ((NODE *)np->n_u.cc.l2)->n_u.l;
		}
		nfreed += 2;
		break;
	case 0:
	case 7:
		error(2, "Undefined symbol\n");
		break;
	case 5:
		switch (np->n_u.cc.c2) {
		case 0x23:
			if (((NODE *)np->n_u.cc.l1)->n_type == 0x19)
				v = exprval(np->n_u.cc.l1);
			else
				v = symval(((NODE *)np->n_u.cc.l1)->n_u.pw.w);
			if (((unsigned long)v & 0xFFFF) > 0xFF)
				error(1, "Value too large\n");
			v = (((unsigned long)v >> 16) & 0xFF00) | (v & 0xFF);
			break;
		case 0x24:
			if (((NODE *)np->n_u.cc.l1)->n_type == 0x19)
				v = exprval(np->n_u.cc.l1);
			else
				v = symval(((NODE *)np->n_u.cc.l1)->n_u.pw.w);
			v = v & 0xFFFF;
			break;
		case 0x25:
			if (((NODE *)np->n_u.cc.l1)->n_type == 0x19)
				v = exprval(np->n_u.cc.l1);
			else
				v = symval(((NODE *)np->n_u.cc.l1)->n_u.pw.w);
			v = (unsigned long)v >> 16;
			break;
		default:
			error(2, "Expression must be relocatable\n");
		}
		nfreed++;
		break;
	case 6:
		error(2, "Erroneous expression type\n");
		break;
	}
	return (v);
}

/* |  | */
NODE *
opshort(np)
NODE *np;
{
	register NODE *p;
	unsigned char t;
	unsigned char c1;

	t = exprtype(np);
	if (t == 6)
		error(2, "Invalid expression type for |  | operator\n");
	if (t == 3) {
		p = mknode1(3, (((unsigned long)np->n_u.l >> 16) & 0xFF00) | (unsigned char)np->n_u.l);
		nfreed++;
		return (p);
	}
	switch (t) {
	case 1:
	case 4:
		c1 = 5;
		break;
	case 0:
	case 7:
		c1 = 0;
		break;
	default:
		c1 = t;
	}
	return (mknode25(c1, (unsigned char)0x23, (long)np, 0L));
}

/* ^S */
NODE *
opseg(np)
NODE *np;
{
	register NODE *p;
	unsigned char t;
	unsigned char c1;

	t = exprtype(np);
	if (t == 6)
		error(2, "Invalid expression type for ^S operator\n");
	if (t == 3) {
		p = mknode1(3, ((unsigned long)np->n_u.l >> 16) & 0xFF00);
		nfreed++;
		return (p);
	}
	switch (t) {
	case 1:
	case 4:
		c1 = 5;
		break;
	case 0:
	case 7:
		c1 = 0;
		break;
	default:
		c1 = t;
	}
	return (mknode25(c1, (unsigned char)0x25, (long)np, 0L));
}

/* ^O */
NODE *
opoff(np)
NODE *np;
{
	register NODE *p;
	unsigned char t;
	unsigned char c1;

	t = exprtype(np);
	if (t == 6)
		error(2, "Invalid expression type for ^O operator\n");
	if (t == 3) {
		p = mknode1(3, (unsigned char)np->n_u.l);
		nfreed++;
		return (p);
	}
	switch (t) {
	case 1:
	case 4:
		c1 = 5;
		break;
	case 0:
	case 7:
		c1 = 0;
		break;
	default:
		c1 = t;
	}
	return (mknode25(c1, (unsigned char)0x24, (long)np, 0L));
}

/* ^X */
NODE *
opxor(l, r)
NODE *l;
NODE *r;
{
	unsigned char rt, lt;
	NODE *np;

	rt = exprtype(r);
	if (rt == 6)
		error(2, "Invalid expression type for right side of ^X operator\n");
	lt = exprtype(l);
	if (lt == 6)
		error(2, "Invalid expression type for left side of ^X operator\n");
	if (rt == 3 && lt == 3) {
		np = mknode1(3, l->n_u.l ^ r->n_u.l);
		nfreed += 2;
		return (np);
	}
	if (rt == 0 || lt == 0)
		return (mknode25(0, (unsigned char)0x22, (long)l, (long)r));
	error(2, "Both sides of ^X must be constants\n");
}

/* ^| */
NODE *
opor(l, r)
NODE *l;
NODE *r;
{
	unsigned char rt, lt;
	NODE *np;

	rt = exprtype(r);
	if (rt == 6)
		error(2, "Invalid expression type for right side of ^| operator\n");
	lt = exprtype(l);
	if (lt == 6)
		error(2, "Invalid expression type for left side of ^| operator\n");
	if (rt == 3 && lt == 3) {
		np = mknode1(3, l->n_u.l | r->n_u.l);
		nfreed += 2;
		return (np);
	}
	if (rt == 0 || lt == 0)
		return (mknode25(0, (unsigned char)0x21, (long)l, (long)r));
	error(2, "Both sides of ^| must be constants\n");
}

/* ^& */
NODE *
opand(l, r)
NODE *l;
NODE *r;
{
	unsigned char rt, lt;
	NODE *np;

	rt = exprtype(r);
	if (rt == 6)
		error(2, "Invalid expression type for right side of ^& operator\n");
	lt = exprtype(l);
	if (lt == 6)
		error(2, "Invalid expression type for left side of ^& operator\n");
	if (rt == 3 && lt == 3) {
		np = mknode1(3, l->n_u.l & r->n_u.l);
		nfreed += 2;
		return (np);
	}
	if (rt == 0 || lt == 0)
		return (mknode25(0, (unsigned char)0x20, (long)l, (long)r));
	error(2, "Both sides of ^& must be constants\n");
}

/* ^> */
NODE *
opshr(l, r)
NODE *l;
NODE *r;
{
	unsigned char rt, lt;
	NODE *np;

	rt = exprtype(r);
	if (rt == 6)
		error(2, "Invalid expression type for right side of ^> operator\n");
	lt = exprtype(l);
	if (lt == 6)
		error(2, "Invalid expression type for left side of ^> operator\n");
	if (rt == 3 && lt == 3) {
		np = mknode1(3, (unsigned long)l->n_u.l >> r->n_u.l);
		nfreed += 2;
		return (np);
	}
	if (rt == 0 || lt == 0)
		return (mknode25(0, (unsigned char)0x1F, (long)l, (long)r));
	error(2, "Both sides of ^> must be constants\n");
}

/* ^< */
NODE *
opshl(l, r)
NODE *l;
NODE *r;
{
	unsigned char rt, lt;
	NODE *np;

	rt = exprtype(r);
	if (rt == 6)
		error(2, "Invalid expression type for right side of ^< operator\n");
	lt = exprtype(l);
	if (lt == 6)
		error(2, "Invalid expression type for left side of ^< operator\n");
	if (rt == 3 && lt == 3) {
		np = mknode1(3, l->n_u.l << r->n_u.l);
		nfreed += 2;
		return (np);
	}
	if (rt == 0 || lt == 0)
		return (mknode25(0, (unsigned char)0x1E, (long)l, (long)r));
	error(2, "Both sides of ^< must be constants\n");
}

/* * */
NODE *
opmul(l, r)
NODE *l;
NODE *r;
{
	unsigned char rt, lt;
	NODE *np;

	rt = exprtype(r);
	if (rt == 6)
		error(2, "Invalid expression type for right side of * operator\n");
	lt = exprtype(l);
	if (lt == 6)
		error(2, "Invalid expression type for left side of * operator\n");
	if (rt == 3 && lt == 3) {
		np = mknode1(3, l->n_u.l * r->n_u.l);
		nfreed += 2;
		return (np);
	}
	if (rt == 0 || lt == 0)
		return (mknode25(0, (unsigned char)0x1C, (long)l, (long)r));
	error(2, "Both sides of * must be constants\n");
}

/* / */
NODE *
opdiv(l, r)
NODE *l;
NODE *r;
{
	unsigned char rt, lt;
	NODE *np;

	rt = exprtype(r);
	if (rt == 6)
		error(2, "Invalid expression type for right side of / operator\n");
	lt = exprtype(l);
	if (lt == 6)
		error(2, "Invalid expression type for left side of / operator\n");
	if (rt == 3 && lt == 3) {
		np = mknode1(3, (unsigned long)l->n_u.l / r->n_u.l);
		nfreed += 2;
		return (np);
	}
	if (rt == 0 || lt == 0)
		return (mknode25(0, (unsigned char)0x1D, (long)l, (long)r));
	error(2, "Both sides of / must be constants\n");
}

/* unary minus */
NODE *
opneg(np)
NODE *np;
{
	NODE *zero;

	zero = mknode1(3, 0L);
	return (opsub(zero, np));
}

/* subtraction */
NODE *
opsub(l, r)
NODE *l;
NODE *r;
{
	unsigned char rt, lt;
	long lv, rv;
	NODE *np;

	rt = exprtype(r);
	if (rt == 6)
		error(2, "Invalid expression type for right side of subtraction\n");
	lt = exprtype(l);
	if (lt == 6)
		error(2, "Invalid expression type for left side of subtraction\n");
	if (rt == 3 && lt == 3) {
		np = mknode1(3, l->n_u.l - r->n_u.l);
		nfreed += 2;
		return (np);
	}
	if (rt == 0 || lt == 0)
		return (mknode25(0, (unsigned char)0x1B, (long)l, (long)r));
	if (rt == 3 && lt == 5)
		return (mknode25(5, (unsigned char)0x1B, (long)l, (long)r));
	if (rt == 5 && lt == 5)
		return (mknode25(5, (unsigned char)0x1B, (long)l, (long)r));
	if (rt == 3 && (lt == 4 || lt == 1 || lt == 7)) {
		if (l->n_type == 7)
			return (mknode25(lt, (unsigned char)0x1B, (long)l, (long)r));
		lv = ((NODE *)l->n_u.cc.l2)->n_u.l;
		rv = r->n_u.l;
		if (l->n_u.cc.c2 == 0x1A) {
			if ((unsigned long)lv > rv)
				np = mknode1(3, lv - rv);
			else {
				l->n_u.cc.c2 = 0x1B;
				np = mknode1(3, rv - lv);
			}
		} else
			np = mknode1(3, lv + rv);
		nfreed += 2;
		l->n_u.cc.l2 = (long)np;
		return (l);
	}
	if (rt == 7 || lt == 7)
		return (mknode25(0, (unsigned char)0x1B, (long)l, (long)r));
	if (rt == 1 && lt == 1) {
		if (r->n_type == 7)
			rv = symval(r->n_u.pw.w);
		else {
			rv = symval(((NODE *)r->n_u.cc.l1)->n_u.pw.w);
			if (r->n_u.cc.c2 == 0x1A)
				rv += ((NODE *)r->n_u.cc.l2)->n_u.l;
			else
				rv -= ((NODE *)r->n_u.cc.l2)->n_u.l;
			nfreed += 2;
		}
		if (l->n_type == 7)
			lv = symval(l->n_u.pw.w);
		else {
			lv = symval(((NODE *)l->n_u.cc.l1)->n_u.pw.w);
			if (l->n_u.cc.c2 == 0x1A)
				lv += ((NODE *)l->n_u.cc.l2)->n_u.l;
			else
				lv -= ((NODE *)l->n_u.cc.l2)->n_u.l;
			nfreed += 2;
		}
		nfreed += 2;
		return (mknode1(3, lv - rv));
	}
	error(2, "Invalid subtraction expression\n");
}

/* unary plus */
NODE *
opplus(np)
NODE *np;
{
	NODE *zero;

	zero = mknode1(3, 0L);
	return (opadd(zero, np));
}

/* addition */
NODE *
opadd(l, r)
NODE *l;
NODE *r;
{
	unsigned char rt, lt;
	long rv, lv;
	NODE *np;

	rt = exprtype(r);
	if (rt == 6)
		error(2, "Invalid expression type for right side of addition\n");
	lt = exprtype(l);
	if (lt == 6)
		error(2, "Invalid expression type for left side of addition\n");
	if (rt == 3 && lt == 3) {
		np = mknode1(3, r->n_u.l + l->n_u.l);
		nfreed += 2;
		return (np);
	}
	if (rt == 0 || lt == 0)
		return (mknode25(0, (unsigned char)0x1A, (long)l, (long)r));
	if (rt == 3 && lt == 5)
		return (mknode25(5, (unsigned char)0x1A, (long)l, (long)r));
	if (rt == 5 && lt == 3)
		return (mknode25(5, (unsigned char)0x1A, (long)l, (long)r));
	if ((rt == 4 || rt == 1 || rt == 7) && lt == 3) {
		if (r->n_type == 7)
			return (mknode25(rt, (unsigned char)0x1A, (long)r, (long)l));
		lv = l->n_u.l;
		rv = ((NODE *)r->n_u.cc.l2)->n_u.l;
		if (r->n_u.cc.c2 == 0x1A)
			np = mknode1(3, lv + rv);
		else if ((unsigned long)lv > rv) {
			r->n_u.cc.c2 = 0x1A;
			np = mknode1(3, lv - rv);
		} else
			np = mknode1(3, rv - lv);
		nfreed += 2;
		r->n_u.cc.l2 = (long)np;
		return (r);
	}
	if (rt == 3 && (lt == 4 || lt == 1 || lt == 7)) {
		if (l->n_type == 7)
			return (mknode25(lt, (unsigned char)0x1A, (long)l, (long)r));
		rv = r->n_u.l;
		lv = ((NODE *)l->n_u.cc.l2)->n_u.l;
		if (l->n_u.cc.c2 == 0x1A)
			np = mknode1(3, lv + rv);
		else if ((unsigned long)lv > rv)
			np = mknode1(3, lv - rv);
		else {
			l->n_u.cc.c2 = 0x1A;
			np = mknode1(3, rv - lv);
		}
		nfreed += 2;
		l->n_u.cc.l2 = (long)np;
		return (l);
	}
	if (rt == 7 || lt == 7)
		return (mknode25(0, (unsigned char)0x1A, (long)l, (long)r));
	error(2, "Invalid addition expression\n");
}
