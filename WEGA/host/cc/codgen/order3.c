/*
 * order3.c - the order of evaluation: Sethi-Ullman numbers, rewrites, arguments
 */
#include "mfile3"

static int orderadd(NODE *p);
static int lmuldiv(NODE *p);
static NODE *argscan(NODE *p);
static void argsort(void);
static void fixstarg(NODE *p);
static void argstrip(NODE *p, NODE *rest);
static void argplace(void);

extern int argfreg;
extern struct arg argl[];
extern int crslab;
extern int nargs;
extern int ordflag;
static char sccsid[] __attribute__((unused)) = "@[$]order3.c\t2.4  08/24/83 13:27:43 Copr. 1981 87wega3.2.";
int fltused = 0;

#define FREE_	100

#ifdef SEG
#define OFFSTARL(l)	offstar(l)
#else
#define OFFSTARL(l)	offstar((l)->in.left)
#endif

#define OFFSTARA(l)	offstar((l)->in.left)

#define max(a, b)	((a) > (b) ? (a) : (b))

#define min(a, b)	((a) < (b) ? (a) : (b))

#ifdef OLD_D3K
/* v0: not the register of a parameter (it is copied before an operator changes it) */
#define istnode(q)	((q)->in.op == REG && istreg((q)->tn.rval) && !((q)->in.sname && (q)->in.sname->sclass == PARAM))
#else
#define istnode(q)	((q)->in.op == REG && istreg((q)->tn.rval))
#endif

static int orderadd();
static int lmuldiv();
static NODE *argscan();
static void argsort();
static void fixstarg();
static void argstrip();
static void argplace();

/* an assignment under an indirection: nothing to store, the check only */
int
stoasg(NODE *p, int o)
{
	/* should the assignment op p be stored,
	   given that it lies as the right operand of o
	   (or the left, if o==UNARY MUL) */
	(void) o;
	return( shltype(p->in.left->in.op, p->in.left ) );
	}

/* store the subtree of p with the larger register need: stotree and stocook for codgen */
void
mkadrs(NODE *p)
{
	register int o;

	o = p->in.op;
	if asgop(o) {
		if (p->in.left->in.su > p->in.right->in.su ||
		    (p->in.left->in.su == p->in.right->in.su && p->in.left->in.fsu >= p->in.right->in.fsu)) {
			if (p->in.left->in.op == UNARY MUL) {
				if (p->in.left->in.su > 0 || p->in.left->in.fsu > 0)
					stotree = p->in.left->in.left;
				else if (p->in.right->in.su > 0 || p->in.right->in.fsu > 0)
					stotree = p->in.right;
				else {
					cerror("store finds both sides trivial");
					return;
				}
			} else if (p->in.left->in.op == FLD && p->in.left->in.left->in.op == UNARY MUL)
				stotree = p->in.left->in.left->in.left;
			else	/* should be only structure assignment */
				stotree = p->in.left;
		} else
			stotree = p->in.right;
	} else {
		if (p->in.left->in.su > p->in.right->in.su || (p->in.left->in.su == p->in.right->in.su && p->in.left->in.fsu > p->in.right->in.fsu))
			stotree = p->in.left;
		else
			stotree = p->in.right;
	}
	stocook = INTEMP;
}

/* 0x49c0: is the register (rval of a REG node) unusable as the base of an address: r0, or class 1 in the high byte */
int
notoff(int t, int r, long off, void *cp)
{
	/* is it legal to make an OREG or NAME entry which has an
	   offset of off, (from a register of r), if the
	   resulting thing had type t; WEGA: not with r0 as the base */
	(void) t; (void) off; (void) cp;
	if( R2TEST(r) ) return( R2UPK1(r) == R0 );
	return( r == R0 );
	}

