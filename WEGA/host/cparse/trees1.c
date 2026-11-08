# include "mfile1"

static char sccsid[] __attribute__((unused)) = "@[$]trees1.c\t2.8  08/24/83 15:07:35 Copr. 1981 87wega3.2.";

/*	some special actions, used in finding the type of nodes */
# define NCVT 01
# define PUN 02
# define TYPL 04
# define TYPR 010
# define TYMATCH 040
# define LVAL 0100
# define CVTO 0200
# define CVTL 0400
# define CVTR 01000
# define PTMATCH 02000
# define OTHER 04000
# define NCVTR 010000

/* node conventions:

	NAME:	rval>0 is stab index for external
		rval<0 is -inlabel number
		lval is offset in bits
	ICON:	lval has the value
		rval has the STAB index, or - label number,
			if a name whose address is in the constant
		rval = NONAME means no name
	REG:	rval is reg. identification cookie

	*/






















/* 	the intent of this table is to examine the
	operators, and to check them for
	correctness.

	The table is searched for the op and the
	modified type (where this is one of the
	types INT (includes char and short), LONG,
	DOUBLE (includes FLOAT), and POINTER

	The default action is to make the node type integer

	The actions taken include:
		PUN	  check for puns
		CVTL	  convert the left operand
		CVTR	  convert the right operand
		TYPL	  the type is determined by the left operand
		TYPR	  the type is determined by the right operand
		TYMATCH	  force type of left and right to match, by inserting conversions
		PTMATCH	  like TYMATCH, but for pointers
		LVAL	  left operand must be lval
		CVTO	  convert the op
		NCVT	  do not convert the operands
		OTHER	  handled by code
		NCVTR	  convert the left operand, not the right...

	*/

# define MINT 01  /* integer */
# define MDBI 02   /* integer or double */
# define MSTR 04  /* structure */
# define MPTR 010  /* pointer */
# define MPTI 020  /* pointer or integer */
# define MENU 040 /* enumeration variable or member */
# define MVOID 0 /* WEGA: void */








# ifdef STDPRTREE
# ifndef ONEPASS


# else


# endif
# endif

