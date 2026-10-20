static char sccsid[] = "@[$]/as/as/getops.c\t1.2 05/27/83 15:47:17 - 87wega3.2";

/*
 * getops - read operands and expressions from the intermediate file and the symbol file
 *
 * An expression is written in prefix order (ifile.c); a name that is defined by now
 * becomes a constant, the operators are the functions of expr.c.
 */
#include "cas.h"




/* an expression from the intermediate file */
NODE *
igetexpr()
{
	register int idx;
	register NODE *l;
	register NODE *r;
	unsigned char tag;
	long v;

	tag = igetb();
	switch (tag) {
	case 7:
		idx = igetw();
		if (isdef(idx)) {
			v = symval(idx);
			return (mknode1(3, v));
		}
		return (mknode7(idx));
	case 3:
		v = igetl();
		return (mknode1(3, v));
	case 26:
		l = igetexpr();
		r = igetexpr();
		return (opadd(l, r));
	case 27:
		l = igetexpr();
		r = igetexpr();
		return (opsub(l, r));
	case 28:
		l = igetexpr();
		r = igetexpr();
		return (opmul(l, r));
	case 29:
		l = igetexpr();
		r = igetexpr();
		return (opdiv(l, r));
	case 30:
		l = igetexpr();
		r = igetexpr();
		return (opshl(l, r));
	case 31:
		l = igetexpr();
		r = igetexpr();
		return (opshr(l, r));
	case 32:
		l = igetexpr();
		r = igetexpr();
		return (opand(l, r));
	case 33:
		l = igetexpr();
		r = igetexpr();
		return (opor(l, r));
	case 34:
		l = igetexpr();
		r = igetexpr();
		return (opxor(l, r));
	case 35:
		l = igetexpr();
		return (opshort(l));
	case 36:
		l = igetexpr();
		return (opoff(l));
	case 37:
		return (opseg(igetexpr()));
	}
}

/* an expression from the symbol file */
NODE *
sgetexpr()
{
	register int idx;
	register NODE *l;
	register NODE *r;
	unsigned char tag;
	long v;

	tag = sgetb();
	switch (tag) {
	case 7:
		idx = sgetw();
		if (isdef(idx)) {
			v = symval(idx);
			return (mknode1(3, v));
		}
		return (mknode7(idx));
	case 3:
		v = sgetl();
		return (mknode1(3, v));
	case 26:
		l = sgetexpr();
		r = sgetexpr();
		return (opadd(l, r));
	case 27:
		l = sgetexpr();
		r = sgetexpr();
		return (opsub(l, r));
	case 28:
		l = sgetexpr();
		r = sgetexpr();
		return (opmul(l, r));
	case 29:
		l = sgetexpr();
		r = sgetexpr();
		return (opdiv(l, r));
	case 30:
		l = sgetexpr();
		r = sgetexpr();
		return (opshl(l, r));
	case 31:
		l = sgetexpr();
		r = sgetexpr();
		return (opshr(l, r));
	case 32:
		l = sgetexpr();
		r = sgetexpr();
		return (opand(l, r));
	case 33:
		l = sgetexpr();
		r = sgetexpr();
		return (opor(l, r));
	case 34:
		l = sgetexpr();
		r = sgetexpr();
		return (opxor(l, r));
	case 35:
		l = sgetexpr();
		return (opshort(l));
	case 36:
		l = sgetexpr();
		return (opoff(l));
	case 37:
		return (opseg(sgetexpr()));
	}
}

/* operand n of an instruction from the intermediate file */
getop(n)
unsigned char n;
{
	register int idx;
	unsigned char tag;
	long v;
	NODE *np;

	tag = igetb();
	switch (tag) {
	case 1:
		v = igetb();
		np = mknode1(3, v);
		break;
	case 2:
		v = (unsigned)igetw();
		np = mknode1(3, v);
		break;
	case 3:
		v = igetl();
		np = mknode1(3, v);
		break;
	case 7:
	case 8:
	case 9:
		idx = igetw();
		if (isdef(idx)) {
			v = symval(idx);
			np = mknode1(3, v);
		} else
			np = mknode7(idx);
		break;
	case 23:
	case 24:
	case 25:
		np = igetexpr();
		break;
	default:
		error(2, "Unexpected tag in intermediate file\n");
		return;
	}
	ops[n] = np;
}

/* a node from the symbol file */
NODE *
sgetnode()
{
	register int idx;
	register NODE *np;
	unsigned char tag;
	long v;

	tag = sgetb();
	switch (tag) {
	case 1:
		v = sgetb();
		np = mknode1(3, v);
		break;
	case 2:
		v = sgetw();
		np = mknode1(3, v);
		break;
	case 3:
		v = sgetl();
		np = mknode1(3, v);
		break;
	case 7:
	case 8:
	case 9:
		idx = sgetw();
		if (isdef(idx)) {
			v = symval(idx);
			np = mknode1(3, v);
		} else
			np = mknode7(idx);
		break;
	case 23:
	case 24:
	case 25:
		np = sgetexpr();
		break;
	default:
		error(2, "Unexpected tag in symbol file\n");
	}
	return (np);
}
