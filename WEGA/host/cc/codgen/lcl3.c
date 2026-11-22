/*
 * lcl3.c - switch statements
 */
#include "cg.h"
#include "cg2.h"

static void chk(NODE *p);

extern int dope[];
extern struct sym *fpsym1;
extern struct sym *fpsym2;
extern NODE node[];
extern char *rnames[];
static char sccsid[] __attribute__((unused)) = "@[$]lcl3.c\t2.4  10/06/83 10:39:09 Copr. 1981 87wega3.2.";

#define INCREF(t)	((((t) & ~0xF) << 2) | 0x10 | ((t) & 0xF))

#define DECREF(t)	((((t) >> 2) & ~0xF) | ((t) & 0xF))

#ifdef OLD_D3P
#define D3PWORD(t)	((t) == 4 || (t) == 14 || ((t) & 0x30) == 0x10)
#endif

#define SCONV	110



/* the code for a switch: a jump table, or a search with cpir */
#ifdef SEG
void
gswitch(NODE *p)
{
	register long n;
	register int dlab, tab;
	NODE *r, *l;
	int reg, a6, save, t2, tlab, rb, t, cnt;

	l = p->in.left;
	r = p->in.right;
	n = r->in.right->tn.lval;
	tab = r->in.right->tn.rval;
	dlab = -r->in.left->tn.lval;
	reg = (l->in.op == REG ? l : l->in.left)->tn.rval;
	if (l->in.op != 124) {
		mapnm(l->in.left);
		delay(l);
		reclaim(l, 0, 0);
		printf("\tcp\t%s,#%ld\n", rnames[reg], n);
		printf("\tjpr\tugt,L%d\n", dlab);
		if (reg == 0) {
			save = reg;
			reg = getreg(2, -1, 0x1000, 0);
			rmove(reg, save, 3);
			rfree(save, 3);
		}
		a6 = getreg(3, -1, 0x1000, 0);
		if (reg == a6 || (reg ^ 1) == a6 || a6 == 0) {
			save = a6;
			a6 = getreg(3, -1, 0x1000, 0);
			if (reg == a6 || (reg ^ 1) == a6 || a6 == 0) {
				t2 = a6;
				a6 = getreg(3, -1, 0x1000, 0);
				rfree(t2, 5);
			}
			rfree(save, 5);
		}
		printf("\trl\t%s,#2\n", rnames[reg]);
		printf("\tldl\tr%s,L%d(%s)\n", rnames[a6], tab, rnames[reg]);
		printf("\tjp\t@r%s\n", rnames[a6]);
		return;
	}
	if (l->in.op == 124) {
		cnt = (int)l->in.right->tn.lval;
		tlab = l->in.right->tn.rval;
		a6 = getreg(3, -1, 0x1000, 0);
		if (reg == a6 || (reg ^ 1) == a6 || a6 == 0) {
			t = a6;
			a6 = getreg(3, -1, 0x1000, 0);
			if (reg == a6 || (reg ^ 1) == a6 || a6 == 0) {
				t2 = a6;
				a6 = getreg(3, -1, 0x1000, 0);
				rfree(t2, 5);
			}
			rfree(t, 5);
		}
		rb = getreg(2, -1, 0x1000, 0);
		if (reg == rb || rb == 0) {
			t = rb;
			rb = getreg(2, -1, 0x1000, 0);
			if (reg == rb || rb == 0) {
				t2 = rb;
				rb = getreg(2, -1, 0x1000, 0);
				rfree(t2, 3);
			}
			rfree(t, 3);
		}
		printf("\tldl\tr%s,#L%d\n", rnames[a6], tlab);
		printf("\tld\t%s,#%d\n", rnames[rb], cnt);
		printf("\tcpir\t%s,@r%s,%s,eq\n", rnames[reg], rnames[a6], rnames[rb]);
		printf("\tjpr\tne,L%d\n", dlab);
		printf("\tsll\t%s,#2\n", rnames[rb]);
		printf("\tneg\t%s\n", rnames[rb]);
		printf("\tldl\tr%s,L%d+%d(%s)\n", rnames[a6], tab, (cnt - 1) << 2, rnames[rb]);
		printf("\tjp\t@r%s\n", rnames[a6]);
		rfree(a6, 5);
		rfree(rb, 3);
		rfree(reg, 3);
	}
}
#else
void
gswitch(NODE *p)
{
	register long n;
	register int dlab, tab;
	NODE *r, *l;
	int reg, save, t2, tlab, rb, ra, t, cnt;
#ifdef OLD_D2T
	struct rs *tn;
#endif

	l = p->in.left;
	r = p->in.right;
	n = r->in.right->tn.lval;
	tab = r->in.right->tn.rval;
	dlab = -r->in.left->tn.lval;
	reg = (l->in.op == REG ? l : l->in.left)->tn.rval;
	if (l->in.op != 124) {
		mapnm(l->in.left);
		delay(l);
		reclaim(l, 0, 0);
		printf("\tcp\t%s,#%ld\n", rnames[reg], n);
		printf("\tjpr\tugt,L%d\n", dlab);
		if (reg == 0) {
			save = reg;
			reg = getreg(2, -1, 0x1000, 0);
			rmove(reg, save, 3);
			rfree(save, 3);
		}
		printf("\trl\t%s,#1\n", rnames[reg]);
		printf("\tld\t%s,L%d(%s)\n", rnames[reg], tab, rnames[reg]);
		printf("\tjp\t@%s\n", rnames[reg]);
		return;
	}
	if (l->in.op == 124) {
	cnt = (int)l->in.right->tn.lval;
	tlab = l->in.right->tn.rval;
	if (reg == 0) {
		t = reg;
		reg = getreg(2, -1, 0x1000, 0);
		rmove(reg, t, 3);
		rfree(t, 3);
	}
#ifdef OLD_D2T
	if ((tn = rfind(reg, 2)) != 0)	/* the register of the switch value is in use here (OLD_D2T frees it in between) */
		tn->busy++;
	else
		getreg(2, reg, 0x1000, 0);
#endif
	ra = getreg(2, -1, 0x1000, 0);
	if (reg == ra || ra == 0) {
		t = ra;
		ra = getreg(2, -1, 0x1000, 0);
		if (reg == ra || ra == 0) {
			t2 = ra;
			ra = getreg(2, -1, 0x1000, 0);
			rfree(t2, 3);
		}
		rfree(t, 3);
	}
	rb = getreg(2, -1, 0x1000, 0);
#ifdef OLD_D6R
#define D6RBAD(r)	(reg == (r) || (r) <= 1)	/* v0: the count of cpir in neither half of rr0 */
#else
#define D6RBAD(r)	(reg == (r) || (r) == 0)
#endif
	if (D6RBAD(rb)) {
		t = rb;
		rb = getreg(2, -1, 0x1000, 0);
		if (D6RBAD(rb)) {
			t2 = rb;
			rb = getreg(2, -1, 0x1000, 0);
			rfree(t2, 3);
		}
		rfree(t, 3);
	}
	printf("\tld\t%s,#L%d\n", rnames[ra], tlab);
	printf("\tld\t%s,#%d\n", rnames[rb], cnt);
	printf("\tcpir\t%s,@%s,%s,eq\n", rnames[reg], rnames[ra], rnames[rb]);
	printf("\tjpr\tne,L%d\n", dlab);
#ifdef OLD_D6T
	/* v0: the target from the pointer cpir leaves behind the value found (the labels follow the values) */
	printf("\tld\t%s,%d(%s)\n", rnames[reg], (cnt - 1) * 2, rnames[ra]);
#else
	printf("\tsll\t%s,#1\n", rnames[rb]);
	printf("\tneg\t%s\n", rnames[rb]);
	printf("\tld\t%s,L%d+%d(%s)\n", rnames[reg], tab, (cnt - 1) * 2, rnames[rb]);
#endif
	printf("\tjp\t@%s\n", rnames[reg]);
	rfree(ra, 3);
	rfree(rb, 3);
	rfree(reg, 3);
	}
}
#endif

