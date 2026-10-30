# include "mfile1"

extern int ddebug;
struct symtab *relook();

falloc( p, w, new, pty )  register struct symtab *p; NODE *pty; {
	/* allocate a field of width w */
	/* new is 0 if new entry, 1 if redefinition, -1 if alignment */

	register al,sz,type;

	type = (new<0)? pty->type : p->stype;

	/* this must be fixed to use the current type in alignments */
	switch( new<0?pty->type:p->stype ){

	case ENUMTY:
		{
			int s;
			s = new<0 ? pty->csiz : p->sizoff;
			al = dimtab[s+2];
			sz = dimtab[s];
			break;
			}

	case CHAR:
	case UCHAR:
		al = ALCHAR;
		sz = SZCHAR;
		break;

	case SHORT:
	case USHORT:
		al = ALSHORT;
		sz = SZSHORT;
		break;

	case INT:
	case UNSIGNED:
		al = ALINT;
		sz = SZINT;
		break;
#ifdef LONGFIELDS

	case LONG:
	case ULONG:
		al = ALLONG;
		sz = SZLONG;
		break;
#endif

	default:
		if( new < 0 ) {
			uerror( "illegal field type" );
			al = ALINT;
			}
		else {
			al = fldal( p->stype );
			sz =SZINT;
			}
		}

	if( w > sz ) {
		uerror( "field too big");
		w = sz;
		}

	if( w == 0 ){ /* align only */
		SETOFF( strucoff, al );
		if( new >= 0 ) uerror( "zero size field");
		return(0);
		}

	if( strucoff%al + w > sz ) SETOFF( strucoff, al );
	if( new < 0 ) {
		strucoff += w;  /* we know it will fit */
		return(0);
		}

	/* establish the field */

	if( new == 1 ) { /* previous definition */
		if( p->offset != strucoff || p->sclass != (FIELD|w) ) return(1);
		}
	p->offset = strucoff;
	strucoff += w;
	p->stype = type;
	fldty( p );
	return(0);
	}

nidcl( p ) NODE *p; { /* handle unitialized declarations */
	/* assumed to be not functions */
	register class;
	register commflag;  /* flag for labelled common declarations */

	commflag = 0;

	/* compute class */
	if( (class=curclass) == SNULL ){
		if( blevel > 1 ) class = AUTO;
		else if( blevel != 0 || instruct ) cerror( "nidcl error" );
		else { /* blevel = 0 */
			class = noinit();
			if( class == EXTERN ) commflag = 1;
			}
		}

	defid( p, class );

	if( class==EXTDEF || class==STATIC ){
		/* simulate initialization by 0 */
		beginit(p->rval);
		endinit();
		}
	if( commflag ) commdec( p->rval );
	}

TWORD
types( t1, t2, t3 ) TWORD t1, t2, t3; {
	/* return a basic type from basic types t1, t2, and t3 */

	TWORD t[3], noun, adj, unsg;
	register i;

	t[0] = t1;
	t[1] = t2;
	t[2] = t3;

	unsg = INT;  /* INT or UNSIGNED */
	noun = UNDEF;  /* INT, CHAR, or FLOAT */
	adj = INT;  /* INT, LONG, or SHORT */

	for( i=0; i<3; ++i ){
		switch( t[i] ){

		default:
		bad:
			uerror( "illegal type combination" );
			return( INT );

		case UNDEF:
			continue;

		case UNSIGNED:
			if( unsg != INT ) goto bad;
			unsg = UNSIGNED;
			continue;

		case LONG:
		case SHORT:
			if( adj != INT ) goto bad;
			adj = t[i];
			continue;

		case INT:
		case CHAR:
		case FLOAT:
			if( noun != UNDEF ) goto bad;
			noun = t[i];
			continue;
			}
		}

	/* now, construct final type */
	if( noun == UNDEF ) noun = INT;
	else if( noun == FLOAT ){
		if( unsg != INT || adj == SHORT ) goto bad;
		return( adj==LONG ? DOUBLE : FLOAT );
		}
	else if( noun == CHAR && adj != INT ) goto bad;

	/* now, noun is INT or CHAR */
	if( adj != INT ) noun = adj;
	if( unsg == UNSIGNED ) return( noun + (UNSIGNED-INT) );
	else return( noun );
	}

