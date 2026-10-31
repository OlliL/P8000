# include "mfile1"

static char sccsid[] = "@[$]vinit1.c\t2.3  10/06/83 13:13:44 Copr. 1981 87wega3.2.";

extern int regflag;	/* the flag at 0x1aa0 (char and short register variables get a word on the stack; name is a guess) */
OFFSZ upoff();

struct instk {	/* WEGA: 22 bytes */
	OFFSZ in_sz;   /* size of array element */
	int in_x;    /* current index for structure member in structure initializations */
	int in_n;    /* number of initializations seen */
	int in_s;    /* sizoff */
	int in_d;    /* dimoff */
	TWORD in_t;    /* type */
	struct symtab *in_id;   /* the symbol (PCC: stab index) */
	int in_fl;   /* flag which says if this level is controlled by {} */
	OFFSZ in_off;  /* offset of the beginning of this level */
	}
instack[10],
*pstk;

	/* defines used for getting things off of the initialization stack */

int ibseen = 0;  /* the number of } constructions which have been filled */

int iclass;  /* storage class of thing being initialized */

int ilocctr = 0;  /* location counter for current initialization */

	/* the string literals that are kept until the end of the function (putstrs) */

resetmem( p ) char *p; { /* give the memory of getmem back (name is a guess) */
	brk( p );
	memcur = memtop = p;
	--memtop;
	}

beginit( curid ) struct symtab *curid; {
	/* beginning of initilization; set location ctr and set type */
	register struct symtab *p;
	OFFSZ sz;
	int al;

	p = curid;

	iclass = p->sclass;
	if( curclass == EXTERN ) iclass = EXTERN;
	switch( iclass ){

	case UNAME:
		return;

	case EXTERN:
		lineid();
		return;

	case AUTO:
	case REGISTER:
		break;

	case EXTDEF:
	case STATIC:
		ilocctr = ISARY(p->stype)?ADATA:DATA;
		locctr( ilocctr );
		defalign( al = talign( p->stype, p->sizoff ) );
		if( (sz = (tsize( p->stype, p->dimoff, p->sizoff ) + 7) / SZCHAR) > 0xfffe )
			uerror( "size of %s too large", &ntab[p->sname] );
		else if( sectfull( 1, sz ) ) newsect( 1 );
		lineid();
		defnam( p );

		}

	inoff = 0;
	ibseen = 0;

	pstk = 0;

	instk( curid, p->stype, p->dimoff, p->sizoff, inoff );

	}

instk( id, t, d, s, off ) struct symtab *id; OFFSZ off; TWORD t; {
	/* make a new entry on the parameter stack to initialize id */

	register struct symtab *p;

	for(;;){

		/* save information on the stack */

		if( !pstk ) pstk = instack;
		else ++pstk;

		pstk->in_fl = 0;	/* { flag */
		pstk->in_id =  id ;
		pstk->in_t =  t ;
		pstk->in_d =  d ;
		pstk->in_s =  s ;
		pstk->in_n = 0;  /* number seen */
		pstk->in_x =  t==STRTY ?dimtab[s+1] : 0 ;
		pstk->in_off =  off;   /* offset at the beginning of this element */
		/* if t is an array, DECREF(t) can't be a field */
		/* INS_sz has size of array elements, and -size for fields */
		if( ISARY(t) ){
			pstk->in_sz = tsize( DECREF(t), d+1, s );
			}
		else if( id->sclass & FIELD ){
			pstk->in_sz = - ( id->sclass & FLDSIZ );
			}
		else {
			pstk->in_sz = 0;
			}

		if( (iclass==AUTO || iclass == REGISTER ) &&
			(ISARY(t) || t==STRTY) ) uerror( "no automatic aggregate initialization" );

		/* now, if this is not a scalar, put on another element */

		if( ISARY(t) ){
			t = DECREF(t);
			++d;
			continue;
			}
		else if( t == STRTY ){
			id = (struct symtab *) dimtab[pstk->in_x];
			p = id;
			if( p->sclass != MOS && !(p->sclass&FIELD) ) cerror( "insane structure member list" );
			t = p->stype;
			d = p->dimoff;
			s = p->sizoff;
			off += p->offset;
			continue;
			}
		else return;
		}
	}