void
sucomp(NODE *p)
{
	register int o, ty, sul, sur, nr;
	int fl, fr, nf, t;

	o = p->in.op;
	ty = optype(o);
	if ((t = p->in.type) == FLOAT || t == DOUBLE) {
		nf = 1;
		nr = 0;
	} else {
		nf = 0;
		nr = max(szty(t), dope[p->in.op] & MULFLG ? 2 : 1);
	}
	if callop(o) {			/* a call takes all registers */
		p->in.su = fregs;
		p->in.fsu = ffregs;
		return;
	}
	switch (ty) {
	case LTYPE:
		p->in.su = 0;
		p->in.fsu = 0;
		return;
	case UTYPE:
		if (p->in.op == STARG && (unsigned)p->stn.stsize > 4) {
			p->in.su = max(p->in.left->in.su, fregs - 3);
			p->in.fsu = p->in.left->in.fsu;
			return;
		}
		p->in.su = max(p->in.left->in.su, nr);
		p->in.fsu = max(p->in.left->in.fsu, nf);
#ifdef SEG
		if (p->in.op == UNARY MUL)		/* an indirection through a long pointer needs a register pair */
			p->in.su = max(p->in.su, 2);
#endif
		return;
	case BITYPE:
		sul = p->in.left->in.su;
		fl = p->in.left->in.fsu;
		sur = p->in.right->in.su;
		fr = p->in.right->in.fsu;
		if (o == ANDAND || o == OROR) {		/* && and || */
			p->in.su = max(max(sul, sur), nr);
			p->in.fsu = max(max(fl, fr), nf);
			return;
		}
		if asgop(o) {			/* assignments */
			if (sur == 0) {
				if (sul == 0)
					p->in.su = nr;
				else
					p->in.su = max(sul, nr);
			} else if (sul <= sur && sul + nr > fregs)
				p->in.su = fregs + nr;
			else if (sul == 0)
				p->in.su = max(sur, nr + nr);
			else
				p->in.su = max(sur, max(sul, 1 + nr + nr));
			if (fr == 0) {
				if (fl == 0)
					p->in.fsu = nf;
				else
					p->in.fsu = max(fl, nf);
			}
			else if (fl <= fr && fl + nf > ffregs)
				p->in.fsu = ffregs + nf;
			else if (fl == 0)
				p->in.fsu = max(fr, nf + nf);
			else
				p->in.fsu = max(fr, max(fl, 1 + nf + nf));
		}
		else if ((dope[p->in.op] & MULFLG) && (p->in.left->in.type == LONG || p->in.left->in.type == ULONG)) {	/* long * / % */
			p->in.su = max(fregs - 1, min(max(sul, nr + sur), max(nr + sul, sur)));
			if (sur == sul)
				p->in.fsu = max(ffregs - 1, min(max(fl, nf + fr), max(nf + fl, fr)));
			else
				p->in.fsu = max(fl, fr);
		}
		else if ((dope[p->in.op] & MULFLG) && optype(p->in.left->in.op) == LTYPE && nr > szty(p->in.left->in.type)) {
			p->in.su = max(nr, max(sul, sur));
			if (sur == sul)
				p->in.fsu = max(nf, max(fl, fr));
			else
				p->in.fsu = max(fl, fr);
		}
		else {
		p->in.su = max(nr + nr, min(max(sul, nr + sur), max(sur, nr + sul)));
		if (sur == sul)
			p->in.fsu = max(nf + nf, min(max(fl, nf + fr), max(fr, nf + fl)));
		else
			p->in.fsu = max(nf + nf, max(max(fl, nf + fr), max(fr, nf + fl)));
		}
	}
}

/* make sure the use of p gets done with register r: simulates offstar */
void
mkrall(NODE *p, int r)
{
	/* insure that the use of p gets done with register r; in effect, */
	/* simulate offstar */
	if( p->in.op == FLD ){
		p->in.left->in.rall = p->in.rall;
		p = p->in.left;
		}
	if( p->in.op != UNARY MUL ) return;  /* no more to do */
	p = p->in.left;
	if( p->in.op == UNARY MUL ){
		p->in.rall = r;
		p = p->in.left;
		}
	if( p->in.op == PLUS && p->in.right->in.op == ICON ){
		p->in.rall = r;
		p = p->in.left;
		}
	rallo( p, r );
	}

