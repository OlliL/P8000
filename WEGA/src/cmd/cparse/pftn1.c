# include "mfile1"


static char sccsid[] = "@[$]pftn1.c\t2.7  08/24/83 14:40:11 Copr. 1981 87wega3.2.";

	/* defines used for getting things off of the initialization stack */


int pregfull = 0;	/* the parameter registers are used up (getpreg) */
int pregvar, pfregvar;	/* the next parameter register, the next float one */









































#ifndef checkst
#endif

defid( q, class )  NODE *q; {
	register struct symtab *p;
	struct symtab *idp;
	TWORD type;
	TWORD stp;
	int scl;
	int dsym, ddef;
	int slev, temp;
	int len;
	char *cp1, *cp2;	/* cp1 also walks over paramstk */
# define PSYM (*(struct symtab **)cp1)

	if( q == NIL ) return;  /* an error was detected */

	if( q < node || q >= &node[TREESZ] ) cerror( "defid call" );

	idp = STP(q->rval);

	if( idp == 0 ) cerror( "tyreduce" );
	p = idp;

	fixtype( q, class );

	type = q->type;
	class = fixclass( class, type );

	stp = p->stype;
	slev = p->slevel;

	if( stp == UNDEF|| stp == FARG ){
		if( blevel==1 && stp!=FARG ) switch( class ){

		default:
			if(!(class&FIELD)) uerror( "declared argument %s is missing", &ntab[p->sname] );
		case MOS:
		case STNAME:
		case MOU:
		case UNAME:
		case MOE:
		case ENAME:
		case TYPEDEF:
			;
			}
		goto enter;
		}
	scl = p->sclass;
	if( class == PARAM && blevel == 1 && stp != FARG && !(class&FIELD) )
		uerror( "declared argument %s is missing", &ntab[p->sname] );
	if( type != stp ) goto mismatch;
	/* test (and possibly adjust) dimensions */
	dsym = p->dimoff;
	ddef = q->cdim;
	for( temp=type; temp&TMASK; temp = DECREF(temp) ){
		if( ISARY(temp) ){
			if( dimtab[dsym] ){
				if( dimtab[ddef]!=0 && dimtab[dsym] != dimtab[ddef] ){
				mismatch:	/* WEGA: this block stands here in the original */
				/* WEGA: members and structure names have their own name space */
				if( ( (class==MOU || class==MOS || class&FIELD) && (scl==MOU || scl==MOS || scl&FIELD) ) ||
						( (class==MOU || class==MOS || class&FIELD) && (scl==UNAME || scl==STNAME) ) ||
						( (class==UNAME || class==STNAME) && (scl==MOU || scl==MOS || class&FIELD) ) ){
					p->sflags |= SMEMB;
					for( cp1 = (char *) &paramstk[paramno-1]; PSYM; cp1 -= sizeof(int) ){
						if( PSYM->sclass == STNAME || PSYM->sclass == UNAME ) break;
						if( PSYM->sflags&SMEMB && !strcmp( &ntab[p->sname], &ntab[PSYM->sname] ) ){
							uerror( "redeclaration1 of: %s", &ntab[p->sname] );
							break;
							}
						}
					p = copysym( &idp );
					goto enter;
					}
					goto redecl;
					}
				}
			else dimtab[dsym] = dimtab[ddef];
			++dsym;
			++ddef;
			}
		}

	/* check that redeclarations are to the same structure */
	if( (temp==STRTY||temp==UNIONTY||temp==ENUMTY) && p->sizoff != q->csiz &&
			class != STNAME && class != UNAME && class != ENAME ) {
		goto mismatch;
		}

	scl = ( p->sclass );

	if( class&FIELD ){
		/* redefinition */
		if( !falloc( p, class&FLDSIZ, 1, NIL ) ) {
			/* successful allocation */
			psave( (int) idp );
			return;
			}
		/* blew it: resume at end of switch... */
		}

	else switch( class ){

	case EXTERN:
		switch( scl ){
		case STATIC:
		case USTATIC:
			if( slev==0 ) return;
			break;
		case EXTDEF:
		case EXTERN:
			return;
			}
		break;

	case STATIC:
		if( scl==USTATIC || (scl==EXTERN && blevel==0) ){
			p->sclass = STATIC;
			if( ISFTN(type) ) curftn = idp;
			return;
			}
		break;

	case USTATIC:
		if( scl==STATIC || scl==USTATIC ) return;
		break;

	case LABEL:
		if( scl == ULABEL ){
			p->sclass = LABEL;
			deflab( (int) p->offset );
			return;
			}
		break;

	case TYPEDEF:
		if( scl == class ) return;
		break;

	case MOU:
	case MOS:
		if( scl == class ) {
			if( oalloc( p, &strucoff ) ) break;
			if( class == MOU ) strucoff = 0;
			psave( (int) idp );
			return;
			}
		break;

	case MOE:
		if( scl == class ){
			if( p->offset!= strucoff++ ) break;
			psave( (int) idp );
			}
		break;

	case EXTDEF:
		if( scl == EXTERN ) {
			p->sclass = EXTDEF;
			if( ISFTN(type) ) curftn = idp;
			return;
			}
		break;

	case STNAME:
	case UNAME:
	case ENAME:
		if( scl != class ) break;
		if( dimtab[p->sizoff] == 0 ) return;  /* previous entry just a mention */
		break;

	case ULABEL:
		if( scl == LABEL || scl == ULABEL ) return;
	case PARAM:
	case AUTO:
	case REGISTER:
		;  /* mismatch.. */

		}
	goto mismatch;

	redecl:
	if( blevel > slev && class != EXTERN && !( class == LABEL && slev >= 2 ) ){
		q->rval = (int) (idp = hide( p ));
		p = idp;
		goto enter;
		}
	uerror( "redeclaration of %s", &ntab[p->sname] );
	if( class==EXTDEF && ISFTN(type) ) curftn = idp;
	return;

	enter:  /* make a new entry */

	if( type == UNDEF ) uerror( "%s", "illegal use of void type" );
	p->stype = type;
	p->sclass = class;
	p->slevel = blevel;
	p->sfld8 = ftnno+1;
	p->offset = NOOFFSET;
	p->suse = lineno;
	p->symno = symcount++;
	if( class == STNAME || class == UNAME || class == ENAME ) {
		p->sizoff = curdim;
		dstash( (OFFSZ) 0 );  /* size */
		dstash( (OFFSZ) -1 ); /* index to members of str or union */
		dstash( (OFFSZ) ALSTRUCT );  /* alignment */
		dstash( (OFFSZ) idp );
		}
	else {
		switch( BTYPE(type) ){
		case STRTY:
		case UNIONTY:
		case ENUMTY:
			p->sizoff = q->csiz;
			break;
		default:
			p->sizoff = BTYPE(type);
			}
		}

	/* copy dimensions */

	p->dimoff = q->cdim;

	/* allocate offsets */
	if( class&FIELD ){
		falloc( p, class&FLDSIZ, 0, NIL );  /* new entry */
		psave( (int) idp );
		}
	else switch( class ){

	case AUTO:
		oalloc( p, &autooff );
		break;
	case STATIC:
	case EXTDEF:
		p->offset = getlab();
		if( ISFTN(type) ) curftn = idp;
		break;
	case ULABEL:
	case LABEL:
		p->offset = getlab();
		p->slevel = 2;
		if( class == LABEL ){
			locctr( PROG );
			deflab( (int) p->offset );
			}
		break;

	case EXTERN:
		p->offset = getlab();
		p->slevel = 0;
		if( blevel ) refid( idp );
		if( blevel ){ /* the name goes to the globals at the top of ntab */
			len = strlen( &ntab[p->sname] ) + 1;
			ntab_globals -= len;
			cp1 = &ntab[ntab_globals];
			cp2 = &ntab[p->sname];
			p->sname = ntab_globals;
			while( *cp1++ = *cp2 ) ++cp2;
			ntabindx -= len;
			if( ntabindx > ntab_globals ){
				fprintf( stderr, "ntabindx = %d, ntab_gl obals = %d\n", ntabindx, ntab_globals );
				cerror( "name table overflow - 1" );
				}
			}
		break;
	case MOU:
	case MOS:
		oalloc( p, &strucoff );
		if( class == MOU ) strucoff = 0;
		psave( (int) idp );
		break;

	case MOE:
		p->offset = strucoff++;
		psave( (int) idp );
		break;
	case REGISTER:
		if( p->stype == DOUBLE || p->stype == FLOAT ){
			p->sfldb = fregvar--;
			if( fminrvar > fregvar ) fminrvar = fregvar;
			}
		else setreg( p, p->stype );
		if( blevel == 1 ) p->sflags |= SSET;
		break;
		}

	}

