/*
 * alias2.c - gopt: the alias analysis (the affect matrix, the alias matrix, the bit matrices). Written from /lib/gopt (no source known);
 * names guessed, the callees of other modules by the address (fnXXXX).
 */
#include "gopt.h"

static char sccsid[] = "@[$]alias2.c\t1.5  02/07/83 15:50:16 Copr. 1981 87wega3.2.";

extern int aflag;
extern int nadr, nptr, nal;
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
