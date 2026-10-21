/*
 * assemble - assemble an instruction in pass 2
 *
 * A record 10 of the intermediate file points to a row of itab1. The operands are
 * read (getop) into ops[]; opcls[] says what to do with each of them, and the
 * descriptor instlen[kind].x[] says which bits of the instruction words are made of
 * the opcode word and which of the operands. The bits are collected by putbits and
 * written as words (genw).
 */
#include "cas.h"

static int nbits = 0;			/* bits of the word that are filled */
static uint16_t word;			/* the word being assembled */

static void asmword(struct irow *row);
static void asmfield(int code);
static void asmimm(int code);
static void asmfix(int code);
static void asmfix2(int code);
static void asmrel(int i, int32_t pc, int cls);
static void asmreg(int i);

/* assemble the instruction of the row */
void
asmins(struct irow *row)
{
	const unsigned char *p;
	int i, n;

	n = 0;
	p = opcls + row->ir_cls;
	while (*p != 0x12 && *p != 0x1C) {
		switch (*p) {
		case 6:
		case 12:
		case 13:
			getop(n);
			n++;
			getop(n);
			break;
		case 9:
		case 10:
		case 11:
			getop(n);
			asmrel(n, (uint16_t)((int16_t)lctab[cursec][cursub] + instlen[row->ir_kind].len), *p);
			break;
		case 17:
			getop(n);
			ops[n]->n_val = (uint32_t)ops[n]->n_val >> 4;
			break;
		case 8:
			getop(n);
			ops[n]->n_val = ops[n]->n_val & 0xF;
			break;
		default:
			getop(n);
		}
		n++;
		p++;
	}
	if (*p == 0x1C) {
		getop(n);
		n++;
	}
	asmword(row);
	for (i = 0; i < n; i++) {
		nfreed++;
		ops[i] = 0;
	}
}

/* the words of the instruction: the descriptor of the row says where the bits come from */
static void
asmword(struct irow *row)
{
	uint16_t w;
	int j;
	unsigned char kind, code;

	j = 0;
	w = row->ir_0;
	kind = row->ir_kind;
	code = instlen[kind].x[j];
	while (j != 5 && code != 0) {
		j++;
		switch (code) {
		case 0xFF:
			kind++;
			j = 0;
			break;
		case 9:
			putbits(16, w);
			break;
		case 1:
			putbits(8, w >> 8);
			break;
		case 2:
			putbits(4, w >> 12);
			break;
		case 3:
			putbits(4, (w >> 4) & 0xF);
			break;
		case 4:
			putbits(4, w & 0xF);
			break;
		case 5:
			putbits(1, (w >> 7) & 1);
			break;
		case 6:
			putbits(1, (w >> 3) & 1);
			break;
		case 7:
			putbits(2, (w >> 6) & 3);
			break;
		case 8:
			putbits(7, (w >> 9) & 0x7F);
			break;
		default:
			asmfield(code);
		}
		code = j < 5 ? instlen[kind].x[j] : 0;
	}
}

/* a field of the instruction from an operand */
static void
asmfield(int code)
{
	switch (code) {
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
		asmreg(code - 14);
		break;
	case 63:
		code = 1;
		if ((uint32_t)ops[code]->n_val > 7)
			error(2, "Register must be 0-7\n");
		asmreg(code);
		break;
	case 61:
	case 62:
		code -= 0x3C;
		if ((uint32_t)ops[code]->n_val > 7)
			error(2, "Register must be 0-7\n");
		asmreg(code);
		break;
	case 21:
	case 22:
	case 23:
		code -= 20;
		ops[code]->n_val -= 1;
		asmreg(code);
		break;
	case 73:
	case 74:
		code -= 0x47;
		ops[code]->n_val = ops[code]->n_val * 5;
		ops[code]->n_val -= 1;
		asmreg(code);
		break;
	case 26:
	case 27:
	case 28:
		code -= 0x1A;
		genbop(ops[code], 1);
		break;
	case 29:
	case 30:
	case 31:
		code -= 0x1D;
		genwop(ops[code], 0);
		break;
	case 32:
	case 33:
	case 34:
		code -= 0x20;
		genaddr(ops[code]);
		break;
	case 36:
	case 37:
		code -= 0x23;
		genlop(ops[code], 0);
		break;
	default:
		asmimm(code);
	}
}

/* the value of a constant operand as an int (0 for the others) */
static int16_t
constop(NODE *np)
{
	if (np != 0 && (np->n_type == 3 || np->n_type == 0x13))
		return (np->n_val);
	return (0);
}