NODE *buildtree(int o, NODE *l, NODE *r)
{
	register NODE *p, *q;
	int actions;
	int opty;
	register struct symtab *sp;
	NODE *lr, *ll;
	intptr_t i;
	TWORD t;

	opty = optype(o);

	/* check for constants */

	if( opty == UTYPE && l->in.op == ICON ){

		switch( o ){

		case NOT:
		case UNARY MINUS:
		case COMPL:
			if( conval( l, o, l ) ) return(l);
			break;

			}
		}

	else if( o==UNARY MINUS && l->in.op==FCON ){
		l->fpn.dval = -l->fpn.dval;
		return(l);
		}

	else if( o==QUEST && l->in.op==ICON ) {
		r->in.op = FREE;
		l->in.op = FREE;
		if( l->tn.lval ){
			tfree( r->in.right );
			return( r->in.left );
			}
		else {
			tfree( r->in.left );
			return( r->in.right );
			}
		}

	else if( (o==ANDAND || o==OROR) && (l->in.op==ICON||r->in.op==ICON) ) goto ccwarn;

	else if( opty == BITYPE && l->in.op == ICON && r->in.op == ICON ){

		switch( o ){

		case ULT:
		case UGT:
		case ULE:
		case UGE:
		case LT:
		case GT:
		case LE:
		case GE:
		case EQ:
		case NE:
		case ANDAND:
		case OROR:
		case CBRANCH:

		ccwarn:
		case PLUS:
		case MINUS:
		case MUL:
		case DIV:
		case MOD:
		case AND:
		case OR:
		case ER:
		case LS:
		case RS:
			if( conval( l, o, r ) ) {
				r->in.op = FREE;
				return(l);
				}
			break;
			}
		}

	else if( opty == BITYPE && (l->in.op==FCON||l->in.op==ICON) &&
		(r->in.op==FCON||r->in.op==ICON) ){
		switch(o){
		case PLUS:
		case MINUS:
		case MUL:
		case DIV:
			if( l->in.op == ICON ){
				l->fpn.dval = l->tn.lval;
				}
			if( r->in.op == ICON ){
				r->fpn.dval = r->tn.lval;
				}
			l->in.op = FCON;
			l->in.type = l->fn.csiz = DOUBLE;
			r->in.op = FREE;
			switch(o){
			case PLUS:
				l->fpn.dval += r->fpn.dval;
				return(l);
			case MINUS:
				l->fpn.dval -= r->fpn.dval;
				return(l);
			case MUL:
				l->fpn.dval *= r->fpn.dval;
				return(l);
			case DIV:
				if( r->fpn.dval == 0 ) uerror( "division by 0." );
				else l->fpn.dval /= r->fpn.dval;
				return(l);
				}
			}
		}

	/* its real; we must make a new node */

	p = block( o, l, r, INT, 0, INT );

	actions = opact(p);

	if( actions&LVAL ){ /* check left descendent */
		if( notlval(p->in.left) ) {
			uerror( "lvalue required" );
			}
		}

	if( actions & NCVTR ){
		p->in.left = pconvert( p->in.left );
		}
	else if( !(actions & NCVT ) ){
		switch( opty ){

		case BITYPE:
			p->in.right = pconvert( p->in.right );
		/* fall through */
		case UTYPE:
			p->in.left = pconvert( p->in.left );

			}
		}

	if( (actions&PUN) && o!=CAST ){
		chkpun(p);
		}

	if( actions & (TYPL|TYPR) ){

		q = (actions&TYPL) ? p->in.left : p->in.right;

		p->in.type = q->in.type;
		p->fn.cdim = q->fn.cdim;
		p->fn.csiz = q->fn.csiz;
		}

	if( actions & CVTL ) p = convert( p, CVTL );
	if( actions & CVTR ) p = convert( p, CVTR );
	if( actions & TYMATCH ) p = tymatch(p);
	if( actions & PTMATCH ) p = ptmatch(p);

	if( actions & OTHER ){
		l = p->in.left;
		r = p->in.right;

		switch(o){

		case NAME:
			sp = idname;
			if( sp->stype == UNDEF ){
				uerror( "%s undefined", &ntab[sp->sname] );
				/* make p look reasonable */
				p->in.type = p->fn.cdim = p->fn.csiz = INT;
				p->tn.rval = (intptr_t) idname;
				p->tn.lval = 0;
				defid( p, SNULL );
				break;
				}
			p->in.type = sp->stype;
			p->fn.cdim = sp->dimoff;
			p->fn.csiz = sp->sizoff;
			p->tn.lval = 0;
			p->tn.rval = (intptr_t) idname;
			/* special case: MOETY is really an ICON... */
			if( p->in.type == MOETY ){
				p->tn.rval = NONAME;
				p->tn.lval = sp->offset;
				p->fn.cdim = 0;
				p->in.type = ENUMTY;
				p->in.op = ICON;
				}
			break;

		case ICON:
			p->in.type = INT;
			p->fn.cdim = 0;
			p->fn.csiz = INT;
			break;

		case STRING:
			p->in.op = LAB;
			p->in.type = CHAR+ARY;
			p->tn.lval = 0;
			p->tn.rval = NOLAB;
			p->fn.cdim = curdim;
			p->fn.csiz = CHAR;
			break;

		case FCON:
			p->tn.lval = 0;
			p->tn.rval = 0;
			p->in.type = DOUBLE;
			p->fn.cdim = 0;
			p->fn.csiz = DOUBLE;
			break;

		case ANDAND:
		case OROR:
		case NOT:
			p->in.type = p->fn.csiz = INT;
			break;

		case STREF:
			/* p->x turned into *(p+offset) */
			/* rhs must be a name; check correctness */
#ifdef SEG
			p->in.type = INCREF( p->in.right->in.type );
#endif

			sp = STP(i = r->tn.rval);
			if( sp->sclass != MOS && sp->sclass != MOU && !(sp->sclass&FIELD) ){
				uerror( "member of structure or union required" );
				}
			else if( (sp->sflags&SMEMB) && (l->in.type == PTR+STRTY || l->in.type == PTR+UNIONTY) ){
				/* WEGA: a member name of several structures: the member of this one */
				char *cp2, *cp1;
				int j;
				intptr_t k;

				for( j = dimtab[l->fn.csiz+1]; (k = dimtab[j]); ++j ){
					cp1 = &ntab[STP(k)->sname];
					cp2 = &ntab[STP(i)->sname];
					if( (STP(k)->sflags&SMEMB) && !strcmp( cp2, cp1 ) ){
						r->tn.rval = i = k;
						r->in.type = STP(k)->stype;
						p->in.type = INCREF( STP(k)->stype );
						if( !STP(k)->slevel ) refid( STP(k) );
						break;
						}
					}
				}
			else if( l->in.type != PTR+STRTY && l->in.type != PTR+UNIONTY ){
				if( STP(i)->sflags&SMEMB ) uerror( "non-unique name demands struct/union or struct/union pointer" );
				else if( tsize( l->in.type, l->fn.cdim, l->fn.csiz ) != SZPOINT ) uerror( "illegal type of struct/union pointer" );
				else werror( "struct/union or struct/union pointer required" );
				}
			else {
				int j;

				if( (j=l->fn.csiz+1)<0 ) cerror( "undefined structure or union" );
				else if( !chkstr( i, (int)dimtab[j], DECREF(l->in.type) ) ){
					werror( "illegal member use: %s", &ntab[STP(i)->sname] );
					}
				}

			p = stref( p );
			break;

		case UNARY MUL:
			if( l->in.op == UNARY AND ){
				p->in.op = l->in.op = FREE;
				p = l->in.left;
				}
			if( !ISPTR(l->in.type))uerror("illegal indirection");
			p->in.type = DECREF(l->in.type);
			p->fn.cdim = l->fn.cdim;
			p->fn.csiz = l->fn.csiz;
			break;

		case UNARY AND:
			switch( l->in.op ){

			case UNARY MUL:
				p->in.op = l->in.op = FREE;
				p = l->in.left;
			/* fall through */
			case NAME:
			case LAB:
				p->in.type = INCREF( l->in.type );
				p->fn.cdim = l->fn.cdim;
				p->fn.csiz = l->fn.csiz;
				break;

			case COMOP:
				lr = buildtree( UNARY AND, l->in.right, NIL );
				p->in.op = l->in.op = FREE;
				p = buildtree( COMOP, l->in.left, lr );
				break;

			case QUEST: {

				lr = buildtree( UNARY AND, l->in.right->in.right, NIL );
				ll = buildtree( UNARY AND, l->in.right->in.left, NIL );
				p->in.op = l->in.op = l->in.right->in.op = FREE;
				p = buildtree( QUEST, l->in.left, buildtree( COLON, ll, lr ) );
				break;
				}

			default:
				uerror( "unacceptable operand of &" );
				break;
				}
			break;

		case LS:
		case RS:
			if(tsize(p->in.left->in.type, p->in.left->fn.cdim, p->in.left->fn.csiz)
			   < SZINT) {
				p->in.left = makety(p->in.left, INT, 0, INT );
				p->in.type = ISUNSIGNED(p->in.left->in.type) ? UNSIGNED : INT;
			}
		/* fall through */
		case ASG LS:
		case ASG RS:
			if(tsize(p->in.right->in.type, p->in.right->fn.cdim, p->in.right->fn.csiz)
			   == SZINT) break;
		/* fall through */
		case SUBSR:
		intright: __attribute__((unused));
			p->in.right = makety(p->in.right, INT, 0, INT );
			break;

		case RETURN:
		case ASSIGN:
		case CAST:
			/* structure assignment */
			/* take the addresses of the two sides; then make an
			   operator using STASG and
			   the addresses of left and right */

			{
				TWORD t;
				int d, s;

				if( l->fn.csiz != r->fn.csiz ) uerror( "assignment of diff structures" );

				r = buildtree( UNARY AND, r, NIL );
				t = r->in.type;
				d = r->fn.cdim;
				s = r->fn.csiz;

				l = block( STASG, l, r, t, d, s );

				if( o == RETURN ){
					p->in.op = FREE;
					p = l;
					break;
					}

				p->in.op = UNARY MUL;
				p->in.left = l;
				p->in.right = NIL;
				break;
				}
		case COLON:
			/* structure colon */

			if( l->fn.csiz != r->fn.csiz ) uerror( "type clash in conditional" );
			break;

		case CALL:
			p->in.right = r = strargs( p->in.right );
		/* fall through */
		case UNARY CALL:
			if( !ISPTR(l->in.type)) uerror("illegal function");
			p->in.type = DECREF(l->in.type);
			if( !ISFTN(p->in.type)) uerror("illegal function");
			p->in.type = DECREF( p->in.type );
			p->fn.cdim = l->fn.cdim;
			p->fn.csiz = l->fn.csiz;
			if( p->in.type == STRTY || p->in.type == UNIONTY ){
				/* function returning structure */
				/*  make function really return ptr to str., with * */

				p->in.op += STCALL-CALL;
				p->in.type = INCREF( p->in.type );
				p = buildtree( UNARY MUL, p, NIL );

				}
			break;

		case UNARY MINUS:	/* WEGA: of an unsigned char */
			p->in.left = block( SCONV, p->in.left, NIL, INT, 0, INT );
			p->in.type = INT;
			break;

		case ASG DIV:
		case ASG MOD:	/* WEGA: a char divided by an int */
			if( (t = l->in.type) == CHAR || t == UCHAR ){
				p->in.right = makety( p->in.right, INT, 0, INT );
				p->in.type = l->in.type;
				p->fn.cdim = l->fn.cdim;
				p->fn.csiz = l->fn.csiz;
				}
			else p = tymatch( p );
			break;

		case ASG MUL:	/* WEGA: the product of words is long */
			if( (t = l->in.type) == CHAR || t == UCHAR || t == INT || t == UNSIGNED ){
				p->in.right = makety( p->in.right, INT, 0, INT );
				p->in.type = LONG;
				p->fn.cdim = 0;
				p->fn.csiz = LONG;
				p = makety( p, t, 0, l->fn.csiz );
				}
			else p = tymatch( p );
			break;

#ifndef SEG
		case ASG PLUS:
		case ASG MINUS:
		case INCR:
		case DECR:	/* WEGA: a pointer and a long */
			if( ISPTR(p->in.left->in.type) && (p->in.right->in.type == LONG || p->in.right->in.type == ULONG) ) goto intright;
			break;
#endif

		default:
			cerror( "other code %d [%%%x]", o, o );
			}

		}

	if( actions & CVTO ) p = oconvert(p);
	p = clocal(p);

	return(p);

	}

