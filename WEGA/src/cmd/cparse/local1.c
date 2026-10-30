# include "mfile1"

char *dignames[] = { "0", "1", "2" };	/* a name that starts with one of these gets $ (exname; name is a guess) */
int inwd = 0;	/* current bit offsed in word */
int word = 0;	/* word being built from fields */
char extext[128] = "";	/* the external name made by exname */

NODE *
cast( p, t ) register NODE *p; TWORD t; {
	/* cast node p to type t */

	p = buildtree( CAST, block( NAME, NIL, NIL, t, 0, (int)t ), p );
	p->left->op = FREE;
	p->op = FREE;
	return( p->right );
	}
NODE *
clocal(p) NODE *p; {

	/* this is called to do local transformations on
	   an expression tree preparitory to its being
	   written out in intermediate code.
	*/

	/* the major essential job is rewriting the
	   automatic variables and arguments in terms of
	   REG and OREG nodes */
	/* conversion ops which are not necessary are also clobbered here */
	/* in addition, any special features (such as rewriting
	   exclusive or) are easily handled here as well */

	register struct symtab *q;
	register NODE *r;
	register o;
	register m, ml;

	switch( o = p->op ){

	case NAME:
		if( p->rval < 0 ) { /* already processed; ignore... */
			return(p);
		}
		q = STP(p->rval);
		switch( q->sclass ){

		case AUTO:
		case PARAM:
			/* fake up a structure reference */
			r = block( REG, NIL, NIL, PTR+STRTY, 0, 0 );
			r->lval = 0;
			r->rval = (q->sclass==AUTO?STKREG:ARGREG);
			p = stref( block( STREF, r, p, 0, 0, 0 ) );
			break;

		case ULABEL:
		case LABEL:
		case STATIC:
			if( q->slevel == 0 ) break;
			p->lval = 0;
			p->rval = -q->offset;
			break;

		case REGISTER:
			p->op = REG;
			p->lval = 0;
			p->rval = q->offset;
			break;

		}
		break;
	case LT:
	case LE:
	case GT:
	case GE:
		if( ISPTR( p->left->type ) || ISPTR( p->right->type ) ){
			p->op += (ULT-LT);
		}
		break;

	case PCONV:
		/* do pointer conversions for char and longs */
		ml = p->left->type;
		if( ml == FLOAT )
			goto pclobber;
		if( ( ml==CHAR || ml==UCHAR || ml==LONG || ml==ULONG )
		   && p->left->op != ICON ) break;

pclobber:
		/* pointers all have the same representation;
		   the type is inherited */
		p->left->type = p->type;
		p->left->cdim = p->cdim;
		p->left->csiz = p->csiz;
		p->op = FREE;
		return( p->left );

	case SCONV:
/*
 *		m = (p->type == FLOAT || p->type == DOUBLE );
 *		ml = (p->left->type == FLOAT || p->left->type == DOUBLE );
 *		if( m != ml ) break;
 */

		/* now, look for conversions downwards */

		m = p->type;
		ml = p->left->type;
		if( p->left->op == ICON ){ /* simulate the conversion here */
			CONSZ val;
			val = p->left->lval;
			switch( m ){
			case CHAR:
				p->left->lval = (char) val;
				goto clobber;
			case UCHAR:
				p->left->lval = val & 0XFF;
				goto clobber;
			case UNSIGNED:
				p->left->lval = val & 0XFFFFL;
				break;
			case INT:
				p->left->lval = (int)val;
				break;
			}
		} else if ( p->left->op != FCON )  {
			/* meaningful ones are conversion of int to char,
			   int to short,
			   and short to char, and unsigned version of them */
			if( m==CHAR || m==UCHAR ){
				if( ml != CHAR && ml != UCHAR ) break;
			}
			else if( m==INT || m==UNSIGNED ){
				if( ml != INT && ml != UNSIGNED ) break;
			}
			else if( m==LONG || m==ULONG ) {
				if( ml != LONG && ml != ULONG ) break;
			}
			else if( m==DOUBLE ) {
				if( ml != DOUBLE ) break;
			}
			else if( m==FLOAT ) {
				if( ml != FLOAT ) break;
			}
		}

		/* clobber conversion */
		p->left->type = m; /* inherit type */
	clobber:
		p->op = FREE;
		return( p->left );  /* conversion gets clobbered */

	case PVCONV:
	case PMCONV:
		if( p->right->op != ICON ) cerror( "bad conversion", 0);
		p->op = FREE;
		return( buildtree( o==PMCONV?MUL:DIV, p->left, p->right ) );

	case PLUS:
	case MINUS:
	case LS:
	case MUL:
		/* optimize address calculations with long indexes */
		if( ISPTR( p->type ) || ISARY( p->type ) ) {
			if( p->left->type==LONG || p->left->type==ULONG )
				p->left = cast( p->left, INT );
			if( p->right->type==LONG || p->right->type==ULONG )
				p->right = cast( p->right, INT );
		}
		break;

	}

	return(p);
}
cisreg( t ) register TWORD t; { /* is an automatic variable of type t OK for a register variable */

	return( t==INT || t==UNSIGNED || t==LONG || t==ULONG || ISPTR(t) || t==ENUMTY ||
			t==DOUBLE || t==FLOAT || t==SHORT || t==USHORT );
	}
