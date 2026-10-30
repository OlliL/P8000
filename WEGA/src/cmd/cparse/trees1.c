# include "mfile1"

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

int bdebug = 0;
















int tdebug = 0;





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







int edebug = 0;

# ifdef STDPRTREE
# ifndef ONEPASS


# else


# endif
# endif

NODE *
buildtree( o, l, r ) register NODE *l, *r; {
	register NODE *p, *q;
	register actions;
	register opty;
	register struct symtab *sp;
	register NODE *lr, *ll;
	int i;
	extern int eprint();

	if( bdebug ) printf( "buildtree( %s, %o, %o )\n", opst[o], l, r );
	opty = optype(o);

	/* check for constants */

	if( opty == UTYPE && l->op == ICON ){

		switch( o ){

		case NOT:
			if( hflag ) werror( "constant argument to NOT" );
		case UNARY MINUS:
		case COMPL:
			if( conval( l, o, l ) ) return(l);
			break;

			}
		}

	else if( o==UNARY MINUS && l->op==FCON ){
		l->dval = -l->dval;
		return(l);
		}

	else if( o==QUEST && l->op==ICON ) {
		l->op = FREE;
		r->op = FREE;
		if( l->lval ){
			tfree( r->right );
			return( r->left );
			}
		else {
			tfree( r->left );
			return( r->right );
			}
		}

	else if( (o==ANDAND || o==OROR) && (l->op==ICON||r->op==ICON) ) goto ccwarn;

	else if( opty == BITYPE && l->op == ICON && r->op == ICON ){

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
			if( hflag ) werror( "constant in conditional context" );

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
			{ register NODE *t = 0;
			/*
			 * if either side is of type long then both
			 * sides should be long
			 */
			if( l->type == LONG && r->type != LONG )
				t = r;
			if( r->type == LONG && l->type != LONG )
				t = l;
			if( t ) t->type = LONG;
			}
			if( conval( l, o, r ) ) {
				r->op = FREE;
				return(l);
				}
			break;
			}
		}

	else if( opty == BITYPE && (l->op==FCON||l->op==ICON) &&
		(r->op==FCON||r->op==ICON) ){
		switch(o){
		case PLUS:
		case MINUS:
		case MUL:
		case DIV:
			if( l->op == ICON ){
				l->dval = l->lval;
				}
			if( r->op == ICON ){
				r->dval = r->lval;
				}
			l->op = FCON;
			l->type = l->csiz = DOUBLE;
			r->op = FREE;
			switch(o){
			case PLUS:
				l->dval += r->dval;
				return(l);
			case MINUS:
				l->dval -= r->dval;
				return(l);
			case MUL:
				l->dval *= r->dval;
				return(l);
			case DIV:
				if( r->dval == 0 ) uerror( "division by 0." );
				else l->dval /= r->dval;
				return(l);
				}
			}
		}

	/* its real; we must make a new node */

	p = block( o, l, r, INT, 0, INT );

	actions = opact(p);

	if( actions&LVAL ){ /* check left descendent */
		if( notlval(p->left) ) {
			uerror( "lvalue required" );
			}
		}

	if( actions & NCVTR ){
		p->left = pconvert( p->left );
		}
	else if( !(actions & NCVT ) ){
		switch( opty ){

		case BITYPE:
			p->right = pconvert( p->right );
		case UTYPE:
			p->left = pconvert( p->left );

			}
		}

	if( (actions&PUN) && (o!=CAST||cflag) ){
		chkpun(p);
		}

	if( actions & (TYPL|TYPR) ){

		q = (actions&TYPL) ? p->left : p->right;

		p->type = q->type;
		p->cdim = q->cdim;
		p->csiz = q->csiz;
		}

	if( actions & CVTL ) p = convert( p, CVTL );
	if( actions & CVTR ) p = convert( p, CVTR );
	if( actions & TYMATCH ) p = tymatch(p);
	if( actions & PTMATCH ) p = ptmatch(p);

	if( actions & OTHER ){
		l = p->left;
		r = p->right;

		switch(o){

		case NAME:
			sp = STP(idname);
			if( sp->stype == UNDEF ){
				uerror( "%.8s undefined", sp->sname );
				/* make p look reasonable */
				p->type = p->cdim = p->csiz = INT;
				p->rval = idname;
				p->lval = 0;
				defid( p, SNULL );
				break;
				}
			p->type = sp->stype;
			p->cdim = sp->dimoff;
			p->csiz = sp->sizoff;
			p->lval = 0;
			p->rval = idname;
			/* special case: MOETY is really an ICON... */
			if( p->type == MOETY ){
				p->rval = NONAME;
				p->lval = sp->offset;
				p->cdim = 0;
				p->type = ENUMTY;
				p->op = ICON;
				}
			break;

		case ICON:
			p->type = INT;
			p->cdim = 0;
			p->csiz = INT;
			break;

		case STRING:
			p->op = NAME;
			p->type = CHAR+ARY;
			p->lval = 0;
			p->rval = NOLAB;
			p->cdim = curdim;
			p->csiz = CHAR;
			break;

		case FCON:
			p->lval = 0;
			p->rval = 0;
			p->type = DOUBLE;
			p->cdim = 0;
			p->csiz = DOUBLE;
			break;

		case STREF:
			/* p->x turned into *(p+offset) */
			/* rhs must be a name; check correctness */

			i = r->rval;
			if( i<0 || ((sp= STP(i))->sclass != MOS && sp->sclass != MOU && !(sp->sclass&FIELD)) ){
				uerror( "member of structure or union required" );
				}
			else {
				register j;
				if( l->type != PTR+STRTY && l->type != PTR+UNIONTY ){
					werror( "struct/union or struct/union pointer required" );
					}
				else if( (j=l->csiz+1)<0 ) cerror( "undefined structure or union" );
				else if( !chkstr( i, dimtab[j], DECREF(l->type) ) ){
					werror( "illegal member use: %.8s", STP(i)->sname );
					}
				}

			p = stref( p );
			break;

		case UNARY MUL:
			if( l->op == UNARY AND ){
				p->op = l->op = FREE;
				p = l->left;
				}
			if( !ISPTR(l->type))uerror("illegal indirection");
			p->type = DECREF(l->type);
			p->cdim = l->cdim;
			p->csiz = l->csiz;
			break;

		case UNARY AND:
			switch( l->op ){

			case UNARY MUL:
				p->op = l->op = FREE;
				p = l->left;
			case NAME:
				p->type = INCREF( l->type );
				p->cdim = l->cdim;
				p->csiz = l->csiz;
				break;

			case COMOP:
				lr = buildtree( UNARY AND, l->right, NIL );
				p->op = l->op = FREE;
				p = buildtree( COMOP, l->left, lr );
				break;

			case QUEST:
				lr = buildtree( UNARY AND, l->right->right, NIL );
				ll = buildtree( UNARY AND, l->right->left, NIL );
				p->op = l->op = l->right->op = FREE;
				p = buildtree( QUEST, l->left, buildtree( COLON, ll, lr ) );
				break;

			default:
				uerror( "unacceptable operand of &" );
				break;
				}
			break;

		case LS:
		case RS:
			if(tsize(p->left->type, p->left->cdim, p->left->csiz)
			   < SZINT) {
				p->left = makety(p->left, INT, 0, INT );
				/* find some way to type '<<' as an int */
				p->type += (INT - CHAR);
			}
		case ASG LS:
		case ASG RS:
			if(tsize(p->right->type, p->right->cdim, p->right->csiz)
			   != SZINT)
				p->right = makety(p->right, INT, 0, INT );
			break;

		case RETURN:
		case ASSIGN:
		case CAST:
			/* structure assignment */
			/* take the addresses of the two sides; then make an
			/* operator using STASG and
			/* the addresses of left and right */

			{
				register TWORD t;
				register d, s;

				if( l->csiz != r->csiz ) uerror( "assignment of different structures" );

				r = buildtree( UNARY AND, r, NIL );
				t = r->type;
				d = r->cdim;
				s = r->csiz;

				l = block( STASG, l, r, t, d, s );

				if( o == RETURN ){
					p->op = FREE;
					p = l;
					break;
					}

				p->op = UNARY MUL;
				p->left = l;
				p->right = NIL;
				break;
				}
		case COLON:
			/* structure colon */

			if( l->csiz != r->csiz ) uerror( "type clash in conditional" );
			break;

		case CALL:
			p->right = r = strargs( p->right );
		case UNARY CALL:
			if( !ISPTR(l->type)) uerror("illegal function");
			p->type = DECREF(l->type);
			if( !ISFTN(p->type)) uerror("illegal function");
			p->type = DECREF( p->type );
			p->cdim = l->cdim;
			p->csiz = l->csiz;
			if( l->op == UNARY AND && l->left->op == NAME &&
				l->left->rval >= 0 && l->left->rval != NONAME &&
				( (i=STP(l->left->rval)->sclass) == FORTRAN || i==UFORTRAN ) ){
				p->op += (FORTCALL-CALL);
				}
			if( p->type == STRTY || p->type == UNIONTY ){
				/* function returning structure */
				/*  make function really return ptr to str., with * */

				p->op += STCALL-CALL;
				p->type = INCREF( p->type );
				p = buildtree( UNARY MUL, p, NIL );

				}
			break;

		default:
			cerror( "other code %d", o );
			}

		}

	if( actions & CVTO ) p = oconvert(p);
	p = clocal(p);

	if( bdebug ) fwalk( p, eprint, 0 );

	return(p);

	}

