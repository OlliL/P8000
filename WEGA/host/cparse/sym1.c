# include "mfile1"

static char sccsid[] __attribute__((unused)) = "@[$]sym1.c\t2.8  08/24/83 14:45:09 Copr. 1981 87wega3.2.";

extern int ddebug;
extern FILE *sfile;	/* the .S file: the symbols for the debugger */

	/* counters of the free register variables (names are guesses) */
extern int regvar, lregvar, fregvar;

	/* the .S file (WEGA): a header, the symbols (22 bytes each), the names, the member lists, the references */
extern int symcount;
extern int regflag;

int hash(char *s)
{ /* the hash chain of the name s */
	int h;
	char *p;

	h = 0;
	for( p = s; *p; ++p ) h += *p;
	return( h %= HASHSZ );
	}

OFFSZ upoff(OFFSZ size, int alignment, OFFSZ *poff)
{
	/* update the offset pointed to by poff; return the
	   offset of a value of size `size', alignment `alignment',
	   given that off is increasing */

	register OFFSZ off;

	off = *poff;
	SETOFF( off, alignment );
	*poff = off+size;
	return( off );
	}

int oalloc(struct symtab *p, OFFSZ *poff)
{
	/* allocate p with offset *poff, and update *poff */
	int al;
	register OFFSZ off, tsz;
	OFFSZ noff;

	al = talign( p->stype, p->sizoff );
	noff = off = *poff;
	tsz = tsize( p->stype, p->dimoff, p->sizoff );
	if( (p->sclass == PARAM || p->sclass == REGISTER) && p->sfldc == 2 &&
			(p->stype==CHAR||p->stype==UCHAR||p->stype==SHORT||p->stype==USHORT) ){
	both:
		upoff( (long)SZINT, ALINT, &noff );
		off = noff - tsz;
		}
	else if( (p->sclass == AUTO || p->sclass == PARAM || (regflag && p->sclass == REGISTER)) &&
			(p->stype==CHAR||p->stype==UCHAR||p->stype==SHORT||p->stype==USHORT) ) goto both;
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
int nrefs = 0;	/* entries of reftab */
int nrefsym = 0;	/* entries of refsym */
int ntaglist = 0;	/* entries of taglist */
struct shdr shdr;
long shdrpos;
struct symtab *refsym[551];	/* the referenced symbols of a function */
struct ref reftab[551];	/* the references of a function */
int taglist[500];	/* the member lists */
int commflag;	/* the declaration is a common (nidcl, commdec) */
struct symtab *hashtab[HASHSZ];	/* the hash chains */
char ntab[NTABSZ];	/* the names */

int falloc(struct symtab *p, int w, int new, NODE *pty)
{
	/* allocate a field of width w */
	/* new is 0 if new entry, 1 if redefinition, -1 if alignment */

	int al,sz = 0;
	register TWORD type;

	type = (new<0)? pty->in.type : p->stype;

	/* this must be fixed to use the current type in alignments */
	switch( new<0?pty->in.type:p->stype ){

	case ENUMTY:
		{
		int s;

		s = new<0 ? pty->fn.csiz : p->sizoff;
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

void nidcl(NODE *p)
{ /* handle unitialized declarations */
	/* assumed to be not functions */
	int class;
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

	if( ISARY(p->in.type) && dimtab[p->fn.cdim]==0 && (class==AUTO || class==REGISTER) )
		werror( "empty array declaration - no space reserved" );

	defid( p, class );

	if( class==EXTDEF || class==STATIC ){
		beginit( STP(p->tn.rval) );
		endinit();
		}
	if( commflag ){
		commdec( STP(p->tn.rval) );
		commflag = 0;
		}
	if( class == AUTO ){
		if( (sz = tsize( STP(p->tn.rval)->stype, STP(p->tn.rval)->dimoff, STP(p->tn.rval)->sizoff )/SZCHAR) > 0xfffe )
			uerror( "size too large" );
		}
	}

TWORD types(TWORD t1, TWORD t2, TWORD t3)
{
	/* return a basic type from basic types t1, t2, and t3 */

	TWORD t[3], noun, adj, unsg;
	int i;

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

NODE *tymerge(NODE *typ, NODE *idp)
{
	/* merge type typ with identifier idp  */

	register unsigned t;
	int i;
	extern int eprint();

	if( typ->in.op != TYPE ) cerror( "tymerge: arg 1" );
	if( idp == 0 ) return( 0 );

	idp->in.type = typ->in.type;
	idp->fn.cdim = curdim;
	tyreduce( idp );
	idp->fn.csiz = typ->fn.csiz;

	for( t=typ->in.type, i=typ->fn.cdim; t&TMASK; t = DECREF(t) ){
		if( ISARY(t) ) dstash( dimtab[i++] );
		}

	/* now idp is a single node: fix up type */

	idp->in.type = ctype( idp->in.type );

	if( (t = BTYPE(idp->in.type)) != STRTY && t != UNIONTY && t != ENUMTY ){
		idp->fn.csiz = t;  /* in case ctype has rewritten things */
		}

	return( idp );
	}

void tyreduce(NODE *p)
{

	/* build a type, and stash away dimensions, from a parse tree of the declaration */
	/* the type is build top down, the dimensions bottom up */
	int o, temp;
	register unsigned t;
	OFFSZ off;

	o = p->in.op;
	p->in.op = FREE;

	if( o == NAME ) return;

	t = INCREF( p->in.type );
	if( o == UNARY CALL ) t += (FTN-PTR);
	else if( o == LB ){
		t += (ARY-PTR);
		temp = p->in.right->tn.lval;
		p->in.right->in.op = FREE;
		}

	p->in.left->in.type = t;
	tyreduce( p->in.left );

	if( o == LB ){
		off = (unsigned) temp;
		dstash( off );
		}

	p->tn.rval = p->in.left->tn.rval;
	p->in.type = p->in.left->in.type;

	}

void fixtype(NODE *p, int class)
{
	register unsigned t, type;
	int mod1, mod2;
	/* fix up the types, and check for legality */

	if( (type = p->in.type) == UNDEF ) return;
	if( (mod2 = (type&TMASK)) ){
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
			++p->fn.cdim;
			type += (PTR-ARY);
			}
		else if( ISFTN(type) ) type = INCREF(type);

		}

	if( instruct && ISFTN(type) ){
		uerror( "function illegal in structure or union" );
		type = INCREF(type);
		}
	p->in.type = type;
	}

int uclass(int class)
{
	/* give undefined version of class */
	if( class == SNULL ) return( EXTERN );
	else if( class == STATIC ) return( USTATIC );
	else return( class );
	}

int fixclass(int class, TWORD type)
{

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
		/* fall through */
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

int regfree(TWORD t)
{ /* is a register free for a variable of type t (name is a guess) */
	switch( t ){

	default:
#ifdef SEG
		if( ISPTR(t) ) goto lng;
#endif
		if( regvar >= 8 ) return( 1 );
		return( 0 );

	case FLOAT:
	case DOUBLE:
		if( fregvar >= 20 ) return( 1 );
		return( 0 );

	case LONG:
	case ULONG:
#ifdef SEG
	lng:
#endif
		if( lregvar >= 8 ) return( 1 );
		return( 0 );
		}
	}

struct symtab *lookup(char *name, int s, int mk, int cls)
{
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
		next = (struct symtab *)((uintptr_t)q->slink ^ (uintptr_t)prev);
		t = &ntab[q->sname];
		u = name;
		if( cls && q->sclass != STNAME && q->sclass != UNAME ) continue;
		if( (q->sflags & (SMOS|SHIDDEN)) != s ) continue;
		if( strcmp( t, u ) ) continue;
		if( mk && blevel && !q->slevel && q->sclass != PARAM ) refid( q );
		return( q );
		}
	if( !mk ) return( 0 );	/* the original returns without a value */

	if( (q = (struct symtab *) zalloc( Z_SYMTAB, sizeof(struct symtab), 1 )) == 0 )
		cerror( "Symbol table out of space" );
	q->slink = hashtab[i];
	if( hashtab[i] ) hashtab[i]->slink = (struct symtab *)((uintptr_t)hashtab[i]->slink ^ (uintptr_t)q);
	hashtab[i] = q;
	q->sflags = s;
	q->stype = UNDEF;
	q->sclass = SNULL;
	q->sfldc = 0;
	if( blevel ){
		q->sname = ntabindx;
		t = &ntab[ntabindx];
		while( (*t++ = *name) ){
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
		while( (*t++ = *name) ) ++name;
		}
	return( q );
	}

struct symtab *copysym(struct symtab **pp)
{ /* a copy of the entry *pp in a new entry, which replaces it (name is a guess) */
	struct symtab *q;
	char *t;
	int n;
	char *s;
	int h;

	if( (q = (struct symtab *) zalloc( Z_SYMTAB, sizeof(struct symtab), 1 )) == 0 )
		cerror( "Symbol table out of space" );
	*q = **pp;
	h = hash( &ntab[q->sname] );
	q->sflags = SMOS|040;
	*pp = q;
	q->slink = hashtab[h];
	hashtab[h]->slink = (struct symtab *)((uintptr_t)hashtab[h]->slink ^ (uintptr_t)q);
	hashtab[h] = q;
	q->symno = symcount++;
	s = &ntab[q->sname];
	if( blevel ){
		q->sname = ntabindx;
		while( (ntab[ntabindx++] = *s) ) ++s;
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
		while( (*t++ = *s) ) ++s;
		}
	return( q );
	}

struct symtab *hide(struct symtab *p)
{
	register struct symtab *q;
	int i;

	if( (q = (struct symtab *) zalloc( Z_SYMTAB, sizeof(struct symtab), 1 )) == 0 )
		cerror( "Symbol table out of space" );
	*q = *p;
	i = hash( &ntab[p->sname] );
	q->slink = hashtab[i];
	hashtab[i]->slink = (struct symtab *)((uintptr_t)hashtab[i]->slink ^ (uintptr_t)q);
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

void unhide(struct symtab *p, struct symtab *prev)
{
	register struct symtab *q, *next;
	int s;

	s = p->sflags & SMOS;
	q = (struct symtab *)((uintptr_t)p->slink ^ (uintptr_t)prev);
	prev = p;
	for( ; q; prev = q, q = next ){
		next = (struct symtab *)((uintptr_t)q->slink ^ (uintptr_t)prev);
		if( (q->sflags&SMOS) == s && !strcmp( &ntab[p->sname], &ntab[q->sname] ) ){
			q->sflags &= ~SHIDDEN;
			return;
			}
		}
	cerror( "unhide fails" );
	}

void refid(struct symtab *p)
{ /* note that the symbol p is used: it goes into the .S file */
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

void clearst(int lev)
{ /* clear entries of internal scope  from the symbol table */
	register struct symtab *p, *prev, *next;
	register int temp;
	int i;

	temp = lineno;
	for( i=0; i<HASHSZ; ++i ){
		for( p = hashtab[i], prev = 0; p; prev = p, p = next ){
			next = (struct symtab *)((uintptr_t)p->slink ^ (uintptr_t)prev);
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
					next->slink = (struct symtab *)((uintptr_t)next->slink ^ (uintptr_t)p);
					next->slink = (struct symtab *)((uintptr_t)next->slink ^ (uintptr_t)prev);
					}
				if( prev ){
					prev->slink = (struct symtab *)((uintptr_t)prev->slink ^ (uintptr_t)p);
					prev->slink = (struct symtab *)((uintptr_t)prev->slink ^ (uintptr_t)next);
					}
				else hashtab[i] = next;
				/* the original reads the entry again through dimtab (a structure tag) after the free: zalloc.c */
				zfree( p );
				p = prev;
				}
			}
		}
	lineno = temp;
	}

/* the header of the .S file: magic (2), syms (4), names (4), tags (2), refs (2) */
static void putshdr(void)
{
	put16( sfile, shdr.h_magic );
	put32( sfile, shdr.h_syms );
	put32( sfile, shdr.h_names );
	put16( sfile, shdr.h_tags );
	put16( sfile, shdr.h_refs );
	}

void sinit(void)
{ /* the header of the .S file */
	shdr.h_magic = 1;
	shdr.h_syms = 0;
	shdr.h_names = 0;
	shdr.h_tags = 0;
	shdr.h_refs = 0;
	shdrpos = ftell( sfile );
	putshdr();
	}

void sheader(void)
{ /* write the header of the .S file again */
	fseek( sfile, shdrpos, 0 );
	putshdr();
	fseek( sfile, 0L, 2 );
	}

void putrefsyms(void)
{ /* the symbols that are used */
	int   i;

	shdr.h_magic = 0;
	for( i=0; i<nrefsym; ++i ) putsym( refsym[i] );
	}

void putsym(struct symtab *p)
{ /* a symbol into the .S file */
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
	int i;

	if( (p->stype == EXTERN || p->stype == EXTDEF) && (int) strlen( &ntab[p->sname] ) >= ncharsig )
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
		while( dimtab[i] ) taglist[ntaglist++] = ((struct symtab *) dimtab[i++])->symno;
		taglist[ntaglist++] = dimtab[i];
		}
	else x.s_tag = 0;
	put16( sfile, x.s_name );
	put16( sfile, x.s_type );
	putc( x.s_class, sfile );
	putc( x.s_b, sfile );
	put16( sfile, x.s_8 );
	put32( sfile, x.s_offset );
	put32( sfile, x.s_size );
	put16( sfile, (x.s_no << 4) | (x.s_f0 << 3) | (x.s_f1 << 2) | (x.s_f2 << 1) | x.s_f3 );
	put16( sfile, x.s_tag );
	put16( sfile, x.s_x );
	shdr.h_syms += 22;
	}

void putnames(void)
{ /* the names into the .S file */
	int i;
	long pos, end;

	pos = ftell( sfile );
	if( blevel < 0 ) for( i=ntab_globals; i<NTABSZ; ++i ) sputc( ntab[i], sfile );
	else for( i=0; i<ntabindx; ++i ) sputc( ntab[i], sfile );
	end = ftell( sfile );
	shdr.h_names = end - pos;
	}

void puttags(void)
{ /* the member lists of the structures into the .S file */
	int i;

	for( i=0; i<ntaglist; ++i ) put16( sfile, taglist[i] );
	shdr.h_tags = ntaglist * 2;
	ntaglist = 0;
	}

void putrefs(void)
{ /* the references into the .S file */
	int i;

	for( i=0; i<nrefs; ++i ){	/* 6 bytes: the id, the flag and a pad byte */
		put32( sfile, reftab[i].r_id );
		putc( reftab[i].r_flag, sfile );
		putc( 0, sfile );
		}
	shdr.h_refs = nrefs * 6;
	nrefs = 0;
	}