NODE *
tymerge( typ, idp ) NODE *typ, *idp; {
	/* merge type typ with identifier idp  */

	register unsigned t;
	register i;
	extern int eprint();

	if( typ->op != TYPE ) cerror( "tymerge: arg 1" );
	if(idp == NIL ) return( NIL );

	if( ddebug > 2 ) fwalk( idp, eprint, 0 );

	idp->type = typ->type;
	idp->cdim = curdim;
	tyreduce( idp );
	idp->csiz = typ->csiz;

	for( t=typ->type, i=typ->cdim; t&TMASK; t = DECREF(t) ){
		if( ISARY(t) ) dstash( dimtab[i++] );
		}

	/* now idp is a single node: fix up type */

	idp->type = ctype( idp->type );

	if( (t = BTYPE(idp->type)) != STRTY && t != UNIONTY && t != ENUMTY ){
		idp->csiz = t;  /* in case ctype has rewritten things */
		}

	return( idp );
	}

tyreduce( p ) register NODE *p; {

	/* build a type, and stash away dimensions, from a parse tree of the declaration */
	/* the type is build top down, the dimensions bottom up */
	register o, temp;
	register unsigned t;

	o = p->op;
	p->op = FREE;

	if( o == NAME ) return;

	t = INCREF( p->type );
	if( o == UNARY CALL ) t += (FTN-PTR);
	else if( o == LB ){
		t += (ARY-PTR);
		temp = p->right->lval;
		p->right->op = FREE;
		}

	p->left->type = t;
	tyreduce( p->left );

	if( o == LB ) dstash( (OFFSZ) temp );

	p->rval = p->left->rval;
	p->type = p->left->type;

	}

fixtype( p, class ) register NODE *p; {
	register unsigned t, type;
	register mod1, mod2;
	/* fix up the types, and check for legality */

	if( (type = p->type) == UNDEF ) return;
	if( mod2 = (type&TMASK) ){
		t = DECREF(type);
		while( mod1=mod2, mod2 = (t&TMASK) ){
			if( mod1 == ARY && mod2 == FTN ){
				uerror( "array of functions is illegal" );
				type = 0;
				}
			else if( mod1 == FTN && ( mod2 == ARY || mod2 == FTN ) ){
				uerror( "function returns illegal type" );
				type = 0;
				}
			t = DECREF(t);
			}
		}

	/* detect function arguments, watching out for structure declarations */

	if( class==SNULL && blevel==1 && !(instruct&(INSTRUCT|INUNION)) ) class = PARAM;
	if( class == PARAM || ( class==REGISTER && blevel==1 ) ){
		if( type == FLOAT ) type = DOUBLE;
		else if( ISARY(type) ){
			++p->cdim;
			type += (PTR-ARY);
			}
		else if( ISFTN(type) ) type = INCREF(type);

		}

	if( instruct && ISFTN(type) ){
		uerror( "function illegal in structure or union" );
		type = INCREF(type);
		}
	p->type = type;
	}

uclass( class ) register class; {
	/* give undefined version of class */
	if( class == SNULL ) return( EXTERN );
	else if( class == STATIC ) return( USTATIC );
	else if( class == FORTRAN ) return( UFORTRAN );
	else return( class );
	}

fixclass( class, type ) TWORD type; {

	/* first, fix null class */

	if( class == SNULL ){
		if( instruct&INSTRUCT ) class = MOS;
		else if( instruct&INUNION ) class = MOU;
		else if( blevel == 0 ) class = EXTDEF;
		else if( blevel == 1 ) class = PARAM;
		else class = AUTO;

		}

	/* now, do general checking */

	if( ISFTN( type ) ){
		switch( class ) {
		default:
			uerror( "function has illegal storage class" );
		case AUTO:
			class = EXTERN;
		case EXTERN:
		case EXTDEF:
		case FORTRAN:
		case TYPEDEF:
		case STATIC:
		case UFORTRAN:
		case USTATIC:
			;
			}
		}

	if( class&FIELD ){
		if( !(instruct&INSTRUCT) ) uerror( "illegal use of field" );
		return( class );
		}

	switch( class ){

	case MOU:
		if( !(instruct&INUNION) ) uerror( "illegal class" );
		return( class );

	case MOS:
		if( !(instruct&INSTRUCT) ) uerror( "illegal class" );
		return( class );

	case MOE:
		if( instruct & (INSTRUCT|INUNION) ) uerror( "illegal class" );
		return( class );

	case REGISTER:
		if( blevel == 0 ) uerror( "illegal register declaration" );
		else if( regvar >= MINRVAR && cisreg( type ) ) return( class );
		if( blevel == 1 ) return( PARAM );
		else return( AUTO );

	case AUTO:
	case LABEL:
	case ULABEL:
		if( blevel < 2 ) uerror( "illegal class" );
		return( class );

	case PARAM:
		if( blevel != 1 ) uerror( "illegal class" );
		return( class );

	case UFORTRAN:
	case FORTRAN:
# ifdef NOFORTRAN
			NOFORTRAN;    /* a condition which can regulate the FORTRAN usage */
# endif
		if( !ISFTN(type) ) uerror( "fortran declaration must apply to function" );
		else {
			type = DECREF(type);
			if( ISFTN(type) || ISARY(type) || ISPTR(type) ) {
				uerror( "fortran function has wrong type" );
				}
			}
	case STNAME:
	case UNAME:
	case ENAME:
	case EXTERN:
	case STATIC:
	case EXTDEF:
	case TYPEDEF:
	case USTATIC:
		return( class );

	default:
		cerror( "illegal class: %d", class );
		/* NOTREACHED */

		}
	}

