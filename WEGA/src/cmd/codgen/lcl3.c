/*
 * lcl3.c - switch statements
 */
#include "cg.h"
#include "cg2.h"

extern int cerror();
extern NODE *clocal();
extern void delay();
extern int dope[];
extern struct sym *fpsym1;
extern struct sym *fpsym2;
extern int getreg();
extern void gswitch();
extern NODE *makeblock();
extern void mapnm();
extern NODE *member();
extern NODE node[];
extern void reclaim();
extern struct rs *rfind();
extern void rfree();
extern void rmove();
extern char *rnames[];
extern int szty();
extern int tbytes();
static char sccsid[] = "@[$]lcl3.c\t2.4  10/06/83 10:39:09 Copr. 1981 87wega3.2.";

#define INCREF(t)	((((t) & ~0xF) << 2) | 0x10 | ((t) & 0xF))

#define DECREF(t)	((((t) >> 2) & ~0xF) | ((t) & 0xF))

#ifdef OLD_D3P
#define D3PWORD(t)	((t) == 4 || (t) == 14 || ((t) & 0x30) == 0x10)
#endif

#define SCONV	110


static void chk();

/* the code for a switch: a jump table, or a search with cpir */
#ifdef SEG
void
gswitch(p)
	register NODE *p;
{
	register NODE *x, *y;
	register long n;
	register int dlab, tab;
	NODE *r, *l;
	int reg, a6, save, t2, ac, tlab, rb, ra, t, cnt;

	l = p->left;
	r = p->right;
	n = r->right->lval;
	tab = r->right->rval;
	dlab = -r->left->lval;
	reg = (l->op == REG ? l : l->left)->rval;
	if (l->op != 124) {
		mapnm(l->left);
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
	if (l->op == 124) {
		cnt = (int)l->right->lval;
		tlab = l->right->rval;
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
gswitch(p)
	register NODE *p;
{
	register NODE *x, *y;
	register long n;
	register int dlab, tab;
	NODE *r, *l;
	int reg, a6, save, t2, ac, tlab, rb, ra, t, cnt;
#ifdef OLD_D2T
	struct rs *tn;
#endif

	l = p->left;
	r = p->right;
	n = r->right->lval;
	tab = r->right->rval;
	dlab = -r->left->lval;
	reg = (l->op == REG ? l : l->left)->rval;
	if (l->op != 124) {
		mapnm(l->left);
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
	if (l->op == 124) {
	cnt = (int)l->right->lval;
	tlab = l->right->rval;
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
chk(p)
	register NODE *p;
{
	register NODE *r;

	r = p->left;
	p->right->type = r->type;
	p->left = p->right;
	if (r->op != REG)
		cerror("badly formed offset");
	p->right = r;
	r->type = 4;
}
#else
/* a debugging hook that does nothing */
static void
chk()
{
	register int spare;
}
#endif

/* the node p after the conversions: names to addresses of their class, conversions of constants */
NODE *
clocal(p)
	NODE *p;
{
	register int o, pf, lf;
	register struct sym *s;
	register NODE *r, *l, *q;
	int x;

	l = p->left;
	r = p->right;
	switch (o = p->op) {
	case NAME:
		s = p->sname;
		switch (s->sclass) {
		case 9:
			r = makeblock(REG, 0, 0, 0x18);
			r->lval = s->soffset / 8;
			r->rval = 15;
			p = member(makeblock(STREF, r, p, 0));
			q = p->left;
			if (q->op == PLUS) {
				if (q->right->op == ICON) {
					if (s->sflags >> 3 & 1)
						q->right->sname = fpsym2;
					else
						q->right->sname = fpsym1;
				}
				chk(q);
			}
			break;
		case 1:
			r = makeblock(REG, 0, 0, 0x18);
			r->lval = s->soffset / 8;
			r->rval = 15;
			p = member(makeblock(STREF, r, p, 0));
			q = p->left;
			if (q->op == PLUS) {
				if (q->right->op == ICON)
					q->right->sname = fpsym1;
				chk(q);
			}
			break;
		case 3:
		case 6:
			if ((s->sflags >> 2 & 1) == 1)
				return;
			p->lval = 0;
			p->rval = -s->soffset;
			break;
		case 4:
			p->op = REG;
			p->lval = 0;
			p->rval = s->sbyte5;
			break;
		case 18:
			break;
		default:
			cerror("lcl: NAME op has illegal class\n");
		}
		break;
	case SCONV:
		pf = p->type == 6 || p->type == 7;
		lf = l->type == 6 || l->type == 7;
		if (pf || lf)
			break;
		while (l->op == SCONV) {
			if (tbytes(l->type) <= tbytes(p->type))
				break;
			if (l->left->type == 7 || l->left->type == 6)
				break;
			l->op = FREE;
			p->left = l = l->left;
		}
		pf = p->type;
		lf = l->type;
		{
		long val;

		if (l->op == ICON) {
			val = l->lval;
#ifdef SEG
			if (pf == 4 && (lf & 0x30) == 0x10) {
				l->lval = (long)(int)val;
				goto done;
			}
			if (pf == 14 && (lf & 0x30) == 0x10) {
				l->lval = val & 0xFFFFL;
				goto done;
			}
#endif
			switch (pf) {
			case 2:
				l->lval = (long)(char)val;
				break;
			case 12:
				l->lval = val & 0xFF;
				break;
			case 14:
				l->lval = val & 0xFFFFL;
				break;
			case 4:
				l->lval = (long)(int)val;
				break;
			}
			l->type = pf;
		} else if (l->op == MUL && szty(pf) - szty(lf) == 1)
			l->type = pf;
		else if (tbytes(pf) != tbytes(lf))
			break;
#ifdef SEG
		else if ((pf & 0x30) == 0x10)
			break;
#endif
		if (tbytes(pf) == tbytes(lf)) {
			l->type = p->type;
			l->rval = p->rval;
			l->lval = p->lval;
			l->sname = p->sname;
			p = l;
			goto ret;
		}
		p->op = FREE;
		p = l;
		goto ret;
		}
	}
#ifdef SEG
done:
#endif
	if (logop(p->op) && r->op == ICON && l->op == SCONV &&
	    (l->left->type == 2 || l->left->type == 12) && r->lval >= -256 && r->lval <= 255) {
		l->op = FREE;
		l = p->left = l->left;
		p->type = r->type = l->type;
	}
ret:
	return p;
}
