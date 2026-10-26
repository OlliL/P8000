/*
 * z23.c - popt, part 3: register liveness (isused) and the
 * "stripldm" pass (procopt), decompiled from the WEGA /lib/popt.
 */
#include <stdio.h>
#include <ctype.h>
#include "z2.h"

/* write the decimal digits of n to buf */
static void
itoa_(n, buf)
int n;
char *buf;
{
	int i, j, t;

	i = 0;
	do {
		buf[i++] = n % 10 + '0';
		n /= 10;
	} while (n > 0);
	buf[i] = 0;
	for (j = 0, i--; j < i; j++, i--) {
		t = buf[j];
		buf[j] = buf[i];
		buf[i] = t;
	}
}

/*
 * Look for "r<pat>" in text, where pat is a register number, optionally
 * with an h/l/r/q prefix.  The match must not be followed by an
 * alphanumeric character.  The scan resumes after a failed comparison
 * at the character where the comparison stopped (not at the 'r'+1).
 */
static int
scan(text, pat)
char *text;
char *pat;
{
	char *t, *q;
	int a, b;

	t = text;
	while (*t) {
		if (*t++ != 'r')
			continue;
		q = pat;
		for (;;) {
			a = *q++;
			b = *t++;
			if (a != b)
				break;
			if (*q)
				continue;
			if (*t == 0)
				return 1;
			if (isalnum((unsigned char)*t))
				continue;
			return 1;
		}
	}
	return 0;
}

/* does the operand text mention a register overlapping register `name'? */
static int
ovlap(text, name)
char *text;
char *name;
{
	char b[40], *p;
	int n, max, i;

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
		if (scan(text, b + 1))
			return 1;
		b[1] = 'h';
		strcpy(b + 2, p);
		if (scan(text, b + 1))
			return 1;
		b[1] = 'l';
		if (scan(text, b + 1))
			return 1;
		b[1] = 'r';
		if (n % 2)
			itoa_(n - 1, b + 2);
		else
			strcpy(b + 2, p);
		if (scan(text, b + 1))
			return 1;
		b[1] = 'q';
		itoa_(n - n % 4, b + 2);
		return scan(text, b + 1);
	case 'r':
		p++;
		n = atoi(p);
		if (n < 0 || n > max)
			return 0;
		b[1] = 'r';
		strcpy(b + 2, p);
		if (scan(text, b + 1))
			return 1;
		strcpy(b + 1, p);
		if (scan(text, b + 1))
			return 1;
		itoa_(n + 1, b + 1);
		if (scan(text, b + 1))
			return 1;
		b[1] = 'h';
		strcpy(b + 2, p);
		if (scan(text, b + 1))
			return 1;
		b[1] = 'l';
		if (scan(text, b + 1))
			return 1;
		itoa_(n + 1, b + 2);
		if (scan(text, b + 1))
			return 1;
		b[1] = 'h';
		if (scan(text, b + 1))
			return 1;
		b[1] = 'q';
		itoa_(n - n % 4, b + 2);
		return scan(text, b + 1);
	case 'q':
		p++;
		n = atoi(p);
		if (n < 0 || n > max)
			return 0;
		b[1] = 'q';
		strcpy(b + 2, p);
		if (scan(text, b + 1))
			return 1;
		b[1] = 'r';
		if (scan(text, b + 1))
			return 1;
		itoa_(n + 2, b + 2);
		if (scan(text, b + 1))
			return 1;
		for (i = n; i < n + 4; i++) {
			itoa_(i, b + 1);
			if (scan(text, b + 1))
				return 1;
			b[1] = 'h';
			itoa_(i, b + 2);
			if (scan(text, b + 1))
				return 1;
			b[1] = 'l';
			if (scan(text, b + 1))
				return 1;
		}
		return 0;
	}
}

/*
 * Is register number `reg' (spelled `str') read by the code following
 * node before it is overwritten?  Unknown cases count as "used".
 */