NODE *
offcon( off, t, d, s ) OFFSZ off; TWORD t; register d, s; {

	/* return a node, for structure references, which is suitable for
	   being added to a pointer of type t, in order to be off bits offset
	   into a structure */

	register NODE *p;

	/* t, d, and s are the type, dimension offset, and sizeoffset */
	/* in general they  are necessary for offcon, but not on H'well */

	p = bcon(0);
	p->lval = off/SZCHAR;
	return(p);

	}

incode( p, sz ) register NODE *p; register sz; {

	/* generate initialization code for assigning a constant c
		to a field of width sz */
	/* we assume that the proper alignment has been obtained */
	/* inoff is updated to have the proper final value */
	/* we also assume sz  < SZINT */

	word |= p->lval << (SZINT-sz-inwd);
	if( sz+inwd <= SZINT ) inwd += sz;
	inoff += sz;
	if( inwd == 8 ) fprintf( dfile, "\t.byte\t%%%x\n", word>>8 );
	else if( sz == SZINT ) fprintf( dfile, "\t.word\t%%%x\n", word );
	else if( sz == SZLONG ) fprintf( dfile, "\t.long\t%%%lx\n", p->lval );
	else return;
	inwd = 0;
	word = 0;
	}

fincode( d, sz ) double d; register sz; {
	/* output code to initialize space of size sz to the value d */
	/* the proper alignment has been obtained */
	/* inoff is updated to have the proper final value */
	/* on the target machine, write it out in octal! */

	register int *dp, *fp;
	float f;

	dp = (int *) &d;
	fp = (int *) &f;
	if( sz == SZDOUBLE )
		fprintf( dfile, "\t.word\t%%%x\n\t.word\t%%%x\n\t.word\t%%%x\n\t.word\t%%%x\n", dp[0], dp[1], dp[2], dp[3] );
	else {
		f = d;
		fprintf( dfile, "\t.word\t%%%x\n\t.word\t%%%x\n", fp[0], fp[1] );
		}
	inoff += sz;
	}

cinit( p, sz ) register NODE *p; register sz; {
	/* arrange for the initialization of p into a space of size sz */
	struct symtab *q;
	char *s;

	switch( p->left->op ){

	case NAME:
		q = STP(p->left->rval);
		if( q->sclass == STATIC && q->slevel > 0 ) fprintf( dfile, "\t.addr\tL%ld\n", q->offset );
		else {
			s = exname( q->sname );
			fprintf( dfile, "\t.addr\t%s\n", s );
			}
		break;

	case ICON:
		incode( p->left, sz );
		return;

	case LAB:
		fprintf( dfile, "\t.addr\tL%d\n", -p->left->rval );
		break;

	default:
		if( ISPTR(p->type) ) fprintf( dfile, "\t.addr\t" );
		else fprintf( dfile, "\t.word\t" );
		cprint( p );
		putc( '\n', dfile );
		}
	inoff += sz;
	}

