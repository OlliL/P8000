/*
 * ebbo2.c - gopt: the optimization of the extended basic blocks (segment 5). Written from /lib/gopt (no source known); names guessed, the
 * callees by the address (fnXXXX).
 */
static char sccsid[] = "@[$]ebbo2.c\t1.8  03/04/83 15:54:14 Copr. 1981 87wega3.2.";
#include "gopt.h"

extern int s4d80, s4d94, a0d4, a0d2, s71e4, s71e6, s71e8, s71ea;
extern struct quad *qhead;
extern char *a0cc;
struct aln { int x0; struct aln *next; struct aln *link; };
struct hent { struct hent *next; int op; int v1; int v2; int val; unsigned cst:1, num:15; struct quad *q; };	/* an entry of htab (guessed) */
extern struct hent *htab[32];
extern char ctpos[], ctneg[];
extern int betanew();
extern long constbl[];
extern struct aln *usetbl[];
extern int inuse(), canopt();
extern int nptr, nadr;
extern struct { struct quad *q; struct sym *s; } a71ec[];
extern struct aln *killtbl[];
extern struct sym *symtab;
extern int eqopnd(), killed();
struct bt { int x0; int x1; int x[2]; unsigned f15:1, f14:1, bopt:1, rest:13; };	/* an entry of betatbl: 10 bytes (guessed) */
extern struct bt *betatbl;
/* an entry of the value table (name guessed): 16 bytes */
struct vn { struct vn *next; struct qa a; int val; int al; unsigned cst:1, dead:1, rest:14; };
extern int almap();
extern void propnd(), normop(), denorm();
extern int isvarty(), cerror();
extern char *taballoc();
extern struct quad *checkalias();
extern void tabfree(), prqall();
extern struct quad *vnumber();
extern void ebbfix(), resetbb(), ebbclean();
extern void checkuse(), clear(), addstore(), exprenter(), ebbcall(), prquad();
extern int identop(), nostore();
extern unsigned a0d0;
extern struct qa constfold();
extern struct hent *exprfind();
extern struct quad *delquad();

extern struct { int rv; double d; } fconsttbl[];
extern int constlook(), dconstlook();
/* forward declarations (the functions are in the order of the original) */
void ebbinit();
struct vn * vnfind();
void updateopnd();

/* every extended basic block of the quads is optimized (name guessed) */
void
ebbopt()
{
	struct quad *q, *r;
	char *t;

	if (s4d80 > 1) {
		printf("// before extended basic block optimization\n");
		prqall();
	}
	s4d94 = a0d4;
	t = taballoc(0, 0);
	for (q = qhead; q != 0; ) {
		ebbinit();
		r = q;
		q = vnumber(q);
		ebbfix(r, q);
		if (a0cc != 0)
			tabfree(0, a0cc);
		resetbb();
	}
	ebbclean();
	tabfree(0, t);
	a0d4 = s4d94;
	if (s4d80 > 1) {
		printf("// after extended basic block optimization\n");
		prqall();
	}
}

/* the tables of the extended basic block are cleared (name guessed) */
void
ebbinit()
{
	int i;

	a0cc = 0;
	s71e8 = 1;
	for (i = 0; i < 32; i++)
		htab[i] = 0;
	s71e6 = 0;
	s71e4 = 0;
	s71ea = 0;
	a0d2 = 0;
}

/* the quads of one extended basic block, from q up to the next label, value numbered (name guessed); the label quad is returned */
struct quad *
vnumber(q)
struct quad *q;
{
	int fl;
	struct hent *vx;
	struct vn *va, *vb, *vc, *vd;
	struct quad *stk[20];
	struct qa tmp;
	int n;