psave( i ){
	if( paramno >= PARAMSZ ){
		cerror( "parameter stack overflow");
		}
	paramstk[ paramno++ ] = i;
	}

psavel( l ) long l; { /* a long on the parameter stack (two words) */
	register int *ip;

	if( paramno >= PARAMSZ-1 ){
		cerror( "parameter stack overflow");
		}
	*(long *)&paramstk[paramno] = l;
	paramno += 2;
	}

int firstfn = 1;	/* no function has been ended yet */
long maxoff;	/* written into the function head by ftnend (set by initpreg) */
long dpos;	/* position of the .D file after the last function */

ftnend(){ /* end of function */
	long pos;

	if( retlab != NOLAB ){ /* inside a real function */
		efcode();
		putnames();
		puttags();
		putrefs();
		sheader();
		}
	retstat = 0;
	tcheck();
	curclass = SNULL;
	brklab = contlab = retlab = NOLAB;
	flostat = 0;
	if( nerrors == 0 ){
		if( psavbc != & asavbc[0] ) cerror("bcsave error");
		if( paramno != 0 ) cerror("parameter reset error");
		if( swx != 0 ) cerror( "switch error");
		}
	psavbc = &asavbc[0];
	paramno = 0;
	minrvar = regvar = MAXRVAR;
	fminrvar = fregvar = MAXFRVAR;
	lregvar = MAXLRVAR;
	reached = 1;
	swx = 0;
	swp = swtab;
	if( firstfn ){
		firstfn = 0;
		fprintf( dfile, "0000000\t\n" );
		}
	else {
		locctr(DATA);
		if( ftell( outfile ) > ftnpos ){ /* the sizes into the function head of the .I file */
			fseek( outfile, ftnpos, 0 );
			fprintf( outfile, "%05.5ld\t", maxoff );
			fprintf( outfile, "%04.4d", tmpoff );
			fseek( outfile, 0L, 2 );
			}
		locctr(PROG);
		pos = ftell( dfile );	/* the size of the data of the function into the .D file */
		fseek( dfile, dpos-9, 0 );
		fprintf( dfile, "%07.7ld", pos-dpos );
		fseek( dfile, 0L, 2 );
		fprintf( dfile, "0000000\t\n" );
		}
	dpos = ftell( dfile );
	}

