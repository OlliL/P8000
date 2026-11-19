/*
 * ebbo2.c - gopt: the optimization of the extended basic blocks (segment 5). Written from /lib/gopt (no source known); names guessed, the
 * callees by the address (fnXXXX).
 */
#include "gopt.h"

static char sccsid[] = "@[$]ebbo2.c\t1.8  03/04/83 15:54:14 Copr. 1981 87wega3.2.";

extern int s4d80, s4d94, a0d4, a0d2, s71e4, s71e6, s71e8, s71ea;
extern struct quad *qhead;
extern char *a0cc;
struct hent { struct hent *next; char x4[8]; unsigned cst:1, rest:15; struct quad *q; };	/* an entry of htab (guessed) */
extern struct hent *htab[32];
extern char a050e[], a0010[];
extern int fn4_faa4();
struct bt { int x0; int x1; int x[2]; unsigned f15:1, f14:1, bopt:1, rest:13; };	/* an entry of betatbl: 10 bytes (guessed) */
extern struct bt *betatbl;
/* an entry of the value table (name guessed): 16 bytes */
struct vn { struct vn *next; struct qa a; int val; int al; unsigned cst:1, dead:1, rest:14; };
extern int almap();
extern void propnd(), normop(), denorm();
extern int fn4_fbe8(), cerror();
extern char *taballoc();
extern void tabfree(), prqall();
extern struct quad *vnumber();
extern void ebbfix(), fn5_1ffc(), fn5_206c();
extern void fn5_1cd0(), fn5_1854(), fn5_1f2c(), fn5_0f7c(), fn5_1bc0(), prquad();
extern int fn5_1684(), fn5_1742(), a0d0;
extern struct qa *fn5_1068();
struct ae { struct ae *next; char x4[6]; int val; int xc; struct quad *q; };	/* (guessed) */
extern struct ae *fn5_0d7a();
extern struct quad *delquad();

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
		fn5_1ffc();
	}
	fn5_206c();
	tabfree(0, t);
	a0d4 = s4d94;
	if (s4d80 > 1) {
		printf("// after extended basic block optimization\n");
		prqall();
	}
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
			if (flag == 1 || (flag == 0 && !fn4_fbe8(e->a.b2))) {
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

/* the quads of one extended basic block, from q up to the next label, value numbered (name guessed); the label quad is returned */
struct quad *
vnumber(q)
struct quad *q;
{
	int fl;
	struct ae *vx;
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
			tmp = *fn5_1068(q);
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
L39c:			fn5_1cd0(q);
			if (q->c.b2 != 8 && q->c.b2 != 6)
				continue;
L3c6:			fn5_1854(q, 1);
			fn5_1f2c(q);
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
L44c:			if (fn5_1684(q))
				goto L3e2;
			vc = vnfind(&q->c);
			vx = fn5_0d7a(q, &fl);
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
			fn5_1cd0(q);
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
			vx = fn5_0d7a(q, &fl);
			vc = vnfind(&q->c);
			if (fl != 0) {
				if (fn5_1742(vx->q, q)) {
					vc->val = vx->val;
					vc->dead = 1;
					if (q->c.b2 == 3)
						goto L35a;
				} else
					fn5_0f7c(q);
			}
			fn5_1cd0(q);
			if (q->c.b2 == 8)
				goto L3c6;
			continue;
		case 51:
			stk[n++] = q;
		case 52:
			va = vnfind(&q->a);
			if (va->dead)
				updateopnd(va->val, &q->a, 1);
			fn5_1cd0(q);
			continue;
		case 7:
			va = vnfind(&q->a);
			if (va->dead)
				updateopnd(va->val, &q->a, 1);
			fn5_1cd0(q);
			fn5_1bc0(q, stk, n);
			n = 0;
			q->f7 = 1;
			a0d2 = 1;
			continue;
		case 27:
			va = vnfind(&q->a);
			if (va->dead)
				updateopnd(va->val, &q->a, 1);
			fn5_1cd0(q);
			fn5_1ffc();
			continue;
		case 42:
			va = vnfind(&q->a);
			if (va != 0 && va->dead)
				updateopnd(va->val, &q->a, 1);
			fn5_1cd0(q);
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
				fn5_1cd0(q);
				goto L3c6;
			}
			fn5_1cd0(q);
			continue;
		}
	}
	return q;
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
			fn4_faa4(s4d94);
			x = h->q->c.w4;
			y = s4d94;
			h->q->c.w4 = s4d94++;
			h->q->f4 = 1;
			normop(&h->q->c);
			for (p = first; p != last; p = p->next) {
				if (p->a.b2 == 3 && p->a.w4 == x) {
					p->a.b2 = 6;
					p->a.w4 = y;
					--a050e[x];
					p->f6 = 1;
					++a0010[y];
				}
				if (p->b.b2 == 3 && p->b.w4 == x) {
					p->b.b2 = 6;
					p->b.w4 = y;
					--a050e[x];
					p->f5 = 1;
					++a0010[y];
				}
			}
		}
	}
}