	n = 0;
	for (; (q = q->next) != 0; ) {
		if (q->op == 28)
			return q;
		q->f7 = 0;
		q->f6 = 0;
		q->f5 = 0;
		q->f4 = 0;
		if (q->op == 59)
			a0d0 = q->a.w0;
		if (s4d80 > 3)
			printf("// opcode = %d\n", q->op);
		switch (q->op) {
		case 1: case 2: case 3: case 4: case 13: case 14: case 19: case 22: case 23:
		case 25: case 29: case 32: case 34: case 35: case 36: case 37: case 39:
		case 44: case 45: case 48: case 49:
			va = vnfind(&q->a);
			vb = q->b.b2 ? vnfind(&q->b) : 0;
			if (va != 0 && va->dead && q->op != 3) {
				updateopnd(va->val, &q->a, 1);
				va = vnfind(&q->a);
			}
			if (vb != 0 && vb->dead) {
				updateopnd(vb->val, &q->b, 1);
				vb = vnfind(&q->b);
			}
			if (!va->cst)
				goto L44c;
			if (vb != 0 && !vb->cst)
				goto L44c;
			switch (q->op) {
			case 2: case 3: case 25:
				goto L44c;
			case 13:
				if (q->c.w0 == 7 || q->c.w0 == 6)
					goto L44c;
			}
			tmp = constfold(q);
			vd = vnfind(&tmp);
			vc = vnfind(&q->c);
			vc->val = vd->val;
			vc->cst = 1;
			vc->dead = 1;
			if (q->c.b2 == 3)
				goto L35a;
			q->op = 60;
			q->a = tmp;
L392:			q->b.b2 = 0;
L39c:			checkuse(q);
			if (q->c.b2 != 8 && q->c.b2 != 6)
				continue;
L3c6:			clear(q, 1);
			addstore(q);
			continue;
L35a:			q = delquad(q);
			continue;
L3e2:			vc = vnfind(&q->c);
			vc->val = va->val;
			vc->cst = va->cst;
			vc->dead = 1;
			if (q->c.b2 == 3)
				goto L35a;
			q->op = 60;
			goto L392;
L44c:			if (identop(q))
				goto L3e2;
			vc = vnfind(&q->c);
			vx = exprfind(q, &fl);
			vc->val = vx->val;
			if (fl != 0) {
				vc->dead = 1;
				if (q->c.b2 == 3)
					goto L35a;
			}
			goto L39c;
		case 33: case 47:
			va = vnfind(&q->a);
			if (va->dead) {
				updateopnd(va->val, &q->a, 1);
				va = vnfind(&q->a);
			}
			vc = vnfind(&q->c);
			if (vc->dead && q->c.b2 != 4) {
				updateopnd(vc->val, &q->c, 0);
				vc = vnfind(&q->c);
			}
			checkuse(q);
			vc->cst = va->cst;
			if (q->c.b2 != 8)
				vc->dead = va->cst;
			if (q->c.b2 != 4 && q->c.b2 != 6)
				goto L3c6;
			if (s4d80 > 3) {
				printf("// ebbo :");
				prquad(q);
				printf("//   value = %d\n", va->val);
			}
			vc->val = va->val;
			goto L3c6;
		case 24: case 30:
			va = vnfind(&q->a);
			if (va->dead) {
				updateopnd(va->val, &q->a, 1);
				va = vnfind(&q->a);
			}
			vx = exprfind(q, &fl);
			vc = vnfind(&q->c);
			if (fl != 0) {
				if (nostore(vx->q, q)) {
					vc->val = vx->val;
					vc->dead = 1;
					if (q->c.b2 == 3)
						goto L35a;
				} else
					exprenter(q);
			}
			checkuse(q);
			if (q->c.b2 == 8)
				goto L3c6;
			continue;
		case 51:
			stk[n++] = q;
		case 52:
			va = vnfind(&q->a);
			if (va->dead)
				updateopnd(va->val, &q->a, 1);
			checkuse(q);
			continue;
		case 7:
			va = vnfind(&q->a);
			if (va->dead)
				updateopnd(va->val, &q->a, 1);
			checkuse(q);
			ebbcall(q, stk, n);
			n = 0;
			q->f7 = 1;
			a0d2 = 1;
			continue;
		case 27:
			va = vnfind(&q->a);
			if (va->dead)
				updateopnd(va->val, &q->a, 1);
			checkuse(q);
			resetbb();
			continue;
		case 42:
			va = vnfind(&q->a);
			if (va != 0 && va->dead)
				updateopnd(va->val, &q->a, 1);
			checkuse(q);
			continue;
		case 60:
			va = vnfind(&q->a);
			if (va->dead) {
				updateopnd(va->val, &q->a, 1);
				va = vnfind(&q->a);
			}
			if (q->c.b2 == 3 && !q->f0) {
				vc = vnfind(&q->c);
				vc->val = va->val;
				vc->dead = 1;
				goto L35a;
			}
			if (q->c.b2 != 3) {
				checkuse(q);
				goto L3c6;
			}
			checkuse(q);
			continue;
		}
	}
	return q;
}

/* the entry of the value table for the operand p; made when there is none (name guessed) */
struct vn *
vnfind(p)
struct qa *p;
{
	register struct vn *e, *prev;