argalloc( a, n ) NODE *a[]; { /* the offsets of the arguments a[0] ... a[n-1] that are not in registers */
	register i;
	register struct symtab *p, *q;
	register sz, j;

	j = 0;
	for( i=n-1; i>=0; --i ){
		p = STP(a[i]->rval);
		if( p->sfldc == 1 && p->stype != DOUBLE ){
			if( j ){
				q = STP(a[j]->rval);
				sz = (tsize( q->stype, q->dimoff, q->sizoff ) + 15) / 16;
				if( -a[j]->lval + sz != -a[i]->lval )
					autooff += (-a[i]->lval + a[j]->lval - sz) * SZINT;
				}
			j = i;
			if( oalloc( p, &autooff ) ) cerror( "bad argument" );
			}
		}
	for( i=n-1; i>=0; --i ){
		p = STP(a[i]->rval);
		if( p->sfldc == 1 && p->stype == DOUBLE ){
			if( oalloc( p, &autooff ) ) cerror( "bad argument" );
			}
		}
	}

dclargs(){
	register i, n;
	register struct symtab *p;
	register char *cp;
	register NODE *q, *t;
	register struct symtab *s;
	NODE *args[PARAMSZ];
	OFFSZ off;

	initpreg();
	n = 0;
	tmpno = 0;
	tmpoff = 0;
	off = ARGINIT;
	for( i=n; i<paramno; ++i ){
		p = STP(paramstk[i]);
		{ /* the name of a parameter goes to the locals of ntab */
			char *dst, *src;

			dst = &ntab[ntabindx];
			src = &ntab[p->sname];
			p->sname = ntabindx;
			while( *dst++ = *src++ ) ++ntabindx;
			++ntabindx;
			if( ntabindx > ntab_globals ){
				fprintf( outfile, "ntabindx = %d, ntab_globals = %d\n", ntabindx, ntab_globals );
				cerror( "name table overflow - 2" );
				}
			}
		if( p->stype == FARG ) {
			q = block( NAME, NIL, NIL, INT, 0, INT );
			q->rval = (int) p;
			defid( q, PARAM );
			}
		else {
			q = block( NAME, NIL, NIL, p->stype, 0, p->sizoff );
			q->rval = (int) p;
			}
		args[n++] = q;
		q->lval = getpreg( p, p->stype );
		if( p->sfldc == 2 ){ /* on the stack: the offset */
			long o;
			char c;

			s = STP(paramstk[i]);
			c = s->sclass;
			o = s->offset;
			s->sclass = PARAM;
			s->offset = NOOFFSET;
			oalloc( s, &off );
			q->lval = s->offset/SZCHAR;
			s->sclass = c;
			s->offset = o;
			}
		else q->lval = -q->lval;
		}
	locctr(PROG);
	defalign(ALINT);
	if( paramno > 0 ){
		argalloc( args, paramno );
		t = args[0];
		for( i=1; i<n; ++i ) t = block( CM, args[i], t, 0, 0, 0 );
		q = block( NAME, NIL, NIL, curftn->stype, 0, curftn->sizoff );
		q->rval = (int) curftn;
		t = block( PROC, q, t, q->type, 0, 0 );
		}
	else {
		q = block( NAME, NIL, NIL, curftn->stype, 0, curftn->sizoff );
		q->rval = (int) curftn;
		t = block( UPROC, q, NIL, q->type, 0, 0 );
		}
	bfcode( paramstk, paramno );
	ecode( t );
	tfree( t );
	paramno = 0;
	}

