# include "mfile1"

static char sccsid[] = "@[$]local1.c\t2.7  08/24/83 14:27:54 Copr. 1981 87wega3.2.";

char *dignames[] = { "0", "1", "2" };	/* a name that starts with one of these gets $ (exname; name is a guess) */

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

	/* WEGA: labels of statics, float and unsigned compares, conversions that change nothing,
	   pointer arithmetic with long operands, char operands of divisions and compares */

	register o;
	register ml, m;
	register struct symtab *q;
	register NODE *r, *l;
	register x;
	int i;
	long val;

	l = p->left;
	r = p->right;
	o = p->op;

	switch( o ){

	case NAME:
		q = STP(p->rval);
		if( (q->sclass == LABEL || q->sclass == ULABEL) && q->slevel ){
			p->op = LAB;
			p->lval = 0;
			p->rval = -q->offset;
			}
		break;

	case EQ:
	case NE:
		if( l->type == DOUBLE || l->type == FLOAT || r->type == DOUBLE || r->type == FLOAT ){
		flt:
			p->op += FEQ-EQ;
			}
		break;

	case LT:
	case LE:
	case GT:
	case GE:
		if( l->type == DOUBLE || l->type == FLOAT || r->type == DOUBLE || r->type == FLOAT ) goto flt;
		if( ISPTR(l->type) || ISPTR(r->type) ) p->op += ULE-LE;
		break;

	case PCONV:
		m = l->type;
		if( (m == CHAR || m == UCHAR || m == LONG || m == ULONG) && l->op != ICON ){
			p->op = o = SCONV;
			break;
			}
		goto paint;

	case SCONV:
		ml = p->type == FLOAT || p->type == DOUBLE;
		m = l->type == FLOAT || l->type == DOUBLE;
		if( ml ) break;
		if( m ) break;
		while( l->op == SCONV && tbytes( l->type ) > tbytes( p->type ) &&
				l->left->type != DOUBLE && l->left->type != FLOAT ){
			l->op = FREE;
			p->left = l = l->left;
			}
		ml = p->type;
		m = l->type;
		if( l->op == ICON ){ /* fold the conversion into the constant */
			val = l->lval;
			switch( ml ){

			case CHAR:
				l->lval = (char) val;
				break;

			case SHORT:
			case INT:
				l->lval = (int) val;
				break;

			case UCHAR:
				l->lval = val & 0377;
				break;

			case USHORT:
			case UNSIGNED:
				l->lval = val & 0177777;
				break;
				}
		settype:
			l->type = ml;
			}
		else if( l->op == MUL && twords( ml ) - twords( m ) == 1 ) goto settype;	/* the product is long */
		else if( tbytes( ml ) != tbytes( m ) ) break;
		if( tbytes( ml ) == tbytes( m ) ){
		paint:
			l->type = p->type;
			l->cdim = p->cdim;
			l->csiz = p->csiz;
			}
		p->op = FREE;
		return( l );

	case ASSIGN:
		if( r->op == SCONV ){
			ml = r->type;
			m = r->left->type;
			}
		break;

	case PVCONV:
		if( nncon( r ) && (i = ispow2( r->lval )) > 0 ){
			p->op = FREE;
			r->lval = i;
			return( buildtree( RS, l, r ) );
			}
	case PMCONV:
		if( r->op != ICON ) cerror( "bad conversion", 0 );
		p->op = FREE;
		return( buildtree( o == PMCONV ? MUL : DIV, l, r ) );

	case PLUS:
	case MINUS:
	case MUL:
	case LS:	/* WEGA: pointer arithmetic is done in words */
		if( ISPTR(p->type) || ISARY(p->type) ){
			if( l->type == LONG || l->type == ULONG ) l = p->left = cast( l, INT );
			if( r->type == LONG || r->type == ULONG ) r = p->right = cast( r, INT );
			}
		break;

	case DIV:
	case MOD:	/* WEGA: a char operand is used without its conversion */
		if( l->op == SCONV && bsize( l->type ) == SZINT && bsize( l->left->type ) == SZCHAR ){
			l->op = FREE;
			l = p->left = l->left;
			}
		break;

	case UNARY MINUS:
		if( p->type == UCHAR ) p->type = CHAR;
		break;
		}

	/* WEGA: a compare of a char with a constant that fits */
	if( logop( p->op ) && r->op == ICON && l->op == SCONV && (l->left->type == CHAR || l->left->type == UCHAR) &&
			r->lval >= -256 && r->lval <= 255 ){
		l->op = FREE;
		p->left = l = l->left;
		r->type = l->type;
		if( !logop( p->op ) ) p->type = l->type;
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

int inwd = 0;	/* current bit offsed in word */
int word = 0;	/* word being built from fields */

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

		case ICON:
			fprintf( dfile, "%d", p->lval );
			break;

		case NAME:
			q = STP(p->rval);
			if( q->sclass == STATIC && q->slevel > 0 ) fprintf( dfile, "L%ld", q->offset );
			else if( q->sclass == MOS ) fprintf( dfile, "%d", q->offset/SZCHAR );
			else {
				s = exname( q->sname );
				fprintf( dfile, "%s", s );
				}
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

char extext[128] = "";	/* the external name made by exname */

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
