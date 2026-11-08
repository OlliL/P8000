/*
 * z23.c - popt, part 3: register liveness (isused) and the
 * "stripldm" pass (procopt), decompiled from the WEGA /lib/popt.
 */
#include <ctype.h>
#include "z2.h"

static char sccsid[] __attribute__((unused)) = "@[$]z23.c\t1.6  03/25/83 18:10:30 Copr.1981 87wega3.2.";

struct rent;
static void regname(char *dst, int n);
static void rename_regs(struct rent *t, struct node *first_ldm, struct node *last_ldm);
static void fixldm(struct node *p, int n);
static int usesreg(struct node *node, char *str, int reg);
static int ovlap(char *text, char *name);
static void markregs(char *s, char *b, char *a);
#ifdef OLD_D1F
static void stripframe(void);
static int fpfirst(char *c);
#endif







struct rent {
	int len;
	char s1[4];
	char s2[4];
};





/*
 * The register save/restore ldm pair of a function is shortened by
 * renaming register variables to unused low registers.
 */
void
procopt(void)
{
	register int r14;
	register struct node *p;
	register char *s;
	register struct node *ld1, *ld2;
	register int r9, r8;
	int op, k, n;
#ifdef SPOPT
	char b[18], a[8];		/* the original has 14 and writes behind it */
#else
	char b[18], a[8];		/* the original has 15 and writes behind it */
#endif
	struct rent t[9];

	ld1 = 0;
	ld2 = 0;
	a[0] = 1;
	for (r14 = 1; r14 < 8; r14++)
		a[r14] = 0;
#ifdef SPOPT
	for (r14 = 0; r14 < 14; r14++)
#else
	for (r14 = 0; r14 < 15; r14++)
#endif
		b[r14] = 0;
	for (p = &first; p; p = p->forw) {
		switch (op = p->op) {
		case LD: case CP: case ADD: case SUB: case BIT: case AND:
		case OR: case MUL: case DIV: case SDA: case XOR: case PUSH:
		case POP: case SET: case RES: case SDL:
			dualop(p);
			markregs(RT2, b, a);
			markregs(RT1, b, a);
			break;
		case JP: case CLR: case COM: case INC: case DEC: case NEG:
		case TST: case SRA: case SLA: case EXTS: case SOB: case LDA:
		case 38: case 41: case 43: case SRL: case SLL:
			markregs(p->code, b, a);
			break;
		case LDM:
			if (ld1 == 0)
				ld1 = p;
			else
				ld2 = p;
			break;
		case CALL:
			fprintf(stderr, "CALL instruction in stripldm()\n");
			return;
		}
	}
	if (ld1 == 0 || ld2 == 0)
		return;
	s = ld1->code;
	while (*s++ != ',')
		;
	while (*s++ != ',')
		;
	if (*s++ != '#') {
		fprintf(stderr, "malformed ldm instruction:\tldm\t%s\n", ld1->code);
		return;
	}
	r8 = atoi(s);
	r9 = 0;
	for (r14 = 1; r14 < 8; r14++)
		if (a[r14] == 0)
			r9++;
	if (r9 == 0)
		return;
	k = 0;
	for (r14 = 1; r9 > k && r14 < 8 && k >= 0 && k < 8; r14++) {	/* k < r9 < 8 holds: the tests tell the compiler */
		if (a[r14])
			continue;
#ifdef SPOPT
		n = 13 - k;
#else
		n = 14 - k;
#endif
		if ((n & 1) && b[n - 1] && n <= 13) {
			if ((r14 & 1) || r14 + 1 >= 8 || a[r14 + 1])
				continue;
			regname(t[k].s1, n - 1);	/* sprintf of the old libc returns its first argument */
			t[k].len = strlen(t[k].s1);
			regname(t[k++].s2, r14++);
			regname(t[k].s1, n);
			t[k].len = strlen(t[k].s1);
			regname(t[k++].s2, r14);
		} else {
			regname(t[k].s1, n);
			t[k].len = strlen(t[k].s1);
			regname(t[k++].s2, r14);
		}
	}
	if (r8 < k) {
		k = r8 < 0 ? 0 : r8;	/* the count of an ldm is not negative */
		r8 = 0;
	} else
		r8 -= k;
	nrename += k;
	t[k].len = -1;
	rename_regs(t, ld1, ld2);
	fixldm(ld1, r8);
	fixldm(ld2, r8);
#ifdef OLD_D1F
	if (r8 == 0)
		stripframe();
#endif
}