/* register preferences for the subtrees */
void
rallo(NODE *p, int down)
{
	/* do register allocation */
	register int o, down1, down2, ty;

	if (p->in.type == FLOAT || p->in.type == DOUBLE) {
		fltused++;
		return;
	}
	down2 = NOPREF;
	p->in.rall = down;
	down &= ~MUSTDO;
	down1 = down;
	o = p->in.op;
	ty = optype(o);
	switch (o) {
	case ASSIGN:
		down1 = NOPREF;
		down2 = down;
		break;
	case ANDAND:
	case OROR:
	case CALL:
	case NOT:
	case EQ:
	case NE:
	case LE:
	case LT:
	case GE:
	case GT:
	case STASG:
	case STARG:
		down1 = NOPREF;
		break;
	case MUL:
		if (p->in.left->in.type == LONG || p->in.left->in.type == ULONG) {
			down1 = MUSTDO | R6;
#ifndef OLD_D2U
			if (p->in.right->in.op != REG)
				down2 = MUSTDO | R4;	/* (OLD_D2U: v0 wants no register for the right operand) */
#endif
		}
		break;
	case ASG MUL:
		if (p->in.left->in.type == LONG || p->in.left->in.type == ULONG) {
			down1 = MUSTDO | R6;
			down2 = MUSTDO | R4;
		}
		break;
	case DIV:
	case ASG DIV:
	case MOD:
	case ASG MOD:
		if (p->in.left->in.type == LONG || p->in.left->in.type == ULONG) {
			down1 = MUSTDO | R6;
			down2 = MUSTDO | R2;
		}
#ifdef OLD_D2D
		else if (down == NOPREF)
			down1 = MUSTDO | R1;		/* the odd half of rr0 */
		else if (!(down & 1) && down < 8)
			down1 = MUSTDO | (down + 1);	/* the odd half of the pair of the wanted register */
#endif
		break;
#ifdef OLD_D5T
	case PLUS:
		if ((p->in.rall & MUSTDO) && p->in.right->in.op == REG && p->in.right->tn.rval == FP && p->in.left->in.op == PLUS)
			down1 = NOPREF;	/* an element of a local array as an address: as OLD_D5A */
		break;
#endif
#ifdef OLD_D5A
	case UNARY MUL:
		if ((p->in.rall & MUSTDO) && p->in.left->in.op == PLUS && p->in.left->in.right->in.op == REG && p->in.left->in.right->tn.rval == FP)
			down1 = NOPREF;	/* v0p: the address of a local array element wanted in a given register goes anywhere */
		break;
#endif
	case RET:
		if (p->in.type != FLOAT && p->in.type != DOUBLE)
			down1 = MUSTDO | R2;
		else
			down1 = MUSTDO | F0;
		break;
	}
	if (ty != LTYPE)
		rallo(p->in.left, down1);
	if (ty == BITYPE)
		rallo(p->in.right, down2);
}

#ifdef SEG
/* an indirection through a register plus a constant or a register plus a register becomes an OREG
   (rval: R2PACK(base, index)); 1 when it did */
int
mkoreg(NODE *p)
{
	register NODE *l, *r;
	register struct sym *sym;
	register int rv;
	NODE *q;
	long off;

	if (p->in.op != UNARY MUL)
		return 0;
	q = p->in.left;
	l = q->in.left;
	r = q->in.right;
	if ((q->in.op == PLUS || q->in.op == MINUS) && r->in.op == ICON && l->in.op == REG && p->in.type != FLOAT && p->in.type != DOUBLE) {
		off = r->tn.lval;
		rv = R2BASE(l->tn.rval);
		sym = r->in.sname;
		if (q->in.op == MINUS) {
			if (sym && sym->nameoff)	/* an ICON without symbol: the original reads address 0 (zero) */
				return 0;
			off = -off;
		}
		goto set;
	}
	if (q->in.op == PLUS && r->in.op == REG && l->in.op == REG && p->in.type != FLOAT && p->in.type != DOUBLE) {
		if (r->tn.rval == 0)
			return 0;
		rv = R2PACK(l->tn.rval, r->tn.rval);
		off = 0;
		sym = r->in.sname;
		goto set;
	}
	oreg2(p);
	if (p->in.op != OREG)
		return 0;
	return 1;
set:
	if (R2UPK1(rv) == 0)
		return 0;
	p->in.op = OREG;
	p->tn.rval = rv;
	p->tn.lval = off;
	p->stn.stsize = 0;
	if (sym == symtab0 || sym == fpsym1 || sym == fpsym2)
		p->in.sname = sym;
	else
		p->in.sname = 0;
	tfree(q);
	return 1;

}
#endif

