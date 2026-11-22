/*
 * lpind2.c - gopt: the induction variables of the loops (segment 5). Written from /lib/gopt (no source known); names guessed,
 * the callees by the address (fnXXXX).
 */
static char sccsid[] __attribute__((unused)) = "@[$]lpind2.c\t1.5  02/07/83 15:54:28 Copr. 1981 87wega3.2.";
#include "mfile2.h"

/* 2 to the power n (name guessed) */
long lpow2(int n)
{
	long r;

	r = 1;
	while (n > 0) {
		r = r << 1;
		n--;
	}
	return r;
}

/* the value of the operation op of the constants a and b (name from the debug output) */
long ceval(struct qa a, int op, struct qa b)
{
	long x, y, l, m, r = 0;	/* host: r = 0 (the original returns what the frame holds for another op) */

	x = b.b2 == OAICON ? (long)b.w4 : constbl[b.w4];
	y = a.b2 == OAICON ? (long)a.w4 : constbl[a.w4];
	switch (op) {
	case QADD:
		r = x;
		break;
	case QSUB:
		r = L32(-(uint32_t)x);
		break;
	case QMULT:
		r = L32((uint32_t)x * (uint32_t)y);
		break;
	case QSHIFTL:
		r = L32((uint32_t)x * (uint32_t)lpow2((int)y));
		break;
	case QDIV:
	case QSHIFTR:
		if ((l = ispow2(x)) > 0 && (m = ispow2(y)) > 0 && l > m)
			r = l - m;
		r = 0;
		break;
	}
	if (pdebug > 3)
		printf("%ld = ceval()\n", r);
	return r;
}

/* the quad q is moved behind phquad (name guessed) */
struct quad *qmove(struct quad *q)
{

	if (pdebug > 5)
		prquad(q);
	q = movequad(q, phquad);
	phquad = phquad->next;
	nqmoved++;
	return q;
}

/* a move of the result of q to a new stemp is inserted and the quads are moved (name from the debug output) */
struct quad *insrtstmpmove(struct quad *q)
{
	struct quad *nq;
	struct qa a, b, c;

	if (pdebug > 2) {
		printf("insrtstmpmove(), q:\n");
		prquad(q);
	}
	a = q->c;
	b.b2 = OASTEMP;
	b.w4 = maxtmp;
	betanew(maxtmp);
	maxtmp++;
	nstemps++;
	b.w0 = a.w0;
	c.b2 = OANONE;
	nq = insertquad(QCOPY, &a, &c, &b, q);
	nq = insertquad(QCOPY, &b, &c, &a, nq);
	nq->f3 = 0;
	nq->f7 = 0;
	nq->f4 = 0;
	if (pdebug > 3) {
		printf("insrtstmpmove(), before move, invq, invq->link\n");
		prquad(q);
		prquad(q->next);
	}
	q = qmove(q);
	if (pdebug > 3) {
		printf("insrtstmpmove(), after first move invq, invq->link\n");
		prquad(q);
		prquad(q->next);
	}
	q = qmove(q->next);
	if (pdebug > 3) {
		printf("insrtstmpmove(), after second move, invq, invq->link\n");
		prquad(q);
		prquad(q->next);
	}
	return q;
}

/* the quad opqd gets a new stemp as its result operand; the stemp is entered into the definition (name from the debug output) */
struct quad *moveindquad(struct quad *x, struct quad *opqd, struct qa *out, struct dl *d, struct dl *d2, int kind)
{
	if (pdebug > 2) {
		printf("moveindquad(), opqd:\n");
		prquad(opqd);
	}
	if (opqd->op == QMULT && opqd->next->op == QCONV) {
		opqd = qmove(opqd);
		opqd = opqd->next;
	}
	denorm(&x->a);
	x->a.b2 = OASTEMP;
	denorm(&opqd->c);
	opqd->c.b2 = OASTEMP;
	x->a.w4 = maxtmp;
	normop(&x->a);
	d2->x20 = maxtmp;
	betanew(maxtmp);
	opqd->c.w4 = maxtmp;
	maxtmp++;
	normop(&opqd->c);
	nstemps++;
	*out = opqd->c;
	if (d->f13) {
		if (kind == 2) {
			denorm(&opqd->a);
			opqd->a.b2 = OASTEMP;
			opqd->a.w4 = d->x20;
			normop(&opqd->a);
		} else if (kind == 3) {
			denorm(&opqd->b);
			opqd->b.b2 = OASTEMP;
			opqd->b.w4 = d->x20;
			normop(&opqd->b);
		}
	}
	opqd = qmove(opqd);
	return opqd;	/* the original has no return: rr2 still holds the result of qmove() */
}

