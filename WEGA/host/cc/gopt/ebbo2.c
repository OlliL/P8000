/*
 * ebbo2.c - gopt: the optimization of the extended basic blocks (segment 5). Written from /lib/gopt (no source known); names guessed, the
 * callees by the address (fnXXXX).
 */
static char sccsid[] __attribute__((unused)) = "@[$]ebbo2.c\t1.8  03/04/83 15:54:14 Copr. 1981 87wega3.2.";
#include "mfile2.h"

extern int ebbcalls, nkill, nstores, vnnext, nuse;
extern char *vnlist;
extern struct hent *htab[32];
extern struct aln *usetbl[];
extern struct a71e stores[];
extern struct aln *killtbl[];
extern struct bt *betatbl;
/* an entry of the value table (name guessed): 16 bytes */
extern unsigned curblk;

/* forward declarations (the functions are in the order of the original) */
void ebbinit();
struct vn * vnfind();
void updateopnd();

/* every extended basic block of the quads is optimized (name guessed) */
void ebbopt(void)
{
	struct quad *q, *r;
	char *t;

	if (bdebug > 1) {
		printf("// before extended basic block optimization\n");
		prqall();
	}
	maxtmp = nbeta;
	t = tabzalloc(0, 0, 0);
	for (q = qhead; q != 0; ) {
		ebbinit();
		r = q;
		q = vnumber(q);
		ebbfix(r, q);
		if (vnlist != 0)
			tabfree(0, vnlist);
		resetbb();
	}
	ebbclean();
	tabfree(0, t);
	nbeta = maxtmp;
	if (bdebug > 1) {
		printf("// after extended basic block optimization\n");
		prqall();
	}
}

/* the tables of the extended basic block are cleared (name guessed) */
void ebbinit(void)
{
	int i;

	vnlist = 0;
	vnnext = 1;
	for (i = 0; i < 32; i++)
		htab[i] = 0;
	nstores = 0;
	nkill = 0;
	nuse = 0;
	ebbcalls = 0;
}

/* the quads of one extended basic block, from q up to the next label, value numbered (name guessed); the label quad is returned */
struct quad *vnumber(struct quad *q)
{
	int fl;
	struct hent *vx;
	struct vn *va, *vb, *vc, *vd;
	struct quad *stk[20];
	struct qa tmp;
	int n;