NODE *strargs(NODE *p)
{ /* rewrite structure flavored arguments */

	if( p->in.op == CM ){
		p->in.left = strargs( p->in.left );
		p->in.right = strargs( p->in.right );
		return( p );
		}

	if( p->in.type == STRTY || p->in.type == UNIONTY ){
		p = block( STARG, p, NIL, p->in.type, p->fn.cdim, p->fn.csiz );
		p->in.left = buildtree( UNARY AND, p->in.left, NIL );
		p = clocal(p);
		}
	return( p );
	}

int chkstr(intptr_t i, int j, TWORD type)
{
	/* is the MOS or MOU i OK for strict reference by a ptr */
	/* i has been checked to contain a MOS or MOU */
	/* j is the index in dimtab of the members... */
	int k;
	intptr_t kk;

	if( (k = j) < 0 ) uerror( "undefined structure or union" );
	else {
		for( ; (kk = dimtab[k]); ++k ){
			/* the original tests kk >= SYMTSZ (a pointer as int: never true, the pointers are above 0x8000) */
			if( kk == i ) return( 1 );
			switch( STP(kk)->stype ){

			case STRTY:
			case UNIONTY:
				if( type == STRTY ) continue;  /* no recursive looking for strs */
				if( chkstr( i, (int)dimtab[STP(kk)->sizoff+1], STP(kk)->stype ) ) return(1);
				}
			}
		}
	return( 0 );
	}

