/*
 * lcl3.c - switch statements
 */
#include "cg.h"
#include "cg2.h"

extern int cerror();
extern struct node *clocal();
extern void delay();
extern int dope[];
extern struct sym *fpsym1;
extern struct sym *fpsym2;
extern int getreg();
extern void gswitch();
extern struct node *makeblock();
extern void mapnm();
extern struct node *member();
extern struct node node[];
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


static int newreg();
static int newlreg();
static void chk();

/* a free register other than avoid and r0 */
static int
newreg(avoid)
	int avoid;
{
	int r, t, t2;

	r = getreg(2, -1, 0x1000, 0);
	if (r == avoid || r == 0) {
		t = r;
		r = getreg(2, -1, 0x1000, 0);
		if (r == avoid || r == 0) {
			t2 = r;
			r = getreg(2, -1, 0x1000, 0);
			rfree(t2, 3);
		}
		rfree(t, 3);
	}
	return r;
}

#ifdef SEG
/* a free long register that does not overlap avoid, not rr0 */
static int
newlreg(avoid)
	int avoid;
{
	int r, t, t2;

	r = getreg(3, -1, 0x1000, 0);
	if (r == avoid || (avoid ^ 1) == r || r == 0) {
		t = r;
		r = getreg(3, -1, 0x1000, 0);
		if (r == avoid || (avoid ^ 1) == r || r == 0) {
			t2 = r;
			r = getreg(3, -1, 0x1000, 0);
			rfree(t2, 5);
		}
		rfree(t, 5);
	}
	return r;
}
#endif

/* the code for a switch: a jump table, or a search with cpir */
void
gswitch(p)
	register struct node *p;
{
	register struct node *l, *r;
	register struct node *x;
	register long n;
	register int dlab;
	int tab, reg, save, ra, rb, cnt, tlab, t, t2;
#ifdef OLD_D2T
	struct rs *tn;
#endif

	l = p->left;
	r = p->right;
	n = r->right->lval;
	tab = r->right->rval;
	dlab = -r->left->lval;
	x = l->op == REG ? l : l->left;
	reg = x->rval;
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
#ifdef SEG
		ra = newlreg(reg);
		printf("\trl\t%s,#2\n", rnames[reg]);
		printf("\tldl\tr%s,L%d(%s)\n", rnames[ra], tab, rnames[reg]);
		printf("\tjp\t@r%s\n", rnames[ra]);
#else
		printf("\trl\t%s,#1\n", rnames[reg]);
		printf("\tld\t%s,L%d(%s)\n", rnames[reg], tab, rnames[reg]);
		printf("\tjp\t@%s\n", rnames[reg]);
#endif
		return;
	}
	cnt = (int)l->right->lval;
	tlab = l->right->rval;
#ifdef SEG
	ra = newlreg(reg);
	rb = newreg(reg);
	printf("\tldl\tr%s,#L%d\n", rnames[ra], tlab);
	printf("\tld\t%s,#%d\n", rnames[rb], cnt);
	printf("\tcpir\t%s,@r%s,%s,eq\n", rnames[reg], rnames[ra], rnames[rb]);
	printf("\tjpr\tne,L%d\n", dlab);
	printf("\tsll\t%s,#2\n", rnames[rb]);
	printf("\tneg\t%s\n", rnames[rb]);
	printf("\tldl\tr%s,L%d+%d(%s)\n", rnames[ra], tab, (cnt - 1) << 2, rnames[rb]);
	printf("\tjp\t@r%s\n", rnames[ra]);
	rfree(ra, 5);
	rfree(rb, 3);
	rfree(reg, 3);
#else
	if (reg == 0) {
		save = reg;
		reg = getreg(2, -1, 0x1000, 0);
		rmove(reg, save, 3);
		rfree(save, 3);
	}
#ifdef OLD_D2T
	if ((tn = rfind(reg, 2)) != 0)	/* the register of the switch value is in use here (OLD_D2T frees it in between) */
		tn->busy++;
	else
		getreg(2, reg, 0x1000, 0);