/* order the operands of an address sum that are not yet addressable; returns the number of orders done */
static int
orderadd(NODE *p)
{
	register int n;

	n = 0;
	if (p->in.op == PLUS || p->in.op == MINUS) {
		if (p->in.left->in.op == CALL || p->in.left->in.op == UNARY CALL) {
			if (p->in.right->in.op == CALL || p->in.right->in.op == UNARY CALL) {
				order(p->in.right, INTEMP);
				n++;
				order(p->in.left, INAREG|INTAREG);
				n++;
			} else {
				order(p->in.left, INAREG|INTAREG);
				n++;
				if (p->in.right->in.op != ICON && p->in.right->in.op != REG) {
					order(p->in.right, INAREG|INTAREG);
					n++;
				}
			}
		} else {
			if (p->in.right->in.op != ICON && p->in.right->in.op != REG) {
				order(p->in.right, INAREG|INTAREG);
				n++;
			}
			if (p->in.left->in.op != ICON && p->in.left->in.op != REG) {
				order(p->in.left, INAREG|INTAREG);
				n++;
			}
		}
	}
	return n;
}

#ifdef SEG

/* handle indirections: get the address of *p into a register; p is the UNARY MUL itself or its operand */
void
offstar(NODE *p)
{
	register NODE *l;

	if (p->in.op == UNARY MUL) {
		l = p->in.left;
		if (l->in.op == PLUS || l->in.op == MINUS) {
			orderadd(l);
			if (mkoreg(p)) {
				if (p->in.type == FLOAT || p->in.type == DOUBLE)
					order(p, INBREG|INTBREG);
				else
					order(p, INAREG|INTAREG);
				return;
			}
		}
		if (l->in.op == REG) {
			oreg2(p);
			if (p->in.op == OREG && !(p->in.rall & MUSTDO))
				return;
		}
		if (p->in.rall & MUSTDO) {
			if (optype(p->in.op) != LTYPE) {
				orderadd(l);
				order(l, INAREG|INTAREG);
			}
			if (p->in.type == FLOAT || p->in.type == DOUBLE)
				order(p, INBREG|INTBREG);
			else
				order(p, INAREG|INTAREG);
			return;
		}
		p = l;
	}
	orderadd(p);
	order(p, INAREG|INTAREG);
}
#else
/* handle indirections: get the address of *p into a register */
void
offstar(NODE *p)
{
	register NODE *star;
	register int save;

	star = 0;
	if (p->in.op == UNARY MUL) {
		if (p->in.rall & MUSTDO)
			star = p;
		p = p->in.left;
	}
	if (orderadd(p) == 0) {
		if (p->in.op == REG && p->tn.rval == 0) {
			save = ordflag;
			ordflag = 0x14;
			order(p, INAREG|INTAREG);
			ordflag = save;
		} else
			order(p, INAREG|INTAREG);
	}
	if (star) {
		if (star->in.type == FLOAT || star->in.type == DOUBLE)
			order(star, INBREG|INTBREG);
		else
			order(star, INAREG|INTAREG);
	}
}
#endif

/* is p an indirection that offstar can make addressable */
int
niceuty(NODE *p)
{
	register int t;
	return( p->in.op == UNARY MUL && (t=p->in.type)!=CHAR &&
		t!= UCHAR && t!= FLOAT && t!= DOUBLE &&
		shumul( p->in.left) != STARREG );
	}