/* the operand is set to the integer v (w0 as its first word); t == 1: v is used as it is, else it is negated (name guessed) */
void mkconstopnd(struct qa *p, long v, int t, int w0)
{
	p->w0 = w0;
	if (t == 1) {
		if (v > 32767) {
			p->b2 = OALCON;
			p->w4 = constlook(v);
		} else {
			p->b2 = OAICON;
			p->w4 = v;
		}
	} else if (v < -32767) {
		p->b2 = OALCON;
		p->w4 = constlook(-v);
	} else {
		p->b2 = OAICON;
		p->w4 = -v;
	}
}

/* a new stemp is made for the operation of the definitions p and t (name from the debug output) */
int insrtopstmp(struct quad *opqd, struct dl *p, struct dl *t, int kind, struct quad *outq)
{
	static struct qa zero;
	struct qa a, b, c;
	int op;

	if (pdebug > 2) {
		printf("insrtopstmp(), opqd:, t->dfquad, p->dfquad\n");
		prquad(opqd);
		prquad(t->q);
		prquad(p->q);
	}
	switch (opqd->op) {
	case QADD:
		op = 47;
		a = p->e14;
		b = zero;
		break;
	case QSUB:
		op = 48;
		mkconstopnd(&a, 0L, 1, p->e14.w0);
		b = p->e14;
		break;
	case QMULT:
	case QSHIFTL:
		a = p->e14;
		op = opqd->op;
		if (kind == 2)
			b = opqd->b;
		else
			b = opqd->a;
		break;
	default:
		if (pdebug > 3)
			printf("insrtopstmp(), returning FALSE\n");
		return 0;
	}
	if (op == 34) {
		c.w0 = 5;
		c.b2 = OATEMP;
		c.w4 = nexttmp++;
		nltemps++;
		phquad = insertquad(op, &a, &b, &c, phquad);
		b.b2 = OANONE;
		a.b2 = OASTEMP;
		betanew(maxtmp);
		a.w4 = maxtmp;
		maxtmp++;
		phquad = insertquad(QCONV, &c, &b, &a, phquad);
		c = a;
	} else {
		c.b2 = OASTEMP;
		betanew(maxtmp);
		c.w4 = maxtmp;
		maxtmp++;
		c.w0 = a.w0;
		phquad = insertquad(op, &a, &b, &c, phquad);
	}
	nstemps++;
	t->e14 = c;
	outq->a = c;
	if (pdebug > 3) {
		printf("insrtopnd(), ip->opndstmp=\n");
		propnd(&c);
		printf("\n");
	}
	return 1;
}

/* the stemp of the definition p is used for t (name from the debug output) */
int copyopndstmp(struct quad *opqd, struct dl *p, struct dl *t, int kind, struct quad *outq)
{
	if (pdebug > 2) {
		printf("copyopndstmp(), opqd:\n");
		prquad(opqd);
	}
	if (!p->f11 &&
	    ((kind == 2 && (opqd->b.b2 == OAICON || opqd->b.b2 == OALCON) && (opqd->op == QADD || opqd->op == QSUB)) ||
	     (kind == 3 && (opqd->a.b2 == OAICON || opqd->a.b2 == OALCON) && (opqd->op == QADD || opqd->op == QSUB)))) {
		t->e14 = p->e14;
		if (pdebug > 3) {
			printf("copyopndstmp(), copied indopnd: \n");
			propnd(&p->e14);
			printf("\n");
		}
		return 1;
	}
	return insrtopstmp(opqd, p, t, kind, outq);
}

extern struct nd *freend;

/* the stemp of the definition p is used for the operand of opqd of t; a node of the induction variable lists is filled (name from the debug output) */
struct quad *fillindvar(struct quad *qd, struct quad *opqd, int kind, struct dl *p, struct dl *t)
{
	struct nd *n, *tmp;
	int ok;

