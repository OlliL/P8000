/*
 * out3.c - printing of operands and the template expander (codgen / scodgen)
 *
 * Reconstructed from /lib/codgen: acon() 0x437e, oregput() 0x409a, conput() 0x3fda, upput() 0x40d4, adrput() 0x41d0,
 * hopcode() 0x2de0, expand() 0x6444. The output goes to the standard output. The macros Z (zzzcode) and C of
 * the templates are not done yet.
 */
#include "cg.h"

extern int cerror(const char *, ...);
extern struct node *getlr(struct node *p, int c);
extern int ttype(int t, int tword);
extern char *opst[];		/* operator names (comm3) */
extern int fldsz, fldshf;	/* the field that tshape found */
extern char *rnames[];		/* r0 .. r14 fp f0 .. f3, 3fca */
extern char *rlnames[];		/* rl0 .. rl7, 27ca */
extern int rlmode, rmode;	/* 0x280c (W: low byte registers) and 0x280a (V: long registers) */
extern int upperlval;		/* 0xa148: print the high word of a long constant */
extern int tflag;		/* 0x35d0: set by the macro T */
extern int zzzcode(struct node *p, int c);	/* 0x2edc */

/* a name or constant: label, name, +offset */
void
acon(struct node *p)
{
	struct sym *s;
	char c0;

	if (p->op == ICON && p->rval < 0) {
		printf("L%d", -p->rval);
		goto off;
	}
	if (p->op == OREG && p->stsize < 0) {
		printf("L%d", -p->stsize);
		goto off;
	}
	if (p->op == NAME && p->rval < 0) {
		printf("L%d", -p->rval);
		goto off;
	}
	if (p->sname == 0) {
		printf("%ld", p->lval);
		return;
	}
	s = p->sname;
	c0 = ascii[s->nameoff];
	if (c0 != '~') {
		if (p->op == NAME && s->sclass == 3 && !(s->sflags >> 2 & 1)) {	/* a local static: its label */
			putchar('L');
			printf("%ld", s->soffset);
			p->lval = p->rval;
		} else {
			putchar('_');
			printf("%s", ascii + s->nameoff);
		}
	} else if (ascii[fpsym1->nameoff] == c0 || ascii[fpsym2->nameoff] == c0)
		printf("%s", ascii + s->nameoff);		/* ~L1, ~L2 */
	else
		goto off;
off:
	if (p->lval) {
		putchar('+');
		printf("%ld", p->lval);
	}
}

/* offset(register) of an OREG */
void
oregput(struct node *p)
{
	acon(p);
	if (p->rval == 15)
		printf("(fp)");
	else
		printf("(%s)", rnames[p->rval]);
}

/* a constant or a register as an operand part */
void
conput(struct node *p)
{
	switch (p->op) {
	case ICON:
		acon(p);
		break;
	case REG:
		if (rlmode)
			printf("%s", rlnames[p->rval]);
		else {
			if (rmode)
				putchar('r');
			printf("%s", rnames[p->rval]);
		}
		break;
	case OREG:
		if (p->lval >= 0x100)
			cerror("bad oreg in conput");
		if (p->lval != 0 || p->sname != 0 || (p->op == OREG && p->stsize < 0))
			oregput(p);
		else
			printf("@%s", rnames[p->rval]);
		break;
	default:
		cerror("illegal conput");
	}
}

void
insput(struct node *p)
{
	(void)p;
	cerror("insput");
}

/* the second word of an operand */
void
upput(struct node *p)
{
	long save;
	int r;

	if (p->op == FLD)
		p = p->left;
	save = p->lval;
	switch (p->op) {
	case NAME:
		p->lval += 2;
		if (p->rval < 0)
			acon(p);
		else {
			r = p->rval;
			p->rval += 2;
			acon(p);
			p->rval = r;
		}
		break;
	case ICON:
		p->lval &= 0xFFFF;
		putchar('#');
		acon(p);
		break;
	case REG:
		if (rlmode)
			printf("%s", rlnames[p->rval + 1]);
		else
			printf("%s", rnames[p->rval + 1]);
		break;
	case OREG:
		if (p->rval >= 0x100)
			cerror("illegal OREG upput");
		p->lval += 2;
		oregput(p);
		break;
	default:
		cerror("illegal upper address");
	}
	p->lval = save;
}