void chkpun(NODE *p)
{

	/* checks p for the existance of a pun */

	/* this is called when the op of p is ASSIGN, RETURN, CAST, COLON, or relational */

	/* one case is when enumerations are used: this applies only to lint */
	/* in the other case, one operand is a pointer, the other integer type */
	/* we check that this integer is in fact a constant zero... */

	/* in the case of ASSIGN, any assignment of pointer to integer is illegal */
	/* this falls out, because the LHS is never 0 */

	register NODE *q;
	int t1, t2;
	int d1, d2;

	t1 = p->in.left->in.type;
	t2 = p->in.right->in.type;

	if( t1==ENUMTY || t2==ENUMTY ) { /* check for enumerations */
		if( t1==ENUMTY && t2==ENUMTY ){
			if( p->in.left->fn.csiz != p->in.right->fn.csiz )
				werror( "operands are from different enumerations, operator %s", opst[p->in.op] );
			}
		else if( t1 != INT && t2 != INT ) uerror( "enumeration type clash, operator %s", opst[p->in.op] );
		return;
		}

	if( ISPTR(t1) || ISARY(t1) ) q = p->in.right;
	else q = p->in.left;

	if( !ISPTR(q->in.type) && !ISARY(q->in.type) ){
		if( q->in.op != ICON || q->tn.lval != 0 ){
			werror( "illegal combination of pointer and integer");
			}
#ifdef SEG
		if( q->in.op == ICON ) q->tn.lval &= 0x7f00ffffL;
#endif
		}
	else {
		d1 = p->in.left->fn.cdim;
		d2 = p->in.right->fn.cdim;
		for( ;; ){
			if( t1 == t2 ) {;
				if( p->in.left->fn.csiz != p->in.right->fn.csiz ) {
					werror( "illegal structure pointer combination" );
					}
				return;
				}
			/* WEGA: a function returning pointer to float against one returning pointer to double */
			if( t1 == ((FTN<<TSHIFT)|PTR|FLOAT) && t2 == ((FTN<<TSHIFT)|PTR|DOUBLE) ) return;
			if( ISARY(t1) || ISPTR(t1) ){
				if( !ISARY(t2) && !ISPTR(t2) ) break;
				if( ISARY(t1) && ISARY(t2) && dimtab[d1] != dimtab[d2] ){
					werror( "illegal array size combo" );
					return;
					}
				if( ISARY(t1) ) ++d1;
				if( ISARY(t2) ) ++d2;
				}
			else break;
			t1 = DECREF(t1);
			t2 = DECREF(t2);
			}
		werror( "illegal pointer combination" );
		}

	}

NODE *stref(NODE *p)
{

	TWORD t;
	int d, s, dsc;
	NODE *r;
	int ptr;	/* the left side was a pointer */
	register struct symtab *q;

	/* make p->x */
	/* this is also used to reference automatic variables */
	/* WEGA: the STREF node stays, codgen computes the offset */

	ptr = 1;
	q = STP(p->in.right->tn.rval);
	dsc = q->sclass;
	t = INCREF( q->stype );
	d = q->dimoff;
	s = q->sizoff;

	p = clocal( makety( p, t, d, s ) );

	if( !ISPTR(p->in.type) ) p->in.type = p->in.type + PTR;
	if( !ISPTR(p->in.left->in.type) ){
		ptr = 0;
		p->in.left->in.type = p->in.left->in.type + PTR;
		}

	if( dsc & FIELD ){ /* build field info */
		if( ptr ) p->in.left = buildtree( UNARY MUL, p->in.left, NIL );
		p->in.op = FLD;
		p->in.type = q->stype;
		p->fn.csiz = q->sizoff;
		if( p->in.left->in.op == UNARY AND && p->in.left->in.left->in.op == NAME ){
			p->in.left->in.op = FREE;
			p->in.left = p->in.left->in.left;
			}
		else if( p->in.left->in.op != NAME && !ptr ){
			r = talloc();
			ncopy( r, p->in.left );
			r->in.op = UNARY MUL;
			r->in.type = p->in.type;
			r->in.left = p->in.left;
			p->in.left = r;
			}
		}
	else p = buildtree( UNARY MUL, p, NIL );

	return( p );
	}

int notlval(NODE *p)
{

	/* return 0 if p an lvalue, 1 otherwise */

	switch( p->in.op ){

	case NAME:
	case STREF:
	case UNARY MUL:
	case FLD:
		if( ISARY(p->in.type) || ISFTN(p->in.type) ) return(1);
		return(0);

	default:
		return(1);

		}

	}