	if (pdebug > 2) {
		printf("fillindvar(), OP=%s, opqd:\n", kind == 2 ? "OP1" : "OP2");
		prquad(opqd);
	}
	if (freend != 0) {
		n = freend;
		freend = freend->link;
	} else
		n = (struct nd *)tabzalloc((int)sizeof(struct nd), 22, 7);
	t->f11 = 0;
	n->f15 = 0;
	n->v = 0;
	ok = 0;
	if (kind == 3) {
		if (p->f11) {
			if (opqd->a.b2 == OAICON || opqd->a.b2 == OALCON || opqd->op == QADD || opqd->op == QSUB) {
				n->f15 = 1;
				n->v = ceval(opqd->a, opqd->op, p->e14);
				t->f11 = 1;
				if (n->v != 0) {
					if (n->v > 0) {
						mkconstopnd(&t->e14, n->v, 1, p->e14.w0);
						ok = 1;
					} else {
						mkconstopnd(&t->e14, n->v, 0, p->e14.w0);
						ok = 1;
					}
				}
			} else
				ok = copyopndstmp(opqd, p, t, kind, (struct quad *)n);
		} else
			ok = copyopndstmp(opqd, p, t, kind, (struct quad *)n);
		if (0) {
			/* the kind 2 code stands here, reached by goto from below: so the original's block order */
k2:
			if (opqd->b.b2 == OAICON || opqd->b.b2 == OALCON || opqd->op == QADD || opqd->op == QSUB) {
				n->f15 = 1;
				n->v = ceval(opqd->b, opqd->op, p->e14);
				if (n->v != 0) {
					t->f11 = 1;
					if (n->v > 0) {
						mkconstopnd(&t->e14, n->v, 1, p->e14.w0);
						ok = 1;
					} else {
						mkconstopnd(&t->e14, n->v, 0, p->e14.w0);
						ok = 1;
					}
				}
			} else
				ok = copyopndstmp(opqd, p, t, kind, (struct quad *)n);
		}
	} else if (kind == 2) {
		if (p->f11)
			goto k2;
		ok = copyopndstmp(opqd, p, t, kind, (struct quad *)n);
	}
	if (ok) {
		ivagain = 1;
		if (p->f14) {
			tmp = p->l6;
			p->l6 = n;
			n->link = tmp;
			t->f13 = 1;
			t->f12 = 0;
			t->f14 = 0;
			t->l10 = p;
		} else if (p->f13) {
			tmp = p->l10->l6;
			p->l10->l6 = n;
			n->link = tmp;
			t->f12 = 1;
			t->f13 = 0;
			t->f14 = 0;
			t->l10 = p->l10;
			t->f10 = p->f10;
		}
		opqd = moveindquad(qd, opqd, (struct qa *)n, p, t, kind);
		return opqd;
	}
	freelist(0, n);
	return 0;
}

/* the induction variable ind is entered in the program by an operation behind opqd (name from the debug output) */
void indupdate(struct nd *ind, struct quad *opqd, int opcode)
{
	struct qa c;

	if (pdebug > 2) {
		printf("indupdate(), opcode=%d, ind->cnst=%d, ind->incsize=%d, q:\n", opcode, ind->f15, (int)(intptr_t)(ind->v));
		prquad(opqd);
	}
	if (ind->f15) {
		if (ind->v > 0) {
			mkconstopnd(&c, ind->v, 1, ind->c0.w0);
			opqd = insertquad(opcode, &ind->c0, &c, &ind->c0, opqd);
		} else {
			opcode = opcode == 1 ? 48 : 1;
			mkconstopnd(&c, ind->v, 0, ind->c0.w0);
			opqd = insertquad(opcode, &ind->c0, &c, &ind->c0, opqd);
		}
	} else {
		opqd = insertquad(opcode, &ind->c0, &ind->c12, &ind->c0, opqd);
		if (pdebug > 3) {
			printf("indupdate(), else clause, ind->stmp, ind->opndstmp\n");
			propnd(&ind->c0);
			printf("\n");
			propnd(&ind->c12);
			printf("\n");
		}
	}
	opqd->f3 = 0;
	opqd->f7 = 0;
	opqd->f4 = 0;
	nivelim++;
}

/* the node n is put at the end of the list of the free definitions (flag) or of the free induction nodes (name guessed) */
void freelist(int flag, struct nd *n)
{
	struct dl *x;
	struct nd *y;

	if (flag && lpdefs != 0) {
		x = flpdefs;
		if (x != 0) {
			for (; x->next != 0; x = x->next)
				;
			x->next = lpdefs;
		} else
			flpdefs = lpdefs;
	} else if (!flag) {
		y = freend;
		if (y != 0) {
			for (; y->link != 0; y = y->link)
				;
			y->link = n;
		} else
			freend = n;
	}
}

/* the result of q is moved to a new temporary: a move quad is inserted behind q and made a loop definition (name guessed) */
struct quad *insrtlpdef(struct quad *q, int blk)
{
	struct quad *nq;
	struct qa a, b, c;

