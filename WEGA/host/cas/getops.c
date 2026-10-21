/*
 * getops - read operands and expressions from the intermediate file and the symbol file
 *
 * An expression is written in prefix order (ifile.c); a name that is defined by now
 * becomes a constant, the operators are the functions of expr.c.
 */
#include "cas.h"

NODE *ops[4];				/* the operands of the instruction */

/* one of the two files */
struct reader {
	int (*getb)(void);
	int (*getw)(void);
	int32_t (*getl)(void);
};

static const struct reader irdr = { igetb, igetw, igetl };
static const struct reader srdr = { sgetb, sgetw, sgetl };

/* a name: a constant when it is defined by now, else the node of the name */
static NODE *
namenode(int idx)
{
	if (isdef(idx))
		return (mknode1(3, symval(idx)));
	return (mknode7(idx));
}

/* an expression */
static NODE *
getexpr(const struct reader *rd)
{
	NODE *l, *r;
	int tag;

	tag = rd->getb();
	switch (tag) {
	case 7:
		return (namenode(rd->getw()));
	case 3:
		return (mknode1(3, rd->getl()));
	case 26:
	case 27:
	case 28:
	case 29:
	case 30:
	case 31:
	case 32:
	case 33:
	case 34:
		l = getexpr(rd);
		r = getexpr(rd);
		switch (tag) {
		case 26:
			return (opadd(l, r));
		case 27:
			return (opsub(l, r));
		case 28:
			return (opmul(l, r));
		case 29:
			return (opdiv(l, r));
		case 30:
			return (opshl(l, r));
		case 31:
			return (opshr(l, r));
		case 32:
			return (opand(l, r));
		case 33:
			return (opor(l, r));
		default:
			return (opxor(l, r));
		}
	case 35:
		return (opshort(getexpr(rd)));
	case 36:
		return (opoff(getexpr(rd)));
	case 37:
		return (opseg(getexpr(rd)));
	}
	return (errnode());
}

/* an expression from the intermediate file */
NODE *
igetexpr(void)
{
	return (getexpr(&irdr));
}

/* an expression from the symbol file */
NODE *
sgetexpr(void)
{
	return (getexpr(&srdr));
}

/* operand n of an instruction from the intermediate file */
void
getop(int n)
{
	int tag;
	NODE *np;

	tag = igetb();
	switch (tag) {
	case 1:
		np = mknode1(3, igetb());
		break;
	case 2:
		np = mknode1(3, (uint16_t)igetw());
		break;
	case 3:
		np = mknode1(3, igetl());
		break;
	case 7:
	case 8:
	case 9:
		np = namenode(igetw());
		break;
	case 23:
	case 24:
	case 25:
		np = igetexpr();
		break;
	default:
		error(2, "Unexpected tag in intermediate file\n");
		np = errnode();			/* the original leaves the old operand */
		break;
	}
	ops[n] = np;
}

/* a node from the symbol file */
NODE *
sgetnode(void)
{
	int tag;

	tag = sgetb();
	switch (tag) {
	case 1:
		return (mknode1(3, sgetb()));
	case 2:
		return (mknode1(3, sgetw()));
	case 3:
		return (mknode1(3, sgetl()));
	case 7:
	case 8:
	case 9:
		return (namenode(sgetw()));
	case 23:
	case 24:
	case 25:
		return (sgetexpr());
	}
	error(2, "Unexpected tag in symbol file\n");
	return (errnode());
}