NODE *bcon(int16_t i)
{ /* make a constant node with value i */
	register NODE *p;

	p = block( ICON, NIL, NIL, INT, 0, INT );
	p->tn.lval = i;
	p->tn.rval = NONAME;
	return( clocal(p) );
	}

NODE *bpsize(NODE *p)
{
	return( offcon( psize(p), p->in.type, p->fn.cdim, p->fn.csiz ) );
	}

OFFSZ psize(NODE *p)
{
	/* p is a node of type pointer; psize returns the
	   size of the thing pointed to */

	if( !ISPTR(p->in.type) ){
		uerror( "pointer required");
		return( SZINT );
		}
	/* note: no pointers to fields */
	return( tsize( DECREF(p->in.type), p->fn.cdim, p->fn.csiz ) );
	}

NODE *convert(NODE *p, int f)
{
	/*  convert an operand of p
	    f is either CVTL or CVTR
	    operand has type int, and is converted by the size of the other side
	    */

	register NODE *q, *r;

	if( f == CVTL ){
		q = p->in.right;
		r = p->in.left;
		}
	else {
		q = p->in.left;
		r = p->in.right;
		}
	if( p->in.op == COLON ){
		r = makety( r, q->in.type, 0, q->fn.csiz );
		if( f == CVTL ) p->in.left = r;
		else p->in.right = r;
		return( p );
		}
	r = optim( clocal( block( PMCONV, r, bpsize(q), INT, 0, INT ) ) );
	p->in.left = q;	/* WEGA: for CVTL the operands change places */
	p->in.right = r;
	return(p);

	}

void econvert(NODE *p)
{

	/* change enums to ints, or appropriate types */

	register TWORD ty;

	if( (ty=BTYPE(p->in.type)) == ENUMTY || ty == MOETY ) {
		if( dimtab[ p->fn.csiz ] == SZCHAR ) ty = CHAR;
		else if( dimtab[ p->fn.csiz ] == SZINT ) ty = INT;
		else if( dimtab[ p->fn.csiz ] == SZSHORT ) ty = SHORT;
		else ty = LONG;
		ty = ctype( ty );
		p->fn.csiz = ty;
		MODTYPE(p->in.type,ty);
		if( p->in.op == ICON && ty != LONG ) p->in.type = p->fn.csiz = INT;
		}
	}

NODE *pconvert(NODE *p)
{

	/* if p should be changed into a pointer, do so */

	if( ISARY( p->in.type) ){
		p->in.type = DECREF( p->in.type );
		++p->fn.cdim;
		return( buildtree( UNARY AND, p, NIL ) );
		}
	if( ISFTN( p->in.type) )
		return( buildtree( UNARY AND, p, NIL ) );

	return( p );
	}

NODE *oconvert(NODE *p)
{
	/* convert the result itself: used for pointer and unsigned */
	register NODE *q;

	switch(p->in.op) {

	case LE:
	case LT:
	case GE:
	case GT:
		if( ISUNSIGNED(p->in.left->in.type) || ISUNSIGNED(p->in.right->in.type) )  p->in.op += (ULE-LE);
	/* fall through */
	case EQ:
	case NE:
		return( p );

	case MINUS:
		p->in.left = block( SCONV, q = p->in.left, NIL, INT, 0, INT );
		p->in.right = block( SCONV, p->in.right, NIL, INT, 0, INT );
		return(  clocal( block( PVCONV,
			p, bpsize(q), INT, 0, INT ) ) );
		}

	cerror( "illegal oconvert: %d", p->in.op );
	}

NODE *ptmatch(NODE *p)
{

	/* makes the operands of p agree; they are
	   either pointers or integers, by this time */
	/* with MINUS, the sizes must be the same */
	/* with COLON, the types must be the same */

	int o, d2, d, s2, s;
	register TWORD t1;
	TWORD t2, t;

	o = p->in.op;
	t = t1 = p->in.left->in.type;
	t2 = p->in.right->in.type;
	d = p->in.left->fn.cdim;
	d2 = p->in.right->fn.cdim;
	s = p->in.left->fn.csiz;
	s2 = p->in.right->fn.csiz;

	switch( o ){

	case ASSIGN:
	case RETURN:
	case CAST:
		{  break; }

	case MINUS:
		{  if( psize(p->in.left) != psize(p->in.right) ){
			uerror( "illegal pointer subtraction");
			}
		   break;
		   }
	case COLON:
		{  if( t1 != t2 ) uerror( "illegal types in :");
		   break;
		   }
	default:  /* must work harder: relationals or comparisons */

		if( !ISPTR(t1) ){
			t = t2;
			d = d2;
			s = s2;
			break;
			}
		if( !ISPTR(t2) ){
			break;
			}

		/* both are pointers */
		if( talign(t2,s2) < talign(t,s) ){
			t = t2;
			s = s2;
			}
		break;
		}

	p->in.left = makety( p->in.left, t, d, s );
	p->in.right = makety( p->in.right, t, d, s );
	if( o!=MINUS && !logop(o) ){

		p->in.type = t;
		p->fn.cdim = d;
		p->fn.csiz = s;
		}

	return(clocal(p));
	}