NODE *
strargs( p ) register NODE *p;  { /* rewrite structure flavored arguments */

	if( p->op == CM ){
		p->left = strargs( p->left );
		p->right = strargs( p->right );
		return( p );
		}

	if( p->type == STRTY || p->type == UNIONTY ){
		p = block( STARG, p, NIL, p->type, p->cdim, p->csiz );
		p->left = buildtree( UNARY AND, p->left, NIL );
		p = clocal(p);
		}
	return( p );
	}

chkstr( i, j, type ) TWORD type; {
	/* is the MOS or MOU i OK for strict reference by a ptr */
	/* i has been checked to contain a MOS or MOU */
	/* j is the index in dimtab of the members... */
	int k, kk;

	if( (k = j) < 0 ) uerror( "undefined structure or union" );
	else {
		for( ; kk = dimtab[k]; ++k ){
			if( kk >= SYMTSZ ){
				cerror( "gummy structure" );
				return(1);
				}
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

chkpun(p) register NODE *p; {

	/* checks p for the existance of a pun */

	/* this is called when the op of p is ASSIGN, RETURN, CAST, COLON, or relational */

	/* one case is when enumerations are used: this applies only to lint */
	/* in the other case, one operand is a pointer, the other integer type */
	/* we check that this integer is in fact a constant zero... */

	/* in the case of ASSIGN, any assignment of pointer to integer is illegal */
	/* this falls out, because the LHS is never 0 */

	register NODE *q;
	register t1, t2;
	register d1, d2;

	t1 = p->left->type;
	t2 = p->right->type;

	if( t1==ENUMTY || t2==ENUMTY ) { /* check for enumerations */
		if( logop( p->op ) && p->op != EQ && p->op != NE ) {
			uerror( "illegal comparison of enums" );
			return;
			}
		if( t1==ENUMTY && t2==ENUMTY && p->left->csiz==p->right->csiz ) return;
		werror( "enumeration type clash, operator %s", opst[p->op] );
		return;
		}

	if( ISPTR(t1) || ISARY(t1) ) q = p->right;
	else q = p->left;

	if( !ISPTR(q->type) && !ISARY(q->type) ){
		if( q->op != ICON || q->lval != 0 ){
			werror( "illegal combination of pointer and integer");
			}
		}
	else {
		d1 = p->left->cdim;
		d2 = p->right->cdim;
		for( ;; ){
			if( t1 == t2 ) {;
				if( p->left->csiz != p->right->csiz ) {
					werror( "illegal structure pointer combination" );
					}
				return;
				}
			if( ISARY(t1) || ISPTR(t1) ){
				if( !ISARY(t2) && !ISPTR(t2) ) break;
				if( ISARY(t1) && ISARY(t2) && dimtab[d1] != dimtab[d2] ){
					werror( "illegal array size combination" );
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

NODE *
stref( p ) register NODE *p; {

	TWORD t;
	int d, s, dsc;
	OFFSZ off;
	register struct symtab *q;

	/* make p->x */
	/* this is also used to reference automatic variables */

	q = STP(p->right->rval);
	p->right->op = FREE;
	p->op = FREE;
	p = pconvert( p->left );

	/* make p look like ptr to x */

	if( !ISPTR(p->type)){
		p->type = PTR+UNIONTY;
		}

	t = INCREF( q->stype );
	d = q->dimoff;
	s = q->sizoff;

	p = makety( p, t, d, s );

	/* compute the offset to be added */

	off = q->offset;
	dsc = q->sclass;

	if( dsc & FIELD ){ /* make fields look like ints */
		off = (off/ALINT)*ALINT;
		s = INT;
		}
	if( off != 0 ) p = clocal( block( PLUS, p, offcon( off, t, d, s ), t, d, s ) );

	p = buildtree( UNARY MUL, p, NIL );

	/* if field, build field info */

	if( dsc & FIELD ){
		p = block( FLD, p, NIL, q->stype, 0, q->sizoff );
		p->rval = PKFIELD( dsc&FLDSIZ, q->offset%ALINT );
		}

	return( clocal(p) );
	}

notlval(p) register NODE *p; {

	/* return 0 if p an lvalue, 1 otherwise */

	again:

	switch( p->op ){

	case FLD:
		p = p->left;
		goto again;

	case NAME:
	case OREG:
	case UNARY MUL:
		if( ISARY(p->type) || ISFTN(p->type) ) return(1);
	case REG:
		return(0);

	default:
		return(1);

		}

	}

NODE *
bcon( i ){ /* make a constant node with value i */
	register NODE *p;

	p = block( ICON, NIL, NIL, INT, 0, INT );
	p->lval = i;
	p->rval = NONAME;
	return( clocal(p) );
	}

NODE *
bpsize(p) register NODE *p; {
	return( offcon( psize(p), p->type, p->cdim, p->csiz ) );
	}

OFFSZ
psize( p ) register NODE *p; {
	/* p is a node of type pointer; psize returns the
	   size of the thing pointed to */

	if( !ISPTR(p->type) ){
		uerror( "pointer required");
		return( SZINT );
		}
	/* note: no pointers to fields */
	return( tsize( DECREF(p->type), p->cdim, p->csiz ) );
	}

NODE *
convert( p, f )  register NODE *p; {
	/*  convert an operand of p
	    f is either CVTL or CVTR
	    operand has type int, and is converted by the size of the other side
	    */

	register NODE *q, *r;

	if( f == CVTL ){
		q = p->right;
		r = p->left;
		}
	else {
		q = p->left;
		r = p->right;
		}
	if( p->op == COLON ){
		r = makety( r, q->type, 0, q->csiz );
		if( f == CVTL ) p->left = r;
		else p->right = r;
		return( p );
		}
	r = optim( clocal( block( PMCONV, r, bpsize(q), INT, 0, INT ) ) );
	p->left = q;	/* WEGA: for CVTL the operands change places */
	p->right = r;
	return(p);

	}

econvert( p ) register NODE *p; {

	/* change enums to ints, or appropriate types */

	register TWORD ty;

	if( (ty=BTYPE(p->type)) == ENUMTY || ty == MOETY ) {
		if( dimtab[ p->csiz ] == SZCHAR ) ty = CHAR;
		else if( dimtab[ p->csiz ] == SZINT ) ty = INT;
		else if( dimtab[ p->csiz ] == SZSHORT ) ty = SHORT;
		else ty = LONG;
		ty = ctype( ty );
		p->csiz = ty;
		MODTYPE(p->type,ty);
		if( p->op == ICON && ty != LONG ) p->type = p->csiz = INT;
		}
	}

NODE *
pconvert( p ) register NODE *p; {

	/* if p should be changed into a pointer, do so */

	if( ISARY( p->type) ){
		p->type = DECREF( p->type );
		++p->cdim;
		return( buildtree( UNARY AND, p, NIL ) );
		}
	if( ISFTN( p->type) )
		return( buildtree( UNARY AND, p, NIL ) );

	return( p );
	}

NODE *
oconvert(p) register NODE *p; {
	/* convert the result itself: used for pointer and unsigned */
	register NODE *q;

	switch(p->op) {

	case LE:
	case LT:
	case GE:
	case GT:
		if( ISUNSIGNED(p->left->type) || ISUNSIGNED(p->right->type) )  p->op += (ULE-LE);
	case EQ:
	case NE:
		return( p );

	case MINUS:
		q = p->left;
		p->left = block( SCONV, q, NIL, INT, 0, INT );
		p->right = block( SCONV, p->right, NIL, INT, 0, INT );
		return(  clocal( block( PVCONV,
			p, bpsize(q), INT, 0, INT ) ) );
		}

	cerror( "illegal oconvert: %d", p->op );

	return(p);
	}

NODE *
ptmatch(p)  register NODE *p; {

	/* makes the operands of p agree; they are
	   either pointers or integers, by this time */
	/* with MINUS, the sizes must be the same */
	/* with COLON, the types must be the same */

	TWORD t1, t2, t;
	int o, d2, d, s2, s;

	o = p->op;
	t = t1 = p->left->type;
	t2 = p->right->type;
	d = p->left->cdim;
	d2 = p->right->cdim;
	s = p->left->csiz;
	s2 = p->right->csiz;

	switch( o ){

	case ASSIGN:
	case RETURN:
	case CAST:
		{  break; }

	case MINUS:
		{  if( psize(p->left) != psize(p->right) ){
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

	p->left = makety( p->left, t, d, s );
	p->right = makety( p->right, t, d, s );
	if( o!=MINUS && !logop(o) ){

		p->type = t;
		p->cdim = d;
		p->csiz = s;
		}

	return(clocal(p));
	}

NODE *
tymatch(p)  register NODE *p; {

	/* satisfy the types of various arithmetic binary ops */

	/* rules are:
		if assignment, op, type of LHS
		if any float or doubles, make double
		if any longs, make long
		WEGA: if a logical op without int operands, make char
		otherwise, make int
		if either operand is unsigned, the result is...
	*/

	register TWORD t1, t2, t, tu;
	register o, u;
	int cvl;	/* the left operand may be converted */

	o = p->op;

	t1 = p->left->type;
	t2 = p->right->type;
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
		tu = p->left->type;
		}
	else {
		tu = (u && UNSIGNABLE(t))?ENUNSIGN(t):t;
		cvl = 1;
		}

	if( t != t1 && cvl ) p->left = makety( p->left, tu, 0, (int)tu );

	if( t != t2 || o==CAST ) p->right = makety( p->right, tu, 0, (int)tu );

	if( asgop(o) ){
		p->type = p->left->type;
		p->cdim = p->left->cdim;
		p->csiz = p->left->csiz;
		}
	else if( !logop(o) ){
		p->type = tu;
		p->cdim = 0;
		p->csiz = t;
		}
	else {
		p->type = p->csiz = tu = INT;
		p->cdim = 0;
		}

	/* WEGA: a product of two words is long */
	if( (o == MUL || o == ASG MUL) && twords( p->left->type ) == 1 && twords( p->right->type ) == 1 ){
		p->type = u ? ULONG : LONG;
		p = makety( p, tu, 0, (int)tu );
		}

	return(p);
	}

NODE *
makety( p, t, d, s ) register NODE *p; register TWORD t; {
	/* make p into type t by inserting a conversion */
	register TWORD pt;

	pt = p->type;
	if( p->type == ENUMTY && p->op == ICON ){
		econvert(p);
	same:
		p->cdim = d;
		p->csiz = s;
		return( p );
		}
	if( t == pt ) goto same;

	if( t & TMASK ){
		/* non-simple type */
		return( block( PCONV, p, NIL, t, d, s ) );
		}

	if( p->op == ICON && ( t==DOUBLE||t==FLOAT ) ){
		p->op = FCON;
		p->dval = p->lval;
		p->type = p->csiz = t;
		return( clocal(p) );
		}

	/* WEGA: between float and the short integer types through long */
# define SHORTI(x) ((x)==INT||(x)==UNSIGNED||(x)==CHAR||(x)==UCHAR||(x)==SHORT||(x)==USHORT)
	if( ( (t==DOUBLE||t==FLOAT) && SHORTI(pt) ) || ( (pt==FLOAT||pt==DOUBLE) && SHORTI(t) ) )
		p = block( SCONV, p, NIL, LONG, d, LONG );

	return( clocal( block( SCONV, p, NIL, t, d, s ) ) );

	}

NODE *
block( o, l, r, t, d, s ) register NODE *l, *r; TWORD t; {

	register NODE *p;

	p = talloc();
	p->op = o;
	p->left = l;
	p->right = r;
	p->type = t;
	p->cdim = d;
	p->csiz = s;
	return(p);
	}

CONSZ
lcons(p) register NODE *p; {
	/* if p is an integer constant, return its value (long) */
	CONSZ val;

	if( p->op != ICON ){
		uerror( "constant expected");
		val = 1;
		}
	else val = p->lval;
	tfree( p );
	return(val);
	}

icons(p) register NODE *p; {
	/* if p is an integer constant, return its value */
	int val;

	if( p->op != ICON ){
		uerror( "constant expected");
		val = 1;
		}
	else {
		val = p->lval;
		if( val != p->lval ) uerror( "constant too big for cross-compiler" );
		}
	tfree( p );
	return(val);
	}

opact( p )  NODE *p; {

	register mt12, mt1, mt2, o;

	mt12 = 0;

	switch( optype(o=p->op) ){

	case BITYPE:
		mt12=mt2 = moditype( p->right->type );
	case UTYPE:
		mt12 &= (mt1 = moditype( p->left->type ));

		}

	switch( o ){

	case NAME :
	case STRING :
	case ICON :
	case FCON :
	case CALL :
	case UNARY CALL:
	case UNARY MUL:
		{  return( OTHER ); }
	case UNARY MINUS:
		if( mt1 & MDBI ) return( TYPL );
		break;

	case COMPL:
		if( mt1 & MINT ) return( TYPL );
		break;

	case UNARY AND:
		{  return( NCVT+OTHER ); }
	case INIT:
	case CM:
	case NOT:
	case CBRANCH:
	case ANDAND:
	case OROR:
		return( 0 );

	case MUL:
	case DIV:
		if( mt12 & MDBI ) return( TYMATCH );
		break;

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
		return( TYPR );

	case STREF:
		return( NCVT+OTHER );

	case RET:	/* PCC: FORCE */
		return( TYPL );

	case COLON:
		if( mt12 & MENU ) return( NCVT+PUN+PTMATCH );
		else if( mt12 & MDBI ) return( TYMATCH );
		else if( mt12 & MPTR ) return( TYPL+PTMATCH+PUN );
		else if( (mt1&MINT) && (mt2&MPTR) ) return( TYPR+PUN );
		else if( (mt1&MPTR) && (mt2&MINT) ) return( TYPL+PUN );
		else if( mt12 & MSTR ) return( NCVT+TYPL+OTHER );
		break;

	case ASSIGN:
	case RETURN:
		if( mt12 & MSTR ) return( LVAL+NCVT+TYPL+OTHER );
	case CAST:
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
		if( mt12 & MDBI ) return( LVAL+TYMATCH );
		break;

	case ASG MOD:
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
		else if( (mt1&MPTR) && (mt2&MINT) ) return( TYPL+LVAL+CVTR );
		break;

	case MINUS:
		if( mt12 & MPTR ) return( CVTO+PTMATCH+PUN );
		if( mt2 & MPTR ) break;
	case PLUS:
		if( mt12 & MDBI ) return( TYMATCH );
		else if( (mt1&MPTR) && (mt2&MINT) ) return( TYPL+CVTR );
		else if( (mt1&MINT) && (mt2&MPTR) ) return( TYPR+CVTL );

		}
	uerror( "operands of %s have incompatible types", opst[o] );
	return( NCVT );
	}

moditype( ty ) TWORD ty; {

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

NODE *
doszof( p )  register NODE *p; {
	/* do sizeof p */
	register i;

	/* whatever is the meaning of this if it is a bitfield? */
	i = tsize( p->type, p->cdim, p->csiz )/SZCHAR;

	tfree(p);
	if( i == 0 ) werror( "sizeof returns 0" );
	else if( i < 0 ) werror( "sizeof overflow; may be meaningful if cast unsigned" );
	return( bcon( i ) );
	}

prtdcon( p ) register NODE *p; {
	int lab;
	long size;
	long pos;
	int l;

	if( p->op == FCON ){ /* a new constant goes to the .D file, an old one is found by its label */
		pos = 0;
		if( (lab = fltlab( p->dval )) == crslab ){
			defalign( ALDOUBLE );
			size = SZDOUBLE/SZCHAR;
			if( sectfull( DATA, (long) (SZDOUBLE/SZCHAR) ) ) newsect( DATA );
			sectadd( DATA, size );
			l = lab = getlab();
			fprintf( dfile, "L%d: \n", lab );
			pos = ftell( dfile );
			fincode( p->dval, SZDOUBLE );
			}
		p->lval = pos;
		p->rval = -lab;
		p->type = DOUBLE;
		}
	}

ecomp( p ) register NODE *p; {
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

prtree(p) register NODE *p; {

	register struct symtab *q;
	register ty;
	register n;

	ty = optype(p->op);

	fprintf( outfile, "%d\t", p->op );

	if( ty == LTYPE ) {
		fprintf( outfile, CONFMT, p->lval );
		putc( '\t', outfile );
		}
	if( ty != BITYPE ) {
		if( p->op == ICON ) fprintf( outfile, "0\t" );
		else if( p->op == NAME ) fprintf( outfile, "%d\t", STP(p->rval)->symno );
		else fprintf( outfile, "%d\t", p->rval );
		}
	if( p->op == TEMP ) fprintf( outfile, "%d\t", p->csiz );

	fprintf( outfile, "%0.2o\t", p->type );

	/* handle special cases */

	switch( p->op ){

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
		fprintf( outfile, CONFMT, tsize( STRTY, p->left->cdim, p->left->csiz ) );
		fprintf( outfile, "\t%d\t\n", talign( STRTY, p->left->csiz ) );
		break;

	default:
		putc( '\n', outfile );
		}

	if( p->op == STCALL || p->op == CALL ){ /* the stack words of the arguments */
		aregs = 7;
		afregs = 19;
		afull = 0;
		n = argregs( p->right );
		if( n > maxoff ) maxoff = n;
		}

	if( ty != LTYPE ) prtree( p->left );
	if( ty == BITYPE ) prtree( p->right );

	}

argregs( p ) register NODE *p; { /* the stack words of the arguments p of a call (those that are not in registers) */
	register n;

	if( p->op == CM ) return( argregs( p->left ) + argregs( p->right ) );
	n = argsize( p );
	if( ( (p->type == FLOAT || p->type == DOUBLE) && afregs < 16 ) || afull ){
	full:
		afull = 1;
		return( n+n );
		}
	if( p->type != FLOAT && p->type != DOUBLE && aregs < 2 ) goto full;
	if( p->type == FLOAT || p->type == DOUBLE ){
		--afregs;
		return( 0 );
		}
	switch( n ){

	default:
	nofit:
		aregs = 1;
		goto full;

	case 2:
		if( --aregs & 1 ) --aregs;
		if( aregs < 2 ) goto nofit;
	case 1:
		--aregs;
		return( 0 );

	case 4:
		if( aregs != 7 ) goto nofit;
		aregs -= 4;
		return( 0 );
		}
	}

NODE *
tempnode( t, flag ) TWORD t; { /* a TEMP node of type t; flag 1: it gets the next place in the frame */
	NODE *q;

	q = block( TEMP, NIL, NIL, t, 0, INT );
	q->rval = tmpno++;
	if( flag == 1 ){
		q->csiz = tmpoff;
		tmpoff += twords( t ) * 2;
		}
	return( q );
	}








