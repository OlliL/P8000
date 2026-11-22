# include "mfile1"

static char sccsid[] __attribute__((unused)) = "@[$]optim1.c\t2.1  11/15/82 13:10:49 Copr. 1981 87wega3.2.";
static char sccsid2[] __attribute__((unused)) = "@[$]optim\t2.5  10/06/83 10:17:28 Copr. 1981 87wega3.2.";

# define SWAP(p,q) {sp=p; p=q; q=sp;}
# define RCON(p) (p->in.right->in.op==ICON)
# define RO(p) p->in.right->in.op
# define RV(p) p->in.right->tn.lval
# define LCON(p) (p->in.left->in.op==ICON)
# define LO(p) p->in.left->in.op
# define LV(p) p->in.left->tn.lval

extern short revrel[];

NODE *fortarg(NODE *p)
{
	/* fortran function arguments */

	if( p->in.op == CM ){
		p->in.left = fortarg( p->in.left );
		p->in.right = fortarg( p->in.right );
		return(p);
		}

	while( ISPTR(p->in.type) ){
		p = buildtree( UNARY MUL, p, NIL );
		}
	return( optim(p) );
	}

NODE *optim(NODE *p)
{
	/* local optimizations, most of which are probably machine independent */

	int o, ty;
	register NODE *a, *b;
	int i;
	NODE *sp;
	TWORD t;

	if( (t=BTYPE(p->in.type))==ENUMTY || t==MOETY ) econvert(p);
	ty = optype( o=p->in.op);
	if( ty == LTYPE ) return(p);

	if( ty == BITYPE ) p->in.right = optim(p->in.right);
	p->in.left = optim(p->in.left);

	/* collect constants */

	switch(o){

	case SCONV:
	case PCONV:
		return( clocal(p) );

	case FORTCALL:
		p->in.right = fortarg( p->in.right );
		break;

	case UNARY MUL:
		if( LO(p) != ICON ) break;
		LO(p) = NAME;

		/* paint over the type of the left hand side with the type of the top */
		p->in.left->in.type = p->in.type;
		p->in.left->fn.cdim = p->fn.cdim;
		p->in.left->fn.csiz = p->fn.csiz;
		p->in.op = FREE;
		return( p->in.left );

	case MINUS:
		/* WEGA: (x+a)-(x+b) is a-b, (x-a)-(x-b) is b-a, x-x is 0 */
		if( LO(p) == PLUS && RO(p) == PLUS && treeeq( p->in.left->in.left, p->in.right->in.left ) &&
				p->in.left->in.right->in.type == p->in.type && p->in.right->in.right->in.type == p->in.type ){
			a = (NODE *) tcopy( p->in.left->in.right );
			b = (NODE *) tcopy( p->in.right->in.right );
			goto rebuild;
			}
		goto minus1;
	minus2:
		if( RO(p) == MINUS && treeeq( p->in.left->in.left, p->in.right->in.left ) &&
				p->in.left->in.right->in.type == p->in.type && p->in.right->in.right->in.type == p->in.type ){
			b = (NODE *) tcopy( p->in.left->in.right );
			a = (NODE *) tcopy( p->in.right->in.right );
		rebuild:
			tfree( p->in.left );
			tfree( p->in.right );
			p->in.left = a;
			p->in.right = b;
			p = clocal( p );
		minus1:
			if( LO(p) == MINUS ) goto minus2;
			}
		if( treeeq( p->in.left, p->in.right ) && p->in.type != FLOAT && p->in.type != DOUBLE ){
			tfree( p->in.left );
			tfree( p->in.right );
			p->in.op = ICON;
			p->tn.rval = NONAME;
			p->tn.lval = 0;
			p = clocal( p );
			break;
			}
		if( !nncon(p->in.right) ) break;
		RV(p) = -RV(p);
		o = p->in.op = PLUS;

	/* fall through */
	case MUL:
	case PLUS:
	case AND:
	case OR:
	case ER:
		/* commutative ops; for now, just collect constants */
		/* someday, do it right */
		if( nncon(p->in.left) || ( LCON(p) && !RCON(p) && !( RO(p) == REG && p->in.right->tn.rval == 15 ) &&
				!ISPTR(p->in.type) ) ) SWAP( p->in.left, p->in.right );
		if( (o == MINUS || o == PLUS) && nncon(p->in.right) && RV(p) == 0 ) goto zapright;
		/* make ops tower to the left, not the right */
		if( RO(p) == o && !ISPTR(p->in.type) && !ISPTR(p->in.right->in.left->in.type) ){
			NODE *t1, *t2, *t3;
			t1 = p->in.left;
			sp = p->in.right;
			t2 = sp->in.left;
			t3 = sp->in.right;
			/* now, put together again */
			p->in.left = sp;
			sp->in.left = t1;
			sp->in.right = t2;
			p->in.right = t3;
			}
		if(o == PLUS && LO(p) == MINUS && RCON(p) && RCON(p->in.left) &&
		  conval(p->in.right, MINUS, p->in.left->in.right)){
			zapleft:
			RO(p->in.left) = FREE;
			LO(p) = FREE;
			p->in.left = p->in.left->in.left;
			}
		if( RCON(p) && LO(p)==o && RCON(p->in.left) && conval( p->in.right, o, p->in.left->in.right ) ) goto zapleft;
		if( ISPTR(p->in.type) && LCON(p) && RO(p) == o && RCON(p->in.right) && conval( p->in.left, o, p->in.right->in.right ) ){
			p->in.right->in.right->in.op = FREE;
			RO(p) = FREE;
			p->in.right = p->in.right->in.left;
			}
		else if( LCON(p) && RCON(p) && conval( p->in.left, o, p->in.right ) ){
		zapright:
			RO(p) = FREE;
			p->in.left = makety( p->in.left, p->in.type, p->fn.cdim, p->fn.csiz );
			p->in.op = FREE;
			return( clocal( p->in.left ) );
			}

		/* change muls to shifts */

		if( o==MUL && nncon(p->in.right) && (i=ispow2(RV(p)))>=0){
			if( i == 0 ){ /* multiplication by 1 */
				goto zapright;
				}
			o = LS;
			p->in.right->in.type = p->in.right->fn.csiz = INT;
			RV(p) = i;
			p->in.op = FREE;
			p = buildtree( o, p->in.left, p->in.right );
			}

		/* change +'s of negative consts back to - */
		if( o==PLUS && nncon(p->in.right) && RV(p)<0 ){
			RV(p) = -RV(p);
			o = p->in.op = MINUS;
			}
		break;

	case DIV:
		if( nncon( p->in.right ) && p->in.right->tn.lval == 1 ) goto zapright;
	/* fall through */
	case ASG DIV:
	case MOD:
	case ASG MOD:	/* WEGA: unsigned by a power of 2 as a shift or an and */
		a = p->in.right;
		if( ISUNSIGNED(p->in.type) && nncon(a) && ispow2(a->tn.lval) > 0 ){
			if( o == MOD || o == ASG MOD ){
				p->in.op = (o == ASG MOD) ? ASG AND : AND;
				a->tn.lval -= 1;
				a->in.type = p->in.type = p->in.left->in.type;
				}
			else {
				p->in.op = (o == ASG DIV) ? ASG RS : RS;
				a->tn.lval = ispow2( a->tn.lval );
				if( p->in.op == RS && p->in.left->in.type != p->in.type )
					p->in.left = makety( p->in.left, p->in.type, p->fn.cdim, p->fn.csiz );
				}
			}
		break;

	case ASG PLUS:
	case ASG MINUS:
		if( nncon(p->in.right) && RV(p) == 0 ) goto zapright;
		break;

	case EQ:
	case NE:
	case LT:
	case LE:
	case GT:
	case GE:
	case ULT:
	case ULE:
	case UGT:
	case UGE:
		if( !LCON(p) ) break;

		/* exchange operands */

		sp = p->in.left;
		p->in.left = p->in.right;
		p->in.right = sp;
		p->in.op = revrel[p->in.op - EQ ];
		break;

		}

	return(p);
	}