	if (s4d80 > 2) {
		printf("// find: ");
		propnd(p);
		printf("\n");
	}
	for (e = (struct vn *)a0cc; e != 0; prev = e, e = e->next) {
		if (e->a.b2 == p->b2 && e->a.w4 == p->w4) {
			if (e->val != 0)
				goto ret;
			e->val = s71e8++;
			e->dead = 0;
			e->cst = 0;
			goto ret;
		}
	}
	e = (struct vn *)taballoc(16, 0);
	if (a0cc == 0)
		a0cc = (char *)e;
	else
		prev->next = e;
	e->next = 0;
	e->a = *p;
	e->val = s71e8++;
	e->dead = 0;
	if (p->b2 == 8)
		e->al = betatbl[p->w4].x0;
	else if (p->b2 == 3)
		e->al = -p->w4;
	else if (p->b2 == 4)
		e->al = almap(p, 47);
	else
		e->al = -9999;
	if (p->b2 == 7 || p->b2 == 1 || p->b2 == 2)
		e->cst = 1;
	else
		e->cst = 0;
ret:	return e;
}

/* the operand p replaced by the one of the value table entry with the value number vn (name from the error message) */
void
updateopnd(vn, p, flag)
int vn;
struct qa *p;
int flag;
{
	struct vn *e;

	e = (struct vn *)a0cc;
	while (e != 0) {
		if (e->val == vn && !e->dead) {
			if (flag == 1 || (flag == 0 && !isvarty(e->a.b2))) {
				if (s4d80) {
					printf("//update ");
					propnd(p);
					printf(" to ");
					propnd(&e->a);
					printf("(value = %d)\n", vn);
				}
				denorm(p);
				*p = e->a;
				normop(p);
			}
			return;
		}
		e = e->next;
	}
	if (s4d80 != 0)
		for (e = (struct vn *)a0cc; e != 0; e = e->next) {
			propnd(&e->a);
			printf(" value = %d,alnumber = %d, dead = %s\n", e->val, e->al, e->dead ? "TRUE" : "FALSE");
		}
	cerror("error in updateopnd vn - %d", vn);
}

/* the operands of the quads from first up to last (not included) replaced for the entries of htab that were folded (name guessed) */
void
ebbfix(first, last)
struct quad *first, *last;
{
	int x, y, i;
	struct quad *p;
	struct hent *h;

	for (i = 0; i < 32; i++) {
		for (h = htab[i]; h != 0; h = h->next) {
			if (!h->cst)
				continue;
			if (s4d80 > 1) {
				printf("// fixup :");
				prquad(h->q);
			}
			denorm(&h->q->c);
			h->q->c.b2 = 6;
			betanew(s4d94);
			x = h->q->c.w4;
			y = s4d94;
			h->q->c.w4 = s4d94++;
			h->q->f4 = 1;
			normop(&h->q->c);
			for (p = first; p != last; p = p->next) {
				if (p->a.b2 == 3 && p->a.w4 == x) {
					p->a.b2 = 6;
					p->a.w4 = y;
					--ctpos[x];
					p->f6 = 1;
					++ctneg[y];
				}
				if (p->b.b2 == 3 && p->b.w4 == x) {
					p->b.b2 = 6;
					p->b.w4 = y;
					--ctpos[x];
					p->f5 = 1;
					++ctneg[y];
				}
			}
		}
	}
}

/* the entry of htab for the expression of q (op and the value numbers of its operands); made when there is none; *fl tells whether it was found (name guessed) */
struct hent *
exprfind(q, fl)
struct quad *q;
int *fl;
{
	int va, vb, vc, h;
	struct hent *e;

	va = vnfind(&q->a)->val;
	if (q->b.b2)
		vb = vnfind(&q->b)->val;
	else
		vb = 0;
	h = q->op + va + vb;
	h = h % 32;
	*fl = 1;
	for (e = htab[h]; e != 0; e = e->next) {
		if (e->op == q->op && e->v1 == va && e->v2 == vb && e->q->c.w0 == q->c.w0) {
			vc = vnfind(&e->q->c)->val;
			if (vc == e->val) {
				if (e->q->c.b2 == 3 && e->num != a0d0)
					e->cst = 1;
				return e;
			}
		}
	}
	*fl = 0;
	e = (struct hent *)taballoc(18, 0);
	e->next = htab[h];
	htab[h] = e;
	e->op = q->op;
	e->v1 = va;
	e->v2 = vb;
	e->val = s71e8++;
	e->num = a0d0;
	e->cst = 0;
	e->q = q;
	return e;
}

