/*
 * reader3.c - the driver of the code generation: order of the trees, stores, conditional branches (codgen / scodgen)
 *
 * Reconstructed from /lib/codgen: codgen() 0x0aaa, prcook() 0x0af6, order() 0x0ba2, store() 0x1084, constore() 0x11da,
 * markcall() 0x1212, stoarg() 0x126a, cbranch() 0x12bc, ffld() 0x15d0, oreg2() 0x175e, canon() 0x1884, argprep() 0x18be.
 * The structure follows the classic PCC reader, with two register counts per node (su, fsu). Names are guesses.
 */
#include "cg.h"

#define FOREFF	01
#define FORCC	040
#define INTEMP_	0x1000
#define MUSTDO	0x1000
#define NOMATCH	0x1000
#define MDONE	0x1001
#define FORREW	0x4000
#define FREE_	100
#define INCREF(t)	((((t) & ~0xF) << 2) | 0x10 | ((t) & 0xF))
#define DECREF(t)	((((t) >> 2) & ~0xF) | ((t) & 0xF))

extern int cerror(), uerror();
extern int match(), tshape(), allo(), szty(), notoff();
extern int getlab(), deflab();
extern int nextcook(), lastchance();
extern void cbgen(), rallo(), offstar(), reclaim(), tfree(), fwalk(), fwalkd(), rfree(), rfreenode(), rfreeall(), ncopy();
extern int setstr(), setasop(), setasg(), setbin(), gencall(), genscall();
extern int mkadrs(), stoasg();
extern void mycanon();
extern struct node *talloc(), *tcopy(), *optim();
extern void genargs();
extern void sucomp();
extern void rcount();
extern char *opst[];
extern char *cnames[];		/* 0x120: SANY .. SWADD, 0 */
extern int dope[];
extern int negrel[];		/* 0x274 */
extern int fregs, ffregs;
extern struct node resc[];
int lmuldiv;		/* 0x3e: the second long multiplication or division in an expression forces a store */
struct node *stotree;	/* 0x8f92: the subtree that has to be stored first */
int stocook;		/* 0x8fc6 */
int callflag;		/* 0x8f5a */

void delay(), order(), cbranch(), store(), constore(), markcall(), stoarg();
struct node *canon(), *argprep();

/* generate the code for p: stores first, then the tree itself */
void
codgen(p, cookie)
	struct node *p;
	int cookie;
{
	for (;;) {
		p = canon(optim(p));
		stotree = 0;
		store(p);
		lmuldiv = 0;
		if (stotree == 0)
			break;
		order(stotree, stocook);
	}
	order(p, cookie);
}

/* print a cookie (debug) */
void
prcook(cookie)
	int cookie;
{
	int i, flag;

	if (cookie & 0x8000) {
		if (cookie == 0x8000)
			printf("SZERO");
		else if (cookie == 0x8002)
			printf("SSCON");
		else if (cookie == 0x8004)
			printf("SICON");
		else
			printf("SPECIAL+%%%0.2x", cookie & 0x7FFF);
		return;
	}
	flag = 0;
	for (i = 0; cnames[i]; i++)
		if (cookie & (1 << i)) {
			if (flag)
				putchar('|');
			flag++;
			printf(cnames[i]);
		}
	printf(" [%%%0.2x]", cookie);
}

