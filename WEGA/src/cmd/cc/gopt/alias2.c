/*
 * alias2.c - gopt: the alias analysis (the affect matrix, the alias matrix, the bit matrices). Written from /lib/gopt (no source known);
 * names guessed, the callees of other modules by the address (fnXXXX).
 */
static char sccsid[] = "@[$]alias2.c\t1.5  02/07/83 15:50:16 Copr. 1981 87wega3.2.";
#include "mfile2.h"

extern struct { int tag; int x; } numtab[];
extern int *affect, *modval;
extern int matw, affdim;
struct aln { int x0; struct aln *next; struct aln *link; };	/* a node of the alias lists (guessed; link: 6, the list of the lists) */
extern struct aln *alfree, *allist, *alcur, *alast;
extern int ndist, nlist;
extern struct { int pos; int y[4]; } *betatbl;	/* a table of 10 byte entries (guessed) */
extern struct quad *qhead;
extern struct quad *pcur;
extern struct qa opnd70c8;
extern int op70d8;
extern struct quad *fndcall();

/* forward declarations (the functions are in the order of the original) */
void pralias();

/* the position of the operand a in the alias table, 0 when it is not there (the first function of the module: its callers in
   setposition use calr) */
int
itfind(a)
	struct qa *a;
{
	int i;