	if (q->op == QMULT && q->next->op == QCONV)
		q = q->next;
	a = q->c;
	denorm(&q->c);
	b.b2 = OATEMP;
	b.w4 = nexttmp++;
	nltemps++;
	b.w0 = a.w0;
	q->c = b;
	normop(&q->c);
	c.b2 = OANONE;
	nq = insertquad(QCOPY, &b, &c, &a, q);
	copyins = 1;
	if (blk == BLKUNSET)
		cerror("host gopt: insrtlpdef() without its block (the original takes a register left over)");
	alloclpdef(nq, blk);
	nq->f3 = 0;
	nq->f7 = 0;
	nq->f4 = 0;
	return nq;
}

/* the definitions of the loops with a temporary as the result are moved to the list of the free ones (name from the debug output) */
void deltmplpdefs(void)
{
	struct dl *t, *prev, *u;

	if (pdebug > 2) {
		printf("deltmplpdefs(), on entry, lpdefs:\n");
		for (u = lpdefs; u != 0; u = u->next)
			prquad(u->q);
		printf("flpdefs:\n");
		for (u = flpdefs; u != 0; u = u->next)
			prquad(u->q);
	}
	copyins = 0;
	prev = t = lpdefs;
	while (t != 0) {
		if (t->q->c.b2 == OATEMP) {
			if (t == lpdefs)
			{
				lpdefs = lpdefs->next;
				prev = lpdefs;
			}
			else
				prev->next = t->next;
			if (flpdefs != 0) {
				u = flpdefs;
				flpdefs = t;
				t->next = u;
			} else {
				flpdefs = t;
				t->next = 0;
			}
			t = prev;
		} else {
			prev = t;
			t = t->next;
		}
	}
	if (pdebug > 3) {
		printf("deltmplpdefs(), on exit, lpdefs:\n");
		for (u = lpdefs; u != 0; u = u->next)
			prquad(u->q);
		printf("flpdefs:\n");
		for (u = flpdefs; u != 0; u = u->next)
			prquad(u->q);
	}
}

/* the induction temporary stmpno is removed from the list of d (name guessed) */
void delindvar(struct dl *d, int stmpno)
{
	struct nd *prev, *p;

	if (pdebug > 2)
		printf("delindvar(stmpno=%d), stmp_cnt=%d\n", stmpno, ctneg[stmpno]);
	if (ctneg[stmpno] <= 2) {
		prev = p = d->l6;
		while (p != 0) {
			if (pdebug > 3)
				printf("delindvar(), p->indstmp.opnd=%d\n", p->c0.w4);
			if (p->c0.w4 == stmpno) {
				if (pdebug > 3)
					printf("delindvar(), deleting  p->indstmp.opnd=%d\n", p->c0.w4);
				--ctneg[stmpno];
				if (prev == p)
					d->l6 = p->link;
				else
					prev->link = p->link;
				freelist(0, p);
				p->link = 0;
				return;
			}
			prev = p;
			p = p->link;
		}
		cerror("delindvar(), stmp not found\n");
	}
}

/* the induction variable definitions of the loop are checked for the quad q; the new definition is returned (name from the debug output) */
/* host: the block that chklpdefs() passes to insrtlpdef() in the original: r5 as sameopnd(q, d, op, 1) left it. When the
   operands are the same (kind and number) sameopnd() does not touch r5: it still holds the low word of the address of d,
   a quad of area 1 (its offset in the segment: tabzoff()). (The debug output of sameopnd, checkalias, almap
   changes r5: not modelled.) */
static int zblk(struct quad *q, struct quad *d, int op)
{
	struct qa *a = op == 2 ? &q->a : op == 3 ? &q->b : &q->c;

	if (a->b2 == d->c.b2 && a->w4 == d->c.w4)
		return (int16_t)tabzoff(1, (char *)d);
	/* else sameopnd() was true through checkalias(q, op), the last call: r5 is what it passed to almap() (almap()
	   leaves r5 alone), or its own ot when the operand is a beta (no almap()) */
	if (op == 2) {
		if (q->a.b2 == OABTEMP)
			return 2;
		if (q->op == QADDR)
			return 3;
		if (q->op == QIDR || q->op == QLOAD)
			return 0x18;
		return 0x2f;
	}
	if (op == 1)
		return q->c.b2 == OABTEMP ? 1 : 0x2f;
	return q->b.b2 == OABTEMP ? op : 0x2f;
}

