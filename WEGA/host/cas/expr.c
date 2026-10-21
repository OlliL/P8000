/*
 * expr - expressions
 *
 * An expression is a tree of nodes: a constant (type 3), a name (7, the index of a
 * symbol) or an operation (type 25: n_c1 the type of the result, n_c2 the operator, n_a and
 * n_b the operands; the operators are 0x1A +, 0x1B -, 0x1C *, 0x1D /, 0x1E ^<, 0x1F ^>,
 * 0x20 ^&, 0x21 ^|, 0x22 ^X, 0x23 | |, 0x24 ^O, 0x25 ^S). The functions of the parser
 * fold what is constant and leave the rest to pass 2 (exprval). The type of an
 * expression (exprtype): 0 not known yet, 1 relocatable, 3 constant, 4 external, 5
 * short segmented address, 6 and 7 undefined or invalid.
 *
 * After an error message the functions return the constant 0 (the original returns what
 * the registers hold).
 */
#include "cas.h"

int shortop;				/* a short address was seen in pass 2 (read and cleared in assemble.c) */

/* the type of an expression */
int
exprtype(NODE *np)
{
	struct sym *sp;

	switch (np->n_type) {
	case 3:
		return (3);
	case 7:
		sp = symof(np->n_w);
		if (sp->s_type == 0)
			return (7);
		if (sp->s_type != 1)
			return (6);
		if ((sp->s_7 & 6) == 0)
			return (4);
		return (1);
	case 0x19:
		return (np->n_c1);
	default:
		error(2, "Cannot determine expression type\n");
		return (6);
	}
}

/* the value of a simple expression as an int: a name, or a name plus a constant */
int
evalint(NODE *np)
{
	switch (np->n_type) {
	case 7:
		return ((int16_t)symval(np->n_w));
	case 0x19:
		return ((int16_t)(symval(np->n_a->n_w) + np->n_b->n_val));
	default:
		error(2, "Must be simple expression\n");
		return (0);
	}
}