NODE *tymatch(NODE *p)
{

	/* satisfy the types of various arithmetic binary ops */

	/* rules are:
		if assignment, op, type of LHS
		if any float or doubles, make double
		if any longs, make long
		WEGA: if a logical op without int operands, make char
		otherwise, make int
		if either operand is unsigned, the result is...
	*/

	int o, u;
	register TWORD t1, t2, t, tu;
	int cvl;	/* the left operand may be converted */

	o = p->in.op;

	t1 = p->in.left->in.type;
	t2 = p->in.right->in.type;
	if( (t1==UNDEF || t2==UNDEF) && o!=CAST )
		uerror( "void type illegal in expression" );

	u = 0;
	if( ISUNSIGNED(t1) ){
		u = 1;
		t1 = DEUNSIGN(t1);
		}
	if( ISUNSIGNED(t2) ){
		u = 1;
		t2 = DEUNSIGN(t2);
		}

	if( t1==DOUBLE || t1==FLOAT || t2==DOUBLE || t2==FLOAT ) t = DOUBLE;
	else if( t1==LONG || t2==LONG ) t = LONG;
	else if( !logop(o) || t1==INT || t2==INT ) t = INT;
	else t = CHAR;

	if( asgop(o) ){
		cvl = 0;
		t = t1;
		tu = p->in.left->in.type;
		}
	else {
		tu = (u && UNSIGNABLE(t))?ENUNSIGN(t):t;
		cvl = 1;
		}

	if( t != t1 && cvl ) p->in.left = makety( p->in.left, tu, 0, (int)tu );

	if( t != t2 || o==CAST ) p->in.right = makety( p->in.right, tu, 0, (int)tu );

	if( asgop(o) ){
		p->in.type = p->in.left->in.type;
		p->fn.cdim = p->in.left->fn.cdim;
		p->fn.csiz = p->in.left->fn.csiz;
		}
	else if( !logop(o) ){
		p->in.type = tu;
		p->fn.cdim = 0;
		p->fn.csiz = t;
		}
	else {
		p->in.type = p->fn.csiz = tu = INT;
		p->fn.cdim = 0;
		}

	/* WEGA: a product of two words is long */
	if( (o == MUL || o == ASG MUL) && twords( p->in.left->in.type ) == 1 && twords( p->in.right->in.type ) == 1 ){
		p->in.type = u ? ULONG : LONG;
		p = makety( p, tu, 0, (int)tu );
		}

	return(p);
	}

NODE *makety(NODE *p, TWORD t, int d, int s)
{
	/* make p into type t by inserting a conversion */
	register TWORD pt;

	pt = p->in.type;
	if( p->in.type == ENUMTY && p->in.op == ICON ){
		econvert(p);
	same:
		p->fn.cdim = d;
		p->fn.csiz = s;
		return( p );
		}
	if( t == pt ) goto same;

	if( t & TMASK ){
		/* non-simple type */
		return( block( PCONV, p, NIL, t, d, s ) );
		}

	if( p->in.op == ICON && ( t==DOUBLE||t==FLOAT ) ){
		p->in.op = FCON;
		p->fpn.dval = p->tn.lval;
		p->in.type = p->fn.csiz = t;
		return( clocal(p) );
		}

	/* WEGA: between float and the short integer types through long */
# define SHORTI(x) ((x)==INT||(x)==UNSIGNED||(x)==CHAR||(x)==UCHAR||(x)==SHORT||(x)==USHORT)
	if( ( (t==DOUBLE||t==FLOAT) && SHORTI(pt) ) || ( (pt==FLOAT||pt==DOUBLE) && SHORTI(t) ) )
		p = block( SCONV, p, NIL, LONG, d, LONG );

	return( clocal( block( SCONV, p, NIL, t, d, s ) ) );

	}

NODE *block(int o, NODE *l, NODE *r, TWORD t, int d, int s)
{

	register NODE *p;

	p = talloc();
	p->tn.rval = 0;	/* WEGA */
	p->in.op = o;
	p->in.left = l;
	p->in.right = r;
	p->in.type = t;
	p->fn.cdim = d;
	p->fn.csiz = s;
	return(p);
	}

CONSZ lcons(NODE *p)
{
	/* if p is an integer constant, return its value (long) */
	CONSZ val;

	if( p->in.op != ICON ){
		uerror( "constant expected");
		val = 1;
		}
	else val = p->tn.lval;
	tfree( p );
	return(val);
	}

int icons(NODE *p)
{
	/* if p is an integer constant, return its value */
	int16_t val;

	if( p->in.op != ICON ){
		uerror( "constant expected");
		val = 1;
		}
	else {
		val = (int16_t) p->tn.lval;
		/* the original tests val != p->tn.lval, which never fires: the test compares the long that is still in the registers */
		}
	tfree( p );
	return(val);
	}