struct quad *chklpdefs(struct quad *r, struct quad *q, int op, int x)
{
	(void)x;
	struct dl *dp, *t;
	struct quad *res;
	struct nd *n;
	int v;

	if (pdebug > 1) {
		printf("chklpdefs(), r:\n");
		prquad(r);
		printf("OP=%d, q:\n", op);
		prquad(q);
	}
	dp = lpdefs;
	goto lt;
lb:
	if (0) {
lt2:
		if ((op == 2 ? q->a.b2 : q->b.b2) != 6)
			goto ls;
		goto so;
	}
	if (q->op == QADD || q->op == QSUB)
		goto lt2;
so:
	if (!sameopnd(q, dp->q, op, 1))
		goto ls;
	if (!dp->q->f2)
		goto ls;
	if (dp->f13 && dp->q->c.b2 != OATEMP)
		return 0;
	if (r == 0)
		r = insrtlpdef(q, zblk(q, dp->q, op));	/* the original passes no block: r5 as sameopnd() left it (KNOWLEDGE.md, quirks) */
	if (pdebug > 2) {
		if (pdebug > 2)
			printf("found fam. of bsc ind var, dfqd, p->dfquad:\n");
		prquad(r);
		prquad(dp->q);
	}
	for (t = lpdefs; t != 0; t = t->next)
		if (t->q == r)
			break;
	if (t->q == r) {
		r->f2 = 1;
		if (dp->f14 || dp->f13) {
			res = fillindvar(r, q, op, dp, t);
			if (res != 0 && dp->q->c.b2 == OATEMP) {
				if (pdebug > 3)
					printf("chklpdefs(), tempcnt[temp=%d]=%d\n", dp->q->c.w4, ctpos[dp->q->c.w4]);
				if (ctpos[dp->q->c.w4] <= 1)
					delindvar(dp->l10, dp->x20);
			}
			return res;
		}
	} else
		cerror("chklpdef: quad not found\n");
ls:
	dp = dp->next;
	goto lt;
lh:
	if (r != dp->q)
		goto lb;
	if (dp->l6 != 0) {
		n = dp->l6;
		v = dp->f10 ? 1 : 48;
		while (n != 0) {
			indupdate(n, r, v);
			n = n->link;
		}
		freelist(0, dp->l6);
		dp->l6 = 0;
	}
	goto ls;
lt:
	if (dp != 0)
		goto lh;
	return 0;
}

/* the family of a basic induction variable: the definition r of the quad p is looked for (name from the debug output) */
struct quad *chkfamindvar(struct quad *r, int kind)
{
	struct quad *q, *a, *t;

	a = r;
	q = r->next;
	if (r->op == QMULT && r->next->op == QCONV) {
		a = r->next;
		q = r->next->next;
	}
	t = (q->op == QSTORE && q->a.b2 == a->c.b2 && q->a.w4 == a->c.w4 &&
	    ((q->f1 && !q->f2) || (q->f1 && q->f2 && q->a.b2 != OASTEMP))) ? q : 0;
	q = r;
	r = t;
	if (pdebug > 1) {
		printf("chkfamindvar(), q: , r:\n");
		prquad(q);
		prquad(r);
	}
	switch (q->op) {
	case QADD:
	case QMULT:
	case QSHIFTL:
	case QSUB:
		if (q->a.b2 == OAICON || q->a.b2 == OALCON || ((q->a.b2 == OANAME || q->a.b2 == OASTEMP || q->a.b2 == OABTEMP) &&
		    !isdefined(q, 2) && !(((q->a.w0 & 0x30) == 0x10) ^ ((q->b.w0 & 0x30) == 0x10)))) {
			if (q->op == QSHIFTL && q->b.b2 != OAICON && q->b.b2 != OALCON)
				return 0;
			q = chklpdefs(r, q, 3, kind);
			return q;
		}
	/* fall through */
	case QDIV:
	case QSHIFTR:
		if (q->b.b2 == OAICON || q->b.b2 == OALCON || ((q->b.b2 == OANAME || q->b.b2 == OASTEMP || q->b.b2 == OABTEMP) &&
		    !isdefined(q, 3) && !(((q->a.w0 & 0x30) == 0x10) ^ ((q->b.w0 & 0x30) == 0x10)))) {
			if (q->op == QSHIFTR && q->b.b2 != OAICON && q->b.b2 != OALCON)
				return 0;
			q = chklpdefs(r, q, 2, kind);
			return q;
		}
	}
	return 0;
}