/* the expression of q entered in htab without a search (name guessed) */
void
exprenter(q)
struct quad *q;
{
	int va, vb, h;
	struct hent *e;

	va = vnfind(&q->a)->val;
	vb = vnfind(&q->b)->val;
	h = q->op + va + vb;
	h = h % 32;
	e = (struct hent *)taballoc(18, 0);
	e->next = htab[h];
	htab[h] = e;
	e->op = q->op;
	e->v1 = va;
	e->v2 = vb;
	e->val = s71e8++;
	e->cst = 0;
	e->q = q;
}

/* the value of the operation of q computed at compile time: the operands are constants (name guessed from the error messages); the result is an operand c of the quad */
struct qa
constfold(q)
struct quad *q;
{
	struct qa r;
	long res, op1, op2;
	double d1, d2, dr;
	struct vn *e, *x;
	int found;

	if (s4d80 > 3) {
		printf("// compute :");
		prquad(q);
	}
	x = vnfind(&q->a);
	if (!(x->a.b2 == 7 || x->a.b2 == 1 || x->a.b2 == 2)) {
		found = 0;
		for (e = (struct vn *)a0cc; e != 0; e = e->next) {
			if (e->val == x->val && (e->a.b2 == 7 || e->a.b2 == 1 || e->a.b2 == 2) && e->cst) {
				found = 1;
				break;
			}
		}
		if (found)
			x = e;
		else {
			if (s4d80)
				for (e = (struct vn *)a0cc; e != 0; e = e->next) {
					propnd(&e->a);
					printf(" value = %d, dead = %s\n", e->val, e->dead ? "TRUE" : "FALSE");
				}
			cerror("error in compile time constant propagation (1)");
		}
	}
	if (x->a.b2 == 7)
		op1 = constbl[x->a.w4];
	else if (x->a.b2 == 1)
		op1 = x->a.w4;
	else if (x->a.b2 == 2)
		d1 = fconsttbl[x->a.w4].d;
	if (q->b.b2) {
		x = vnfind(&q->b);
		if (!(!x->a.b2 == 7 || x->a.b2 == 1 || x->a.b2 == 2)) {
			found = 0;
			for (e = (struct vn *)a0cc; e != 0; e = e->next) {
				if (e->val == x->val && (e->a.b2 == 7 || e->a.b2 == 1 || e->a.b2 == 2) && e->cst) {
					found = 1;
					break;
				}
			}
			if (found)
				x = e;
			else
				cerror("error in compile time constant propagation (2)");
		}
		if (x->a.b2 == 7)
			op2 = constbl[x->a.w4];
		else if (x->a.b2 == 1)
			op2 = x->a.w4;
		else if (x->a.b2 == 2)
			d2 = fconsttbl[x->a.w4].d;
	}
	if (x->a.b2 != 2) {
		if (s4d80 > 1)
			printf("// compute: op1 = %ld, op2 = %ld\n", op1, op2);
		switch (q->op) {
		case 1: res = op1 + op2; break;
		case 4: res = op1 & op2; break;
		case 13: res = op1; break;
		case 14: res = op1 / op2; break;
		case 19: res = op1 == op2; break;
		case 22: res = op1 >= op2; break;
		case 23: res = op1 > op2; break;
		case 29: res = op1 <= op2; break;
		case 32: res = op1 < op2; break;
		case 34: res = op1 * op2; break;
		case 35: res = op1 != op2; break;
		case 36: res = !op1; break;
		case 37: res = op1 | op2; break;
		case 39: res = op1 % op2; break;
		case 44: res = op1 << op2; break;
		case 45: res = op2 >> op2; break;
		case 48: res = op1 - op2; break;
		default:
			cerror("error in compile time constant propagation (3) (%d)", q->op);
		}
		r.b2 = 7;
		r.w4 = constlook(res);
	} else {
		switch (q->op) {
		case 1: dr = d1 + d2; break;
		case 13: dr = d1; break;
		case 14: dr = d1 / d2; break;
		case 19: dr = d1 == d2; break;
		case 22: dr = d1 >= d2; break;
		case 23: dr = d1 > d2; break;
		case 29: dr = d1 <= d2; break;
		case 32: dr = d1 < d2; break;
		case 34: dr = d1 * d2; break;
		case 35: dr = d1 != d2; break;
		case 36: dr = !d1; break;
		case 48: dr = d1 - d2; break;
		default:
			cerror("error in compile time constant propagation (4) (%d)", q->op);
		}
		r.b2 = 2;
		r.w4 = dconstlook(dr);
	}
	r.w0 = q->c.w0;
	return r;
}