/* the register number of an operand, also behind an indirection */
int
regof(char *s)
{
	return isreg(*s == '@' ? s + 1 : s);
}

#ifdef OLD_D1F
/*
 * The old popt: when no register is saved any more and no instruction reads the frame, the stores into the frame (the
 * spills of the parameters) and the frame itself (dec/inc fp) go too.
 */
static void
stripframe(void)
{
	register struct node *p;
	register char *c;

	for (p = &first; p; p = p->forw) {
		if (p->code == 0 || p->op == LABEL || p->op == COMMENT)
			continue;
		for (c = p->code; *c; c++)
			if (c[0] == 'f' && c[1] == 'p')
				break;
		if (*c == 0)
			continue;
		if ((p->op == DEC || p->op == INC) && strncmp(p->code, "fp,", 3) == 0)
			continue;
		if ((p->op == LD || p->op == LDM) && strncmp(p->code, "fp", 2) != 0 && fpfirst(p->code))
			continue;
		return;
	}
	for (p = &first; p; p = p->forw) {
		if (p->code == 0 || p->op == LABEL || p->op == COMMENT)
			continue;
		if (((p->op == DEC || p->op == INC) && strncmp(p->code, "fp,", 3) == 0) ||
		    ((p->op == LD || p->op == LDM) && fpfirst(p->code))) {
			p->back->forw = p->forw;
			if (p->forw)
				p->forw->back = p->back;
		}
	}
}

/* is the first operand of the code a frame operand (n(fp))? */
static int
fpfirst(char *c)
{
	for (; *c && *c != ','; c++)
		if (c[0] == '(' && c[1] == 'f' && c[2] == 'p')
			return 1;
	return 0;
}
#endif

/* "rN" for N below 100: sprintf(dst, "r%d", n) of the original, whose result is its first argument */
static void
regname(char *dst, int n)
{
	*dst++ = 'r';
	if ((unsigned)n >= 10)
		*dst++ = '0' + (unsigned)n / 10 % 10;
	*dst++ = '0' + (unsigned)n % 10;
	*dst = 0;
}

/* rename registers in the code between the two ldm's */
static void
rename_regs(struct rent *t, struct node *first_ldm, struct node *last_ldm)
{
	register struct rent *e;
	register char *q, *r11;
	register struct node *p;
	register char *r9;
	char changed;

	for (p = first_ldm->forw; p != last_ldm; p = p->forw) {
		if (p->op == LABEL || p->op == COMMENT)
			continue;
		changed = 0;
		if (p->code == 0)
			continue;	/* the original reads address 0 here: nothing matches */
		for (e = t; e->len > 0; e++) {
			q = e->s1;
			for (r11 = p->code; *r11; r11++) {
				if (strncmp(q, r11, e->len) == 0 && !isalnum(q[-1])) {
					r9 = r11;
					if (*++r9 == '1' && isdigit(r9[1]))
						r9++;
					r9++;
					for (q = e->s2; *q;)
						*r11++ = *q++;
					while (r11 != r9) {
						*r11++ = (char)0xff;
						changed = 1;
					}
					q = e->s1;
					r11--;
				}
			}
		}
		if (changed) {
			for (q = p->code; *q != (char)0xff; q++)
				;
			r11 = q + 1;
			for (;;) {
				while (*r11 == (char)0xff)
					r11++;
				*q++ = *r11;
				if (!*r11++)
					break;
			}
		}
	}
}

/* shorten the ldm to n registers: delete it, make it an ld, or recount */
static void
fixldm(struct node *p, int n)
{
	register char *q;

	switch (n) {
	case 0:
		p->back->forw = p->forw;
		p->forw->back = p->back;
		break;
	case 1:
		p->op = LD;
		*strrchr(p->code, ',') = 0;
		break;
	default:
		q = strrchr(p->code, '#');
		sprintf(++q, "%d", n);
	}
}

int
isused(struct node *p, char *reg)
{
	register int r12;

	if ((r12 = isreg(reg)) < 0)
		return 1;
	switch (reg[1]) {
	case 'r':
		return usesreg(p, reg, r12) || usesreg(p, reg, r12 + 1);
	case 'q':
		return usesreg(p, reg, r12) || usesreg(p, reg, r12 + 1) ||
		    usesreg(p, reg, r12 + 2) || usesreg(p, reg, r12 + 3);
	}
	return usesreg(p, reg, r12);
}