/* generate code for p; cook says in which form the result is wanted */
void
order(p, cook)
	register struct node *p;
	int cook;
{
	register int o, ty, m;
	int cookie, lab, lab2, ck;
	struct node *p1, *p2, *q;

again:
	cookie = cook;
	rcount();
	p = canon(argprep(p, cookie));
	rallo(p, p->rall);
	o = p->op;
	ty = dope[o] & 0xE;
	m = p->op;
	switch (m) {
	case QUEST:
	case ANDAND:
	case OROR:
	case COMOP:
	case CALL:
	case UCALL:
	case FCALL:
	case UFCALL:
	case NOT:
	case STCALL:
	case USTCALL:
	case RET:
	case CBRANCH:
		break;
	case MUL:
	case ASG MUL:
		if (p->right->op == OREG && p->left->op == REG)
			p->right->rall = NOPREF;
	default:
		for (;;) {
			m = match(p, cookie);
			if (m == MDONE)
				goto cleanup;
			if (m != NOMATCH)
				break;
			if (!(cookie = nextcook(p, cookie)))
				goto nomat;
		}
		break;
	}
	p1 = p->left;
	p2 = ty == BITYPE ? p->right : 0;
	switch (m) {
	default:
	nomat:
		cerror("no table entry for op %s", opst[p->op]);
	case RET:
		p = p->left;
		if (p->type == 6 || p->type == 7) {
			p->rall = 0x1010;
			order(p, 0x10);
			tfree(p);
			return;
		}
		cook = 4;
		goto again;
	case CBRANCH:
		cbranch(p1, -1, (int)p2->lval);
		p2->op = FREE;
		p->op = FREE;
		return;
	case COMOP:
		codgen(p1, FOREFF);
		p2->rall = p->rall;
		codgen(p2, cookie);
		ncopy(p, p2);
		p2->op = FREE;
		goto cleanup;
	case QUEST:
		lab = getlab();
		cbranch(p1, -1, lab);
		p2->left->rall = p->rall;
		ck = p2->type == 6 || p2->type == 7 ? 0x10 : 4;
		codgen(p2->left, ck);
		p2->right->rall = p2->left->rval | MUSTDO;
		reclaim(p2->left, 0, 0);
		lab2 = getlab();
		cbgen(0, lab2, 'I');
		deflab(lab);
		rfreeall(1);
		codgen(p2->right, ck);
		deflab(lab2);
		p->op = REG;
		p->lval = 0;
		p->rval = p2->right->rval;
		p->type = p2->right->type;
		tfree(p2->right);
		p2->op = FREE;
		goto cleanup;
	case ANDAND:
	case OROR:
	case NOT:
		lab = getlab();
		cbranch(p, -1, lab);
		p->op = CCODES;
		p->label = lab;
		order(p, 4);
		goto cleanup;
	case FLD:
		if (p1->op == STAR) {
			offstar(p1->left);
			goto again;
		}
	case UMINUS:
		order(p1, p1->type == 6 || p1->type == 7 ? 0x10 : 2);
		goto again;
	case NAME:
		if (o == REG)
			goto nomat;
		order(p, 4);
		goto again;
	case INIT:
		uerror("illegal initialization: order()");
		return;
	case UFCALL:
		p->right = 0;
	case FCALL:
		o = p->op = UFCALL;
		if (gencall(p, cookie))
			goto nomat;
		goto cleanup;
	case UCALL:
		p->right = 0;
	case CALL:
		o = p->op = UCALL;
		if (gencall(p, cookie))
			goto nomat;
		goto cleanup;
	case USTCALL:
		p->right = 0;
	case STCALL:
		o = p->op = USTCALL;
		if (genscall(p, cookie))
			goto nomat;
		goto cleanup;
	case STAR:
		if (cook == FOREFF) {
			order(p->left, FOREFF);
			p->op = FREE;
			return;
		}
		offstar(p->left);
		goto again;
	case INCR:
		if (cook & FOREFF) {
			p->op = p->op == INCR ? ASG PLUS : ASG MINUS;
			goto again;
		}
		p1 = tcopy(p);
		reclaim(p->left, 0, 0);
		p->left = p1;
		p1->op = p->op == INCR ? ASG PLUS : ASG MINUS;
		p->op = p->op == INCR ? MINUS : PLUS;
		goto again;
	case STASG:
		if (setstr(p))
			goto again;
		goto nomat;
	case ASG PLUS:
		if (setasop(p))
			goto again;
		q = tcopy(p);
		p->op = ASSIGN;
		reclaim(p->right, 0, 0);
		p->right = q;
		p = canon(p);
		rallo(p, p->rall);
		order(q->left, q->type == 7 || q->type == 6 ? 0x10 : 4);
		order(q, q->type == 7 || q->type == 6 ? 0x10 : 4);
		goto again;
	case ASSIGN:
		if (setasg(p))
			goto again;
		goto nomat;
	case BITYPE:
		if (setbin(p))
			goto again;
		switch (o) {
		case PLUS:
		case MINUS:
		case MUL:
		case AND:
		case OR:
		case ER:
		case DIV:
		case MOD:
		case LS:
		case RS:
			p->op = o + 1;
			goto again;
		}
		goto nomat;
	}

cleanup:
	if (cook & FOREFF) {
		reclaim(p, 0, 0);
		return;
	}
	if (p->op == FREE)
		return;
	if (tshape(p, cook))
		return;
	if (match(p, cook) == MDONE)
		return;
	if (lastchance(p, cook))
		goto again;
	goto nomat;
}