/* order the operands of a binary operator; 1 when something was ordered, 0 when the template can be used */
int
setbin(NODE *p)
{
	register NODE *r, *l;

	r = p->in.right;
	l = p->in.left;
	if (p->in.right->in.su == 0 && p->in.right->in.fsu == 0) {	/* rhs is addressable */
		if logop(p->in.op) {
			if (l->in.op == UNARY MUL && shumul(l->in.left) != STARREG)
				OFFSTARL(l);
			else
				order(l, INAREG|INTAREG|INBREG|INTBREG|INTEMP);
			return 1;
		}
		if (!istnode(l)) {
			if (niceuty(l))
				offstar(l);
			else
				order(l, INTAREG|INTBREG);
			return 1;
		}
		/* rewrite */
		return 0;
	}
	/* now, rhs is complicated: must do both sides into registers */
	/* do the harder side first */

	if logop(p->in.op) {
		/* relational: do both sides into regs if need be */
		if (r->in.su > l->in.su || (r->in.su == l->in.su && r->in.fsu >= l->in.fsu)) {
			if (niceuty(r)) {
				OFFSTARA(r);
				return 1;
			}
			if (r->in.su > l->in.su || (r->in.su == l->in.su && r->in.fsu > l->in.fsu)) {
				if (!istnode(r)) {
					order(r, INAREG|INTAREG|INBREG|INTBREG|INTEMP);
					return 1;
				}
			}
		}
		if (niceuty(l)) {
			OFFSTARL(l);
			return 1;
		}
		if (!istnode(l)) {
			order(l, INAREG|INTAREG|INBREG|INTBREG|INTEMP);
			return 1;
		}
		if (niceuty(r)) {
			OFFSTARA(r);
			return 1;
		}
		if (!istnode(r)) {
			order(r, INAREG|INTAREG|INBREG|INTBREG|INTEMP);
			return 1;
		}
		cerror("setbin can't deal with %s", opst[p->in.op]);
	}

	/* ordinary operator */
	if (!istnode(r) && (r->in.su > l->in.su || (r->in.su == l->in.su && r->in.fsu > l->in.fsu))) {
		if (niceuty(r)) {
			OFFSTARA(r);
			return 1;
		}
		order(r, INAREG|INTAREG|INBREG|INTBREG|INTEMP);	/* anything goes on rhs */
		return 1;
	}
	if (!istnode(l)) {
		if (niceuty(l))
			offstar(l);
		else
			order(l, INTAREG|INTBREG);
		return 1;
	}
	return 0;
}

/* structure assignment */
int
setstr(NODE *p)
{ /* structure assignment */
	if( p->in.right->in.op != REG ){
		order( p->in.right, INTAREG );
		return(1);
		}
	p = p->in.left;
	if( p->in.op != NAME && p->in.op != OREG ){
		if( p->in.op != UNARY MUL ) cerror( "bad setstr" );
		order( p->in.left, INTAREG );
		return( 1 );
		}
	return( 0 );
	}

/* setup for the assignment operator */
int
setasg(NODE *p)
{
	/* setup for assignment operator */
	register NODE *l, *r;

	l = p->in.left;
	r = p->in.right;
	if (l->in.su > r->in.su && l->in.op == UNARY MUL) {
		OFFSTARA(l);
		return 1;
	}
	if (r->in.su != 0 && r->in.op != REG) {
		if (r->in.op == UNARY MUL)
			OFFSTARL(r);
		else
			order(r, SNAME|SCON|SOREG|INAREG|INBREG);
		return 1;
	}
	if (r->in.op != REG && (p->in.type == FLOAT || p->in.type == DOUBLE)) {
		order(p->in.right, INBREG|INTBREG);
		return 1;
	}
	if (l->in.op == UNARY MUL) {
		OFFSTARA(l);
		return 1;
	}
	if (l->in.op == FLD && l->in.left->in.op == UNARY MUL) {
		OFFSTARA(l->in.left);
		return 1;
	}
	/* if things are really strange, get rhs into a register */
	if (r->in.op != REG) {
		order(r, INAREG|INBREG);
		return 1;
	}
	return 0;
}

/* ASG MUL, ASG DIV, ASG MOD on a long */
static int
lmuldiv(NODE *p)
{
	switch (p->in.op) {
	case ASG MUL:
	case ASG DIV:
	case ASG MOD:
		if (p->in.left->in.type == LONG || p->in.left->in.type == ULONG)
			return 1;
	/* fall through */
	default:
		return 0;
	}
}