NODE *
getstr(){ /* decide if the string is external or an initializer, and get the contents accordingly */

	register l, temp;
	register NODE *p;
	int oloc;

	if( (iclass==EXTDEF||iclass==STATIC) && (pstk->in_t == CHAR || pstk->in_t == UCHAR) &&
			pstk!=instack && ISARY( pstk[-1].in_t ) ){
		/* treat "abc" as { 'a', 'b', 'c', 0 } */
		strflg = 1;
		ilbrace();  /* simulate { */
		inforce( pstk->in_off );
		/* if the array is inflexible (not top level), pass in the size and
			be prepared to throw away unwanted initializers */
		lxstr((pstk-1)!=instack?(int)dimtab[(pstk-1)->in_d]:0);  /* get the contents */
		irbrace();  /* simulate } */
		return( 0 );
		}
	else { /* make a label, and get the contents and stash them away */
		if( iclass != SNULL ){ /* initializing */
			/* fill out previous word, to permit pointer */
			vfdalign( (OFFSZ)ALPOINT );
			}
		oloc = curloc;
		temp = locctr( blevel==0?ISTRNG:STRNG ); /* set up location counter */
		l = getlab();
		strflg = 0;
		savestr( 0, l ); /* get the contents */
#ifndef OLD_D12
		if( curloc == STRNG && strhead && iclass != STATIC ) putstrs();
#endif
		locctr( oloc );
		p = buildtree( STRING, NIL, NIL );
		p->rval = -l;
		return(p);
		}
	}
putbyte( v ){ /* simulate byte v appearing in a list of integer values */
	register NODE *p;
	p = bcon(v);
	incode( p, SZCHAR );
	tfree( p );
	gotscal();
	}

putstrs(){ /* the string literals of the function into the .D file (name is a guess) */
	char *s;
	int i;

#ifdef OLD_D12
	fprintf( dfile, "    .even\n" );
#endif
	strcur = strhead;
	do {
		fprintf( dfile, "L%d:\n", strcur->s_lab );
		s = strcur->s_str;
		i = 0;
		do {
			if( i%8 == 0 ){
				if( i ) fprintf( dfile, "\n" );
				fprintf( dfile, "\t.byte\t" );
				}
			else fprintf( dfile, "," );
			fprintf( dfile, "%%%x", *s & 0377 );
			++i;
			} while( *s++ || i < strcur->s_len );
		fprintf( dfile, "\n" );
		free( strcur->s_str );
		free( strcur );
		} while( strcur = strcur->s_next );
	fprintf( dfile, "    .even\n" );
	strcur = 0;
	strhead = 0;
	}

endinit(){
	register TWORD t;
	register d, s, n, d1;
	OFFSZ sz, tsz;

	switch( iclass ){

	case AUTO:
	case EXTERN:
	case REGISTER:
#ifndef OLD_D12
		if( strhead ) putstrs();
#endif
		return;
		}

	pstk = instack;

	t = pstk->in_t;
	d = pstk->in_d;
	s = pstk->in_s;
	n = pstk->in_n;

	if( ISARY(t) ){
		d1 = dimtab[d];

		vfdalign( pstk->in_sz );  /* fill out part of the last element, if needed */
		n = inoff/pstk->in_sz;  /* real number of initializers */
		if( d1 >= n ){
			/* once again, t is an array, so no fields */
			inforce( tsz = tsize( t, d, s ) );
			n = d1;
			}
		if( d1!=0 && d1!=n ) uerror( "too many initializers");
		if( n==0 ) werror( "empty array declaration - no space reserved");
		dimtab[d] = n;
		}

	else if( t == STRTY || t == UNIONTY ){
		/* clearly not fields either */
		inforce( tsz = tsize( t, d, s ) );
		}
	else if( n > 1 ) uerror( "bad scalar initialization");
	/* this will never be called with a field element... */
	else inforce( tsz = tsize(t,d,s) );

	paramno = 0;
	vfdalign( (OFFSZ)AL_INIT );
	inoff = 0;
	iclass = SNULL;
	if( (sz = (tsize( t, d, s ) + 15) / 16 * 2) > 0xfffe ) uerror( "size too large" );
	else sectadd( 1, sz );
#ifndef OLD_D12
	if( strhead ) putstrs();
#endif

	}
doinit( p ) register NODE *p; {

	/* take care of generating a value for the initializer p */
	/* inoff has the current offset (last bit written)
		in the current word being generated */

	register sz, d, s;
	register TWORD t;

	/* note: size of an individual initializer is assumed to fit into an int */

	if( iclass < 0 ) goto leave;
	switch( iclass ){

	case EXTERN:
		uerror( "cannot initialize extern" );
		iclass = -1;
		goto leave;

	case UNAME:
		uerror( "cannot initialize union" );
		iclass = -1;
		goto leave;

	case TYPEDEF:
		uerror( "cannot initialize typedef" );
		iclass = -1;
		goto leave;

	case AUTO:
	case REGISTER:
		/* do the initialization and get out, without regard
		    for filing out the variable with zeros, etc. */
		bccode();
		idname = pstk->in_id;
		p = buildtree( ASSIGN, buildtree( NAME, NIL, NIL ), p );
		ecomp(p);
		return;
		}

	if( p == NIL ) return;  /* throw away strings already made into lists */

	if( ibseen ){
		uerror( "} expected");
		goto leave;
		}

	t = pstk->in_t;  /* type required */
	d = pstk->in_d;
	s = pstk->in_s;
	if( pstk->in_sz < 0 ){  /* bit field */
		sz = -pstk->in_sz;
		}
	else {
		sz = tsize( t, d, s );
		}

	inforce( pstk->in_off );

	p = buildtree( ASSIGN, block( NAME, NIL,NIL, t, d, s ), p );
	p->left->op = FREE;
	p->left = p->right;
	p->right = NIL;
	p->left = optim( p->left );
	if( p->left->op == UNARY AND ){
		p->left->op = FREE;
		p->left = p->left->left;
		}
	p->op = INIT;

	if( sz < SZINT ){ /* special case: bit fields, etc. */
		if( p->left->op != ICON ) uerror( "illegal initialization" );
		else incode( p->left, sz );
		}
	else if( p->left->op == FCON ){
		fincode( p->left->dval, sz );
		}
	else if( p->left->op == SCONV && p->left->left->op == FCON ){
		fincode( p->left->left->dval, sz );
		}
	else {
		cinit( optim(p), sz );
		}

	gotscal();

	leave:
	tfree(p);
	}