	for (i = 1; i < nal; i++)
		if (altab[i] == a->w4)
			return i;
	return 0;
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
	if (p->b2 == OATEMP)
		return;
	if (hascall && ((symtab[p->w4].sflags >> 2) & 1) && symtab[p->w4].sclass != 8)
		return;
	if (symtab[p->w4].sclass == 9)
		return;
	if (symtab[p->w4].stype == 8 && symtab[p->w4].sx12 != -1) {
		v = mapn(symtab[p->w4].sx12);
		t.b2 = OANAME;
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
				t.b2 = OANAME;
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
		if (p->b2 == OANAME && (symtab[p->w4].stype & 0x30) == 0x30 && (DECREF(symtab[p->w4].stype) & 0x30) == 0x10) {
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
		if (0) {
skh:
			pcur = skiphdrs(q);
			continue;
		}
		q = fndcall(q);
		if (q == 0)
			break;
		if (q->op == QBBLOCK)
			goto skh;
		if (q->op == QSTORE) {
			if (!checkops(q, 0))
				continue;
			cls = op70d8;
			s = opnd70c8;
			if (!checkops(q, 1))
				continue;
			setposition(cls, &s, B, A);
			if (q->a.b2 == OAICON || q->a.b2 == OALCON)
				continue;
			setposition(op70d8, &opnd70c8, B, A);
		} else if (q->op == QPARV) {
			if (q->a.b2 == OATEMP && fndparm(q))
				setposition(QPARV, &opnd70c8, B, A);
			else if (q->a.b2 == OANAME && (DECREF(q->a.w0) & 0x30) == 0x10)
				setposition(QIDR, &q->a, B, A);
		}
	}
	n = nadr;
	nadr = nal;
	taballoc((unsigned)nadr * 2, 6);
	for (i = 0; i < n; i++)
		altab[nal++] = B[i];
	n = nptr;
	nptr = nal;
	taballoc((unsigned)nptr * 2, 6);
	for (i = 0; i < n; i++)
		altab[nal++] = A[i];
	if (aflag)
		pralias();
}

/* the position in the alias table for the index p (name from the strings) */
int
ind(p)
	int p;
{
	int i, v, pos;

	v = altab[p];
	if (p < nptr) {
		if (p < nadr) {
			if ((symtab[altab[p]].stype & 0x30) == 0x10) {
				for (i = nptr; i < nal; i++)
					if (v == altab[i]) {
						pos = i;
						goto done;
					}
				cerror("ind: *p not found, p=%d\n", p);
			}
		neg:
			pos = -1;
		done:
			if (aflag > 2)
				printf("// ind(%d) = %d\n", p, pos);
			return pos;
		}
		pos = symtab[v].sx14;
		goto done;
	}
	if (p >= nal)
		goto neg;
	pos = p;
	goto done;
}

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
		if (q->op == QSTORE || q->op == QPARV)
			break;
	if (aflag > 2 && q) {
		printf("\n");
		if (q->op == QSTORE || q->op == QPARV)
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
	if (p->op == QCALL)
		s = p->b;
	else
		s = p->c;
	while (s.b2 != OATEMP || s.w4 != tmpno) {
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
		if (p->op == QCALL)
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
		if (q->op == QADDR) {
			if (q->a.b2 != OANAME)
				return 0;
			op70d8 = 3;
			opnd70c8 = s;
			if (aflag > 2) {
				printf("// checkand, actual param  & op found:\n");
				prquad(q);
			}
			return 1;
		}
		if (s.b2 != OATEMP)
			return 0;
	}
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
		if (s.b2 != OATEMP) {
			if (s.b2 == OANAME && ISPTR(q->c.w0))
				goto success;
			if (aflag > 2) {
				printf("// Store's result opnd is not a ptr:\n");
				prquad(q);
			}
			goto fail;
			goto m1;
		} else
			fl.fd = 0;
	} else {
m1:
		if (mode == 1) {
			s = q->a;
			if (s.b2 != OATEMP) {
				if ((s.b2 == OANAME || s.b2 == OAICON || s.b2 == OALCON) && ISPTR(q->a.w0))
					goto success;
				goto fail;
			} else
				fl.fd = 1;
		} else
			cerror("illegal checkops call");
	}
	x = 0;
	do {
		q = tmpsearch(pcur, s.w4, q);
		if (n >= 30)
			cerror("checkops: history exceeded");
		s = q->a;
		switch (q->op) {
		case QIDR:
		case QLOAD:
			fl.fb = 1;
			op70d8 = 0x18;
			hist[n++] = 0x18;
		post:
			if (s.b2 == OATEMP)
				break;
			if (s.b2 != OANAME || !ISPTR(q->a.w0))
				goto fail;
			goto success;
		if (0) {
lft:
			opnd70c8 = *&t;
			goto r1;
		}
		case QADDFLD:
			hist[n++] = 2;
			if (!found) {
				if (q->b.b2 == OANAME) {
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
		fail:
			if (aflag > 2)
				printf("//    NULL\n");
			return 0;
		case QINDEX:
			hist[n++] = 0x19;
			if (!fl.fb)
				goto nonstd;
			goto post;
		case QADD:
		case QSUB:
			hist[n++] = 1;
			if (!fl.fb)
				goto nonstd;
			goto post;
		case QADDR:
			if (q->a.b2 == OALABEL)
				goto fail;
			hist[n++] = 3;
			if (!fl.fc)
				goto nonstd;
			if (op70d8 == 0x2f && mode == 1)
				op70d8 = 3;
			if (n > 3 && hist[n - 2] == 0x19 && hist[n - 3] == 0x18)
				goto nonstd;
			if (s.b2 == OATEMP)
				goto post;
			goto success;
		case QCALL:
			hist[n++] = 7;
			if (!fl.fd)
				goto nonstd;
			op70d8 = 7;
		success:
			if (aflag > 2)
				prquad(q);
			if (found)
				goto lft;
			opnd70c8 = *&s;
r1:
			return 1;
		}
		if (aflag > 2) {
			printf("at the bottom of checkops, p = ");
			prquad(q);
		}
	} while (x == 0);
}

/* the position of the operand p in the alias table (0: not tracked, -1: not found); op: the class of the operand (3 address, 0x18 pointer) */
int
almap(p, op)
	struct qa *p;
	int op;
{
	int i;
	int pos;

	if (aflag > 2)
		printf("// almap( optr->opnd=%s,op=%d)\n", ascii + symtab[p->w4].nameoff, op);
	if (ISPTR(p->w0) && (p->b2 == OAICON || p->b2 == OALCON)) {
	zero:
		pos = 0;
		goto done;
	neg:
		pos = -1;
		goto done;
	}
	if (p->b2 != OANAME)
		goto neg;
	if (hascall && ((symtab[p->w4].sflags >> 2) & 1) && symtab[p->w4].sclass != 8)
		goto zero;
	if (symtab[p->w4].sclass == 9)
		goto zero;
	switch (op) {
	case QIDR:
	case QLOAD:
		for (i = nptr; i < nal; i++)
			if (p->w4 == altab[i]) {
				pos = i;
				goto done;
			}
		if (aflag > 2)
			pralias();
		if (!((symtab[p->w4].sflags >> 2) & 1))
			goto neg;
		pos = nptr;
		goto done;
	case QADDR:
		for (i = nadr; i < nptr; i++)
			if (p->w4 == altab[i]) {
				pos = i;
				goto done;
			}
		if (aflag > 2)
			pralias();
		if (!((symtab[p->w4].sflags >> 2) & 1))
			goto neg;
		pos = nadr;
		goto done;
	default:
		pos = symtab[p->w4].sx14;
		if (aflag > 1)
			printf("(almap, symbol=%s, pos=%d)\n", ascii + symtab[p->w4].nameoff, pos);
		goto done;
	}
done:
	if (aflag > 2) {
		if (aflag > 3)
			printf("// almap: found pos=%d\n", pos);
		if (pos == -1)
			printf("warning pos = -1\n");
	}
	return pos;
}

/* the effect of a quad on the affect matrix and the modify matrix: lop the class of the left operand lp (QSTORE 0x2f, QIDR 0x18, 0x34), rop of the right one rp */
void
addaffect(lop, lp, rop, rp)
	int lop;
	struct qa *lp;
	int rop;
	struct qa *rp;
{
	int a, b, c, d;

	if (aflag > 2)
		printf("// entering addaffect(), lp->opnd = %x, lop  = %d, rop = %d, affect=%X, modval=%X\n", lp->w4, lop, rop, affect, modval);
	switch (lop) {
	case QSTORE:
		a = almap(lp, QSTORE);
		c = almap(lp, QIDR);
		switch (rop) {
		case QSTORE:
			d = almap(rp, QIDR);
			setbit(affect, c, d, matw);
			d = almap(rp, QSTORE);
			setbit(modval, a, d, matw);
			if (aflag > 1)
				printf("//  p = q \n");
			break;
		case QIDR:
		case QLOAD:
			b = almap(rp, QIDR);
			setbit(modval, a, b, matw);
			setbit(affect, c, b, matw);
			if (aflag > 1)
				printf("//  p = *q \n");
			break;
		case QADDR:
			d = almap(rp, QADDR);
			setbit(modval, a, d, matw);
			d = almap(rp, QSTORE);
			setbit(affect, c, d, matw);
			if (ISPTR(rp->w0)) {
				d = almap(rp, QIDR);
				setbit(affect, c, d, matw);
				if (aflag > 1)
					printf("//  p = &q \n");
			}
			break;
		case QCALL:
			setbit(modval, a, 0, matw);
			setbit(affect, a, 0, matw);
			if (aflag > 1)
				printf("//  p = f() \n");
			break;
		default:
			cerror("addaffect, lop=QSTORE, rop=default");
		}
		break;
	case QIDR:
	case QLOAD:
		a = almap(lp, QIDR);
		switch (rop) {
		case QSTORE:
			d = almap(rp, QIDR);
			setbit(affect, a, d, matw);
			d = almap(rp, QSTORE);
			setbit(modval, a, d, matw);
			if (aflag > 1)
				printf("//  *p = q \n");
			break;
		case QIDR:
		case QLOAD:
			b = almap(rp, QIDR);
			setbit(affect, a, b, matw);
			setbit(modval, a, b, matw);
			if (aflag > 1)
				printf("//  *p = *q \n");
			break;
		case QADDR:
			d = almap(rp, QADDR);
			setbit(modval, a, d, matw);
			d = almap(rp, QSTORE);
			setbit(affect, a, d, matw);
			if (ISPTR(rp->w0)) {
				d = almap(rp, QIDR);
				setbit(affect, a, d, matw);
				if (aflag > 1)
					printf("//  *p = &q \n");
			}
			break;
		case QCALL:
			setbit(modval, a, 0, matw);
			setbit(affect, a, 0, matw);
			if (aflag > 1)
				printf("//  *p = f() \n");
			break;
		default:
			cerror("addaffect, lop=QIDR, rop=default");
		}
		break;
	case QPARV:
		if (rop == QIDR || rop == QLOAD)
			a = almap(lp, QIDR);
		else
			a = almap(lp, QSTORE);
		setbit(modval, a, 0, matw);
		setbit(affect, a, 0, matw);
		if (aflag > 1)
			printf("//  f(....,&a,..) \n");
		break;
	default:
		cerror("addaffect: illegal lop\n");
	}
}

/* the affect matrix and the modify matrix (affdim rows of matw words) are filled from the quads */
void
initaffect()
{
	struct quad *q;
	struct qa s;
	int cls;
	int i;

	affect = (int *)taballoc(affdim * 2 * matw, 6);
	modval = (int *)taballoc(affdim * 2 * matw, 6);
	if (aflag > 1)
		printf("// initaffect: affect=%X, modval=%X, affdim=%d\n", affect, modval, affdim);
	pcur = q = qhead;
	for (; q != 0; q = q->next) {
		if (0) {
skh:
			pcur = skiphdrs(q);
			continue;
		}
		q = fndcall(q);
		if (q == 0)
			break;
		if (q->op == QBBLOCK)
			goto skh;
		if (q->op == QSTORE) {
			if (!checkops(q, 0))
				continue;
			cls = op70d8;
			s = opnd70c8;
			if (!checkops(q, 1))
				continue;
			addaffect(cls, &s, op70d8, &opnd70c8);
		} else if (q->op == QPARV) {
			if (q->a.b2 == OATEMP && fndparm(q))
				addaffect(0x34, &opnd70c8, 0, (struct qa *)0);
			else if (q->a.b2 == OANAME && (DECREF(q->a.w0) & 0x30) == 0x10)
				addaffect(0x34, &q->a, 0x18, (struct qa *)0);
		}
	}
}

/* a node of the free list alfree, or a new one */
struct aln *
alalloc()
{
	struct aln *p;

	if (alfree == 0)
		p = (struct aln *)taballoc(6, 6);
	else {
		p = alfree;
		alfree = alfree->next;
		if (aflag > 1)
			printf("// alalloc(): node  allocated from alfree\n");
	}
	p->next = 0;
	return p;
}

/* the list p is put on the free list (name guessed) */
void
alret(p)
	struct aln *p;
{
	struct aln *q, *last;

	if (alfree == 0) {
		alfree = p;
		if (aflag > 1)
			printf("// nodes put on alfree for first time\n");
	} else {
		if (aflag > 1)
			printf("// more nodes put on alfree\n");
		for (last = q = alfree; q != 0; q = q->next)
			last = q;
		last->next = p;
	}
}

/* the new alias list alcur is compared with the lists before it: a repeated one is given up (nodes to alfree) */
void
chkallist()
{
	struct aln *p, *a, *b;
	int n;

	n = 0;
	for (p = allist; p != alcur; p = p->link) {
		a = p->next;
		for (b = alcur->next; a != 0; a = a->next, b = b->next) {
			if (b == 0 || a->x0 != b->x0)
				break;
			n++;
		}
		if (a == 0 || b == 0)
			goto repeated;
	}
	if (aflag > 2)
		printf("// chkallist: distinct alias list found\n");
	ndist++;
	return;
repeated:
	if (aflag > 1)
		printf("// chkallist: alias list repeated\n");
	nlist -= n;
	alret(alcur->next);
	alcur->next = p->next;
}

/* a new alias list for the variable v: the list before it is checked (chkallist) */
void
initallist(v)
	int v;
{
	struct aln *t;

	if (aflag > 1)
		printf("// initallist(%d)\n", v);
	if (alcur != 0) {
		if (alcur->next != alast) {
			if (aflag > 1)
				printf("// initallist: previous entry nondiagonal\n");
			chkallist();
			t = alcur;
			alcur = (struct aln *)taballoc(10, 6);
			t->link = alcur;
			alast = 0;
		}
	} else {
		if (aflag > 1)
			printf("// initallist: firstalnode initialized\n");
		alcur = (struct aln *)taballoc(10, 6);
		allist = alcur;
	}
	alcur->x0 = v;
	alcur->next = 0;
	alcur->link = 0;
}

/* the variable v is appended to the current alias list */
void
addallist(v)
	int v;
{
	struct aln *t;

	if (aflag > 1)
		printf("// addallist(%d)\n", v);
	if (alast != 0) {
		if (alcur->next != 0) {
			if (aflag > 1)
				printf("// addallist: next entry on list\n");
			t = alast;
			alast = alalloc();
			t->next = alast;
		} else
			alcur->next = alast;
	} else {
		if (aflag > 1)
			printf("// addallist: first  entry of the list\n");
		alast = alalloc();
		alcur->next = alast;
	}
	alast->x0 = v;
	alast->next = 0;
	nlist++;
}

/* the lists of the aliased variables (debug; name guessed) */
void
pralvars()
{
	struct aln *p, *q;

	p = allist;
	printf("aliased variables:\n");
	for (; p != 0; p = p->link) {
		putchar('\n');
		q = p->next;
		printf("\n%s\t", ascii + symtab[altab[p->x0]].nameoff);
		for (; q != 0; q = q->next)
			printf(" %s", ascii + symtab[altab[q->x0]].nameoff);
	}
	putchar('\n');
}

/* the alias lists (one per variable: the variables its row of the affect matrix names) are built from the matrix; name guessed */
void
mkallist()
{
	int i, j;

	nlist = 0;
	ndist = 0;
	alcur = 0;
	alast = 0;
	alfree = 0;
	for (i = 0; i < affdim; i++) {
		initallist(i);
		for (j = 0; j < affdim; j++)
			if (testbit(affect, i, j, matw))
				addallist(j);
	}
	if (alcur->next != alast)
		chkallist();
	if (aflag)
		pralvars();
}

/* the alias list of the operand ot (2: a, 1: c, else b) of the quad q; 0 when it has none */
struct aln *
checkalias(q, ot)
	struct quad *q;
	int ot;
{
	int pos;
	struct aln *n;

	if (aflag > 4)
		printf("// checkalias opndtype = %d\n", ot);
	if (ot == 2) {
		if (q->a.b2 == OABTEMP)
			pos = betatbl[q->a.w4].pos;
		else if (q->op == QADDR)
			pos = almap(&q->a, QADDR);
		else if (q->op == QIDR || q->op == QLOAD)
			pos = almap(&q->a, QIDR);
		else
			pos = almap(&q->a, QSTORE);
	} else if (ot == 1) {
		if (q->c.b2 == OABTEMP)
			pos = betatbl[q->c.w4].pos;
		else
			pos = almap(&q->c, QSTORE);
	} else {
		if (q->b.b2 == OABTEMP)
			pos = betatbl[q->b.w4].pos;
		else
			pos = almap(&q->b, QSTORE);
	}
	if (pos == -1)
		return 0;
	for (n = allist; n != 0; n = n->link)
		if (pos == n->x0)
			break;
	if (n == 0)
		return 0;
	if (aflag > 4)
		printf("checkalias, returning %s\n", ascii + symtab[altab[n->x0]].nameoff);
	return n->next;
}

/* the size of the affect matrix (affdim bits per row, matw words) from the number of the aliased variables; the table area is made available (name guessed) */
int
affsize()
{
	affdim = nal;
	matw = affdim / 16 + 1;
	if (affdim > 225)
		cerror("affect matrix overflow");
	return tabavail(affdim * 4 * matw + affdim * 3, 6);
}

/* the statistics of the alias analysis (debug; name guessed) */
void
praldim()
{
	printf("//\n//Alias matrix size (bits) = %d\n", affdim);
	printf("//Alias matrix size (words) = %d\n", matw);
	printf("//Number of variables aliased = %d\n", nlist);
	printf("//Number of different aliases = %d\n", ndist);
}

/* the alias analysis: the affect matrix, its closure, the alias lists; 0 when the matrix cannot be made (name guessed) */
int
aliasmat()
{
	int tmp[1800];

	if (!affsize())
		return 0;
	initaffect();
	diag(affect, affdim, matw);
	if (aflag > 1) {
		printf("\n// initial values of affect matrix\n");
		prmat(affect, affdim, matw);
	}
	closure(affect, affdim, matw);
	if (aflag > 2) {
		printf("\n// affect after closure:\n");
		prmat(affect, affdim, matw);
	}
	clrmat(tmp, affdim, matw);
	matmul(tmp, affect, affect, affdim, matw);
	if (aflag > 2) {
		printf("\n// affect after multiplication:\n");
		prmat(tmp, affdim, matw);
	}
	copymat(affect, tmp, affdim, matw);
	if (aflag > 1) {
		printf("\n// alias matrix:\n");
		prmat(affect, affdim, matw);
	}
	mkallist();
	if (sdebug)
		praldim();
	return 1;
}