/* an operand */
void
adrput(struct node *p)
{
	long save;
	int r;

	if (p->op == FLD)
		p = p->left;
	switch (p->op) {
	case NAME:
		acon(p);
		break;
	case ICON:
		if (upperlval) {			/* the high word of a long constant */
			save = p->lval;
			p->lval = (p->lval >> 16) & 0xFFFF;
			putchar('#');
			acon(p);
			p->lval = save;
		} else {
			putchar('#');
			acon(p);
		}
		break;
	case REG:
		if (rlmode)
			printf("%s", rlnames[p->rval]);
		else {
			if (rmode)
				putchar('r');
			printf("%s", rnames[p->rval]);
		}
		break;
	case OREG:
		r = p->rval;
		if (r >= 0x100) {			/* base and index register */
			if (r & 0xFF)
				printf("r%s(%s)", rnames[(r & 0xFF) - 1], rnames[(r >> 8) - 1]);
			else {
				printf("r%s(#", rnames[(r >> 8) - 1]);
				acon(p);
				putchar(')');
			}
		} else if (p->lval == 0 && p->sname == 0 && p->stsize == 0)
			printf("@%s", rnames[r]);
		else
			oregput(p);
		break;
	default:
		cerror("illegal address: %d (%s)", p->op, opst[p->op]);
	}
}

struct hoptab { int op; char *str; int tword; };

/* the instruction mnemonic for an arithmetic operator and the type of the tree */
extern struct hoptab fhoptab[], ihoptab[];	/* floating and integer tables, hoptab_*.c */

void
hopcode(struct node *p)
{
	struct hoptab *q;
	int t = p->type;

	q = (t == 6 || t == 7) ? fhoptab : ihoptab;
	for (; q->op >= 0; q++)
		if (q->op == p->op && ttype(t, q->tword)) {
			printf("%s", q->str);
			return;
		}
	cerror("no hoptab for %s", opst[p->op]);
}

/* expand the template cp for the tree p */
void
expand(struct node *p, int cookie, char *cp)
{
	struct node *r;
	long m;

	tflag = 0;
	for (; *cp; cp++) {
		switch (*cp) {
		case 'A':
			adrput(getlr(p, *++cp));
			break;
		case 'B':
			r = getlr(p, *++cp);
			printf("%ld", r->lval & 1);
			break;
		case 'F':			/* the line is only for a result that is used */
			if (cookie & SANY)
				while (*++cp != '\n')
					;
			break;
		case 'H':
			printf("%d", fldshf);
			break;
		case 'I':
			insput(getlr(p, *++cp));
			break;
		case 'L':
			printf("%d", p->label);
			break;
		case 'M': case 'N':
			m = ((1L << fldsz) - 1) << fldshf;
			printf("%ld", *cp == 'M' ? m : ~m);
			break;
		case 'O':
			hopcode(p);
			cp++;
			break;
		case 'S':
			printf("%d", fldsz);
			break;
		case 'T':
			tflag = 1;
			break;
		case 'U':
			upput(getlr(p, *++cp));
			break;
		case 'V':
			rmode++;
			break;
		case 'W':
			rlmode++;
			break;
		case 'X':
			rlmode = rmode = 0;
			break;
		case 'Y':
			r = getlr(p, 'R');
			if (r->op == FLD)
				r = r->left;
			if (r->op == ICON)
				acon(r);
			else
				adrput(getlr(p, 'R'));
			break;
		case 'Z':
			zzzcode(p, *++cp);
			break;
		case 'C':
			cerror("expand: macro C is not done yet");
			break;
		default:
			putchar(*cp);
		}
	}
	rlmode = rmode = 0;
}
