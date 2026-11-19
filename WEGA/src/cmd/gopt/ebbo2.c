/*
 * ebbo2.c - gopt: the optimization of the extended basic blocks (segment 5). Written from /lib/gopt (no source known); names guessed, the
 * callees by the address (fnXXXX).
 */
#include "gopt.h"

static char sccsid[] = "@[$]ebbo2.c\t1.8  03/04/83 15:54:14 Copr. 1981 87wega3.2.";

extern int s4d80, s4d94, a0d4, a0d2, s71e4, s71e6, s71e8, s71ea;
extern struct quad *qhead;
extern char *a0cc;
extern struct quad *htab[32];
struct bt { int x0; int x1; int x[2]; unsigned f15:1, f14:1, bopt:1, rest:13; };	/* an entry of betatbl: 10 bytes (guessed) */
extern struct bt *betatbl;
/* an entry of the value table (name guessed): 16 bytes */
struct vn { struct vn *next; struct qa a; int val; int al; unsigned cst:1, dead:1, rest:14; };
extern int almap();
extern void propnd(), normop(), denorm();
extern int fn4_fbe8(), cerror();
extern char *taballoc();
extern void tabfree(), prqall();
extern struct quad *fn5_0114();
extern void fn5_0b94(), fn5_1ffc(), fn5_206c();

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
		q = fn5_0114(q);
		fn5_0b94(r, q);
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