#ifdef SEG
/* PLUS(frame register, offset): the offset goes first and takes the pointer type, the register becomes an int */
static void
chk(NODE *p)
{
	register NODE *r;

	r = p->in.left;
	p->in.right->in.type = r->in.type;
	p->in.left = p->in.right;
	if (r->in.op != REG)
		cerror("badly formed offset");
	p->in.right = r;
	r->in.type = 4;
}
#else
/* a debugging hook that does nothing */
static void
chk(NODE *p)
{
	(void)p;
}
#endif

/* the node p after the conversions: names to addresses of their class, conversions of constants */
NODE *
clocal(NODE *p)
{
	register int o, pf, lf;
	register struct sym *s;
	register NODE *r, *l, *q;

	l = p->in.left;
	r = p->in.right;
	switch (o = p->in.op) {
	case NAME:
		s = p->in.sname;
		switch (s->sclass) {
		case 9:
#ifdef OLD_D3K
#ifndef OLD_D3KC
			/* v0: a parameter whose address is taken is not known in its register: it needs its home */
			{
				register struct rs *n;

				for (n = rsuse; n; n = n->next)
					if (n->sym == s)
						n->sym = 0;
			}
#endif
#endif
			r = makeblock(REG, 0, 0, 0x18);
			r->tn.lval = s->soffset / 8;
			r->tn.rval = 15;
			p = member(makeblock(STREF, r, p, 0));
			q = p->in.left;
			if (q->in.op == PLUS) {
				if (q->in.right->in.op == ICON) {
					if (s->sflags >> 3 & 1)
						q->in.right->in.sname = fpsym2;
					else
						q->in.right->in.sname = fpsym1;
				}
				chk(q);
			}
			break;
		case 1:
			r = makeblock(REG, 0, 0, 0x18);
			r->tn.lval = s->soffset / 8;
			r->tn.rval = 15;
			p = member(makeblock(STREF, r, p, 0));
			q = p->in.left;
			if (q->in.op == PLUS) {
				if (q->in.right->in.op == ICON)
					q->in.right->in.sname = fpsym1;
				chk(q);
			}
			break;
		case 3:
		case 6:
			if ((s->sflags >> 2 & 1) == 1)
				return p;	/* the original returns without a value (the caller gets what is left in the register) */
			p->tn.lval = 0;
			p->tn.rval = -s->soffset;
			break;
		case 4:
			p->in.op = REG;
			p->tn.lval = 0;
			p->tn.rval = s->sbyte5;
			break;
		case 18:
			break;
		default:
			cerror("lcl: NAME op has illegal class\n");
		}
		break;
	case SCONV:
		pf = p->in.type == 6 || p->in.type == 7;
		lf = l->in.type == 6 || l->in.type == 7;
		if (pf || lf)
			break;
		while (l->in.op == SCONV) {
			if (tbytes(l->in.type) <= tbytes(p->in.type))
				break;
			if (l->in.left->in.type == 7 || l->in.left->in.type == 6)
				break;
			l->in.op = FREE;
			p->in.left = l = l->in.left;
		}
		pf = p->in.type;
		lf = l->in.type;
		{
		long val;

		if (l->in.op == ICON) {
			val = l->tn.lval;
#ifdef SEG
			if (pf == 4 && (lf & 0x30) == 0x10) {
				l->tn.lval = (long)(int)val;
				goto done;
			}
			if (pf == 14 && (lf & 0x30) == 0x10) {
				l->tn.lval = val & 0xFFFFL;
				goto done;
			}
#endif
			switch (pf) {
			case 2:
				l->tn.lval = (long)(char)val;
				break;
			case 12:
				l->tn.lval = val & 0xFF;
				break;
			case 14:
				l->tn.lval = val & 0xFFFFL;
				break;
			case 4:
				l->tn.lval = (long)(int)val;
				break;
			}
			l->in.type = pf;
		} else if (l->in.op == MUL && szty(pf) - szty(lf) == 1)
			l->in.type = pf;
		else if (tbytes(pf) != tbytes(lf))
			break;
#ifdef SEG
		else if ((pf & 0x30) == 0x10)
			break;
#endif
		if (tbytes(pf) == tbytes(lf)) {
			l->in.type = p->in.type;
			l->tn.rval = p->tn.rval;
			l->tn.lval = p->tn.lval;
			l->in.sname = p->in.sname;
			p = l;
			goto ret;
		}
		p->in.op = FREE;
		p = l;
		goto ret;
		}
	}
#ifdef SEG
done:
#endif
	if (logop(p->in.op) && r->in.op == ICON && l->in.op == SCONV &&
	    (l->in.left->in.type == 2 || l->in.left->in.type == 12) && r->tn.lval >= -256 && r->tn.lval <= 255) {
		l->in.op = FREE;
		l = p->in.left = l->in.left;
		p->in.type = r->in.type = l->in.type;
	}
ret:
	return p;
}
