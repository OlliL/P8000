/*
 * alias2.c - gopt: the alias analysis (the affect matrix, the alias matrix, the bit matrices). Written from /lib/gopt (no source known);
 * names guessed, the callees of other modules by the address (fnXXXX).
 */
#include "gopt.h"

static char sccsid[] = "@[$]alias2.c\t1.5  02/07/83 15:50:16 Copr. 1981 87wega3.2.";

#define DECREF(t)	((((t) >> 2) & ~0xF) | ((t) & 0xF))

extern int aflag;
extern int nadr, nptr, nal, s4d7a;
extern struct { int tag; int x; } numtab[];
extern struct sym *mapn();
extern int tagfind(), itfind();
extern char *taballoc();
extern int *altab;
extern struct sym *symtab;
extern char ascii[];
extern struct quad *qhead;
extern struct quad *pcur;
extern struct qa opnd70c8;
extern int op70d8;
extern void prquad();
extern int cerror();
extern int testbit();
extern struct quad *fn5_2bca();
extern int checkops();
extern struct quad *fndcall();
extern int fndparm();
extern void setbit();

/* bit j of the row i of the matrix m (rows of dim words) */
void
clrbit(m, i, j, dim)
	int *m;
	int i;
	unsigned j;
	int dim;
{
	if (aflag > 2)
		printf("// setbit, i=%d, j=%d, dim=%d, m (in X) = %x\n", i, j, dim, m);
	m[dim * i + (j >> 4)] &= ~(1 << (j & 0xf));
}

void
setbit(m, i, j, dim)
	int *m;
	int i;
	unsigned j;
	int dim;
{
	if (aflag > 6)
		printf("// setbit, i=%d, j=%d, dim=%d, m (in X) = %X\n", i, j, dim, m);
	m[dim * i + (j >> 4)] |= 1 << (j & 0xf);
}

int
testbit(m, i, j, dim)
	unsigned *m;
	int i;
	unsigned j;
	int dim;
{
	if ((m[dim * i + (j >> 4)] >> (j & 0xf)) & 1) {
		if (aflag > 6)
			printf("// testbit=1, i=%d, j=%d, dim=%d, m (in x) = %X\n", i, j, dim, m);
		return 1;
	}
	return 0;
}

/* a matrix of a rows of b words is cleared */
void
clrmat(m, a, b)
	int *m;
	int a, b;
{
	register int i, j, base;

	for (i = 0; i < a; i++) {
		base = b * i;
		for (j = 0; j < b; j++)
			m[base + j] = 0;
	}
}

void
copymat(dst, src, a, b)
	int *dst, *src;
	int a, b;
{
	register int i, j, base;

	for (i = 0; i < a; i++) {
		base = b * i;
		for (j = 0; j < b; j++)
			dst[base + j] = src[base + j];
	}
}

/* a = b x c (the boolean matrix product) */
void
matmul(a, b, c, n, dim)
	int *a, *b, *c;
	int n, dim;
{
	register int i, j, k, v;

	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++) {
			v = 0;
			for (k = 0; k < n; k++)
				v |= testbit(b, k, i, dim) && testbit(c, k, j, dim);
			if (v)
				setbit(a, i, j, dim);
		}
}

/* the transitive closure of the matrix m */
void
closure(m, n, dim)
	int *m;
	int n, dim;
{
	register int i, j, k;

	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			if (testbit(m, j, i, dim))
				for (k = 0; k < n; k++)
					if (testbit(m, j, k, dim) || testbit(m, i, k, dim))
						setbit(m, j, k, dim);
}

/* the diagonal */
void
diag(m, n, dim)
	int *m;
	int n, dim;
{
	register int i;

	for (i = 0; i < n; i++)
		setbit(m, i, i, dim);
}

void
pralias()
{
	int i;

	printf("// aliasmap: adrindx =%d, ptrindx = %d, aliasindx = %d \n", nadr, nptr, nal);
	for (i = 0; i < nal; i++) {
		printf("// \t%d\t%s\t%d\n", i, ascii + symtab[altab[i]].nameoff, altab[i]);
	}
}

/* the matrix m with the names of the variables (debug) */
void
prmat(m, n, dim)
	int *m;
	int n, dim;
{
	register int i, j;

	printf("\n// \t");
	for (i = 0; i < n; i++)
		if (altab[i] == 0)
			printf(" *");
		else
			printf(" %s", ascii + symtab[altab[i]].nameoff);
	printf("\n");
	for (i = 0; i < n; i++) {
		putchar('\n');
		printf("//  %s\t", ascii + symtab[altab[i]].nameoff);
		for (j = 0; j < n; j++) {
			putchar(' ');
			if (testbit(m, i, j, dim))
				putchar('1');
			else
				putchar('0');
		}
	}
	putchar('\n');
}