setreg( p, t ) register struct symtab *p; register TWORD t; { /* a register for the register variable p of type t */
	switch( t ){

	default:
		p->sfldb = regvar--;
		if( minrvar > regvar ) minrvar = regvar;
		if( regvar == lregvar ) lregvar -= 2;
		else regvar = lregvar+1;
		break;

	case LONG:
	case ULONG:
		p->sfldb = lregvar;
		if( lregvar-1 < minrvar ) minrvar = lregvar-1;
		if( lregvar+1 == regvar ) regvar -= 2;
		lregvar -= 2;
		}
	if( p->slevel != 1 ) oalloc( p, &autooff );
	}

initpreg(){ /* before the parameters of a function */
	int i, j;

	pregfull = 0;
	pregvar = 7;
	pfregvar = 19;
	maxoff = 0;
	sinit();
	}

char
getpreg( p, t ) register struct symtab *p; register TWORD t; { /* the register for the parameter p of type t; 15: on the stack */
	int k;

	if( !pregfull ){
		p->sfldc = 1;
		switch( t ){

		default:
			if( ISPTR(t) ) goto intreg;
			cerror( "bad param in getpreg" );
		case STRTY:
		case UNIONTY:
			k = (tsize( STRTY, p->dimoff, p->sizoff ) + 7) / 8;
			if( k == 2 ) goto intreg;
			if( k == 4 ) goto longreg;
			if( k == 8 && pregvar == 7 ){
				pregvar -= 3;
				goto intreg;
				}
			break;
		gotreg:
			return( pregvar-- );

		case DOUBLE:
			if( pfregvar >= 16 ) return( pfregvar-- );
			break;

		case LONG:
		case ULONG:
		longreg:
			if( --pregvar & 1 ) --pregvar;
		case CHAR:
		case INT:
		case ENUMTY:
		case UCHAR:
		case UNSIGNED:
		intreg:
			if( pregvar >= 2 ) goto gotreg;
			break;
			}
		pregfull = 1;
		}
	p->sfldc = 2;
	return( 15 );
	}