/* find the subtree of p that has to be stored: the result is stotree / stocook */
void
store(p)
	register struct node *p;
{
	register int o, ty;

	o = p->op;
	ty = dope[o] & 0xE;
	if (ty == LTYPE)
		return;
	switch (o) {
	case UCALL:
	case UFCALL:
	case USTCALL:
		callflag++;
		break;
	case STAR:
		if (dope[p->left->op] & 1)
			stoasg(p->left, STAR);
		break;
	case CALL:
	case FCALL:
	case STCALL:
		store(p->left);
		stoarg(p->right, o);
		callflag++;
		return;
	case COMOP:
		markcall(p->right);
		if (p->right->su > fregs || p->right->fsu > ffregs) {
			stotree = p;
			stocook = INTEMP_;
		}
		store(p->left);
		return;
	case ANDAND:
	case OROR:
	case QUEST:
		markcall(p->right);
		if (p->right->su > fregs || p->right->fsu > ffregs) {
			stotree = p;
			stocook = INTEMP_;
		}
		constore(p->left);
		return;
	case NOT:
	case CBRANCH:
		constore(p->left);
		return;
	case MUL:
	case ASG MUL:
	case DIV:
	case ASG DIV:
	case MOD:
	case ASG MOD:
		/* the long ones are calls of a library function */
		if (p->left->type == 5 || p->left->type == 15)
			if (lmuldiv++) {
				stotree = p;
				stocook = INTEMP_;
			}
		break;
	}
	if (ty == UTYPE) {
		store(p->left);
		return;
	}
	if (dope[p->right->op] & 1)
		stoasg(p->right, o);
	if (p->su > fregs || p->fsu > ffregs)
		mkadrs(p);
	store(p->right);
	store(p->left);
}

/* store conditional expressions: not inside a conditional context */
void
constore(p)
	register struct node *p;
{
	switch (p->op) {
	case QUEST:
	case ANDAND:
	case OROR:
		markcall(p->right);
	case NOT:
		constore(p->left);
		return;
	}
	store(p);
}

/* count the calls below p */
void
markcall(p)
	register struct node *p;
{
again:
	switch (p->op) {
	case CALL:
	case UCALL:
	case FCALL:
	case UFCALL:
	case STCALL:
	case USTCALL:
		callflag++;
		return;
	}
	switch (dope[p->op] & 0xE) {
	case BITYPE:
		markcall(p->right);
	case UTYPE:
		p = p->left;
		goto again;
	case LTYPE:
		return;
	}
}

/* store the arguments of a call */
void
stoarg(p, calltype)
	register struct node *p;
	int calltype;
{
	if (p->op == CM) {
		stoarg(p->left, calltype);
		p = p->right;
	}
	callflag = 0;
	store(p);
	if (callflag) {
		stotree = p;
		stocook = INTEMP_;
		store(p);
	}
}

/* evaluate p for the truth value and jump to true or false; a label < 0 means to fall through */
void
cbranch(p, true, false)
	struct node *p;
	int true, false;
{
	register int o;
	int lab, flab, tlab, t;
	struct node *c;

	lab = -1;
	switch (o = p->op) {
	case EQ:
	case NE:
	case LE:
	case LT:
	case GE:
	case GT:
	case ULE:
	case ULT:
	case UGE:
	case UGT:
	case 90:
	case 91:
	case 92:
	case 93:
	case 94:
	case 95:
		if (true < 0) {
			o = p->op = negrel[o - EQ];
			true = false;
			false = -1;
		}
		c = p->right;
		if (c->op == ICON && c->lval == 0 && c->sname == 0) {
			if (o == ULT || o == UGE) {
				if (o == UGE)
					cbgen(0, true, 'I');
				goto uncond;
			}
			if (o == UGT || o == ULE)
				o = p->op = o == UGT ? NE : EQ;
		}
		p->label = true;
		codgen(p, FORCC);
	uncond:
		if (false >= 0)
			cbgen(0, false, 'I');
		reclaim(p, 0, 0);
		return;
	case ANDAND:
		lab = false < 0 ? getlab() : false;
		cbranch(p->left, -1, lab);
		cbranch(p->right, true, false);
		if (false < 0)
			deflab(lab);
		p->op = FREE;
		rfreeall(1);
		return;
	case OROR:
		lab = true < 0 ? getlab() : true;
		cbranch(p->left, lab, -1);
		cbranch(p->right, true, false);
		if (true < 0)
			deflab(lab);
		p->op = FREE;
		rfreeall(1);
		return;
	case QUEST:
		flab = false < 0 ? getlab() : false;
		tlab = true < 0 ? getlab() : true;
		lab = getlab();
		cbranch(p->left, -1, lab);
		cbranch(p->right->left, tlab, flab);
		deflab(lab);
		cbranch(p->right->right, true, false);
		if (true < 0)
			deflab(tlab);
		if (false < 0)
			deflab(flab);
		p->right->op = FREE;
		p->op = FREE;
		return;
	case ASSIGN:
		/* x = constant: the truth value is known */
		if (p->right->op == ICON) {
			c = tcopy(p->right);
			order(p, FOREFF);
			goto icon;
		}
		break;
	case COMOP:
		codgen(p->left, FOREFF);
		p->op = FREE;
		cbranch(p->right, true, false);
		return;
	case NOT:
		cbranch(p->left, false, true);
		p->op = FREE;
		return;
	case ICON:
		c = p;
	icon:
		if (c->lval != 0 || c->sname != 0) {
			if (true >= 0)
				cbgen(0, true, 'I');
		} else if (false >= 0)
			cbgen(0, false, 'I');
		c->op = FREE;
		return;
	}
	t = p->type;
	codgen(p, FORCC);
	if (t == 6 || t == 7) {
		if (true >= 0)
			cbgen(FNE, true, 'I');
		if (false >= 0)
			cbgen(true >= 0 ? 0 : FEQ, false, 'I');
	} else {
		if (true >= 0)
			cbgen(NE, true, 'I');
		if (false >= 0)
			cbgen(true >= 0 ? 0 : EQ, false, 'I');
	}
	reclaim(p, 0, 0);
}