/* the first quad of the list q that is a call (0x2f or 0x34), 0 when there is none */
struct quad *
fndcall(q)
	struct quad *q;
{
	for (; q != 0; q = q->next)
		if (q->op == 0x2f || q->op == 0x34)
			break;
	if (aflag > 2 && q) {
		printf("\n");
		if (q->op == 0x2f || q->op == 0x34)
			prquad(q);
		printf("\n");
	}
	return q;
}

/* the quad from p on (round the list) whose operand is the temporary tmpno */
struct quad *
tmpsearch(p, tmpno, q)
	struct quad *p;
	int tmpno;
	struct quad *q;
{
	struct qa s;
	int found;

	found = 1;
	if (aflag > 2) {
		printf("// entering tmpsearch(tmpno=%d), p , q = \n", tmpno);
		prquad(p);
		prquad(q);
	}
	if (p->op == 7)
		s = p->b;
	else
		s = p->c;
	while (s.b2 != 3 || s.w4 != tmpno) {
		if (p == q) {
			if (found) {
				found = 0;
				if (aflag > 2)
					printf("tmpsearch: starting over\n");
				p = qhead;
				continue;
			}
			if (aflag > 2) {
				prquad("p:\n");
				prquad("q:\n");
			}
			cerror("tmpsearch: temp not found in il %d", tmpno);
		}
		p = p->next;
		if (p->op == 7)
			s = p->b;
		else
			s = p->c;
	}
	if (aflag > 2) {
		printf("// tmpsearch(tmpno=%d), found quad = \n", tmpno);
		prquad(p);
	}
	return p;
}

/* the actual parameter whose address is taken (the operand of the quad that makes it): found in opnd70c8 */
int
fndparm(q)
	struct quad *q;
{
	struct qa s;

	s = q->a;
	for (;;) {
		q = tmpsearch(pcur, s.w4, q);
		s = q->a;
		if (q->op == 3) {
			if (q->a.b2 != 4)
				return 0;
			op70d8 = 3;
			opnd70c8 = s;
			if (aflag > 2) {
				printf("// checkand, actual param  & op found:\n");
				prquad(q);
			}
			return 1;
		}
		if (s.b2 != 3)
			return 0;
	}
}

/*
 * the class of an operand is entered in the lists of the addresses (X) and of the pointers (Y), a structure with its members (names
 * guessed, written from /lib/gopt)
 */
void
setposition(cls, p, X, Y)
	int cls;
	struct qa *p;
	int *X, *Y;
{
	struct sym *u;
	int i;
	struct sym *v;
	struct qa t;

	if (aflag > 2) {
		printf("// entering setposition(), p->opnd = %s, class = %d\n", ascii + symtab[p->w4].nameoff, symtab[p->w4].sclass);
		printf("// p->att = %d\n", p->b2);
	}
	if (p->b2 == 3)
		return;
	if (s4d7a && ((symtab[p->w4].sflags >> 2) & 1) && symtab[p->w4].sclass != 8)
		return;
	if (symtab[p->w4].sclass == 9)
		return;
	if (symtab[p->w4].stype == 8 && symtab[p->w4].sx12 != -1) {
		v = mapn(symtab[p->w4].sx12);
		t.b2 = 4;
		t.w4 = v - symtab;
		if (!itfind(&t)) {
			taballoc(2, 6);
			v->sx14 = nal;
			symtab[p->w4].sx14 = nal;
			altab[nal++] = v - symtab;
		} else
			symtab[p->w4].sx14 = v->sx14;
		for (i = v->sx12; numtab[i].tag > 0; i++) {
			if (tagfind(numtab[i].tag) != -1) {
				t.w4 = mapn(numtab[i].tag) - symtab;
				t.b2 = 4;
				if (!itfind(&t))
					setposition(cls, &t, X, Y);
			}
		}
	}
	if (!itfind(p)) {
		taballoc(2, 6);
		symtab[p->w4].sx14 = nal;
		altab[nal++] = p->w4;
	}
	switch (cls) {
	case 0x18:
	case 0x1e:
	case 0x2f:
		for (i = 0; i < nptr; i++)
			if (Y[i] == p->w4)
				return;
		Y[nptr++] = p->w4;
		return;
	case 3:
	case 0x34:
		for (i = 0; i < nadr; i++)
			if (X[i] == p->w4)
				return;
		X[nadr++] = p->w4;
		if (p->b2 == 4 && (symtab[p->w4].stype & 0x30) == 0x30 && (DECREF(symtab[p->w4].stype) & 0x30) == 0x10) {
			Y[nptr++] = p->w4;
		}
		return;
	case 7:
		return;
	default:
		cerror("setposition: illegal operand\n");
	}
}