/* a field made of a constant operand */
static void
asmimm(int code)
{
	int16_t c0, c1;
	unsigned lim;

	c0 = constop(ops[0]);
	c1 = constop(ops[1]);
	switch (code) {
	case 19:
	case 20:
		if (code == 0x13)
			putbits(4, (c0 >> 4) & 0xF);
		else
			putbits(4, c0 & 0xF);
		break;
	case 38:
		rangechk(ops[0]->n_val, 0xFFFFF000);
		putbits(12, c0 & 0xFFF);
		break;
	case 39:
		putbits(2, ~c0 & 3);
		break;
	case 72:
		putbits(2, c0 & 3);
		break;
	case 40:
		putbits(3, c0 & 7);
		break;
	case 41:
		putbits(9, c0 & 0x1FF);
		break;
	case 42:
		putbits(3, c1 & 7);
		break;
	case 43:
	case 44:
	case 45:
	case 50:
	case 51:
	case 52:
		switch (code) {
		case 43:
		case 50:
			lim = 8;
			break;
		case 44:
		case 51:
			lim = 0x10;
			break;
		default:
			lim = 0x20;
			break;
		}
		if ((uint16_t)c1 > lim)
			error(1, "Operand too large\n");
		if (code >= 0x32)
			c1 = -c1;
		putbits(16, c1);
		break;
	case 49:
		putbits(3, -c1 & 7);
		break;
	case 46:
		if ((c1 & 0xFF80) != 0)
			error(1, "Operand too large\n");
		putbits(7, c1 & 0x7F);
		break;
	case 47:
		putbits(15, c1 & 0x7FFF);
		break;
	case 48:
		c1--;
		if ((c1 & 0xFFFE) != 0)
			error(1, "Operand too large\n");
		putbits(1, c1 & 1);
		break;
	case 56:
		putbits(4, (c0 | c1) & 0xF);
		break;
	case 57:
		putbits(4, (c0 | c1 | (int16_t)ops[2]->n_val) & 0xF);
		break;
	case 58:
		putbits(4, (c0 | c1 | (int16_t)ops[2]->n_val | (int16_t)ops[3]->n_val) & 0xF);
		break;
	case 59:
		putbits(2, ~(c0 | c1) & 3);
		break;
	default:
		asmfix(code);
	}
}

/* a field with a fixed value */
static void
asmfix(int code)
{
	switch (code) {
	case 0xa:
		putbits(4, 0);
		break;
	case 0xb:
		putbits(4, 15);
		break;
	case 0xc:
		putbits(4, 14);
		break;
	case 0xd:
		putbits(4, 2);
		break;
	case 0x18:
		putbits(8, 0);
		break;
	case 0x19:
		putbits(8, 0xFF);
		break;
	case 0x35:
		putbits(1, 0);
		break;
	case 0x36:
		putbits(1, 1);
		break;
	case 0x37:
		putbits(2, 0);
		break;
	case 0x3c:
		putbits(16, 1);
		break;
	default:
		asmfix2(code);
	}
}

/* more fixed fields */
static void
asmfix2(int code)
{
	switch (code) {
	case 64:
		putbits(8, 0x8E);
		break;
	case 65:
		putbits(8, 0x8F);
		break;
	case 66:
		putbits(8, 15);
		break;
	case 67:
		putbits(8, 0x4F);
		break;
	case 68:
		putbits(4, 4);
		break;
	case 69:
		putbits(4, 8);
		break;
	case 70:
		putbits(4, 0);
		break;
	case 71:
		putbits(4, 12);
		break;
	default:
		error(2, "Unexpected tag in intermediate file\n");
	}
}

/* a relative address: operand i becomes the distance from pc (class 9: in words, 10: in bytes, 11: backwards in words) */
static void
asmrel(int i, int32_t pc, int cls)
{
	NODE *np;
	int16_t v;

	np = ops[i];
	switch (np->n_type) {
	case 3:
		error(2, "Mixed relocatable and absolute\n");
		break;
	case 7:
	case 0x19:
		if (np->n_type == 7)
			v = symval(np->n_w);
		else
			v = exprval(np);
		nfreed++;
		switch (cls) {
		case 9:
			ops[i] = mknode1(0x13, ((int32_t)(uint16_t)v - pc) >> 1);
			break;
		case 11:
			ops[i] = mknode1(0x13, (pc - (int32_t)(uint16_t)v) >> 1);
			break;
		case 10:
			ops[i] = mknode1(0x13, (int32_t)(uint16_t)v - pc);
			break;
		}
		break;
	default:
		error(2, "Must be relative address\n");
	}
}

/* a register field of 4 bits from the constant operand i */
static void
asmreg(int i)
{
	NODE *np;
	int32_t v;

	np = ops[i];
	v = 0;
	switch (np->n_type) {
	case 3:
	case 0x13:
		v = np->n_val;
		break;
	default:
		error(2, "Must be number or relative address\n");
	}
	rangechk(v, 0xFFFFFFF0);
	putbits(4, (int16_t)v & 0xF);
}

/* add n bits to the word; a full word is written */
void
putbits(int n, int v)
{
	int shift;

	if (nbits + n > 16)
		error(2, "Too many bits assembled for word\n");
	shift = 16 - nbits - n;
	if (shift >= 0)
		word |= (uint16_t)((uint16_t)v << shift);
	nbits += n;
	if (nbits == 16) {
		genw(word, 0, 1);
		nbits = 0;
		word = 0;
	}
}

/* a value must fit in the bits that the mask lets through (or be negative) */
void
rangechk(int32_t v, int32_t mask)
{
	if ((v & mask) == 0)
		return;
	if (((0 - v) & mask) == 0)
		return;
	error(1, "Value too large\n");
}