/* the fields that are not on the left of an assignment become mask and shift */
void
ffld(p, down, down1, down2)
	register struct node *p;
	int down;
	int *down1, *down2;
{
	register int ty, s, o;
	int v;
	register struct node *shp;

	*down1 = dope[p->op] & 1;
	*down2 = 0;
	if (down || p->op != FLD)
		return;
	ty = szty(p->type) == 2 ? 5 : 4;
	v = p->rval;
	s = v & 0x3F;
	o = szty(p->type) * 16 - s - (v >> 6);
	p->left->type = ty;
	p->op = AND;
	p->right = talloc();
	p->right->op = ICON;
	p->right->rall = NOPREF;
	p->right->type = ty;
	p->right->lval = 1;
	p->right->rval = 0;
	p->right->sname = 0;
	p->right->lval <<= s;
	p->right->lval--;
	if (o != 0) {
		shp = talloc();
		shp->op = RS;
		shp->rall = NOPREF;
		shp->type = ty;
		shp->left = p->left;
		shp->right = talloc();
		shp->right->op = ICON;
		shp->right->rall = NOPREF;
		shp->right->type = ty;
		shp->right->rval = 0;
		shp->right->lval = o;
		shp->right->sname = 0;
		p->left = shp;
	}
}

/* an indirection of a register plus a constant becomes an OREG */
void
oreg2(p)
	register struct node *p;
{
	register struct node *q, *ql, *qr;
	register int r;
	struct sym *cp;
	long temp;
	int stw;

	if (p->op != STAR)
		return;
	q = p->left;
	if (q->op == REG) {
		temp = q->lval;
		r = q->rval;
		stw = 0;
		cp = 0;
	} else {
		if (q->op != PLUS && q->op != MINUS)
			return;
		ql = q->left;
		qr = q->right;
		if (ql->op == REG && qr->op == ICON) {
			temp = qr->lval;
			if (q->op == MINUS)
				temp = -temp;
			r = ql->rval;
			stw = 0;
			cp = qr->sname;
		} else if (q->op == PLUS && ql->op == ICON && qr->op == REG) {
			if (ql->rval >= 0)
				stw = 0;
			else if (ql->sname && ql->sname->sclass == 3 && !(ql->sname->sflags >> 2 & 1))
				stw = -(int)ql->sname->soffset;
			else
				stw = ql->rval;
			temp = ql->lval;
			r = qr->rval;
			cp = ql->sname;
		} else
			return;
	}
	if (notoff(p->type, r, temp, cp))
		return;
	p->op = OREG;
	p->rval = r;
	p->lval = temp;
	p->stsize = stw;
	p->sname = cp;
	tfree(q);
}

/* put p in canonical form: fields, OREG nodes, register need */
struct node *
canon(p)
	struct node *p;
{
	fwalkd(p, ffld, 0);
	fwalk(p, oreg2);
	fwalk(p, mycanon);
	fwalk(p, sucomp);
	return p;
}