int conval(NODE *p, int o, NODE *q)
{
	/* apply the op o to the lval part of p; if binary, rhs is val */
	int i, u;
	CONSZ val;

	val = q->tn.lval;
	u = ISUNSIGNED(p->in.type) || ISUNSIGNED(q->in.type);
	if( u && (o==LE||o==LT||o==GE||o==GT)) o += (UGE-GE);

	if( !nncon(p) && !nncon(q) ) return(0);
	if( !nncon(q) && o!=PLUS ) return(0);
	if( !nncon(p) && o!=PLUS && o!=MINUS ) return(0);

	switch( o ){

	case PLUS:
#ifdef SEG
		if( ISPTR(p->in.type) ){
			p->tn.lval = ((val & 0x7f00ffffL) + p->tn.lval) & 0x7f00ffffL;
			break;
			}
#endif
		p->tn.lval += val;
		if( nncon(p) ){
			p->in.type = q->in.type;
			p->tn.rval = q->tn.rval;
			}
		break;
	case MINUS:
#ifdef SEG
		if( ISPTR(p->in.type) ) p->tn.lval = (p->tn.lval - val) & 0xffL;
		else p->tn.lval -= val;
#else
		p->tn.lval -= val;
#endif
		break;
	case MUL:
		p->tn.lval *= val;
		break;
	case DIV:
		if( val == 0 ) uerror( "division by 0" );
		else p->tn.lval /= val;
		break;
	case MOD:
		if( val == 0 ) uerror( "division by 0" );
		else p->tn.lval %= val;
		break;
	case AND:
		p->tn.lval &= val;
		break;
	case OR:
		p->tn.lval |= val;
		break;
	case ER:
		p->tn.lval ^=  val;
		break;
	case LS:
		i = val;
		p->tn.lval = p->tn.lval << i;
		break;
	case RS:
		i = val;
		p->tn.lval = p->tn.lval >> i;
		break;

	case UNARY MINUS:
		p->tn.lval = - p->tn.lval;
		break;
	case COMPL:
		p->tn.lval = ~p->tn.lval;
		break;
	case NOT:
		p->tn.lval = !p->tn.lval;
		break;
	case LT:
		p->tn.lval = p->tn.lval < val;
		break;
	case LE:
		p->tn.lval = p->tn.lval <= val;
		break;
	case GT:
		p->tn.lval = p->tn.lval > val;
		break;
	case GE:
		p->tn.lval = p->tn.lval >= val;
		break;
	case ULT:
		p->tn.lval = (p->tn.lval-val)<0;
		break;
	case ULE:
		p->tn.lval = (p->tn.lval-val)<=0;
		break;
	case UGE:
		p->tn.lval = (p->tn.lval-val)>=0;
		break;
	case UGT:
		p->tn.lval = (p->tn.lval-val)>0;
		break;
	case EQ:
		p->tn.lval = p->tn.lval == val;
		break;
	case NE:
		p->tn.lval = p->tn.lval != val;
		break;
	default:
		return(0);
		}
	return(1);
	}