#endif
	ra = getreg(2, -1, 0x1000, 0);
	if (ra == reg || ra == 0) {
		t = ra;
		ra = getreg(2, -1, 0x1000, 0);
		if (ra == reg || ra == 0) {
			t2 = ra;
			ra = getreg(2, -1, 0x1000, 0);
			rfree(t2, 3);
		}
		rfree(t, 3);
	}
	rb = getreg(2, -1, 0x1000, 0);
#ifdef OLD_D6R
#define D6RBAD(r)	((r) == reg || (r) <= 1)	/* v0: the count of cpir in neither half of rr0 */
#else
#define D6RBAD(r)	((r) == reg || (r) == 0)
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
#endif
}

#ifdef SEG
/* PLUS(frame register, offset): the offset goes first and takes the pointer type, the register becomes an int */
static void
chk(p)
	register struct node *p;
{
	register struct node *r, *c;

	r = p->left;
	c = p->right;
	c->type = r->type;
	p->left = c;
	if (r->op != REG)
		cerror("badly formed offset");
	p->right = r;
	r->type = 4;
}
#else
/* a debugging hook that does nothing */
static void
chk(p)
	struct node *p;
{
}
#endif

/* the node p after the conversions: names to addresses of their class, conversions of constants */
struct node *
clocal(p)
	register struct node *p;
{
	register struct node *l, *r;
	register struct sym *s;
	register struct node *fp, *n, *q;
	long val;
	int o, pf, lf, pt, lt, t, ob;

	l = p->left;
	r = p->right;
	switch (o = p->op) {
	case NAME:
		s = p->sname;
		switch (s->sclass) {
		case 9:
			fp = makeblock(REG, 0, 0, 0x18);
			fp->lval = s->soffset / 8;
			fp->rval = 15;
			n = makeblock(STREF, fp, p, 0);
			p = member(n);
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
			fp = makeblock(REG, 0, 0, 0x18);
			fp->lval = s->soffset / 8;
			fp->rval = 15;
			n = makeblock(STREF, fp, p, 0);
			p = member(n);
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
				return p;
			p->lval = 0;
			p->rval = -(int)s->soffset;
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
			goto done;
		while (l->op == SCONV) {
			if (tbytes(l->type) <= tbytes(p->type))
				break;
			if (l->left->type == 7 || l->left->type == 6)
				break;
			l->op = FREE;
			l = l->left;
			p->left = l;
		}
		pt = p->type;
		lt = l->type;
		if (l->op == ICON) {
			val = l->lval;
#ifdef SEG
			if (pt == 4 && (lt & 0x30) == 0x10) {
				l->lval = (long)(int)val;
				goto done;
			}
			if (pt == 14 && (lt & 0x30) == 0x10) {
				l->lval = val & 0xFFFFL;
				goto done;
			}
#endif
			switch (pt) {
			case 2:
				l->lval = (long)(char)val;
				break;
			case 4:
				l->lval = (long)(int)val;
				break;
			case 12:
				l->lval = val & 0xFF;
				break;
			case 14:
				l->lval = val & 0xFFFFL;
				break;
			}
			l->type = pt;
		} else {
			if (l->op == MUL) {
				if (szty(pt) - szty(lt) != 1)
					if (tbytes(lt) != tbytes(pt))
						goto done;
			} else if (tbytes(lt) != tbytes(pt))
				goto done;
			l->type = pt;
		}
		if (tbytes(pt) != tbytes(lt)) {
#ifdef SEG
			goto done;
#else
			p->op = FREE;
			return l;
#endif
		}
#ifdef SEG
		if ((pt & 0x30) == 0x10)
			goto done;
#endif
		l->type = p->type;
		l->rval = p->rval;
		l->lval = p->lval;
		l->sname = p->sname;
		return l;
	}
done:
	l = p->left;
	r = p->right;
	if ((dope[p->op] & 0x10) && r->op == ICON && l->op == SCONV &&
	    (l->left->type == 2 || l->left->type == 12) && r->lval >= -256 && r->lval <= 255) {
		l->op = FREE;
		p->left = l->left;
		l = p->left;
		r->type = l->type;
		p->type = l->type;
	}
	return p;
}