/* a structure argument is stored through a temporary: STARG becomes an STASG to the stack */
struct node *
argprep(p, cookie)
	struct node *p;
	int cookie;
{
	struct node *copy, *n;
	struct optab fake;

	if (p->op != STARG || cookie != INTEMP_)
		return p;
	copy = tcopy(p->left);
	fwalk(p->left, rfreenode);
	tfree(p->left);
	n = talloc();
	p->left = n;
	ncopy(n, p);
	p->left->op = STASG;
	p->left->type = INCREF(p->left->type);
	p->left->right = copy;
	fake.needs = 0x100;
	fake.nx = 0x20;
	if (!allo(p, &fake))
		cerror("bad STASG allocation");
	resc[0].type = DECREF(p->left->type);
	p->left->left = tcopy(&resc[0]);
	return p->left;
}

/*
 * The "delayed" evaluation of a statement tree (reader3.c, 0x0838 .. 0x0aa8): comma operators are done first, then
 * the tree without the ++ and -- (up to 20 of them), then the increments. A returned value of a RET is kept in a
 * temporary while the increments run.
 */
#define NDELAY	20

extern long freetemp();
extern void walkd();
extern int tbytes();
extern void canon2();
extern void fixmap(), mapregs();	/* 0x20c0: map the symbols of a tree, 0x9512 */
int ndelay;			/* 0x69de */
struct node *deltrees[NDELAY];	/* 0x8f9a */

static void
delstrip(p)
	register struct node *p;
{
	register struct node *q;

	if (p->op == INCR || p->op == DECR) {
		p->right->op = FREE;
		q = p->left;
		ncopy(p, q);
		q->op = FREE;
	}
}

static void
delay2(p)
	register struct node *p;
{
	register struct node *q;
	register int ty;

	switch (p->op) {
	case QUEST:
	case ANDAND:
	case OROR:
	case COMOP:
	case CALL:
	case UCALL:
	case FCALL:
	case UFCALL:
	case NOT:
	case STCALL:
	case USTCALL:
		return;
	case INCR:
	case DECR:
		if (ndelay < NDELAY) {
			q = tcopy(p);
			fwalk(q->left, delstrip);
			fwalk(q->right, delstrip);
			deltrees[ndelay++] = q;
			q = p->left;
			p->right->op = FREE;
			ncopy(p, q);
			q->op = FREE;
		}
		break;
	case CBRANCH:
		return;
	}
	ty = dope[p->op] & 0xE;
	if (ty == BITYPE)
		delay2(p->right);
	if (ty != LTYPE)
		delay2(p->left);
}

/* look for a COMOP that can be done first: 1 if a rewrite happened */
static int
delay1(p)
	register struct node *p;
{
	register struct node *q;
	register int ty;

	ty = dope[p->op] & 0xE;
	if (ty == LTYPE)
		return 0;
	if (ty == UTYPE)
		return delay1(p->left);
	switch (p->op) {
	case QUEST:
	case ANDAND:
	case OROR:
		return delay1(p->left);
	case COMOP:
		delay(p->left);
		q = p->right;
		ncopy(p, p->right);
		q->op = FREE;
		return 1;
	}
	return delay1(p->left) || delay1(p->right);
}

/* a statement tree */
void
delay(p)
	register struct node *p;
{
	register int ret, sz;
	long off;
	int i;

	ret = 0;
	sz = 0;
	ndelay = 0;
	if (p->op == RET) {
		switch (p->type) {
		case 6:
		case 7:
		case 8:
		case 9:
			break;
		default:
			ret = 1;
			sz = szty(p->type);
			off = freetemp(sz) / 8;
		}
	}
	fwalkd(p, fixmap, 0);
	fwalk(p, mapregs);
	canon2(p);
	while (delay1(p))
		;
	ndelay = 0;
	delay2(p);
	codgen(p, FOREFF);
	if (ret && ndelay) {
		printf("/* delay */\n");
		if (sz == 2) {
			printf("\tldl\t~L1+%ld(fp),rr2\n", off);
			rfree(2, 5);
		} else {
			printf("\tld\t~L1+%ld(fp),r2\n", off);
			rfree(2, 4);
		}
	}
	for (i = 0; i < ndelay; i++)
		codgen(deltrees[i], FOREFF);
	if (ret && ndelay) {
		printf("/* delay */\n");
		if (sz == 2)
			printf("\tldl\trr2,~L1+%ld(fp)\n", off);
		else
			printf("\tld\tr2,~L1+%ld(fp)\n", off);
	}
}