cprint( p ) register NODE *p; { /* print the constant expression p into the .D file (name is a guess) */
	register o;
	struct symtab *q;
	char *s;

	if( (o = optype(p->op)) == LTYPE ){
		switch( p->op ){

		case NAME:
			q = STP(p->rval);
			if( q->sclass == STATIC && q->slevel > 0 ) fprintf( dfile, "L%ld", q->offset );
			else if( q->sclass == MOS ) fprintf( dfile, "%d", q->offset/SZCHAR );
			else {
				s = exname( q->sname );
				fprintf( dfile, "%s", s );
				}
			break;

		case ICON:
			fprintf( dfile, "%d", p->lval );
			break;

		case LAB:
			fprintf( dfile, "L%d", -p->rval );
			break;
			}
		return;
		}
	if( o == UTYPE ){
		cprint( p->left );
		return;
		}
	cprint( p->left );
	switch( p->op ){

	case PLUS:
	case STREF:
	case SUBSR:
		putc( '+', dfile );
		break;

	case MINUS:
		putc( '-', dfile );
		break;
		}
	cprint( p->right );
	}

vfdzero( n ){ /* define n bits of zeros in a vfd */

	if( n <= 0 ) return;

	inwd += n;
	inoff += n;
	if( inwd == 8 ){
		fprintf( dfile, "\t.byte\t%d\n", word );
		sectadd( curloc, 1L );
		}
	else if( inoff%SZINT == 0 ){
		fprintf( dfile, "\t.word\t%d\n", word );
		sectadd( curloc, 2L );
		}
	else return;
	inwd = 0;
	word = 0;
	}

char *
exname( s ){ /* make a name look like an external name in the local machine */
	register char *p, *q;
	register i;
	char c;

	p = &ntab[s];
	q = extext;
	if( (c = *p) == *dignames[0] || *dignames[1] == c || *dignames[2] == c ) *q++ = '$';
	else *q++ = '_';
	for( i=1; *p; ++i ){
		if( i > ncharsig ) break;
		*q++ = *p++;
		}
	*q = '\0';
	if( extext[0] == '_' ) extext[ncharsig] = '\0';
	else extext[ncharsig+1] = '\0';
	return( extext );
	}

ctype( type ) TWORD type; { /* map types which are not defined on the local machine */
	switch( BTYPE(type) ){

	case SHORT:
		MODTYPE(type,INT);
		break;

	case USHORT:
		MODTYPE(type,UNSIGNED);
		}
	return( type );
	}

noinit(){ /* curid is a variable which is defined but
	is not initialized (and not a function );
	This routine returns the stroage class for an uninitialized declaration */

	return(EXTERN);

	}

commdec( q ) register struct symtab *q; { /* make a common declaration for q */
	OFFSZ sz;
	char *s;

	lineid();
	sz = tsize( q->stype, q->dimoff, q->sizoff )/SZCHAR;
	s = exname( q->sname );
	fprintf( dfile, ".comm\t%ld,\t%s;\n", sz, s );
	if( sz > 0xfffe ) uerror( "size too large" );
	}

isitfloat( s ) char *s; {
	double atof();
	dcon = atof(s);
	return( FCON );
	}

ecode( p ) NODE *p; {

	/* walk the tree and write out the nodes.. */

	if( nerrors ) return;
	if( p->op == CBRANCH && p->left->op == FREE ){
		p->right->op = FREE;
		p->op = FREE;
		}
	if( p->op == FREE ) return;
	putc( '.', outfile );
	fprintf( outfile, "%d\t%s\n", lineno, ftitle );
	prtree( p );
	}

syserr( s ) char *s; { /* a system call failed: die */
	fprintf( stderr, "system call error: " );
	perror( s );
	dexit( 64 );
	}