gotscal(){
	register t, ix;
	register n;
	struct symtab *p, *id;
	OFFSZ temp;

	for( ; pstk > instack; ) {

		if( pstk->in_fl ) ++ibseen;

		--pstk;

		t = pstk->in_t;

		if( t == STRTY ){
			ix = ++pstk->in_x;
			if( (id = (struct symtab *) dimtab[ix]) == 0 ) continue;

			/* otherwise, put next element on the stack */

			p = id;
			instk( id, p->stype, p->dimoff, p->sizoff, p->offset+pstk->in_off );
			return;
			}
		else if( ISARY(t) ){
			n = ++pstk->in_n;
			if( n >= dimtab[pstk->in_d] && pstk > instack ) continue;

			/* put the new element onto the stack */

			temp = pstk->in_sz;
			instk( pstk->in_id, (TWORD)DECREF(pstk->in_t), pstk->in_d+1, pstk->in_s,
				pstk->in_off+n*temp );
			return;
			}

		}

	}
ilbrace(){ /* process an initializer's left brace */
	register t;
	struct instk *temp;

	temp = pstk;

	for( ; pstk > instack; --pstk ){

		t = pstk->in_t;
		if( t != STRTY && !ISARY(t) ) continue; /* not an aggregate */
		if( pstk->in_fl ){ /* already associated with a { */
			if( pstk->in_n ) uerror( "illegal {");
			continue;
			}

		/* we have one ... */
		pstk->in_fl = 1;
		break;
		}

	/* cannot find one */
	/* ignore such right braces */

	pstk = temp;
	}

irbrace(){
	/* called when a '}' is seen */

	if( ibseen ) {
		--ibseen;
		return;
		}

	for( ; pstk > instack; --pstk ){
		if( !pstk->in_fl ) continue;

		/* we have one now */

		pstk->in_fl = 0;  /* cancel { */
		gotscal();  /* take it away... */
		return;
		}

	/* these right braces match ignored left braces: throw out */

	}

hash( s ) char *s; { /* the hash chain of the name s */
	int h;
	char *p;

	h = 0;
	for( p = s; *p; ++p ) h += *p;
	return( h %= HASHSZ );
	}

OFFSZ
upoff( size, alignment, poff ) register OFFSZ size; register alignment; register OFFSZ *poff; {
	/* update the offset pointed to by poff; return the
	/* offset of a value of size `size', alignment `alignment',
	/* given that off is increasing */

	register OFFSZ off;

	off = *poff;
	SETOFF( off, alignment );
	*poff = off+size;
	return( off );
	}

oalloc( p, poff ) register struct symtab *p; register OFFSZ *poff; {
	/* allocate p with offset *poff, and update *poff */
	register al;
	register OFFSZ off, tsz;
	OFFSZ noff;

	al = talign( p->stype, p->sizoff );
	noff = off = *poff;
	tsz = tsize( p->stype, p->dimoff, p->sizoff );
	if( ( (p->sclass == PARAM || p->sclass == REGISTER) && p->sfldc == 2 &&
			(p->stype==CHAR||p->stype==UCHAR||p->stype==SHORT||p->stype==USHORT) ) ||
	    ( (p->sclass == AUTO || p->sclass == PARAM || (regflag && p->sclass == REGISTER)) &&
			(p->stype==CHAR||p->stype==UCHAR||p->stype==SHORT||p->stype==USHORT) ) ){
		upoff( (long)SZINT, ALINT, &noff );
		off = noff - tsz;
		}
#ifdef OLD_D1
	else off = upoff( tsz, al, &noff ); /* a register variable gets its place on the stack too */
#else
	else if( p->sclass != REGISTER || p->sfldc == 2 || p->sfldc == 1 ) off = upoff( tsz, al, &noff );
	else off = 0;
#endif

	if( p->sclass != REGISTER ){ /* in case we are allocating stack space for register arguments */
		if( p->offset == NOOFFSET ) p->offset = off;
		else if( off != p->offset ) return(1);
		}

	*poff = noff;
	return(0);
	}