	n = 0;
	for (; (q = q->next) != 0; ) {
		if (q->op == QLABEL)
			return q;
		q->f7 = 0;
		q->f6 = 0;
		q->f5 = 0;
		q->f4 = 0;
		if (q->op == QBBLOCK)
			curblk = q->a.w0;
		if (bdebug > 3)
			printf("// opcode = %d\n", q->op);
		switch (q->op) {
		case QADD: case QADDFLD: case QADDR: case QAND: case QCONV: case QDIV: case QEQ: case QGE: case QGT:
		case QINDEX: case QLE: case QLT: case QMULT: case QNE: case QNOT: case QOR: case QREM:
		case QSHIFTL: case QSHIFTR: case QSUB: case QXOR:
			va = vnfind(&q->a);
			vb = q->b.b2 ? vnfind(&q->b) : 0;
			if (va != 0 && va->dead && q->op != QADDR) {
				updateopnd(va->val, &q->a, 1);
				va = vnfind(&q->a);
			}
			if (vb != 0 && vb->dead) {
				updateopnd(vb->val, &q->b, 1);
				vb = vnfind(&q->b);
			}
			if (va->cst && (vb == 0 || vb->cst))
				switch (q->op) {
				case QADDFLD: case QADDR: case QINDEX:
					break;
				case QCONV:
					if (q->c.w0 == 7 || q->c.w0 == 6)
						break;
				/* fall through */
				default:
					tmp = constfold(q);
					vd = vnfind(&tmp);
					vc = vnfind(&q->c);
					vc->val = vd->val;
					vc->cst = 1;
					vc->dead = 1;
					if (q->c.b2 == 3) {
						q = delquad(q);
						continue;
					}
					q->op = QCOPY;
					q->a = tmp;
				ib:
					q->b.b2 = 0;
				cu:
					checkuse(q);
					if (q->c.b2 == 8 || q->c.b2 == 6) {
						clear(q, 1);
						addstore(q);
					}
					continue;
				}
			if (0) {
			idt:
				vc = vnfind(&q->c);
				vc->val = va->val;
				vc->cst = va->cst;
				vc->dead = 1;
				if (q->c.b2 == 3) {
					q = delquad(q);
					continue;
				}
				q->op = QCOPY;
				goto ib;
			}
			if (identop(q))
				goto idt;
			vc = vnfind(&q->c);
			vx = exprfind(q, &fl);
			vc->val = vx->val;
			if (fl != 0) {
				vc->dead = 1;
				if (q->c.b2 == 3) {
					q = delquad(q);
					continue;
				}
			}
			goto cu;
		case QMOVE: case QSTORE:
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
			if (q->c.b2 == 4 || q->c.b2 == 6) {
				if (bdebug > 3) {
					printf("// ebbo :");
					prquad(q);
					printf("//   value = %d\n", va->val);
				}
				vc->val = va->val;
			}
			clear(q, 1);
			addstore(q);
			continue;
		case QIDR: case QLOAD:
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
					if (q->c.b2 == 3) {
						q = delquad(q);
						continue;
					}
				} else
					exprenter(q);
			}
			checkuse(q);
			if (q->c.b2 == 8) {
				clear(q, 1);
				addstore(q);
			}
			continue;
		case QPARR:
			stk[n++] = q;
		/* fall through */
		case QPARV:
			va = vnfind(&q->a);
			if (va->dead)
				updateopnd(va->val, &q->a, 1);
			checkuse(q);
			continue;
		case QCALL:
			va = vnfind(&q->a);
			if (va->dead)
				updateopnd(va->val, &q->a, 1);
			checkuse(q);
			ebbcall(q, stk, n);
			n = 0;
			q->f7 = 1;
			ebbcalls = 1;
			continue;
		case QJPC:
			va = vnfind(&q->a);
			if (va->dead)
				updateopnd(va->val, &q->a, 1);
			checkuse(q);
			resetbb();
			continue;
		case QRET:
			va = vnfind(&q->a);
			if (va != 0 && va->dead)
				updateopnd(va->val, &q->a, 1);
			checkuse(q);
			continue;
		case QCOPY:
			va = vnfind(&q->a);
			if (va->dead) {
				updateopnd(va->val, &q->a, 1);
				va = vnfind(&q->a);
			}
			if (q->c.b2 == 3 && !q->f0) {
				vc = vnfind(&q->c);
				vc->val = va->val;
				vc->dead = 1;
				q = delquad(q);
				continue;
			}
			if (0) {
			c60:
				checkuse(q);
				clear(q, 1);
				addstore(q);
				continue;
			}
			if (q->c.b2 != 3)
				goto c60;
			checkuse(q);
			continue;
		}
	}
	return q;
}

/* the entry of the value table for the operand p; made when there is none (name guessed) */
struct vn *vnfind(struct qa *p)
{
	register struct vn *e, *prev = 0;	/* host: = 0 (the original: what the frame holds) */

	if (bdebug > 2) {
		printf("// find: ");
		propnd(p);
		printf("\n");
	}
	for (e = (struct vn *)vnlist; e != 0; prev = e, e = e->next) {
		if (e->a.b2 == p->b2 && e->a.w4 == p->w4) {
			if (e->val != 0)
				goto ret;
			e->val = vnnext++;
			e->dead = 0;
			e->cst = 0;
			goto ret;
		}
	}
	e = (struct vn *)tabzalloc((int)sizeof(struct vn), 16, 0);
	if (vnlist == 0)
		vnlist = (char *)e;
	else
		prev->next = e;
	e->next = 0;
	e->a = *p;
	e->val = vnnext++;
	e->dead = 0;
	if (p->b2 == 8)
		e->al = betatbl[p->w4].x0;
	else if (p->b2 == 3)
		e->al = -p->w4;
	else if (p->b2 == 4)
		e->al = almap(p, QSTORE);
	else
		e->al = -9999;
	if (p->b2 == 7 || p->b2 == 1 || p->b2 == 2)
		e->cst = 1;
	else
		e->cst = 0;
ret:	return e;
}

