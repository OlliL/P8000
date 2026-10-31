/*
 * z23.c - popt, part 3: register liveness (isused) and the
 * "stripldm" pass (procopt), decompiled from the WEGA /lib/popt.
 */
#include <stdio.h>
#include <ctype.h>
#include "z2.h"

static char sccsid[] = "@[$]z23.c\t1.6  03/25/83 18:10:30 Copr.1981 87wega3.2.";

static void rename_regs();
static void fixldm();
static int usesreg();
static int ovlap();
static void markregs();
#ifdef OLD_D1F
static void stripframe();
static int fpfirst();
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
procopt()
{
	struct node *p, *ld1, *ld2;
	char b[15], a[8];
	struct rent t[16];
	char *s;
	register int i;
	int r8, r9, k, r14, n;

	ld1 = ld2 = 0;
	a[0] = 1;
	for (i = 1; i < 8; i++)
		a[i] = 0;
	for (i = 0; i < 15; i++)
		b[i] = 0;
	for (p = &first; p; p = p->forw) {
		switch (p->op) {
		case JP: case CLR: case COM: case INC: case DEC: case NEG:
		case TST: case SRA: case SLA: case EXTS: case SOB: case LDA:
		case 38: case 41: case 43: case SRL: case SLL:
			markregs(p->code, b, a);
			break;
		case LD: case CP: case ADD: case SUB: case BIT: case AND:
		case OR: case MUL: case DIV: case SDA: case XOR: case PUSH:
		case POP: case SET: case RES: case SDL:
			dualop(p);
			markregs(RT2, b, a);
			markregs(RT1, b, a);
			break;
		case CALL:
			fprintf(stderr, "CALL instruction in stripldm()\n");
			return;
		case LDM:
			if (ld1 == 0)
				ld1 = p;
			else
				ld2 = p;
			break;
		default:
			break;
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
	for (i = 1; i < 8; i++)
		if (a[i] == 0)
			r9++;
	if (r9 == 0)
		return;
	k = 0;
	for (r14 = 1; r9 > k && r14 < 8; r14++) {
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
			sprintf(t[k].s1, "r%d", n - 1);
			t[k].len = strlen(t[k].s1);
			sprintf(t[k].s2, "r%d", r14);
			k++;
			r14++;
			sprintf(t[k].s1, "r%d", n);
			t[k].len = strlen(t[k].s1);
			sprintf(t[k].s2, "r%d", r14);
			k++;
		} else {
			sprintf(t[k].s1, "r%d", n);
			t[k].len = strlen(t[k].s1);
			sprintf(t[k].s2, "r%d", r14);
			k++;
		}
	}
	if (r8 < k) {
		k = r8;
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

#ifdef OLD_D1F
/*
 * The old popt: when no register is saved any more and no instruction reads the frame, the stores into the frame (the
 * spills of the parameters) and the frame itself (dec/inc fp) go too.
 */
static void
stripframe()
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
fpfirst(c)
register char *c;
{
	for (; *c && *c != ','; c++)
		if (c[0] == '(' && c[1] == 'f' && c[2] == 'p')
			return 1;
	return 0;
}
#endif

/* rename registers in the code between the two ldm's */
static void
rename_regs(t, first_ldm, last_ldm)
register struct rent *t;
struct node *first_ldm;
struct node *last_ldm;
{
	register struct node *p;
	register struct rent *e;
	register char *r11, *r12, *r9;
	char *q;
	int changed;

	for (p = first_ldm->forw; p != last_ldm; p = p->forw) {
		if (p->op == LABEL || p->op == COMMENT)
			continue;
		changed = 0;
		if (p->code == 0)
			continue;
		for (e = t; e->len > 0; e++) {
			r11 = p->code;
			while (*r11) {
				if (strncmp(e->s1, r11, e->len) == 0) {
					r9 = r11 + 1;
					if (*r9 == '1' && isdigit((unsigned char)r9[1]))
						r9++;
					r9++;
					for (q = e->s2; *q; )
						*r11++ = *q++;
					while (r11 != r9) {
						*r11++ = (char)0xff;
						changed = 1;
					}
					r11--;
				}
				r11++;
			}
		}
		if (changed) {
			r12 = p->code;
			while (*r12 != (char)0xff)
				r12++;
			r11 = r12 + 1;
			for (;;) {
				while (*r11 == (char)0xff)
					r11++;
				*r12++ = *r11;
				if (*r11++ == 0)
					break;
			}
		}
	}
}

/* shorten the ldm to n registers: delete it, make it an ld, or recount */
static void
fixldm(p, n)
register struct node *p;
register int n;
{
	char *q;

	if (n == 0) {
		p->back->forw = p->forw;
		if (p->forw)
			p->forw->back = p->back;
	} else if (n == 1) {
		p->op = LD;
		q = strrchr(p->code, ',');
		*q = 0;
	} else {
		q = strrchr(p->code, '#');
		sprintf(q + 1, "%d", n);
	}
}

int
isused(p, reg)
register struct node *p;
register char *reg;
{
	register int r12;
	int c;

	r12 = isreg(reg);
	if (r12 < 0)
		return 1;
	c = reg[1];
	if (c == 'q') {
		if (usesreg(p, reg, r12) || usesreg(p, reg, r12 + 1) ||
		    usesreg(p, reg, r12 + 2) || usesreg(p, reg, r12 + 3))
			return 1;
		return 0;
	}
	if (c == 'r') {
		if (usesreg(p, reg, r12))
			return 1;
		return usesreg(p, reg, r12 + 1) != 0;
	}
	return usesreg(p, reg, r12);
}

/*
 * Is register number `reg' (spelled `str') read by the code following
 * node before it is overwritten?  Unknown cases count as "used".
 */
static int
usesreg(node, str, reg)
register struct node *node;
register char *str;
register int reg;
{
	char buf[50];
	register struct node *p;
	register int r10, r9;

	strcpy(buf, str);
	for (p = node->forw; p; p = p->forw) {
		switch (p->op) {
		case CJP:
		case JP:
			goto out1;
		case LD:
		case POP:
		case LDA:
			dualop(p);
			if (p->op == POP) {
				if (ovlap(RT1 + 1, buf)) {
					goto out1;
				}
			} else if (ovlap(RT1, buf)) {
				goto out1;
			}
			if (p->subop == SBYTE && reg == isreg(RT1)) {
				goto out1;
			}
			if (equstr(RT2, buf))
				goto out0;
			if (p->subop == SLONG) {
				r10 = isreg(RT2);
				if (r10 >= 0 && reg == r10 + 1)
					goto out0;
			}
			if (ovlap(RT2, buf)) {
				goto out1;
			}
			continue;
		case CLR:
			r10 = 0;
			if (p->subop == SLONG) {
				r10 = isreg(p->code);
				if (r10 >= 0 && reg == r10 + 1)
					goto out0;
			}
			if (r10 >= 0 && equstr(p->code, buf))
				goto out0;
			dualop(p);
			if (ovlap(RT1, buf)) {
				goto out1;
			}
			continue;
		case EXTS:
			r10 = isreg(p->code);
			if (r10 < 0)
				continue;
			switch (p->subop) {
			case 0:
				if (reg == r10 + 1) {
					goto out1;
				}
				if (r10 != reg)
					continue;
				goto out0;
			case SBYTE:
				if (r10 != reg)
					continue;
				goto out1;
			case SLONG:
				if (reg == r10 + 2 || reg == r10 + 3) {
					goto out1;
				}
				if (r10 == reg || reg == r10 + 1)
					goto out0;
				continue;
			}
			continue;
		case PUSH:
			if (p->subop != SLONG)
				break;
			dualop(p);
			r10 = isreg(RT1);
			if (r10 < 0)
				break;
			if (reg == r10 + 1) {
				goto out1;
			}
			break;
		case MUL:
			dualop(p);
			r10 = isreg(RT2);
			if (r10 < 0)
				break;
			if (p->subop == 0) {
				if (ovlap(RT1, buf) || reg == r10 + 1) {
					goto out1;
				}
				if (reg != r10)
					break;
				goto out0;
			}
			if (p->subop != SLONG)
				break;
			if (ovlap(RT1, buf) || reg == r10 + 2 || reg == r10 + 3) {
				goto out1;
			}
			if (reg == r10 || reg == r10 + 1)
				goto out0;
			break;
		case DIV:
			dualop(p);
			r10 = isreg(RT2);
			if (r10 >= 0 && reg == r10 + 1) {
				goto out1;
			}
			if (p->subop != SLONG)
				break;
			if (reg == r10 + 2 || reg == r10 + 3) {
				goto out1;
			}
			break;
		case CALL:
			r9 = reg;
			if (node->subop == SBYTE && reg > 8)
				r9 = reg - 8;
			if (r9 >= 2 && r9 <= 7) {
				goto out1;
			}
#ifdef SPOPT
			if (reg >= 2) {
				if (p->code[0] != '@')
					continue;
				r10 = isreg(p->code + 1);
				if (reg == r10) {
					goto out1;
				}
				r10 = reg ^ 1;
				if (r10 == 0)
					continue;
				goto out1;
			}
#else
			if (reg >= 2)
				continue;
#endif
			if (p->code[0] != '@')
				goto out0;
			r10 = isreg(p->code + 1);
			if (reg != r10)
				goto out0;
			goto out1;
		case RET:
#ifdef SPOPT
			if (reg == 2 || reg == 3 || reg == 12 || reg == 13) {
#else
			if (reg == 2 || reg == 3 || reg == 14) {
#endif
				goto out1;
			}
			goto out0;
		default:
			break;
		}
		/* default: any mention of the register counts as a use */
		if (ovlap(p->code, buf)) {
			goto out1;
		}
	}
out0:
	dualop(node);
	return 0;
out1:
	dualop(node);
	return 1;
}

/* reverse the string s in place */
static void
reverse(s)
char *s;
{
	int t, j, i;

	i = strlen(s) - 1;
	for (j = 0; j < i; j++, i--) {
		t = s[j];
		s[j] = s[i];
		s[i] = t;
	}
}

/* write the decimal digits of n to buf */
static void
itoa_(n, buf)
int n;
char *buf;
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

/* does the operand text mention a register overlapping register `name'? */
static int
ovlap(text, name)
char *text;
char *name;
{
	char b[40];
	register char *p;
	register int n;
	int max, i;

	if (text == 0)
		text = nullcode;
	p = name;
	if (*p++ != 'r')
		return 0;
	b[0] = 'r';
	max = 15;
	switch (*p) {
	case 'h':
	case 'l':
		max = 7;
		p++;
		/* FALLTHROUGH */
	default:
	word:
		n = atoi(p);
		if (n < 0 || n > max)
			return 0;
		strcpy(b + 1, p);
		{
		register char *st, *sq;
		register int sa, sb;

		st = text;
		while (*st) {
			if (*st++ != 'r')
				continue;
			sq = b + 1;
			for (;;) {
				sa = *sq++;
				sb = *st++;
				if (sa != sb)
					break;
				if (*sq)
					continue;
				if (*st == 0)
					return 1;
				if (isalnum((unsigned char)*st))
					continue;
				return 1;
			}
		}
	}
		b[1] = 'h';
		strcpy(b + 2, p);
		{
		register char *st, *sq;
		register int sa, sb;

		st = text;
		while (*st) {
			if (*st++ != 'r')
				continue;
			sq = b + 1;
			for (;;) {
				sa = *sq++;
				sb = *st++;
				if (sa != sb)
					break;
				if (*sq)
					continue;
				if (*st == 0)
					return 1;
				if (isalnum((unsigned char)*st))
					continue;
				return 1;
			}
		}
	}
		b[1] = 'l';
		{
		register char *st, *sq;
		register int sa, sb;

		st = text;
		while (*st) {
			if (*st++ != 'r')
				continue;
			sq = b + 1;
			for (;;) {
				sa = *sq++;
				sb = *st++;
				if (sa != sb)
					break;
				if (*sq)
					continue;
				if (*st == 0)
					return 1;
				if (isalnum((unsigned char)*st))
					continue;
				return 1;
			}
		}
	}
		b[1] = 'r';
		if (n % 2)
			itoa_(n - 1, b + 2);
		else
			strcpy(b + 2, p);
		{
		register char *st, *sq;
		register int sa, sb;

		st = text;
		while (*st) {
			if (*st++ != 'r')
				continue;
			sq = b + 1;
			for (;;) {
				sa = *sq++;
				sb = *st++;
				if (sa != sb)
					break;
				if (*sq)
					continue;
				if (*st == 0)
					return 1;
				if (isalnum((unsigned char)*st))
					continue;
				return 1;
			}
		}
	}
		b[1] = 'q';
		itoa_(n - n % 4, b + 2);
		{
		register char *st, *sq;
		register int sa, sb;

		st = text;
		while (*st) {
			if (*st++ != 'r')
				continue;
			sq = b + 1;
			for (;;) {
				sa = *sq++;
				sb = *st++;
				if (sa != sb)
					break;
				if (*sq)
					continue;
				if (*st == 0)
					return 1;
				if (isalnum((unsigned char)*st))
					continue;
				return 1;
			}
		}
	}
		return 0;
	case 'r':
		p++;
		n = atoi(p);
		if (n < 0 || n > max)
			return 0;
		b[1] = 'r';
		strcpy(b + 2, p);
		{
		register char *st, *sq;
		register int sa, sb;

		st = text;
		while (*st) {
			if (*st++ != 'r')
				continue;
			sq = b + 1;
			for (;;) {
				sa = *sq++;
				sb = *st++;
				if (sa != sb)
					break;
				if (*sq)
					continue;
				if (*st == 0)
					return 1;
				if (isalnum((unsigned char)*st))
					continue;
				return 1;
			}
		}
	}
		strcpy(b + 1, p);
		{
		register char *st, *sq;
		register int sa, sb;

		st = text;
		while (*st) {
			if (*st++ != 'r')
				continue;
			sq = b + 1;
			for (;;) {
				sa = *sq++;
				sb = *st++;
				if (sa != sb)
					break;
				if (*sq)
					continue;
				if (*st == 0)
					return 1;
				if (isalnum((unsigned char)*st))
					continue;
				return 1;
			}
		}
	}
		itoa_(n + 1, b + 1);
		{
		register char *st, *sq;
		register int sa, sb;

		st = text;
		while (*st) {
			if (*st++ != 'r')
				continue;
			sq = b + 1;
			for (;;) {
				sa = *sq++;
				sb = *st++;
				if (sa != sb)
					break;
				if (*sq)
					continue;
				if (*st == 0)
					return 1;
				if (isalnum((unsigned char)*st))
					continue;
				return 1;
			}
		}
	}
		b[1] = 'h';
		strcpy(b + 2, p);
		{
		register char *st, *sq;
		register int sa, sb;

		st = text;
		while (*st) {
			if (*st++ != 'r')
				continue;
			sq = b + 1;
			for (;;) {
				sa = *sq++;
				sb = *st++;
				if (sa != sb)
					break;
				if (*sq)
					continue;
				if (*st == 0)
					return 1;
				if (isalnum((unsigned char)*st))
					continue;
				return 1;
			}
		}
	}
		b[1] = 'l';
		{
		register char *st, *sq;
		register int sa, sb;

		st = text;
		while (*st) {
			if (*st++ != 'r')
				continue;
			sq = b + 1;
			for (;;) {
				sa = *sq++;
				sb = *st++;
				if (sa != sb)
					break;
				if (*sq)
					continue;
				if (*st == 0)
					return 1;
				if (isalnum((unsigned char)*st))
					continue;
				return 1;
			}
		}
	}
		itoa_(n + 1, b + 2);
		{
		register char *st, *sq;
		register int sa, sb;

		st = text;
		while (*st) {
			if (*st++ != 'r')
				continue;
			sq = b + 1;
			for (;;) {
				sa = *sq++;
				sb = *st++;
				if (sa != sb)
					break;
				if (*sq)
					continue;
				if (*st == 0)
					return 1;
				if (isalnum((unsigned char)*st))
					continue;
				return 1;
			}
		}
	}
		b[1] = 'h';
		{
		register char *st, *sq;
		register int sa, sb;

		st = text;
		while (*st) {
			if (*st++ != 'r')
				continue;
			sq = b + 1;
			for (;;) {
				sa = *sq++;
				sb = *st++;
				if (sa != sb)
					break;
				if (*sq)
					continue;
				if (*st == 0)
					return 1;
				if (isalnum((unsigned char)*st))
					continue;
				return 1;
			}
		}
	}
		b[1] = 'q';
		itoa_(n - n % 4, b + 2);
		{
		register char *st, *sq;
		register int sa, sb;

		st = text;
		while (*st) {
			if (*st++ != 'r')
				continue;
			sq = b + 1;
			for (;;) {
				sa = *sq++;
				sb = *st++;
				if (sa != sb)
					break;
				if (*sq)
					continue;
				if (*st == 0)
					return 1;
				if (isalnum((unsigned char)*st))
					continue;
				return 1;
			}
		}
	}
		return 0;
	case 'q':
		p++;
		n = atoi(p);
		if (n < 0 || n > max)
			return 0;
		b[1] = 'q';
		strcpy(b + 2, p);
		{
		register char *st, *sq;
		register int sa, sb;

		st = text;
		while (*st) {
			if (*st++ != 'r')
				continue;
			sq = b + 1;
			for (;;) {
				sa = *sq++;
				sb = *st++;
				if (sa != sb)
					break;
				if (*sq)
					continue;
				if (*st == 0)
					return 1;
				if (isalnum((unsigned char)*st))
					continue;
				return 1;
			}
		}
	}
		b[1] = 'r';
		{
		register char *st, *sq;
		register int sa, sb;

		st = text;
		while (*st) {
			if (*st++ != 'r')
				continue;
			sq = b + 1;
			for (;;) {
				sa = *sq++;
				sb = *st++;
				if (sa != sb)
					break;
				if (*sq)
					continue;
				if (*st == 0)
					return 1;
				if (isalnum((unsigned char)*st))
					continue;
				return 1;
			}
		}
	}
		itoa_(n + 2, b + 2);
		{
		register char *st, *sq;
		register int sa, sb;

		st = text;
		while (*st) {
			if (*st++ != 'r')
				continue;
			sq = b + 1;
			for (;;) {
				sa = *sq++;
				sb = *st++;
				if (sa != sb)
					break;
				if (*sq)
					continue;
				if (*st == 0)
					return 1;
				if (isalnum((unsigned char)*st))
					continue;
				return 1;
			}
		}
	}
		for (i = n; i < n + 4; i++) {
			itoa_(i, b + 1);
			{
		register char *st, *sq;
		register int sa, sb;

		st = text;
		while (*st) {
			if (*st++ != 'r')
				continue;
			sq = b + 1;
			for (;;) {
				sa = *sq++;
				sb = *st++;
				if (sa != sb)
					break;
				if (*sq)
					continue;
				if (*st == 0)
					return 1;
				if (isalnum((unsigned char)*st))
					continue;
				return 1;
			}
		}
	}
			b[1] = 'h';
			itoa_(i, b + 2);
			{
		register char *st, *sq;
		register int sa, sb;

		st = text;
		while (*st) {
			if (*st++ != 'r')
				continue;
			sq = b + 1;
			for (;;) {
				sa = *sq++;
				sb = *st++;
				if (sa != sb)
					break;
				if (*sq)
					continue;
				if (*st == 0)
					return 1;
				if (isalnum((unsigned char)*st))
					continue;
				return 1;
			}
		}
	}
			b[1] = 'l';
			{
		register char *st, *sq;
		register int sa, sb;

		st = text;
		while (*st) {
			if (*st++ != 'r')
				continue;
			sq = b + 1;
			for (;;) {
				sa = *sq++;
				sb = *st++;
				if (sa != sb)
					break;
				if (*sq)
					continue;
				if (*st == 0)
					return 1;
				if (isalnum((unsigned char)*st))
					continue;
				return 1;
			}
		}
	}
		}
		return 0;
	}
}

/*
 * Record the registers named in operand s: a[n] for r0..r7 (and the
 * partners of long/quad registers), b[n] for the long registers r8..r14.
 */
static void
markregs(s, b, a)
register char *s;
register char *b;
register char *a;
{
	register char *p;
	register int n;
	int flag;

	flag = 0;
	p = s;
	if (p == 0)
		p = nullcode;
	if (*p == '@')
		p++;
	else if (*p != 'r')
		goto scanparen;
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
	/*
	 * "rN(" would recurse with the character after the paren used as
	 * a pointer; this never matters, nothing to do.
	 */
	return;
scanparen:
	while (*p) {
		if (*p == '(') {
			markregs(p + 1, b, a);
			return;
		}
		p++;
	}
}