/* the operation of q does nothing: the second operand is the neutral element (name guessed) */
int
identop(q)
struct quad *q;
{
	int x;

	switch (q->op) {
	case 1:
	case 48:
		x = 1;
		if (q->b.b2 == 7) {
L7:			x = constbl[q->b.w4];
			return 0;
		} else if (q->b.b2 == 1)
			x = q->b.w4;
		if (x == 0)
			return 1;
		break;
	case 14:
	case 34:
		x = 0;
		if (q->b.b2 == 7)
			goto L7;
		if (q->b.b2 == 1)
			x = q->b.w4;
		if (x == 1)
			return 1;
		break;
	}
	return 0;
}

/* nothing between the load1 and load2 stores to where load1 reads from (name guessed from the debug output) */
int
nostore(load1, load2)
struct quad *load1, *load2;
{
	struct quad *p;
	struct quad *al;

	if (s4d80 > 4) {
		printf("// entering compare: load1 and load2 are -\n");
		prquad(load1);
		prquad(load2);
	}
	al = checkalias(load1, 2);
	for (p = load1; p != load2; p = p->next) {
		if (p->op == 47 || p->op == 33) {
			if (s4d80 > 4) {
				printf("// comapare : check quad\n");
				prquad(p);
				printf("//    al = %X\n", al);
			}
			if (al == 0 && load1->a.b2 == p->c.b2 && load1->a.w4 == p->c.w4)
				return 0;
			if (al != 0 && checkalias(p, 1) == al)
				return 0;
		}
	}
	return 1;
}

/* the table entries that may be changed by the store of q (or the load of its operand c) are marked as not valid; the entry is put into the kill table (name from the debug output) */
void
clear(q, ot)
struct quad *q;
int ot;
{
	struct aln *x, *y;
	struct vn *e;
	int k, v1, v2;

	if (q->op == 47 || q->op == 33) {
		x = y = (struct aln *)checkalias(q, ot);
		if (q->c.b2 == 8)
			v1 = betatbl[q->c.w4].x1;
		else
			v1 = q->c.w4;
		for (; x != 0; x = x->next) {
			for (e = (struct vn *)a0cc; e != 0; e = e->next) {
				if (e->al >= 0 && x->x0 == e->al) {
					if (s4d80 > 4) {
						printf("// clear: ");
						propnd(&e->a);
						printf("\n");
					}
					e->val = 0;
				}
			}
		}
		if (y == 0) {
			for (e = (struct vn *)a0cc; e != 0; e = e->next) {
				if (e->a.b2 == 8)
					v2 = betatbl[e->a.w4].x1;
				else if (e->a.b2 == 4)
					v2 = e->a.w4;
				else
					v2 = -1;
				if (e->al < 0 && v1 == v2) {
					if (s4d80 > 4) {
						printf("// clear: ");
						propnd(&e->a);
						printf("\n");
					}
					e->val = 0;
				}
			}
			if (q->c.b2 == 4)
				y = (struct aln *)&symtab[q->c.w4];
			else
				y = (struct aln *)&betatbl[q->c.w4];
		} else {
			for (e = (struct vn *)a0cc; e != 0; e = e->next) {
				if (y->x0 == e->al && eqopnd(&e->a, &q->c) && e->al >= 0) {
					if (q->c.b2 == 4)
						y = (struct aln *)&symtab[q->c.w4];
					else
						y = (struct aln *)&betatbl[q->c.w4];
					break;
				}
			}
		}
	} else
		y = (struct aln *)&betatbl[q->c.w4];
	if (s4d80 > 4)
		printf("// clear : p1 = %X\n", y);
	if (!killed(y) && q->c.b2 != 3) {
		killtbl[s71e4++] = y;
		if (s71e4 >= 1000)
			cerror("kill_tbl overflow");
		q->f7 = 1;
	}
}

/* y is in the kill table (name guessed) */
int
killed(y)
struct aln *y;
{
	int i;

	for (i = 0; i < s71e4; i++)
		if (killtbl[i] == y)
			return 1;
	return 0;
}