/* the operand p replaced by the one of the value table entry with the value number vn (name from the error message) */
void updateopnd(int vn, struct qa *p, int flag)
{
	struct vn *e;

	e = (struct vn *)vnlist;
	while (e != 0) {
		if (e->val == vn && !e->dead) {
			if (flag == 1 || (flag == 0 && !isvarty(e->a.b2))) {
				if (bdebug) {
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
	if (bdebug != 0)
		for (e = (struct vn *)vnlist; e != 0; e = e->next) {
			propnd(&e->a);
			printf(" value = %d,alnumber = %d, dead = %s\n", e->val, e->al, e->dead ? "TRUE" : "FALSE");
		}
	cerror("error in updateopnd vn - %d", vn);
}

/* the operands of the quads from first up to last (not included) replaced for the entries of htab that were folded (name guessed) */
void ebbfix(struct quad *first, struct quad *last)
{
	int x, y, i;
	struct quad *p;
	struct hent *h;

	for (i = 0; i < 32; i++) {
		for (h = htab[i]; h != 0; h = h->next) {
			if (!h->cst)
				continue;
			if (bdebug > 1) {
				printf("// fixup :");
				prquad(h->q);
			}
			denorm(&h->q->c);
			h->q->c.b2 = 6;
			betanew(maxtmp);
			x = h->q->c.w4;
			y = maxtmp;
			h->q->c.w4 = maxtmp++;
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
struct hent *exprfind(struct quad *q, int *fl)
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
				if (e->q->c.b2 == 3 && e->num != curblk)
					e->cst = 1;
				return e;
			}
		}
	}
	*fl = 0;
	e = (struct hent *)tabzalloc((int)sizeof(struct hent), 18, 0);
	e->next = htab[h];
	htab[h] = e;
	e->op = q->op;
	e->v1 = va;
	e->v2 = vb;
	e->val = vnnext++;
	e->num = curblk;
	e->cst = 0;
	e->q = q;
	return e;
}

/* the expression of q entered in htab without a search (name guessed) */
void exprenter(struct quad *q)
{
	int va, vb, h;
	struct hent *e;

	va = vnfind(&q->a)->val;
	vb = vnfind(&q->b)->val;
	h = q->op + va + vb;
	h = h % 32;
	e = (struct hent *)tabzalloc((int)sizeof(struct hent), 18, 0);
	e->next = htab[h];
	htab[h] = e;
	e->op = q->op;
	e->v1 = va;
	e->v2 = vb;
	e->val = vnnext++;
	e->cst = 0;
	e->q = q;
}

/* the value of the operation of q computed at compile time: the operands are constants (name guessed from the error messages); the result is an operand c of the quad */
struct qa constfold(struct quad *q)
{
	struct qa r;
	long res, op1 = 0, op2 = 0;	/* host: = 0 (the original: what the frame holds) */
	double d1 = 0, d2 = 0, dr;	/* host: = 0 (the original: what the frame holds) */
	struct vn *e, *x;
	int found;

	if (bdebug > 3) {
		printf("// compute :");
		prquad(q);
	}
	x = vnfind(&q->a);
	if (!(x->a.b2 == 7 || x->a.b2 == 1 || x->a.b2 == 2)) {
		found = 0;
		for (e = (struct vn *)vnlist; e != 0; e = e->next) {
			if (e->val == x->val && (e->a.b2 == 7 || e->a.b2 == 1 || e->a.b2 == 2) && e->cst) {
				found = 1;
				break;
			}
		}
		if (found)
			x = e;
		else {
			if (bdebug)
				for (e = (struct vn *)vnlist; e != 0; e = e->next) {
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
		d1 = fcget(x->a.w4);
	if (q->b.b2) {
		x = vnfind(&q->b);
		if (!(0 || x->a.b2 == 1 || x->a.b2 == 2)) {	/* the original: !b2 == 7 || ..., always false (KNOWLEDGE.md, quirks) */
			found = 0;
			for (e = (struct vn *)vnlist; e != 0; e = e->next) {
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
			d2 = fcget(x->a.w4);
	}
	if (x->a.b2 != 2) {
		if (bdebug > 1)
			printf("// compute: op1 = %ld, op2 = %ld\n", op1, op2);
		switch (q->op) {
		case QADD: res = L32((uint32_t)op1 + (uint32_t)op2); break;
		case QAND: res = op1 & op2; break;
		case QCONV: res = op1; break;
		case QDIV: res = L32(op1 / op2); break;
		case QEQ: res = op1 == op2; break;
		case QGE: res = op1 >= op2; break;
		case QGT: res = op1 > op2; break;
		case QLE: res = op1 <= op2; break;
		case QLT: res = op1 < op2; break;
		case QMULT: res = L32((uint32_t)op1 * (uint32_t)op2); break;
		case QNE: res = op1 != op2; break;
		case QNOT: res = !op1; break;
		case QOR: res = op1 | op2; break;
		case QREM: res = L32(op1 % op2); break;
		case QSHIFTL: res = L32((uint32_t)op1 << (op2 & 31)); break;
		case QSHIFTR: res = op2 >> op2; break;
		case QSUB: res = L32((uint32_t)op1 - (uint32_t)op2); break;
		default:
			cerror("error in compile time constant propagation (3) (%d)", q->op);
		}
		r.b2 = 7;
		r.w4 = constlook(res);
		r.w0 = q->c.w0;
		return r;
	} else {
		switch (q->op) {
		case QADD: dr = d1 + d2; break;
		case QCONV: dr = d1; break;
		case QDIV: dr = d1 / d2; break;
		case QEQ: dr = d1 == d2; break;
		case QGE: dr = d1 >= d2; break;
		case QGT: dr = d1 > d2; break;
		case QLE: dr = d1 <= d2; break;
		case QLT: dr = d1 < d2; break;
		case QMULT: dr = d1 * d2; break;
		case QNE: dr = d1 != d2; break;
		case QNOT: dr = !d1; break;
		case QSUB: dr = d1 - d2; break;
		default:
			cerror("error in compile time constant propagation (4) (%d)", q->op);
		}
		r.b2 = 2;
		r.w4 = dconstlook(dr);
		r.w0 = q->c.w0;
		return r;
	}
}

/* the operation of q does nothing: the second operand is the neutral element (name guessed) */
int identop(struct quad *q)
{
	int x;

	switch (q->op) {
	case QADD:
	case QSUB:
		x = 1;
		if (q->b.b2 == 7) {
L7:			x = constbl[q->b.w4];
			return 0;
		} else if (q->b.b2 == 1)
			x = q->b.w4;
		if (x == 0)
			return 1;
		break;
	case QDIV:
	case QMULT:
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
int nostore(struct quad *load1, struct quad *load2)
{
	struct quad *p;
	struct aln *al;

	if (bdebug > 4) {
		printf("// entering compare: load1 and load2 are -\n");
		prquad(load1);
		prquad(load2);
	}
	al = checkalias(load1, 2);
	for (p = load1; p != load2; p = p->next) {
		if (p->op == QSTORE || p->op == QMOVE) {
			if (bdebug > 4) {
				printf("// comapare : check quad\n");
				prquad(p);
				printf("//    al = %X\n", (unsigned)(uintptr_t)(al));
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
void clear(struct quad *q, int ot)
{
	struct aln *x, *y;
	struct vn *e;
	int  v1, v2;

	if (q->op == QSTORE || q->op == QMOVE) {
		y = x = (struct aln *)checkalias(q, ot);
		if (q->c.b2 == 8)
			v1 = betatbl[q->c.w4].x1;
		else
			v1 = q->c.w4;
		for (; x != 0; x = x->next) {
			for (e = (struct vn *)vnlist; e != 0; e = e->next) {
				if (e->al >= 0 && x->x0 == e->al) {
					if (bdebug > 4) {
						printf("// clear: ");
						propnd(&e->a);
						printf("\n");
					}
					e->val = 0;
				}
			}
		}
		if (y == 0) {
			for (e = (struct vn *)vnlist; e != 0; e = e->next) {
				if (e->a.b2 == 8)
					v2 = betatbl[e->a.w4].x1;
				else if (e->a.b2 == 4)
					v2 = e->a.w4;
				else
					v2 = -1;
				if (e->al < 0 && v1 == v2) {
					if (bdebug > 4) {
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
			for (e = (struct vn *)vnlist; e != 0; e = e->next) {
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
	if (bdebug > 4)
		printf("// clear : p1 = %X\n", (unsigned)(uintptr_t)(y));
	if (!killed(y) && q->c.b2 != 3) {
		killtbl[nkill++] = y;
		if (nkill >= 1000)
			cerror("kill_tbl overflow");
		q->f7 = 1;
	}
}

/* y is in the kill table (name guessed) */
int killed(struct aln *y)
{
	int i;

	for (i = 0; i < nkill; i++)
		if (killtbl[i] == y)
			return 1;
	return 0;
}

/* the operand p may be optimized (name guessed) */
int canopt(struct qa *p)
{
	if (p->b2 == 4 && ((symtab[p->w4].sflags >> 2) & 1) && ebbcalls)
		return 0;
	return 1;
}

/* at a call: the table entries of the variables that the callee may change are invalid (name guessed) */
void ebbcall(struct quad *q, struct quad **stk, int n)
{
	struct vn *e;
	int i;

	e = (struct vn *)vnlist;
	while (e != 0) {
		if (e->al == 0 || e->al == nptr || e->al == nadr)
			e->val = 0;
		e = e->next;
	}
	for (i = 0; i < nstores; i++) {
		if (stores[i].q->c.b2 == 4 && ((symtab[stores[i].q->c.w4].sflags >> 2) & 1)) {
			stores[i].q = q;
			stores[i].s = symtab;
		}
	}
	for (i = 0; i < n; i++)
		clear(stk[i], 2);
}

/* the operands a and b of q that are variables get into the use table (name guessed from the debug output) */
void checkuse(struct quad *q)
{
	struct aln *p = 0;	/* host: = 0 (the original: what the frame holds when a has another kind) */

	if (bdebug > 3) {
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
			usetbl[nuse++] = p;
			if (nuse >= 1000)
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
			usetbl[nuse++] = p;
			if (nuse >= 1000)
				cerror("use_tbl overflow");
		}
	}
}

/* p is in the use table (name guessed) */
int inuse(struct aln *p)
{
	int i;

	for (i = 0; i < nuse; i++)
		if (usetbl[i] == p)
			return 1;
	return 0;
}

/* the store of q (or the entry of its operand c) is put into the array of the stores of the block (name from the error message) */
void addstore(struct quad *q)
{
	struct sym *x;

	if (q->op == QSTORE || q->op == QMOVE) {
		if (q->c.b2 == 3)
			return;
		x = (struct sym *)checkalias(q, 1);
		if (x == 0)
			x = &symtab[q->c.w4];
	} else
		x = (struct sym *)&betatbl[q->c.w4];
	stores[nstores].q = q;
	stores[nstores].s = x;
	nstores++;
	if (nstores >= 500)
		cerror("stores array overflow");
}

/* the tables of the block are reset at its end (name from the debug output) */
void resetbb(void)
{
	int i;

	nkill = 0;
	nuse = 0;
	for (i = 0; i < nstores; i++) {
		if (bdebug > 3) {
			printf("// resetbasicblock : setting gen_r for ");
			prquad(stores[i].q);
		}
		stores[i].q->f4 = 1;
	}
	nstores = 0;
	ebbcalls = 0;
}

/* the quads with a constant c of which nothing is used are deleted until nothing is deleted (name guessed) */
void ebbclean(void)
{
	struct quad *q;
	int x;

	x = 1;
	while (x) {
		x = 0;
		for (q = qhead; q != 0; q = q->next) {
			if (q->op == QBBLOCK)
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