/* setup for the assignment operators (=ops) */
int
setasop(NODE *p)
{
	/* setup for =ops */
	register int sul, sur;
	register NODE *q, *p2;
	register int fl, fr;

	sul = p->in.left->in.su;
	sur = p->in.right->in.su;
	fl = p->in.left->in.fsu;
	fr = p->in.right->in.fsu;
	if (sur == 0 && fr == 0) {
leftadr:
		/* easy case: if addressable, do left value, op, store */
		if (sul == 0 && fl == 0)
			goto rew;	/* rewrite */

		/* harder; make a left address, val, op, and store */
		if (p->in.left->in.op == UNARY MUL) {
			if (lmuldiv(p)) {
#ifdef SEG
				p->in.left->in.left->in.rall = MUSTDO|R2;
				p->in.left->in.left->in.left->in.rall = MUSTDO|R2;
#else
				p->in.left->in.left->in.rall = MUSTDO|R1;
				p->in.left->in.left->in.left->in.rall = MUSTDO|R1;
#endif
			}
			OFFSTARA(p->in.left);
			return 1;
		}
		if (p->in.left->in.op == FLD && p->in.left->in.left->in.op == UNARY MUL) {
			if (lmuldiv(p)) {
#ifdef SEG
				p->in.left->in.left->in.left->in.rall = MUSTDO|R2;
				p->in.left->in.left->in.left->in.left->in.rall = MUSTDO|R2;
#else
				p->in.left->in.left->in.left->in.rall = MUSTDO|R1;
				p->in.left->in.left->in.left->in.left->in.rall = MUSTDO|R1;
#endif
			}
			OFFSTARA(p->in.left->in.left);
			return 1;
		}
rew:	/* rewrite, accounting for autoincrement and autodecrement */
		q = p->in.left;
		if (q->in.op == FLD)
			q = q->in.left;
		if (q->in.op != UNARY MUL || shumul(q->in.left) != STARREG)
			return 0;	/* let reader.c do it */

		/* mimic code from reader.c */
		p2 = tcopy(p);
		p->in.op = ASSIGN;
		reclaim(p->in.right, RNULL, 0);
		p->in.right = p2;

		/* now, zap INCR on right, ASG MINUS on left */
		if (q->in.left->in.op == INCR) {
			q = p2->in.left;
			if (q->in.op == FLD)
				q = q->in.left;
			if (q->in.left->in.op != INCR)
				cerror("bad incr rewrite");
		} else if (q->in.left->in.op != ASG MINUS)
			cerror(" bad -= rewrite");
		q->in.left->in.right->in.op = FREE;
		q->in.left->in.op = FREE;
		q->in.left = q->in.left->in.left;
		p = canon(p);
		rallo(p, p->in.rall);
		order(p2->in.left, INTAREG|INTBREG);
		order(p2, INTAREG|INTBREG);
		return 1;
	}
	if (p->in.op == ASG MOD || p->in.op == ASG DIV) {
		if (p->in.type == LONG || p->in.type == ULONG) {
#ifdef SEG
			if (sul) {
				p->in.right->in.rall = NOPREF;
				order(p->in.right, INTEMP);
				return 1;
			}
#endif
			order(p->in.right, INTAREG);
			return 1;
		}
	}
	if (sul > sur && p->in.left->in.op == UNARY MUL) {
		if (lmuldiv(p))
			p->in.left->in.left->in.rall = MUSTDO|R2;
		OFFSTARA(p->in.left);
		return 1;
	}
	if (p->in.right->in.op == UNARY MUL) {
		OFFSTARL(p->in.right);
		return 1;
	}
	if (p->in.right->in.op == REG)
		goto leftadr;
	order(p->in.right, INAREG|INBREG);
	return 1;
}

int crslab = 10000;	/* 0x3426 */

int
getlab(void)
{
	return( crslab++ );
	}

void
deflab(int l)
{
	printf( "L%d:\n", l );
	}

void
genargs(NODE *p)
{
	register NODE *rest;

	/* generate code for the arguments */
	nargs = 0;
	argl[nargs].node = 0;
	argreg = R7;
	argfreg = F3;
	argstop = 0;
	rest = argscan(p);
	if (p != rest || (p == rest && argstop == 1))
		argstrip(p, rest);
	argsort();
	argplace();
}