static int
usesreg(node, str, reg)
struct node *node;
char *str;
int reg;
{
	char buf[50];
	struct node *p;
	int r10, r9;
	int res;

	strcpy(buf, str);
	res = 0;
	for (p = node->forw; p; p = p->forw) {
		switch (p->op) {
		case CJP:
		case JP:
			res = 1;
			goto out;
		case LD:
		case POP:
		case LDA:
			dualop(p);
			if (p->op == POP) {
				if (ovlap(RT1 + 1, buf)) {
					res = 1;
					goto out;
				}
			} else if (ovlap(RT1, buf)) {
				res = 1;
				goto out;
			}
			if (p->subop == SBYTE && reg == isreg(RT1)) {
				res = 1;
				goto out;
			}
			if (equstr(RT2, buf))
				goto out;
			if (p->subop == SLONG) {
				r10 = isreg(RT2);
				if (r10 >= 0 && reg == r10 + 1)
					goto out;
			}
			if (ovlap(RT2, buf)) {
				res = 1;
				goto out;
			}
			continue;
		case CLR:
			r10 = 0;
			if (p->subop == SLONG) {
				r10 = isreg(p->code);
				if (r10 >= 0 && reg == r10 + 1)
					goto out;
			}
			if (r10 >= 0 && equstr(p->code, buf))
				goto out;
			dualop(p);
			if (ovlap(RT1, buf)) {
				res = 1;
				goto out;
			}
			continue;
		case EXTS:
			r10 = isreg(p->code);
			if (r10 < 0)
				continue;
			switch (p->subop) {
			case 0:
				if (reg == r10 + 1) {
					res = 1;
					goto out;
				}
				if (r10 != reg)
					continue;
				goto out;
			case SBYTE:
				if (r10 != reg)
					continue;
				res = 1;
				goto out;
			case SLONG:
				if (reg == r10 + 2 || reg == r10 + 3) {
					res = 1;
					goto out;
				}
				if (r10 == reg || reg == r10 + 1)
					goto out;
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
				res = 1;
				goto out;
			}
			break;
		case MUL:
			dualop(p);
			r10 = isreg(RT2);
			if (r10 < 0)
				break;
			if (p->subop == 0) {
				if (ovlap(RT1, buf) || reg == r10 + 1) {
					res = 1;
					goto out;
				}
				if (reg != r10)
					break;
				goto out;
			}
			if (p->subop != SLONG)
				break;
			if (ovlap(RT1, buf) || reg == r10 + 2 || reg == r10 + 3) {
				res = 1;
				goto out;
			}
			if (reg == r10 || reg == r10 + 1)
				goto out;
			break;
		case DIV:
			dualop(p);
			r10 = isreg(RT2);
			if (r10 >= 0 && reg == r10 + 1) {
				res = 1;
				goto out;
			}
			if (p->subop != SLONG)
				break;
			if (reg == r10 + 2 || reg == r10 + 3) {
				res = 1;
				goto out;
			}
			break;
		case CALL:
			r9 = reg;
			if (node->subop == SBYTE && reg > 8)
				r9 = reg - 8;
			if (r9 >= 2 && r9 <= 7) {
				res = 1;
				goto out;
			}
#ifdef SPOPT
			if (reg >= 2) {
				if (p->code[0] != '@')
					continue;
				r10 = isreg(p->code + 1);
				if (reg == r10) {
					res = 1;
					goto out;
				}
				r10 = reg ^ 1;
				if (r10 == 0)
					continue;
				res = 1;
				goto out;
			}
#else
			if (reg >= 2)
				continue;
#endif
			if (p->code[0] != '@')
				goto out;
			r10 = isreg(p->code + 1);
			if (reg != r10)
				goto out;
			res = 1;
			goto out;
		case RET:
#ifdef SPOPT
			if (reg == 2 || reg == 3 || reg == 12 || reg == 13) {
#else
			if (reg == 2 || reg == 3 || reg == 14) {
#endif
				res = 1;
				goto out;
			}
			goto out;
		default:
			break;
		}
		/* default: any mention of the register counts as a use */
		if (ovlap(p->code, buf)) {
			res = 1;
			goto out;
		}
	}
out:
	dualop(node);
	return res;
}

int
isused(p, reg)
struct node *p;
char *reg;
{
	int r12, c;

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
 * Record the registers named in operand s: a[n] for r0..r7 (and the
 * partners of long/quad registers), b[n] for the long registers r8..r14.
 */
static void
markregs(s, a, b)
char *s;
char *a;
char *b;
{
	char *p;
	int n, flag;

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
			markregs(p + 1, a, b);
			return;
		}
		p++;
	}
}

struct rent {
	int len;
	char s1[12];
	char s2[12];
};

/* rename registers in the code between the two ldm's */
static void
rename_regs(t, first_ldm, last_ldm)
struct rent *t;
struct node *first_ldm;
struct node *last_ldm;
{
	struct node *p;
	struct rent *e;
	char *r11, *r12, *r9, *q;
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
struct node *p;
int n;
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
		q = strchr(p->code, '#');
		sprintf(q + 1, "%d", n);
	}
}

/*
 * The register save/restore ldm pair of a function is shortened by
 * renaming register variables to unused low registers.
 */
void
procopt()
{
	char a[16], b[16];
	struct node *p, *ld1, *ld2;
	struct rent t[16];
	char *s;
	int i, r8, r9, k, r14, n;

	ld1 = ld2 = 0;
	clearmem(a, sizeof a);
	clearmem(b, sizeof b);
	a[0] = 1;
	for (p = &first; p; p = p->forw) {
		switch (p->op) {
		case JP: case CLR: case COM: case INC: case DEC: case NEG:
		case TST: case SRA: case SLA: case EXTS: case SOB: case LDA:
		case 38: case 41: case 43: case SRL: case SLL:
			markregs(p->code, a, b);
			break;
		case LD: case CP: case ADD: case SUB: case BIT: case AND:
		case OR: case MUL: case DIV: case SDA: case XOR: case PUSH:
		case POP: case SET: case RES: case SDL:
			dualop(p);
			markregs(RT2, a, b);
			markregs(RT1, a, b);
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
}