/* the alias analysis: the variables that are addressed (the list of the addresses) and the pointers are collected from the quads */
void
aliasm()
{
	struct quad *q;
	struct qa s;
	int cls;
	int A[512];
	int B[512];
	int i, n;

	nptr = 1;
	nadr = 1;
	nal = 1;
	altab = (int *)taballoc(2, 6);
	*altab = 0;
	A[0] = 0;
	B[0] = 0;
	pcur = q = qhead;
	for (; q != 0; q = q->next) {
		q = fndcall(q);
		if (q == 0)
			break;
		if (q->op == 0x3b) {
			pcur = fn5_2bca(q);
			continue;
		}
		if (q->op == 0x2f) {
			if (!checkops(q, 0))
				continue;
			cls = op70d8;
			s = opnd70c8;
			if (!checkops(q, 1))
				continue;
			setposition(cls, &s, B, A);
			if (q->a.b2 == 1 || q->a.b2 == 7)
				continue;
			setposition(op70d8, &opnd70c8, B, A);
		} else if (q->op == 0x34) {
			if (q->a.b2 == 3 && fndparm(q))
				setposition(0x34, &opnd70c8, B, A);
			else if (q->a.b2 == 4 && (DECREF(q->a.w0) & 0x30) == 0x10)
				setposition(0x18, &q->a, B, A);
		}
	}
	n = nadr;
	nadr = nal;
	taballoc(nadr * 2, 6);
	for (i = 0; i < n; i++)
		altab[nal++] = B[i];
	n = nptr;
	nptr = nal;
	taballoc(nal * 2, 6);
	for (i = 0; i < n; i++)
		altab[nal++] = A[i];
	if (aflag)
		pralias();
}

/*
 * follows the definitions of the temporary of q (mode 0: the result operand c, 1: the operand a) back to the operand that is a pointer:
 * in opnd70c8 and op70d8; returns 1 when found.  hist: the ops passed (the history).
 */
int
checkops(q, mode)
	struct quad *q;
	int mode;
{
	struct {
		unsigned fa:1, fb:1, fc:1, fd:1;
	} fl;
	struct qa s;
	int x;
	int found;
	struct qa t;
	int n;
	int hist[30];

	n = 0;
	hist[n++] = 0x2f;
	op70d8 = 0x2f;
	found = 0;
	fl.fa = 1;
	fl.fb = 1;
	fl.fc = 1;
	if (mode == 0) {
		s = q->c;
		if (s.b2 != 3) {
			if (s.b2 == 4 && (q->c.w0 & 0x30) == 0x10)
				goto success;
			if (aflag > 2) {
				printf("// Store's result opnd is not a ptr:\n");
				prquad(q);
			}
		fail:
			if (aflag > 2)
				printf("//    NULL\n");
			return 0;
		} else
			fl.fd = 0;
	} else if (mode == 1) {
		s = q->a;
		if (s.b2 != 3) {
			if ((s.b2 == 4 || s.b2 == 1 || s.b2 == 7) && (q->a.w0 & 0x30) == 0x10)
				goto success;
			goto fail;
		} else
			fl.fd = 1;
	} else
		cerror("illegal checkops call");
	x = 0;
	do {
		q = tmpsearch(pcur, s.w4, q);
		if (n >= 30)
			cerror("checkops: history exceeded");
		s = q->a;
		switch (q->op) {
		case 0x18:
		case 0x1e:
			fl.fb = 1;
			op70d8 = 0x18;
			hist[n++] = 0x18;
		post:
			if (s.b2 == 3)
				break;
			if (s.b2 != 4 || (q->a.w0 & 0x30) != 0x10)
				goto fail;
			goto success;
		case 2:
			hist[n++] = 2;
			if (!found) {
				if (q->b.b2 == 4) {
					t = q->b;
					found = 1;
				} else
					s = q->b;
			}
			if (fl.fb)
				goto post;
		default:
		nonstd:
			if (aflag > 2) {
				werror("// chkops: nonstandard aliasing expr:\n");
				prquad(q);
			}
			goto fail;
		case 0x19:
			hist[n++] = 0x19;
			if (!fl.fb)
				goto nonstd;
			goto post;
		case 1:
		case 0x30:
			hist[n++] = 1;
			if (!fl.fb)
				goto nonstd;
			goto post;
		case 3:
			if (q->a.b2 == 5)
				goto fail;
			hist[n++] = 3;
			if (!fl.fc)
				goto nonstd;
			if (op70d8 == 0x2f && mode == 1)
				op70d8 = 3;
			if (n > 3 && hist[n - 2] == 0x19 && hist[n - 3] == 0x18)
				goto nonstd;
			if (s.b2 == 3)
				goto post;
			goto success;
		case 7:
			hist[n++] = 7;
			if (!fl.fd)
				goto nonstd;
			op70d8 = 7;
		success:
			if (aflag > 2)
				prquad(q);
			opnd70c8 = *(found ? &t : &s);
			return 1;
		}
		if (aflag > 2) {
			printf("// at the bottom of checkops, p = ");
			prquad(q);
		}
	} while (x == 0);
}