/*
 * Is register number `reg' (spelled `str') read by the code following
 * node before it is overwritten?  Unknown cases count as "used".
 */
static int
usesreg(struct node *node, char *str, int reg)
{
	char buf[50];
	register struct node *p;
	register int r10, r9;

	strcpy(buf, str);
	str = buf;
	for (p = node->forw; p; p = p->forw) {
		switch (p->op) {
		case CJP:
		case JP:
			goto out1;
		case PUSH:
			if (p->subop != SLONG)
				goto dflt;
			dualop(p);
			r10 = isreg(RT1);
			if (r10 < 0)
				goto dflt;
			if (reg == r10 + 1)
				goto out1;
			goto dflt;
		case LD:
		case POP:
		case LDA:
			dualop(p);
			if (p->op == POP) {
				if (ovlap(RT1 + 1, str))
					goto out1;
			} else if (ovlap(RT1, str))
				goto out1;
			if (p->subop == SBYTE && reg == isreg(RT1))
				goto out1;
			if (equstr(RT2, str))
				goto out0;
			if (p->subop == SLONG) {
				r10 = isreg(RT2);
				if (r10 >= 0 && reg == r10 + 1)
					goto out0;
			}
			if (ovlap(RT2, str)) {
			out1:
				dualop(node);
				return 1;
			}
			continue;
		case CLR:
			r10 = 0;
			if (p->subop == SLONG) {
				r10 = isreg(p->code);
				if (r10 >= 0 && reg == r10 + 1)
					goto out0;
			}
			if (r10 >= 0 && equstr(p->code, str))
				goto out0;
			dualop(p);
			if (ovlap(RT1, str))
				goto out1;
			continue;
		case EXTS:
			r10 = isreg(p->code);
			if (r10 < 0)
				continue;
			switch (p->subop) {
			case 0:
				if (reg == r10 + 1)
					goto out1;
				if (r10 == reg) {
				out0:
					dualop(node);
					return 0;
				}
				continue;
			case SLONG:
				if (reg == r10 + 2 || reg == r10 + 3)
					goto out1;
				if (r10 == reg || reg == r10 + 1)
					goto out0;
				continue;
			case SBYTE:
				if (r10 != reg)
					continue;
				goto out1;
			}
			continue;
		case RET:
#ifdef SPOPT
			if (reg == 2 || reg == 3 || reg == 12 || reg == 13)
#else
			if (reg == 2 || reg == 3 || reg == 14)
#endif
				goto out1;
			goto out0;
		case DIV:
			dualop(p);
			r10 = isreg(RT2);
			if (r10 >= 0 && reg == r10 + 1)
				goto out1;
			if (p->subop != SLONG)
				goto dflt;
			if (reg == r10 + 2 || reg == r10 + 3)
				goto out1;
			goto dflt;
		case MUL:
			dualop(p);
			r10 = isreg(RT2);
			if (r10 < 0)
				goto dflt;
			if (p->subop == 0) {
				if (ovlap(RT1, str) || reg == r10 + 1)
					goto out1;
				if (reg != r10)
					goto dflt;
				goto out0;
			}
			if (p->subop != SLONG)
				goto dflt;
			if (ovlap(RT1, str) || reg == r10 + 2 || reg == r10 + 3)
				goto out1;
			if (reg != r10 && reg != r10 + 1) {
			dflt:
				if (ovlap(p->code, str))
					goto out1;
				continue;
			}
			goto out0;
		default:
			goto dflt;
		case CALL:
			r9 = reg;
			if (node->subop == SBYTE && reg > 8)
				r9 = reg - 8;
			if (r9 >= 2 && r9 <= 7)
				goto out1;
#ifdef SPOPT
			if (reg < 2) {
				if (p->code[0] != '@')
					goto out0;
				r10 = isreg(p->code + 1);
				if (reg != r10)
					goto out0;
				goto out1;
			}
			if (p->code[0] != '@')
				continue;
			r10 = isreg(p->code + 1);
			if (reg == r10)
				goto out1;
			r10 = reg ^ 1;
			if (r10 == 0)
				continue;
			goto out1;
#else
			if (reg >= 2)
				continue;
			if (p->code[0] != '@')
				goto out0;
			r10 = isreg(p->code + 1);
			if (reg != r10)
				goto out0;
			goto out1;
#endif
		}
	}
	goto out0;
}

/* reverse the string s in place */
static void
reverse(char *s)
{
	int t, j, i;

	for (j = 0, i = strlen(s) - 1; j < i; j++, i--) {
		t = s[j];
		s[j] = s[i];
		s[i] = t;
	}
}

