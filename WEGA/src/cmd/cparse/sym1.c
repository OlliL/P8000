# include "mfile1"

static char sccsid[] = "@[$]sym1.c\t2.8  08/24/83 14:45:09 Copr. 1981 87wega3.2.";

extern int ddebug;
extern FILE *sfile;	/* the .S file: the symbols for the debugger */

	/* counters of the free register variables (names are guesses) */
extern int regvar, lregvar, fregvar;

	/* the .S file (WEGA): a header, the symbols (22 bytes each), the names, the member lists, the references */
extern int symcount;
int nrefs = 0;	/* entries of reftab */
int nrefsym = 0;	/* entries of refsym */
int ntaglist = 0;	/* entries of taglist */
struct { int h_magic; long h_syms; long h_names; int h_tags; int h_refs; } shdr;
long shdrpos;
struct symtab *refsym[551];	/* the referenced symbols of a function */
struct { long r_id; char r_flag; } reftab[551];	/* the references of a function */
int taglist[500];	/* the member lists */
int commflag;	/* the declaration is a common (nidcl, commdec) */
struct symtab *hashtab[HASHSZ];	/* the hash chains */
char ntab[NTABSZ];	/* the names */

falloc( p, w, new, pty )  register struct symtab *p; NODE *pty; {
	/* allocate a field of width w */
	/* new is 0 if new entry, 1 if redefinition, -1 if alignment */

	register al,sz;
	register TWORD type;
	int s;

	type = (new<0)? pty->type : p->stype;

	/* this must be fixed to use the current type in alignments */
	switch( new<0?pty->type:p->stype ){

	case ENUMTY:
		s = new<0 ? pty->csiz : p->sizoff;
		al = dimtab[s+2];
		sz = dimtab[s];
		break;

	case CHAR:
	case UCHAR:
		al = ALCHAR;
		sz = SZCHAR;
		break;

	case SHORT:
	case USHORT:
	case INT:
	case UNSIGNED:
		al = ALINT;
		sz = SZINT;
		break;

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
	return(0);
	}

nidcl( p ) NODE *p; { /* handle unitialized declarations */
	/* assumed to be not functions */
	register class;
	OFFSZ sz;

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

	if( ISARY(p->type) && dimtab[p->cdim]==0 && (class==AUTO || class==EXTDEF) )
		werror( "empty array declaration - no space reserved" );

	defid( p, class );

	if( class==EXTDEF || class==STATIC ){
		beginit( p->rval );
		endinit();
		}
	if( commflag ){
		commdec( p->rval );
		commflag = 0;
		}
	if( class == AUTO ){
		if( (sz = tsize( STP(p->rval)->stype, STP(p->rval)->dimoff, STP(p->rval)->sizoff )/SZCHAR) > 0xfffe )
			uerror( "size too large" );
		}
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
	if( idp == 0 ) return( 0 );

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

	if( o == LB ) dstash( (OFFSZ)(unsigned) temp );

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
		if( mod2 == FTN && t == FLOAT ) type = (type&~BTMASK) | DOUBLE;	/* WEGA: float functions return double */
		else while( mod1=mod2, mod2 = (t&TMASK) ){
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
			uerror( "fcn has illegal storage class - %d", class );
		case AUTO:
			class = EXTERN;
		case EXTERN:
		case EXTDEF:
		case TYPEDEF:
		case STATIC:
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
		else if( regfree( type ) && cisreg( type ) ) return( class );
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

regfree( t ) register TWORD t; { /* is a register free for a variable of type t (name is a guess) */
	switch( t ){

	default:
		if( regvar >= 8 ) return( 1 );
		return( 0 );

	case FLOAT:
	case DOUBLE:
		if( fregvar >= 20 ) return( 1 );
		return( 0 );

	case LONG:
	case ULONG:
		if( lregvar >= 8 ) return( 1 );
		return( 0 );
		}
	}

struct symtab *
lookup( name, s, mk, cls ) char *name; {
	/* look up name: s is the namespace (SMOS, ...), mk != 0 makes an entry when the name is not there,
	   cls != 0 takes only structure and union tags */
	register char *t;
	register char *u;
	register struct symtab *q;
	register struct symtab *prev;
	register struct symtab *next;
	int i;
	int n;

	i = 0;
	for( t = name; *t; ++t, ++i ) if( i >= ncharsig ) break;
	if( i >= ncharsig ) *t = '\0';

	i = hash( name );
	q = hashtab[i];
	prev = 0;
	for( ; q; prev = q, q = next ){
		next = (struct symtab *)((int)q->slink ^ (int)prev);
		t = &ntab[q->sname];
		u = name;
		if( cls && q->sclass != STNAME && q->sclass != UNAME ) continue;
		if( (q->sflags & (SMOS|SHIDDEN)) != s ) continue;
		if( strcmp( t, u ) ) continue;
		if( mk && blevel && !q->slevel && q->sclass != PARAM ) refid( q );
		return( q );
		}
	if( !mk ) return;

	if( (q = (struct symtab *) malloc( sizeof(struct symtab) )) == 0 )
		cerror( "Symbol table out of space" );
	q->slink = hashtab[i];
	if( hashtab[i] ) hashtab[i]->slink = (struct symtab *)((int)hashtab[i]->slink ^ (int)q);
	hashtab[i] = q;
	q->sflags = s;
	q->stype = UNDEF;
	q->sclass = SNULL;
	q->sfldc = 0;
	if( blevel ){
		q->sname = ntabindx;
		t = &ntab[ntabindx];
		while( *t++ = *name ){
			++name;
			++ntabindx;
			}
		++ntabindx;
		if( ntabindx > ntab_globals ){
			fprintf( stderr, "ntabindx = %d, ntab_globals = %d\n", ntabindx, ntab_globals );
			cerror( "name table overflow - 3" );
			}
		}
	else {
		n = strlen( name ) + 1;
		ntab_globals -= n;
		q->sname = ntab_globals;
		t = &ntab[ntab_globals];
		if( ntabindx > ntab_globals ){
			fprintf( stderr, "ntabindx = %d, ntab_globals = %d\n", ntabindx, ntab_globals );
			cerror( "name table overflow - 4" );
			}
		while( *t++ = *name ) ++name;
		}
	return( q );
	}

struct symtab *
copysym( pp ) struct symtab **pp; { /* a copy of the entry *pp in a new entry, which replaces it (name is a guess) */
	struct symtab *q;
	char *t;
	int n;
	char *s;
	register h;

	if( (q = (struct symtab *) malloc( sizeof(struct symtab) )) == 0 )
		cerror( "Symbol table out of space" );
	*q = **pp;
	h = hash( &ntab[q->sname] );
	q->sflags = SMOS|040;
	*pp = q;
	q->slink = hashtab[h];
	hashtab[h]->slink = (struct symtab *)((int)hashtab[h]->slink ^ (int)q);
	hashtab[h] = q;
	q->symno = symcount++;
	s = &ntab[q->sname];
	if( blevel ){
		q->sname = ntabindx;
		while( ntab[ntabindx++] = *s ) ++s;
		if( ntabindx > ntab_globals ){
			fprintf( stderr, "ntabindx = %d, ntab_globals = %d\n", ntabindx, ntab_globals );
			cerror( "name table overflow - 5" );
			}
		}
	else {
		n = strlen( s ) + 1;
		ntab_globals -= n;
		q->sname = ntab_globals;
		t = &ntab[ntab_globals];
		if( ntabindx > ntab_globals ){
			fprintf( stderr, "ntabindx = %d, ntab_globals = %d\n", ntabindx, ntab_globals );
			cerror( "name table overflow - 4" );
			}
		while( *t++ = *s ) ++s;
		}
	return( q );
	}

struct symtab *
hide( p ) register struct symtab *p; {
	register struct symtab *q;
	int i, h;

	if( (q = (struct symtab *) malloc( sizeof(struct symtab) )) == 0 )
		cerror( "Symbol table out of space" );
	*q = *p;
	i = hash( &ntab[p->sname] );
	q->slink = hashtab[i];
	hashtab[i]->slink = (struct symtab *)((int)hashtab[i]->slink ^ (int)q);
	hashtab[i] = q;
	q->symno = symcount++;
	q->sname = ntabindx;
	for( i=0; ntab[p->sname+i]; ++i ) ntab[ntabindx++] = ntab[p->sname+i];
	ntab[ntabindx] = '\0';
	++ntabindx;
	if( ntabindx > ntab_globals ){
		fprintf( stderr, "ntabindx = %d, ntab_globals = %d\n", ntabindx, ntab_globals );
		cerror( "name table overflow - 5" );
		}
	p->sflags |= SHIDDEN;
	q->sflags = (p->sflags&SMOS) | SHIDES;
	return( idname = q );
	}

unhide( p, prev ) register struct symtab *p, *prev; {
	register struct symtab *q, *next;
	register s, j;

	s = p->sflags & SMOS;
	q = (struct symtab *)((int)p->slink ^ (int)prev);
	prev = p;
	for( ; q; prev = q, q = next ){
		next = (struct symtab *)((int)q->slink ^ (int)prev);
		if( (q->sflags&SMOS) == s && !strcmp( &ntab[p->sname], &ntab[q->sname] ) ){
			q->sflags &= ~SHIDDEN;
			return;
			}
		}
	cerror( "unhide fails" );
	}

refid( p ) struct symtab *p; { /* note that the symbol p is used: it goes into the .S file */
	int i, j;

	if( (p->stype&BTMASK) == STRTY && p->sclass != STNAME && (struct symtab *) dimtab[p->sizoff+3] )
		refid( (struct symtab *) dimtab[p->sizoff+3] );
	if( !(p->sflags & SREF) ){
		p->sflags |= SREF;
		refsym[nrefsym] = p;
		reftab[nrefs].r_id = nrefsym * 22;
		++nrefsym;
		}
	else {
		for( i=0; i<nrefsym; ++i ) if( refsym[i] == p ) break;
		if( i == nrefsym ) cerror( "refid: symbol not found" );
		for( j=0; j<nrefs; ++j ) if( reftab[j].r_id == i * 22 ) return;
		reftab[nrefs].r_id = i * 22;
		}
	reftab[nrefs].r_flag = 0;
	++nrefs;
	}

clearst( lev ){ /* clear entries of internal scope  from the symbol table */
	register struct symtab *p, *prev, *next;
	register int temp;
	int i;

	temp = lineno;
	for( i=0; i<HASHSZ; ++i ){
		for( p = hashtab[i], prev = 0; p; prev = p, p = next ){
			next = (struct symtab *)((int)p->slink ^ (int)prev);
			lineno = p->suse;
			if( lineno < 0 ) lineno = - lineno;
			if( p->slevel>lev ){ /* must clobber */
				if( p->stype == UNDEF || ( p->sclass == ULABEL && lev < 2 ) ){
					lineno = temp;
					uerror( "%s undefined", &ntab[p->sname] );
					}
				if( lev >= 0 || (p->sflags & SREF) ) putsym( p );
				if( p->sflags & SHIDES ) unhide( p, prev );
				if( next ){
					next->slink = (struct symtab *)((int)next->slink ^ (int)p);
					next->slink = (struct symtab *)((int)next->slink ^ (int)prev);
					}
				if( prev ){
					prev->slink = (struct symtab *)((int)prev->slink ^ (int)p);
					prev->slink = (struct symtab *)((int)prev->slink ^ (int)next);
					}
				else hashtab[i] = next;
				free( p );
				p = prev;
				}
			}
		}
	lineno = temp;
	}

sinit(){ /* the header of the .S file */
	shdr.h_magic = 1;
	shdr.h_syms = 0;
	shdr.h_names = 0;
	shdr.h_tags = 0;
	shdr.h_refs = 0;
	shdrpos = ftell( sfile );
	fwrite( &shdr, sizeof(shdr), 1, sfile );
	}

sheader(){ /* write the header of the .S file again */
	fseek( sfile, shdrpos, 0 );
	fwrite( &shdr, sizeof(shdr), 1, sfile );
	fseek( sfile, 0L, 2 );
	}

putrefsyms(){ /* the symbols that are used */
	int a, b, i;

	shdr.h_magic = 0;
	for( i=0; i<nrefsym; ++i ) putsym( refsym[i] );
	}

putsym( p ) register struct symtab *p; { /* a symbol into the .S file */
	struct {
		int s_name;
		TWORD s_type;
		char s_class;
		char s_b;
		int s_8;
		long s_offset;
		long s_size;
		unsigned s_no:12, s_f0:1, s_f1:1, s_f2:1, s_f3:1;	/* s_no in bits 4..15, s_f0 in bit 3 */
		int s_tag;
		int s_x;
		} x;
	register i;

	if( (p->stype == EXTERN || p->stype == EXTDEF) && strlen( &ntab[p->sname] ) >= ncharsig )
		ntab[p->sname+ncharsig-1] = '\0';
	x.s_name = p->sname;
	if( p->sname >= ntab_globals ) x.s_name -= ntab_globals;
	x.s_type = p->stype;
	x.s_class = p->sclass;
	x.s_offset = p->offset;
	x.s_b = p->sfldb;
	x.s_size = dimtab[p->sizoff];
	x.s_8 = p->sfld8;
	x.s_no = p->symno;
	x.s_f0 = 0;
	x.s_f1 = 0;
	x.s_f2 = 0;
	x.s_f3 = 0;
	x.s_x = -1;
	if( (x.s_type&BTMASK) == STRTY && x.s_class != STNAME ){
		if( (struct symtab *) dimtab[p->sizoff+3] ) x.s_tag = ((struct symtab *) dimtab[p->sizoff+3])->symno;
		else x.s_tag = -1;
		}
	else if( x.s_class == STNAME ){
		i = dimtab[p->sizoff+1];
		x.s_tag = ntaglist;
		for( ; dimtab[i]; ++i ) taglist[ntaglist++] = ((struct symtab *) dimtab[i])->symno;
		taglist[ntaglist++] = dimtab[i];
		}
	else x.s_tag = 0;
	fwrite( &x, 22, 1, sfile );
	shdr.h_syms += 22;
	}

putnames(){ /* the names into the .S file */
	int i;
	long pos, end;

	pos = ftell( sfile );
	if( blevel < 0 ) for( i=ntab_globals; i<NTABSZ; ++i ) sputc( ntab[i], sfile );
	else for( i=0; i<ntabindx; ++i ) sputc( ntab[i], sfile );
	end = ftell( sfile );
	shdr.h_names = end - pos;
	}

puttags(){ /* the member lists of the structures into the .S file */
	int spare;

	fwrite( taglist, 2, ntaglist, sfile );
	shdr.h_tags = ntaglist * 2;
	ntaglist = 0;
	}

putrefs(){ /* the references into the .S file */
	int spare;

	fwrite( reftab, 6, nrefs, sfile );
	shdr.h_refs = nrefs * 6;
	nrefs = 0;
	}

sputc( c, f ) FILE *f; {
	return( putc( c, f ) );
	}

aobeg(){
	}