/* the relocation word of an expression (pass 2) */
int32_t
exprreloc(NODE *np)
{
	uint32_t v;

	switch (np->n_c1) {
	case 1:
	case 4:
		return (symreloc(np->n_a->n_w));
	case 5:
		switch (np->n_c2) {
		case 0x23:
		case 0x24:
		case 0x25:
			break;
		default:
			return (0);
		}
		if (np->n_a->n_type == 0x19)
			v = exprreloc(np->n_a);
		else
			v = symreloc(np->n_a->n_w);
		shortop++;
		switch (np->n_c2) {
		case 0x23:
			if ((v & 8) == 0)
				v |= 0x20;
			else
				v |= 2;
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
		return (0);
	}
}

/* the value of the left operand of a short address operation */
static int32_t
shortval(NODE *np)
{
	if (np->n_a->n_type == 0x19)
		return (exprval(np->n_a));
	return (symval(np->n_a->n_w));
}

/* the value of an expression (pass 2) */
int32_t
exprval(NODE *np)
{
	int32_t v;

	v = 0;
	switch (np->n_c1) {
	case 1:
	case 4:
		if (np->n_c1 == 4) {
			if (np->n_c2 == 0x1A)
				v = np->n_b->n_val;
			else
				v = 0 - np->n_b->n_val;
		} else {
			v = symval(np->n_a->n_w);
			if (np->n_c2 == 0x1A)
				v += np->n_b->n_val;
			else
				v -= np->n_b->n_val;
		}
		nfreed += 2;
		break;
	case 0:
	case 7:
		error(2, "Undefined symbol\n");
		break;
	case 5:
		switch (np->n_c2) {
		case 0x23:
			v = shortval(np);
			if (((uint32_t)v & 0xFFFF) > 0xFF)
				error(1, "Value too large\n");
			v = (((uint32_t)v >> 16) & 0xFF00) | (v & 0xFF);
			break;
		case 0x24:
			v = shortval(np) & 0xFFFF;
			break;
		case 0x25:
			v = (uint32_t)shortval(np) >> 16;
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

/* the result type of | |, ^S and ^O for an operand of type t */
static int
shorttype(int t)
{
	switch (t) {
	case 1:
	case 4:
		return (5);
	case 0:
	case 7:
		return (0);
	default:
		return (t);
	}
}

/* |  | */
NODE *
opshort(NODE *np)
{
	NODE *p;
	int t;

	t = exprtype(np);
	if (t == 6)
		error(2, "Invalid expression type for |  | operator\n");
	if (t == 3) {
		p = mknode1(3, (((uint32_t)np->n_val >> 16) & 0xFF00) | (uint8_t)np->n_val);
		nfreed++;
		return (p);
	}
	return (mknode25(shorttype(t), 0x23, np, 0));
}

/* ^S */
NODE *
opseg(NODE *np)
{
	NODE *p;
	int t;

	t = exprtype(np);
	if (t == 6)
		error(2, "Invalid expression type for ^S operator\n");
	if (t == 3) {
		p = mknode1(3, ((uint32_t)np->n_val >> 16) & 0xFF00);
		nfreed++;
		return (p);
	}
	return (mknode25(shorttype(t), 0x25, np, 0));
}

/* ^O */
NODE *
opoff(NODE *np)
{
	NODE *p;
	int t;

	t = exprtype(np);
	if (t == 6)
		error(2, "Invalid expression type for ^O operator\n");
	if (t == 3) {
		p = mknode1(3, np->n_val);
		nfreed++;
		return (p);
	}
	return (mknode25(shorttype(t), 0x24, np, 0));
}

/*
 * The shift counts are the low byte of the operand as a signed number; a negative count
 * shifts the other way, a left shift of a negative number and every right shift
 * with a count of 32 or more gives the sign (left) or 0.
 */
static int32_t
shl32(int32_t v, int32_t count)
{
	int c = (int8_t)count;

	if (c >= 0)
		return (c >= 32 ? 0 : (int32_t)((uint32_t)v << c));
	return (c <= -32 ? (v < 0 ? -1 : 0) : v >> -c);
}

static int32_t
shr32(int32_t v, int32_t count)
{
	int c = (int8_t)count;

	if (c >= 0)
		return (c >= 32 ? 0 : (int32_t)((uint32_t)v >> c));
	return (c <= -32 ? 0 : (int32_t)((uint32_t)v << -c));
}

static int32_t
div32(int32_t l, int32_t r)
{
	return (r == 0 ? l : (int32_t)((uint32_t)l / (uint32_t)r));
}

static int32_t xor32(int32_t l, int32_t r) { return (l ^ r); }
static int32_t or32(int32_t l, int32_t r) { return (l | r); }
static int32_t and32(int32_t l, int32_t r) { return (l & r); }
static int32_t mul32(int32_t l, int32_t r) { return ((uint32_t)l * (uint32_t)r); }

/*
 * the operators that only fold constants: both operands must be constants (or not known yet);
 * what is the text of the operator in the messages, op its code in the node
 */
static NODE *
binop(NODE *l, NODE *r, int op, const char *what, const char *kind, int32_t (*fold)(int32_t, int32_t))
{
	char msg[96];
	int rt, lt;
	NODE *np;

	rt = exprtype(r);
	if (rt == 6) {
		snprintf(msg, sizeof msg, "Invalid expression type for right side of %s%s\n", what, kind);
		error(2, msg);
	}
	lt = exprtype(l);
	if (lt == 6) {
		snprintf(msg, sizeof msg, "Invalid expression type for left side of %s%s\n", what, kind);
		error(2, msg);
	}
	if (rt == 3 && lt == 3) {
		np = mknode1(3, fold(l->n_val, r->n_val));
		nfreed += 2;
		return (np);
	}
	if (rt == 0 || lt == 0)
		return (mknode25(0, op, l, r));
	snprintf(msg, sizeof msg, "Both sides of %s must be constants\n", what);
	error(2, msg);
	return (errnode());
}

/* ^X */
NODE *
opxor(NODE *l, NODE *r)
{
	return (binop(l, r, 0x22, "^X", " operator", xor32));
}

/* ^| */
NODE *
opor(NODE *l, NODE *r)
{
	return (binop(l, r, 0x21, "^|", " operator", or32));
}

/* ^& */
NODE *
opand(NODE *l, NODE *r)
{
	return (binop(l, r, 0x20, "^&", " operator", and32));
}

/* ^> */
NODE *
opshr(NODE *l, NODE *r)
{
	return (binop(l, r, 0x1F, "^>", " operator", shr32));
}

/* ^< */
NODE *
opshl(NODE *l, NODE *r)
{
	return (binop(l, r, 0x1E, "^<", " operator", shl32));
}

/* * */
NODE *
opmul(NODE *l, NODE *r)
{
	return (binop(l, r, 0x1C, "*", " operator", mul32));
}

/* / */
NODE *
opdiv(NODE *l, NODE *r)
{
	return (binop(l, r, 0x1D, "/", " operator", div32));
}

/* unary minus */
NODE *
opneg(NODE *np)
{
	return (opsub(mknode1(3, 0), np));
}

/* the value of a name or of name +- constant, for the difference of two relocatable ones */
static int32_t
relval(NODE *np)
{
	int32_t v;

	if (np->n_type == 7)
		return (symval(np->n_w));
	v = symval(np->n_a->n_w);
	if (np->n_c2 == 0x1A)
		v += np->n_b->n_val;
	else
		v -= np->n_b->n_val;
	nfreed += 2;
	return (v);
}

/* subtraction */
NODE *
opsub(NODE *l, NODE *r)
{
	int rt, lt;
	int32_t lv, rv;
	NODE *np;

	rt = exprtype(r);
	if (rt == 6)
		error(2, "Invalid expression type for right side of subtraction\n");
	lt = exprtype(l);
	if (lt == 6)
		error(2, "Invalid expression type for left side of subtraction\n");
	if (rt == 3 && lt == 3) {
		np = mknode1(3, l->n_val - r->n_val);
		nfreed += 2;
		return (np);
	}
	if (rt == 0 || lt == 0)
		return (mknode25(0, 0x1B, l, r));
	if (rt == 3 && lt == 5)
		return (mknode25(5, 0x1B, l, r));
	if (rt == 5 && lt == 5)
		return (mknode25(5, 0x1B, l, r));
	if (rt == 3 && (lt == 4 || lt == 1 || lt == 7)) {
		if (l->n_type == 7)
			return (mknode25(lt, 0x1B, l, r));
		lv = l->n_b->n_val;
		rv = r->n_val;
		if (l->n_c2 == 0x1A) {
			if ((uint32_t)lv > (uint32_t)rv)
				np = mknode1(3, lv - rv);
			else {
				l->n_c2 = 0x1B;
				np = mknode1(3, rv - lv);
			}
		} else
			np = mknode1(3, lv + rv);
		nfreed += 2;
		l->n_b = np;
		return (l);
	}
	if (rt == 7 || lt == 7)
		return (mknode25(0, 0x1B, l, r));
	if (rt == 1 && lt == 1) {
		rv = relval(r);
		lv = relval(l);
		nfreed += 2;
		return (mknode1(3, lv - rv));
	}
	error(2, "Invalid subtraction expression\n");
	return (errnode());
}

/* unary plus */
NODE *
opplus(NODE *np)
{
	return (opadd(mknode1(3, 0), np));
}

/* addition */
NODE *
opadd(NODE *l, NODE *r)
{
	int rt, lt;
	int32_t rv, lv;
	NODE *np;

	rt = exprtype(r);
	if (rt == 6)
		error(2, "Invalid expression type for right side of addition\n");
	lt = exprtype(l);
	if (lt == 6)
		error(2, "Invalid expression type for left side of addition\n");
	if (rt == 3 && lt == 3) {
		np = mknode1(3, r->n_val + l->n_val);
		nfreed += 2;
		return (np);
	}
	if (rt == 0 || lt == 0)
		return (mknode25(0, 0x1A, l, r));
	if (rt == 3 && lt == 5)
		return (mknode25(5, 0x1A, l, r));
	if (rt == 5 && lt == 3)
		return (mknode25(5, 0x1A, l, r));
	if ((rt == 4 || rt == 1 || rt == 7) && lt == 3) {
		if (r->n_type == 7)
			return (mknode25(rt, 0x1A, r, l));
		lv = l->n_val;
		rv = r->n_b->n_val;
		if (r->n_c2 == 0x1A)
			np = mknode1(3, lv + rv);
		else if ((uint32_t)lv > (uint32_t)rv) {
			r->n_c2 = 0x1A;
			np = mknode1(3, lv - rv);
		} else
			np = mknode1(3, rv - lv);
		nfreed += 2;
		r->n_b = np;
		return (r);
	}
	if (rt == 3 && (lt == 4 || lt == 1 || lt == 7)) {
		if (l->n_type == 7)
			return (mknode25(lt, 0x1A, l, r));
		rv = r->n_val;
		lv = l->n_b->n_val;
		if (l->n_c2 == 0x1A)
			np = mknode1(3, lv + rv);
		else if ((uint32_t)lv > (uint32_t)rv)
			np = mknode1(3, lv - rv);
		else {
			l->n_c2 = 0x1A;
			np = mknode1(3, rv - lv);
		}
		nfreed += 2;
		l->n_b = np;
		return (l);
	}
	if (rt == 7 || lt == 7)
		return (mknode25(0, 0x1A, l, r));
	error(2, "Invalid addition expression\n");
	return (errnode());
}