int treeeq(NODE *p, NODE *q)
{ /* are the trees p and q equal */
	int o;

	if( (o = p->in.op) != q->in.op ) return( 0 );
	switch( optype(o) ){

	case LTYPE:
		return( p->tn.lval == q->tn.lval && p->tn.rval == q->tn.rval );

	case BITYPE:
		if( !treeeq( p->in.right, q->in.right ) ) return( 0 );
		/* fall through */
	case UTYPE:
		return( treeeq( p->in.left, q->in.left ) );
		}
	return( 0 );
	}

NODE *rmconv(NODE *p, char c)
{ /* drop a conversion that changes nothing: c 'r' of the right side, 'l' of the left side, else of p */
	register NODE *q, *r;

	if( c == 'r' ){
		q = p->in.right;
		if( q->in.type == q->in.left->in.type || ( bsize( q->in.type ) == bsize( q->in.left->in.type ) &&
				!( notflt( q->in.type ) ^ notflt( q->in.left->in.type ) ) ) ||
				( q->in.type == DOUBLE && q->in.left->in.type == FLOAT ) ){
			r = (NODE *) tcopy( q->in.left );
			tfree( q );
			p->in.right = r;
			}
		}
	else if( c == 'l' ){
		q = p->in.left;
		if( q->in.type == q->in.left->in.type || ( bsize( q->in.type ) == bsize( q->in.left->in.type ) &&
				!( notflt( q->in.type ) ^ notflt( q->in.left->in.type ) ) ) ||
				( q->in.type == DOUBLE && q->in.left->in.type == FLOAT ) ){
			r = (NODE *) tcopy( q->in.left );
			tfree( q );
			p->in.left = r;
			}
		}
	else if( p->in.type == UNDEF ){
		r = (NODE *) tcopy( p->in.left );
		tfree( p );
		p = r;
		}
	return( p );
	}

int notflt(TWORD t)
{ /* is t not a floating type */
	return( t == FLOAT || t == DOUBLE ? 0 : 1 );
	}