static NODE *
argscan(NODE *p)
{
	register NODE *rest;

	rest = p;
	if (p->in.op == CM) {
		rest = argscan(p->in.left);
		if (argstop) {
		ret:
			return rest;
		}
#ifdef OLD_D22
		if (0) {	/* v0p: a double argument goes by its size (rq4 when it is the first one) */
#else
		if (p->in.right->in.type == FLOAT || p->in.right->in.type == DOUBLE) {
#endif
			if (argfreg < F0) {
				if (argstop)
					goto ret;
				argstop = 1;
				return p->in.right;
			}
		} else if (argreg < R2) {
			if (argstop)
				goto ret;
			argstop = 1;
			return p->in.right;
		}
		rest = p;
		p->in.op = FREE;
		p = p->in.right;
	}
#ifdef OLD_D22
	if (0) {
#else
	if (p->in.type == FLOAT || p->in.type == DOUBLE) {
#endif
		if (argfreg < F0)
			goto stop;
		argl[nargs].reg = argfreg;
		argl[nargs].node = p;
		nargs++;
		argl[nargs].node = 0;
		argfreg--;
		goto ret;
	} else
		goto sw;
def:
	argstop = 1;
	argreg = R1;
retp:
	return p;
c2:
	if (--argreg & 1)
		argreg--;
	if (argreg < R2)
		goto def;
c1:
	argl[nargs].reg = argreg;
	argl[nargs].node = p;
	nargs++;
	argl[nargs].node = 0;
	argreg--;
	goto ret;
stop:
	argstop = 1;
	goto retp;
sw:
	switch (argsize(p)) {
	case 4:
		if (argreg != R7)
			goto def;
		argl[nargs].reg = argreg - 3;
		argl[nargs].node = p;
		nargs++;
		argl[nargs].node = 0;
		argreg -= 4;
		goto ret;
	case 1:
		goto c1;
	case 2:
		goto c2;
	default:
		argstop = 1;
		argreg = R1;
		goto retp;
	}
}

/* the largest need first */
static void
argsort(void)
{
	register struct arg *ai, *aj;
	register int r;
	register NODE *n;

#ifdef OLD_D19A
	/* v0: a constant argument counts as needing less than any other one (it is loaded after them) */
#define ARGSU(q)	((q)->in.op == ICON ? -1 : (q)->in.su)
#else
#define ARGSU(q)	((q)->in.su)
#endif
	for (ai = argl; ai->node; ai++)
		for (aj = ai + 1; aj->node; aj++)
			if (ARGSU(ai->node) < ARGSU(aj->node)) {
				r = aj->reg;
				n = aj->node;
				aj->reg = ai->reg;
				aj->node = ai->node;
				ai->reg = r;
				ai->node = n;
			}
}

/* a structure argument: allocate its place on the stack as for a structure assignment */
static void
fixstarg(NODE *p)
{
	struct optab q;

	q.needs = NPTEMP;
	q.nx = 0x20;
	q.cstring = "/* dummy from fixstarg */\n";
	if (!allo(p, &q))
		cerror("bad STARG allocation");
	resc[0].in.type = STRTY;
	p->in.op = STASG;
	p->in.right = p->in.left;
	p->in.left = tcopy(resc);
	p->in.type = PTR|STRTY;
}

/* remove the arguments that were collected from the tree up to rest and order the first of the rest */
static void
argstrip(NODE *p, NODE *rest)
{
	if (rest == p->in.right) {
		p->in.op = FREE;
		p = p->in.right;
	}
	if (p != rest) {
		if (p->in.op == CM || p->in.op == FREE) {
			argstrip(p->in.left, rest);
			p->in.op = FREE;
			p = p->in.right;
		}
	}
	if (p->in.op == STARG) {
		fixstarg(p);
		order(p, FOREFF);
		return;
	}
	if (p->in.op == CM || p->in.op == FREE)
		return;
	if (p->in.op != REG) {
		if (p->in.type == CHAR || p->in.type == UCHAR) {
			order(p, INTAREG);
			if (p->in.type == CHAR)
				printf("\textsb\t%s\n", rnames[p->tn.rval]);
			else
				printf("\tldb\trh%d,#0\n", p->tn.rval);
			p->in.type = INT;
		}
	}
	order(p, FORARG);
}

#ifdef OLD_D3AW
/* forget the registers of the parameters in p that are the whole tree (top) or under an indirection, when they lie in
   the registers lo .. hi the argument is wanted in */
static int d3awlo, d3awhi;

static void
d3aw(NODE *p, int top)
{
	register struct rs *n;
	register int k;

	if (p->in.op == NAME && p->in.sname && p->in.sname->sclass == PARAM) {
		if (top)
			for (n = rsuse; n; n = n->next)
				if (n->cls <= RCFLOAT && n->sym == p->in.sname && n->reg <= d3awhi &&
				    n->reg + (n->cls == RCPAIR) >= d3awlo)
					n->sym = 0;
		return;
	}
	k = optype(p->in.op);
	if (k == LTYPE || p->in.op == QUEST)
		return;
	if (p->in.op == UNARY MUL)
		top = 1;		/* below an indirection: an address */
	else if (p->in.op != PLUS && p->in.op != MINUS && p->in.op != UNARY MINUS && p->in.op != MUL && p->in.op != SCONV)
		top = 0;		/* below another operator: a computed value */
	d3aw(p->in.left, top);
	if (k == BITYPE)
		d3aw(p->in.right, top);
}
#endif

#ifdef OLD_D22
/* v0p: a double argument into the CPU registers reg .. reg+3: from memory with ldm, else through the EPU */
static void
d22arg(NODE *q, int reg)
{
	if (q->in.op == NAME)
		oregfix(q);
	if (q->in.op == NAME || q->in.op == OREG) {
		printf("\tldm\tr%d,", reg);
		adrput(q);
		printf(",#4\n");
		recl2(q);
	} else {
		order(q, INTBREG);
		printf("\tfldd\tr%d,%s\n", reg, rnames[q->tn.rval]);
		rfree(q->tn.rval, q->in.type);
	}
	getreg(RCPAIR, reg, 0, (struct sym *)0);
	getreg(RCPAIR, reg + 2, 0, (struct sym *)0);
}
#endif

/* evaluate the arguments that go to registers; the ones that need much come first */
static void
argplace(void)
{
	register NODE *q;
	register int nleft, fleft, regs, i;
	int reg;
	int cnt[4];

	regs = 3;
	for (i = 1; i < 4; i++)
		cnt[i] = 0;
	nargs = 0;
	q = argl[nargs].node;
	if (q == 0)
		return;
	nleft = 8;
	fleft = 4;
	for (; (q = argl[nargs].node) != 0; nargs++) {
		if (nleft < q->in.su || fleft < q->in.fsu || (regs < 1 && q->in.su > 1)) {
			order(q, INTEMP);
			continue;
		}
#ifdef OLD_D22
		if ((q->in.type == DOUBLE || q->in.type == FLOAT) && argl[nargs].reg < R8) {
			nleft -= 4;
			continue;
		}
#endif
		if (q->in.type == DOUBLE || q->in.type == FLOAT) {
			fleft--;
			continue;
		}
		nleft -= argsize(q);
		regs = 3;
		++cnt[argl[nargs].reg / 2];
		for (i = 1; i < 4; i++)
			if (cnt[i])
				regs--;
	}
	for (nargs = 0; (q = argl[nargs].node) != 0; nargs++) {
		reg = argl[nargs].reg;
#ifdef OLD_D22
		if ((q->in.type == FLOAT || q->in.type == DOUBLE) && reg < R8) {
			d22arg(q, reg);
			tfree(q);
			continue;
		}
#endif
		q->in.rall = reg | MUSTDO;
#ifdef OLD_D3AW
		/* v0p: a parameter that is a whole argument or is used as an address in it is not taken from its
		   register (only there; in a computed value it is) */
		d3awlo = reg;
		d3awhi = reg + szty(q->in.type) - 1;
		d3aw(q, 1);
#endif
		if (q->in.type == FLOAT || q->in.type == DOUBLE)
			order(q, INTBREG);
		else
			order(q, INTAREG);
		if (q->in.type == CHAR)
			printf("\textsb\t%s\n", rnames[reg]);
		else if (q->in.type == UCHAR)
			printf("\tldb\trh%d,#0\n", reg);
		tfree(q);
	}
}
