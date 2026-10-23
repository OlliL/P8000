/*
 * out3.c - printing of operands and the template expander (codgen / scodgen)
 *
 * Reconstructed from /lib/codgen: acon() 0x437e, oregput() 0x409a, conput() 0x3fda, upput() 0x40d4, adrput() 0x41d0,
 * hopcode() 0x2de0, expand() 0x6444. The output goes to the standard output. The macros Z (zzzcode) and C of
 * the templates are done.
 */
#include "cg.h"

extern int cerror();
extern struct node *getlr();
extern int ttype();
extern char *opst[];		/* operator names (comm3) */
extern int fldsz, fldshf;	/* the field that tshape found */
extern char *rnames[];		/* r0 .. r14 fp f0 .. f3, 3fca */
extern char *rlnames[];		/* rl0 .. rl7, 27ca */
int rlmode, rmode;	/* 0x280c (W: low byte registers) and 0x280a (V: long registers) */
extern int nosuffix;
int upperlval;		/* 0xa148: print the high word of a long constant */
extern int tflag;		/* 0x35d0: set by the macro T */
extern int zzzcode();
extern struct sym *symtab0;
extern long frame0;
extern void adrput();
extern int rclass();
extern struct rs *rfind();
extern struct node resc[];	/* 0x2edc */

/* a name or constant: label, name, +offset */
void
acon(p)
	struct node *p;
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
#ifdef SEG
	if (s == symtab0 || c0 == '~') {		/* an address in the stack segment: _stkseg+~L1+offset */
		printf("_stkseg");
		if (c0 == ascii[fpsym1->nameoff] || ascii[fpsym2->nameoff] == c0)
			printf("+%s", ascii + s->nameoff);
		goto off;
	}
	if (p->op == NAME && s->sclass == 3 && !(s->sflags >> 2 & 1)) {	/* a local static: its label */
		putchar('L');
		printf("%ld", s->soffset);
		p->lval = p->rval;
	} else {
		putchar('_');
		printf("%s", ascii + s->nameoff);
	}
#else
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
#endif
off:
	if (p->lval) {
		putchar('+');
		printf("%ld", p->lval);
	}
}

/* offset(register) of an OREG */
void
oregput(p)
	register struct node *p;
{
	int bar = 0;		/* only the segmented version uses it */

#ifdef SEG
	if (p->rval == 15) {
		if ((p->sname == symtab0 && (unsigned)(int)p->lval <= 0xFF) ||
		    (p->sname == fpsym1 && (unsigned)(int)(p->lval + frame0) <= 0xFF)) {
			bar++;
			putchar('|');
		}
		acon(p);
		if (bar)
			putchar('|');
		printf("(fp)");
	} else {
		acon(p);
		printf("(%s)", rnames[p->rval]);
	}
#else
	if (p->rval == 15) {
		acon(p);
		printf("(fp)");
	} else {
		acon(p);
		printf("(%s)", rnames[p->rval]);
	}
#endif
}

/* a constant or a register as an operand part */
void
conput(p)
	struct node *p;
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
#ifdef SEG
			printf("@r%s", rnames[p->rval]);
#else
			printf("@%s", rnames[p->rval]);
#endif
		break;
	default:
		cerror("illegal conput");
	}
}

void
insput(p)
	struct node *p;
{
	cerror("insput");
}

/* the second word of an operand */
void
upput(p)
	register struct node *p;
{
	register long save;
	register int r;

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
#ifdef SEG
		if (p->lval != 0 || p->sname != 0 || (p->op == OREG && p->stsize < 0)) {
			p->lval += 2;
			oregput(p);
		} else {
			p->lval = 2;
			r = p->rval;
			p->rval = (r + 1) << 8;
			adrput(p);
			p->rval = r;
		}
#else
		p->lval += 2;
		oregput(p);
#endif
		break;
	default:
		cerror("illegal upper address");
	}
	p->lval = save;
}

/* an operand */
void
adrput(p)
	register struct node *p;
{
	long save;
	register int r;

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
#ifdef SEG
				printf("r%s(%s)", rnames[(r >> 8) - 1], rnames[(r & 0xFF) - 1]);
#else
				printf("r%s(%s)", rnames[(r & 0xFF) - 1], rnames[(r >> 8) - 1]);
#endif
			else {
				printf("r%s(#", rnames[(r >> 8) - 1]);
				acon(p);
				putchar(')');
			}
		} else if (p->lval == 0 && p->sname == 0 && p->stsize == 0)
#ifdef SEG
			printf("@r%s", rnames[r]);
#else
			printf("@%s", rnames[r]);
#endif
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
hopcode(p)
	register struct node *p;
{
	register int o = p->op;
	register struct hoptab *q;
	register int t = p->type;

	q = (t == 6 || t == 7) ? fhoptab : ihoptab;
	for (; q->op >= 0; q++)
		if (q->op == o && ttype(t, q->tword)) {
			printf("%s", q->str);
			return;
		}
	cerror("no hoptab for %s", opst[o]);
}

/* expand the template cp for the tree p */
void
expand(p, cookie, cp)
	register struct node *p;
	register int cookie;
	register char *cp;
{
	register long m;
	register struct node *r;
	register struct rs *n;
	int k;

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
			/* the result register of a call holds the value of the right operand: note it for the register file */
			r = getlr(p, *++cp);
			k = rclass(p->type, resc[0].rval);
			n = rfind(resc[0].rval, k);
			if (p->op == UCALL)
				n->sym = 0;
			else {
				n->off = r->lval;
				n->sym = fpsym1;
				n->flags |= 0x8000;
			}
			conput(r);
			break;
		default:
			putchar(*cp);
		}
	}
	nosuffix = rlmode = rmode = 0;
}

/*
 * The suffix letter of an instruction for a type: b for bytes (and the W mode), l for longs (and the V mode),
 * s float, d double, nothing for words and pointers (0x2796). Used by the macro ZB and by rmove.
 */
int nosuffix;	/* 0x280e */

void
tsuffix(t)
	int t;
{
	if (nosuffix)
		return;
	switch (t) {
	case 2:
	case 12:
		putchar('b');
		rlmode++;
		break;
	case 3:
	case 4:
	case 13:
	case 14:
		break;
	case 5:
	case 15:
		putchar('l');
		rmode++;
		break;
	case 6:
		putchar('s');
		break;
	case 7:
		putchar('d');
		break;
	default:
#ifdef SEG
		if ((t & 0x30) == 0x10) {
			putchar('l');
			rmode++;
		} else
			cerror("zzzcode - bad type");
#else
		if ((t & 0x30) != 0x10)
			cerror("zzzcode - bad type");
#endif
	}
}