int opact(NODE *p)
{

	int mt12, mt1 = 0, mt2 = 0, o;

	mt12 = 0;

	switch( optype(o=p->in.op) ){

	case BITYPE:
		mt12=mt2 = moditype( p->in.right->in.type );
	/* fall through */
	case UTYPE:
		mt12 &= (mt1 = moditype( p->in.left->in.type ));

		}

	switch( o ){

	case NAME :
	case STRING :
	case ICON :
	case FCON :
	case CALL :
	case UNARY CALL:
	case UNARY MUL:
	case NOT:
	case ANDAND:
	case OROR:
		{  return( OTHER ); }
	case UNARY MINUS:
		if( p->in.left->in.type == UCHAR ) return( OTHER );
		if( mt1 & MDBI ) return( TYPL );
		break;

	case RET:	/* PCC: FORCE */
		return( TYPL );

	case COMPL:
		if( mt1 & MINT ) return( TYPL );
		break;

	case UNARY AND:
	case STREF:
		{  return( NCVT+OTHER ); }
	case INIT:
	case CM:
	case CBRANCH:
		return( 0 );

	case MUL:
	case DIV:
		if( mt12 & MDBI ) return( TYMATCH );

	/* fall through */
	case MOD:
	case AND:
	case OR:
	case ER:
		if( mt12 & MINT ) return( TYMATCH );
		break;

	case LS:
	case RS:
		if( mt12 & MINT ) return( TYPL+OTHER );
		break;

	case EQ:
	case NE:
	case LT:
	case LE:
	case GT:
	case GE:
		if( (mt1&MENU)||(mt2&MENU) ) return( PTMATCH+PUN+NCVT );
		if( mt12 & MDBI ) return( TYMATCH+CVTO );
		else if( mt12 & MPTR ) return( PTMATCH+PUN );
		else if( mt12 & MPTI ) return( PTMATCH+PUN );
		else break;

	case QUEST:
	case COMOP:
		if( mt2&MENU ) return( TYPR+NCVTR );
		return( TYPR+NCVTR );

	case COLON:
		if( mt12 & MENU ) return( NCVT+PUN+PTMATCH );
		else if( mt12 & MDBI ) return( TYMATCH );
		else if( mt12 & MPTR ) return( TYPL+PTMATCH+PUN );
		else if( (mt1&MINT) && (mt2&MPTR) ) return( TYPR+PUN+CVTL );
		else if( (mt1&MPTR) && (mt2&MINT) ) return( TYPL+PUN+CVTR );
		else if( mt12 & MSTR ) return( NCVT+TYPL+OTHER );
		break;

	case ASSIGN:
	case RETURN:
		if( mt12 & MSTR ) return( LVAL+NCVT+TYPL+OTHER );
	case CAST:
		if( o == CAST && mt1 == MVOID ) return( TYPL+TYMATCH );
		if( mt12 & MDBI ) return( TYPL+LVAL+TYMATCH );
		else if( (mt1&MENU)||(mt2&MENU) ) return( LVAL+NCVT+TYPL+PTMATCH+PUN );
		else if( mt1 & MPTR ) return( LVAL+PTMATCH+PUN );
		else if( mt12 & MPTI ) return( TYPL+LVAL+TYMATCH+PUN );
		break;

	case ASG LS:
	case ASG RS:
		if( mt12 & MINT ) return( TYPL+LVAL+OTHER );
		break;

	case ASG MUL:
	case ASG DIV:
	case ASG MOD:
		if( mt12 & MDBI ) return( LVAL+OTHER );
		break;

	case ASG AND:
	case ASG OR:
	case ASG ER:
		if( mt12 & MINT ) return( LVAL+TYMATCH );
		break;

	case ASG PLUS:
	case ASG MINUS:
	case INCR:
	case DECR:
		if( mt12 & MDBI ) return( TYMATCH+LVAL );
#ifdef SEG
		else if( (mt1&MPTR) && (mt2&MINT) ) return( TYPL+LVAL+CVTR );
#else
		else if( (mt1&MPTR) && (mt2&MINT) ) return( TYPL+LVAL+CVTR+OTHER );
#endif
		else if( (mt1&MENU) || (mt2&MENU) ) return( NCVT );
		break;

	case MINUS:
		if( mt12 & MPTR ) return( CVTO+PTMATCH+PUN );
		if( mt2 & MPTR ) break;
	case PLUS:
		if( mt12 & MDBI ) return( TYMATCH );
		else if( (mt1&MPTR) && (mt2&MINT) ) return( TYPL+CVTR );
		else if( (mt1&MINT) && (mt2&MPTR) ) return( TYPR+CVTL );

	case SUBSR:	/* WEGA */
		if( mt12 & MDBI ) return( TYMATCH+OTHER );
		else if( (mt1&MPTR) && (mt2&MINT) ) return( TYPL+CVTR+OTHER );
		else if( (mt1&MINT) && (mt2&MPTR) ) return( TYPR+CVTL+OTHER );

		}
	uerror( "operands of %s have incompatible types ", opst[o] );
	return( NCVT );
	}

int moditype(TWORD ty)
{

	switch( ty ){

	case TVOID:
	case UNDEF:
		return( MVOID );

	case ENUMTY:
	case MOETY:
		return( MENU );

	case STRTY:
	case UNIONTY:
		return( MSTR );

	case CHAR:
	case SHORT:
	case UCHAR:
	case USHORT:
		return( MINT|MDBI );
	case UNSIGNED:
	case ULONG:
	case INT:
	case LONG:
		return( MINT|MDBI|MPTI );
	case FLOAT:
	case DOUBLE:
		return( MDBI );
	default:
		return( MPTR|MPTI );

		}
	}