NODE *
rstruct( idn, soru ){ /* reference to a structure or union, with no definition */
	register struct symtab *p;
	register NODE *q;
	p = STP(idn);
	switch( p->stype ){

	case UNDEF:
	def:
		q = block( FREE, NIL, NIL, 0, 0, 0 );
		q->rval = idn;
		q->type = (soru&INSTRUCT) ? STRTY : ( (soru&INUNION) ? UNIONTY : ENUMTY );
		defid( q, (soru&INSTRUCT) ? STNAME : ( (soru&INUNION) ? UNAME : ENAME ) );
		break;

	case STRTY:
		if( soru & INSTRUCT ) break;
		goto def;

	case UNIONTY:
		if( soru & INUNION ) break;
		goto def;

	case ENUMTY:
		if( !(soru&(INUNION|INSTRUCT)) ) break;
		goto def;

		}
	stwart = instruct;
	return( mkty( p->stype, 0, p->sizoff ) );
	}

moedef( idn ){
	register NODE *q;

	q = block( FREE, NIL, NIL, MOETY, 0, 0 );
	q -> rval = idn;
	if( idn ) defid( q, MOE );
	}

bstruct( idn, soru ) struct symtab *idn; { /* begining of structure or union declaration */
	register NODE *q;

	psave( instruct );
	psave( curclass );
	psavel( strucoff );
	strucoff = 0;
	instruct = soru;
	q = block( FREE, NIL, NIL, 0, 0, 0 );
	q->rval = (int) idn;
	if( instruct==INSTRUCT ){
		curclass = MOS;
		q->type = STRTY;
		if( idn ) defid( q, STNAME );
		}
	else if( instruct == INUNION ) {
		curclass = MOU;
		q->type = UNIONTY;
		if( idn ) defid( q, UNAME );
		}
	else { /* enum */
		curclass = MOE;
		q->type = ENUMTY;
		if( idn ) defid( q, ENAME );
		}
	psave( (int) (idn = STP(q->rval)) );
	if( idn ) idn->suse = lineno;
	return( paramno-5 );
	}

NODE *
dclstruct( oparam ){
	register struct symtab *p;
	register i, al, sa, szindex;
	register TWORD temp;
	register high, low;
	OFFSZ sz;

	/* paramstack contains:
		paramstack[ oparam ] = previous instruct
		paramstack[ oparam+1 ] = previous class
		paramstk[ oparam+2, oparam+3 ] = previous strucoff (a long)
		paramstk[ oparam+4 ] = structure name

		paramstk[ oparam+5, ... ]  = member entries

		*/


	if( (i=paramstk[oparam+4]) == 0 ){
		szindex = curdim;
		dstash( (OFFSZ) 0 );  /* size */
		dstash( (OFFSZ) -1 );  /* index to member names */
		dstash( (OFFSZ) ALSTRUCT );  /* alignment */
		dstash( (OFFSZ) -lineno );	/* the (missing) name */
		}
	else {
		szindex = STP(i)->sizoff;
		}

	temp = (instruct&INSTRUCT)?STRTY:((instruct&INUNION)?UNIONTY:ENUMTY);
	stwart = instruct = paramstk[ oparam ];
	curclass = paramstk[ oparam+1 ];
	dimtab[ szindex+1 ] = curdim;
	al = ALSTRUCT;

	high = low = 0;

	for( i = oparam+5;  i< paramno; ++i ){
		p = STP(paramstk[i]);
		dstash( (OFFSZ) (unsigned) p );
		if( temp == ENUMTY ){
			if( p->offset < low ) low = p->offset;
			if( p->offset > high ) high = p->offset;
			p->sizoff = szindex;
			continue;
			}
		sa = talign( p->stype, p->sizoff );
		if( p->sclass & FIELD ){
			sz = p->sclass&FLDSIZ;
			}
		else {
			sz = tsize( p->stype, p->dimoff, p->sizoff );
			}
		if( sz == 0 ){
			uerror( "illegal zero sized structure member: %s", &ntab[p->sname] );
			}
		if( sz > strucoff ) strucoff = sz;  /* for use with unions */
		SETOFF( al, sa );
		/* set al, the alignment, to the lcm of the alignments of the members */
		}
	dstash( (OFFSZ) 0 );  /* endmarker */
	SETOFF( strucoff, al );

	if( temp == ENUMTY ){
		TWORD ty;

		ty = INT;
		strucoff = tsize( ty, 0, (int)ty );
		dimtab[ szindex+2 ] = al = talign( ty, (int)ty );
		}

	if( strucoff == 0 ) uerror( "zero sized structure" );
	dimtab[ szindex ] = strucoff;
	dimtab[ szindex+2 ] = al;
	dimtab[ szindex+3 ] = paramstk[ oparam+4 ];

	strucoff = *(long *)&paramstk[ oparam+2 ];
	paramno = oparam;

	return( mkty( temp, 0, szindex ) );
	}