/* write the decimal digits of n to buf */
static void
itoa_(int n, char *buf)
{
	int i;

	i = 0;
	do {
		buf[i++] = n % 10 + '0';
		n /= 10;
	} while (n > 0);
	buf[i] = 0;
	reverse(buf);
}

/* look for the register name in b1 (behind its 'r') in text: return 1 from ovlap when found */
#define SCAN() \
	st = text; \
	while (*st) { \
		if (*st++ != 'r') \
			continue; \
		sq = b1; \
		while (*sq++ == *st++) { \
			if (*sq) \
				continue; \
			if (*st == 0) \
				return 1; \
			if (isalnum(*st)) \
				continue; \
			return 1; \
		} \
	}

/* does the operand text mention a register overlapping register `name'? */
static int
ovlap(char *text, char *name)
{
	register char *st, *sq, *b1, *b2;
	register int n, i;
	char b[8];
	int max;

	if (text == 0)
		text = nullcode;
	b1 = b + 1;
	b2 = b + 2;
	if (*name++ != 'r')
		return 0;
	b[0] = 'r';
	max = 15;
	switch (*name) {
	case 'h':
	case 'l':
		max = 7;
		name++;
		/* FALLTHROUGH */
	default:
		n = atoi(name);
		if (n < 0 || n > max)
			return 0;
		strcpy(b1, name);
		SCAN();
		*b1 = 'h';
		strcpy(b2, name);
		SCAN();
		*b1 = 'l';
		SCAN();
		*b1 = 'r';
		if (n % 2)
			itoa_(n - 1, b2);
		else
			strcpy(b2, name);
		SCAN();
		*b1 = 'q';
		itoa_(n - n % 4, b2);
		SCAN();
		return 0;
	case 'r':
		name++;
		n = atoi(name);
		if (n < 0 || n > max)
			return 0;
		*b1 = 'r';
		strcpy(b2, name);
		SCAN();
		strcpy(b1, name);
		SCAN();
		itoa_(n + 1, b1);
		SCAN();
		*b1 = 'h';
		strcpy(b2, name);
		SCAN();
		*b1 = 'l';
		SCAN();
		itoa_(n + 1, b2);
		SCAN();
		*b1 = 'h';
		SCAN();
		*b1 = 'q';
		itoa_(n - n % 4, b2);
		SCAN();
		return 0;
	case 'q':
		name++;
		n = atoi(name);
		if (n < 0 || n > max)
			return 0;
		*b1 = 'q';
		strcpy(b2, name);
		SCAN();
		*b1 = 'r';
		SCAN();
		itoa_(n + 2, b2);
		SCAN();
		for (i = n; i < n + 4; i++) {
			itoa_(i, b1);
			SCAN();
			*b1 = 'h';
			itoa_(i, b2);
			SCAN();
			*b1 = 'l';
			SCAN();
		}
		return 0;
	}
}

/* is the operand a high byte register? */
int
ishigh(char *s)
{
	return *s == 'r' && s[1] == 'h';
}

/* the words of an operand of size type */
int
nwords(int type)
{
	return type == SLONG ? 2 : 1;
}

/*
 * Record the registers named in operand s: a[n] for r0..r7 (and the
 * partners of long/quad registers), b[n] for the long registers r8..r14.
 */
static void
markregs(char *s, char *b, char *a)
{
	register char *p;
	register int n;
	char flag;

	flag = 0;
	p = s ? s : nullcode;
	switch (*p) {
	case '@':
		p++;
		/* fall through */
	case 'r':
		switch (p[1]) {
		case 'h':
		case 'l':
			p++;
			break;
		case 'q':
		case 'r':
			p++;
			flag = 1;
			break;
		}
		n = atoi(p + 1);
		if (n >= 0 && n < 8) {
			a[n] = 1;
			if (flag) {
				a[n + 1] = 1;
				if (*p == 'q') {
					a[n + 2] = 1;
					a[n + 3] = 1;
				}
			}
		} else if (n > 7 && n < 15 && flag) {
			b[n] = 1;
			if (*p == 'q')
				b[n + 2] = 1;
		}
		if (n >= 10)
			p++;
		if (p[2] == '(')
			markregs(p + 3, b, a);	/* the original passes the character p[3] (a bug; the branch does not occur) */
		return;
	}
	while (*p) {
		if (*p == '(') {
			markregs(++p, b, a);
			return;
		}
		p++;
	}
}