/* the operand p may be optimized (name guessed) */
int
canopt(p)
struct qa *p;
{
	if (p->b2 == 4 && ((symtab[p->w4].sflags >> 2) & 1) && a0d2)
		return 0;
	return 1;
}

/* at a call: the table entries of the variables that the callee may change are invalid (name guessed) */
void
ebbcall(q, stk, n)
struct quad *q;
struct quad **stk;
int n;
{
	struct vn *e;
	int i;

	e = (struct vn *)a0cc;
	while (e != 0) {
		if (e->al == 0 || e->al == nptr || e->al == nadr)
			e->val = 0;
		e = e->next;
	}
	for (i = 0; i < s71e6; i++) {
		if (a71ec[i].q->c.b2 == 4 && ((symtab[a71ec[i].q->c.w4].sflags >> 2) & 1)) {
			a71ec[i].q = q;
			a71ec[i].s = symtab;
		}
	}
	for (i = 0; i < n; i++)
		clear(stk[i], 2);
}

/* the operands a and b of q that are variables get into the use table (name guessed from the debug output) */
void
checkuse(q)
struct quad *q;
{
	struct aln *p;

	if (s4d80 > 3) {
		printf("// in checkuse ---");
		prquad(q);
	}
	if (!isvarty(q->a.b2) && q->a.b2 != 3 && q->a.b2 != 5) {
		if (q->a.b2 == 8 || q->a.b2 == 6)
			p = (struct aln *)&betatbl[q->a.w4];
		else if (q->a.b2 == 4 && (p = (struct aln *)checkalias(q, 2)) == 0)
			p = (struct aln *)&symtab[q->a.w4];
		if (!killed(p) && !inuse(p) && canopt(&q->a)) {
			q->f6 = 1;
			usetbl[s71ea++] = p;
			if (s71ea >= 1000)
				cerror("use_tbl overflow");
		}
	}
	if (!isvarty(q->b.b2) && q->b.b2 != 3 && q->b.b2 != 0) {
		if (q->b.b2 == 8 || q->b.b2 == 6)
			p = (struct aln *)&betatbl[q->b.w4];
		else if (q->b.b2 == 4 && (p = (struct aln *)checkalias(q, 3)) == 0)
			p = (struct aln *)&symtab[q->b.w4];
		if (!killed(p) && !inuse(p) && canopt(&q->b)) {
			q->f5 = 1;
			usetbl[s71ea++] = p;
			if (s71ea >= 1000)
				cerror("use_tbl overflow");
		}
	}
}

/* p is in the use table (name guessed) */
int
inuse(p)
struct aln *p;
{
	int i;

	for (i = 0; i < s71ea; i++)
		if (usetbl[i] == p)
			return 1;
	return 0;
}

/* the store of q (or the entry of its operand c) is put into the array of the stores of the block (name from the error message) */
void
addstore(q)
struct quad *q;
{
	int k;
	struct sym *x;

	if (q->op == 47 || q->op == 33) {
		if (q->c.b2 == 3)
			return;
		x = (struct sym *)checkalias(q, 1);
		if (x == 0)
			x = &symtab[q->c.w4];
	} else
		x = (struct sym *)&betatbl[q->c.w4];
	a71ec[s71e6].q = q;
	a71ec[s71e6].s = x;
	s71e6++;
	if (s71e6 >= 500)
		cerror("stores array overflow");
}

/* the tables of the block are reset at its end (name from the debug output) */
void
resetbb()
{
	int i;

	s71e4 = 0;
	s71ea = 0;
	for (i = 0; i < s71e6; i++) {
		if (s4d80 > 3) {
			printf("// resetbasicblock : setting gen_r for ");
			prquad(a71ec[i].q);
		}
		a71ec[i].q->f4 = 1;
	}
	s71e6 = 0;
	a0d2 = 0;
}

/* the quads with a constant c of which nothing is used are deleted until nothing is deleted (name guessed) */
void
ebbclean()
{
	struct quad *q;
	int x;

	x = 1;
	while (x) {
		x = 0;
		for (q = qhead; q != 0; q = q->next) {
			if (q->op == 59)
				continue;
			switch (q->c.b2) {
			case 3:
				if (ctpos[q->c.w4] > 1)
					break;
L1:				q = delquad(q);
				x = 1;
				break;
			case 6:
				if (ctneg[q->c.w4] <= 1)
					goto L1;
				break;
			case 8:
				if (ctpos[betatbl[q->c.w4].x[0]] <= 1)
					goto L1;
				break;
			}
		}
	}
}