yyerror( s ) char *s; { /* error printing routine in parser */

	uerror( s );

	}

yyaccpt(){
	ftnend();
	}

ftnarg( idn ) {
	if( STP(idn)->stype != UNDEF ){
		idn = hide( STP(idn));
		}
	STP(idn)->stype = FARG;
	STP(idn)->sclass = PARAM;
	psave( idn );
	}

talign( ty, s) register unsigned ty; register s; {
	/* compute the alignment of an object with type ty, sizeoff index s */

	register i;
	if( s<0 && ty!=INT && ty!=CHAR && ty!=SHORT && ty!=UNSIGNED && ty!=UCHAR && ty!=USHORT 
#ifdef LONGFIELDS
		&& ty!=LONG && ty!=ULONG
#endif
					){
		return( fldal( ty ) );
		}

	for( i=0; i<=(SZINT-BTSHIFT-1); i+=TSHIFT ){
		switch( (ty>>i)&TMASK ){

		case FTN:
			cerror( "compiler takes alignment of function");
		case PTR:
			return( ALPOINT );
		case ARY:
			continue;
		case 0:
			break;
			}
		}

	switch( BTYPE(ty) ){

	case UNIONTY:
	case ENUMTY:
	case STRTY:
		return( dimtab[ s+2 ] );
	case CHAR:
	case UCHAR:
		return( ALCHAR );
	case FLOAT:
		return( ALFLOAT );
	case DOUBLE:
		return( ALDOUBLE );
	case LONG:
	case ULONG:
		return( ALLONG );
	case SHORT:
	case USHORT:
		return( ALSHORT );
	default:
		return( ALINT );
		}
	}

OFFSZ
tsize( ty, d, s )  register TWORD ty; {
	/* compute the size associated with type ty,
	    dimoff d, and sizoff s */
	/* BETTER NOT BE CALLED WHEN t, d, and s REFER TO A BIT FIELD... */

	register i;
	register OFFSZ mult;

	mult = 1;

	for( i=0; i<=(SZINT-BTSHIFT-1); i+=TSHIFT ){
		switch( (ty>>i)&TMASK ){

		case FTN:
			cerror( "compiler takes size of function");
		case PTR:
			return( SZPOINT * mult );
		case ARY:
			mult *= dimtab[ d++ ];
			continue;
		case 0:
			break;

			}
		}

	if( dimtab[s]==0 ) {
		uerror( "unknown size");
		return( SZINT );
		}
	return( dimtab[ s ] * mult );
	}

inforce( n ) OFFSZ n; {  /* force inoff to have the value n */
	/* inoff is updated to have the value n */
	OFFSZ wb;
	register rest;
	/* rest is used to do a lot of conversion to ints... */

	if( inoff == n ) return;
	if( inoff > n ) {
		cerror( "initialization error");
		}

	wb = inoff;
	SETOFF( wb, SZINT );

	/* wb now has the next higher word boundary */

	if( wb >= n ){ /* in the same word */
		rest = n - inoff;
		vfdzero( rest );
		return;
		}

	/* otherwise, extend inoff to be word aligned */

	rest = wb - inoff;
	vfdzero( rest );

	/* now, skip full words until near to n */

	rest = (n-inoff)/SZINT;
	zecode( rest );

	/* now, the remainder of the last word */

	rest = n-inoff;
	vfdzero( rest );
	if( inoff != n ) cerror( "inoff error");

	}

vfdalign( n ) register OFFSZ n; { /* make inoff have the offset the next alignment of n */
	OFFSZ m;

	m = inoff;
	SETOFF( m, n );
	inforce( m );
	}

f3970( x ){ /* (used by the parser; name and purpose open) */
	register a, b;

	a = x;
	return( x );
	}