/* f94a6: not written yet (0x94a6..0x94e8 of /lib/cparse) */
f94a6()
{
}

/* f94e8: not written yet (0x94e8..0x96e2 of /lib/cparse) */
f94e8()
{
}

/* f96e2: not written yet (0x96e2..0x985a of /lib/cparse) */
f96e2()
{
}

/* f985a: not written yet (0x985a..0x995c of /lib/cparse) */
f985a()
{
}

unhide( p ) register struct symtab *p; {
	register struct symtab *q;
	register s, j;

	s = p->sflags & SMOS;
	q = p;

	for(;;){

		if( q == stab ) q = &stab[SYMTSZ-1];
		else --q;

		if( q == p ) break;

		if( (q->sflags&SMOS) == s ){
			for( j =0; j<NCHNAM; ++j ) if( p->sname[j] != q->sname[j] ) break;
			if( j == NCHNAM ){ /* found the name */
				q->sflags &= ~SHIDDEN;
				if( ddebug ) printf( "unhide uncovered %d from %d\n", q-stab,p-stab);
				return;
				}
			}

		}
	cerror( "unhide fails" );
	}

/* f99ca: not written yet (0x99ca..0x9ae6 of /lib/cparse) */
f99ca()
{
}

clearst( lev ){ /* clear entries of internal scope  from the symbol table */
	register struct symtab *p, *q, *r;
	register int temp, rehash;

	temp = lineno;
	aobeg();

	/* first, find an empty slot to prevent newly hashed entries from
	   being slopped into... */

	for( q=stab; q< &stab[SYMTSZ]; ++q ){
		if( q->stype == TNULL )goto search;
		}

	cerror( "symbol table full");

	search:
	p = q;

	for(;;){
		if( p->stype == TNULL ) {
			rehash = 0;
			goto next;
			}
		lineno = p->suse;
		if( lineno < 0 ) lineno = - lineno;
		if( p->slevel>lev ){ /* must clobber */
			if( p->stype == UNDEF || ( p->sclass == ULABEL && lev < 2 ) ){
				lineno = temp;
				uerror( "%.8s undefined", p->sname );
				}
			else aocode(p);
			if (ddebug) printf("removing %8s from stab[ %d], flags %o level %d\n",
				p->sname,p-stab,p->sflags,p->slevel);
			if( p->sflags & SHIDES ) unhide(p);
			p->stype = TNULL;
			rehash = 1;
			goto next;
			}
		if( rehash ){
			if( (r=relook(p)) != p ){
				movestab( r, p );
				p->stype = TNULL;
				}
			}
		next:
		if( ++p >= &stab[SYMTSZ] ) p = stab;
		if( p == q ) break;
		}
	lineno = temp;
	aoend();
	}

/* f9bd4: not written yet (0x9bd4..0x9c0c of /lib/cparse) */
f9bd4()
{
}

/* f9c0c: not written yet (0x9c0c..0x9c38 of /lib/cparse) */
f9c0c()
{
}

/* f9c38: not written yet (0x9c38..0x9c62 of /lib/cparse) */
f9c38()
{
}

/* f9c62: not written yet (0x9c62..0x9e04 of /lib/cparse) */
f9c62()
{
}

/* f9e04: not written yet (0x9e04..0x9e80 of /lib/cparse) */
f9e04()
{
}

/* f9e80: not written yet (0x9e80..0x9ea6 of /lib/cparse) */
f9e80()
{
}

/* f9ea6: not written yet (0x9ea6..0x9ece of /lib/cparse) */
f9ea6()
{
}

/* f9ece: not written yet (0x9ece..0x9f04 of /lib/cparse) */
f9ece()
{
}

/* f9f04: not written yet (0x9f04..0x9f06 of /lib/cparse) */
f9f04()
{
}