int aregs = 0, afregs = 0;	/* the argument registers of a call that are still free (argregs) */
int afull;	/* the argument registers are used up */

NODE *doszof(NODE *p)
{
	/* do sizeof p */
	int i;

	/* whatever is the meaning of this if it is a bitfield? */
	i = tsize( p->in.type, p->fn.cdim, p->fn.csiz )/SZCHAR;

	tfree(p);
	if( i == 0 ) werror( "sizeof returns 0" );
	else if( i < 0 ) werror( "sizeof overflow; may be meaningful if cast unsigned" );
	return( bcon( i ) );
	}

void prtdcon(NODE *p)
{
	int lab;
	long size;
	long pos;
	int l;

	if( p->in.op == FCON ){ /* a new constant goes to the .D file, an old one is found by its label */
		pos = 0;
		if( (lab = fltlab( p->fpn.dval )) == crslab ){
			defalign( ALDOUBLE );
			if( sectfull( DATA, size = SZDOUBLE/SZCHAR ) ) newsect( DATA );
			sectadd( DATA, size );
			l = lab = getlab();
			fprintf( dfile, "L%d: \n", l );
			pos = ftell( dfile );
			fincode( p->fpn.dval, SZDOUBLE );
			}
		p->tn.lval = pos;
		p->tn.rval = -lab;
		p->in.type = DOUBLE;
		}
	}

void ecomp(NODE *p)
{
	if( !reached ){
		werror( "statement not reached" );
		reached = 1;
		}
	p = optim(p);
	walkf( p, prtdcon );
	locctr( PROG );
	ecode( p );
	tfree(p);
	}

void prtree(NODE *p)
{

	int ty;
	int n;

	ty = optype(p->in.op);

	fprintf( outfile, "%d\t", p->in.op );

	if( ty == LTYPE ) {
		fprintf( outfile, CONFMT, p->tn.lval );
		putc( '\t', outfile );
		}
	if( ty != BITYPE ) {
		if( p->in.op == ICON ) fprintf( outfile, "0\t" );
		else if( p->in.op == NAME ) fprintf( outfile, "%d\t", STP(p->tn.rval)->symno );
		else fprintf( outfile, "%d\t", (int) p->tn.rval );
		}
	if( p->in.op == TEMP ) fprintf( outfile, "%d\t", p->fn.csiz );

	fprintf( outfile, "%o\t", p->in.type );

	/* handle special cases */

	switch( p->in.op ){

	case NAME:
	case ICON:
		putc( '\n', outfile );
		break;

	case STARG:
	case STASG:
	case STCALL:
	case USTCALL:
		/* print out size */
		/* use lhs size, in order to avoid hassles with the structure `.' operator */

		/* note: p->in.left not a field... */
		fprintf( outfile, CONFMT, (CONSZ) tsize( STRTY, p->in.left->fn.cdim, p->in.left->fn.csiz ) );
		fprintf( outfile, "\t%d\t\n", talign( STRTY, p->in.left->fn.csiz ) );
		break;

	default:
		putc( '\n', outfile );
		}

	if( p->in.op == STCALL || p->in.op == CALL ){ /* the stack words of the arguments */
		aregs = 7;
		afregs = 19;
		afull = 0;
		n = argregs( p->in.right );
		if( n > maxoff ) maxoff = n;
		}

	if( ty != LTYPE ) prtree( p->in.left );
	if( ty == BITYPE ) prtree( p->in.right );

	}

int argregs(NODE *p)
{ /* the stack words of the arguments p of a call (those that are not in registers) */
	int n;

	if( p->in.op == CM ){
		n = argregs( p->in.left );
		return( n + argregs( p->in.right ) );
		}
	n = argsize( p );
#ifdef OLD_D22
	if( afull || aregs < 2 ){	/* the old compiler: a double argument in rq4 when it fits */
	full:
		afull = 1;
		return( n*2 );
		}
#else
	if( ( (p->in.type == FLOAT || p->in.type == DOUBLE) && afregs < 16 ) || afull ){
	full:
		afull = 1;
		return( n*2 );
		}
	if( p->in.type != FLOAT && p->in.type != DOUBLE && aregs < 2 ) goto full;
	if( p->in.type == FLOAT || p->in.type == DOUBLE ){
		--afregs;
		goto ret0;
		}
#endif
	goto sw;

nofit:
	aregs = 1;
	goto full;

two:
	if( --aregs & 1 ) --aregs;
	if( aregs < 2 ) goto nofit;
one:
	--aregs;
ret0:
	return( 0 );

sw:
	switch( n ){

	case 1:
		goto one;

	case 2:
		goto two;
		}
	if( n == 4 && aregs == 7 ){
		aregs -= 4;
		goto ret0;
		}
	goto nofit;
	}

NODE *tempnode(TWORD t, int flag)
{ /* a TEMP node of type t; flag 1: it gets the next place in the frame */
	NODE *q;

	q = block( TEMP, NIL, NIL, t, 0, INT );
	q->tn.rval = tmpno++;
	if( flag == 1 ){
		q->fn.csiz = tmpoff;
		tmpoff += twords( t ) * 2;
		}
	return( q );
	}











