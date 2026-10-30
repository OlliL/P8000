# include "mfile1"
# include <signal.h>

static char sccsid[] = "@[$]code1.c\t2.7  08/24/83 14:23:01 Copr. 1981 87wega3.2.";

int lflag = 0;	/* -l: write .line directives into the .D file */
int pflag = 0;	/* -p: profiling, a counter word for each function */
int strftn = 0;	/* is the current function one which returns a value */
int sectpre = 5;	/* length of the name prefix kept when a section is renamed */
char sectname[3][24] = { "modul_p", "modul_d", "modul_b" };	/* the names of the sections */
long sectsize[3] = { 0, 0, 0 };	/* bytes used in each section */

extern int Fflag;
FILE *Ffile;	/* the file of -F */
long ftnpos;	/* position of the function head in the .I file (rewritten by ftnend) */
FILE *outfile, *sfile;	/* the .I and the .S file */
FILE *dfile;	/* the .D file */
char ifname[50], dfname[50], sfname[50];	/* the names of the output files */
char *ipath, *dpath, *spath;
char ibuf[BUFSIZ], dbuf[BUFSIZ], sbuf[BUFSIZ];
extern int crslab, curloc, ftnno, retlab, regflag;
extern struct symtab *curftn;

main( argc, argv ) char *argv[]; {
	int dexit();
	register int i;

	if(signal( SIGHUP, SIG_IGN) != SIG_IGN) signal(SIGHUP, dexit);
	if(signal( SIGINT, SIG_IGN) != SIG_IGN) signal(SIGINT, dexit);
	if(signal( SIGTERM, SIG_IGN) != SIG_IGN) signal(SIGTERM, dexit);
	openfiles( argc, argv );

	for( i=1; i<argc; ++i )
		if( *argv[i] == '-' ) switch( argv[i][1] ){

		case 'l':
			++lflag;
			break;

		case 'L':
			ncharsig = 8;
			break;

		case 'p':
			++pflag;
			break;

		case 'O':
			++regflag;
			break;

		case 'F':
			fnfile( argv[++i] );
			break;
			}

	i = mainp1();

	fclose( outfile );
	fclose( sfile );
	fclose( dfile );
	--lineno;
	lineid();
	return( i );
	}

openfiles( argc, argv ) char *argv[]; { /* -E base: create base.I, base.D and base.S */
	register int i, fd;
	register char *cp;

	for( i=1; i<argc; ++i ){
		cp = argv[i];
		if( *cp == '-' && *++cp == 'E' ){
			++i;
			ipath = strcpy( ifname, argv[i] );
			ipath = strcat( ipath, ".I" );
			if( (fd = creat( ipath, 0666 )) < 0 ) syserr( ipath );
			if( (outfile = fdopen( fd, "w" )) == NULL ) syserr( ipath );
			setbuf( outfile, ibuf );
			dpath = strcpy( dfname, argv[i] );
			dpath = strcat( dpath, ".D" );
			if( (fd = creat( dpath, 0666 )) < 0 ) syserr( dpath );
			if( (dfile = fdopen( fd, "w" )) == NULL ) syserr( dpath );
			setbuf( dfile, dbuf );
			spath = strcpy( sfname, argv[i] );
			spath = strcat( spath, ".S" );
			if( (fd = creat( spath, 0666 )) < 0 ) syserr( spath );
			if( (sfile = fdopen( fd, "w" )) == NULL ) syserr( spath );
			setbuf( sfile, sbuf );
			}
		}
	}

fnfile( s ) char *s; { /* -F s: the names go to the file s */
	++Fflag;
	if( (Ffile = fopen( s, "w" )) == NULL ) syserr( s );
	if( !ftitle[0] ){
		if( fprintf( Ffile, "+%s\n", s ) < 0 ) syserr( s );
		}
	else if( fprintf( Ffile, "+%s\n", ftitle ) < 0 ) syserr( ftitle );
	}

struct fcon {	/* a floating constant of the .D file, with its label */
	struct fcon *fnext;
	double fval;
	int flab;
	};
struct fcon *fhash[61] = { 0 };	/* the hash chains */
struct fcon *fcp = 0;	/* the newest entry */
int fccnt = 0;	/* entries made (50 of them in each malloc) */
int fcfail = 0;	/* malloc failed */
int sectnew = 1;	/* no section has been started yet */
int nsect = 3;	/* the number of sections */

fltlab( d ) double d; { /* the label of the floating constant d; crslab if it is new */
	register struct fcon *p;
	register h;

	h = fhashv( &d, 61 );
	for( p = fhash[h]; p; p = p->fnext ){
		if( p->fval == d ) return( p->flab );
		}
	if( fcfail ) fcp = 0;
	else if( fccnt++ % 50 ) ++fcp;
	else fcp = (struct fcon *) malloc( 50*sizeof(struct fcon) );
	if( fcp == 0 ){
		if( !fcfail ){
			werror( "no space for floating constant chain;\t\t\t\t constants duplicated" );
			fcfail = 1;
			}
		}
	else {
		fcp->fval = d;
		fcp->fnext = fhash[h];
		fcp->flab = crslab;
		fhash[h] = fcp;
		}
	return( crslab );
	}

fhashv( p, n ) char *p; { /* hash of the double at p */
	char *q;
	union { long l; char c[4]; } u;

	q = p;
	u.c[0] = 0;
	u.c[1] = q[3];
	u.c[2] = q[1];
	u.c[3] = q[2];
	return( u.l % n );
	}

newsect( sec ){ /* start a new section sec (DATA or ADATA) */
	char *cp, *q;
	char buf[4];
	int k, n, len;

	if( sec <= 0 || sec >= 3 ) cerror( "declaring improper section" );
	if( nsect >= 3 ) uerror( "too many sections" );
	n = ++nsect;
	cp = &buf[3];
	*cp = 0;
	len = 0;
	while( n ){
		*--cp = n%10 + '0';
		n /= 10;
		++len;
		}
	k = len + sectpre > 21 ? 21 - len : sectpre;
	q = &sectname[sec][k];
	while( *cp ) *q++ = *cp++;
	*q = 0;
	sectnew = 0;
	sectsize[sec] = 0;
	if( sec == DATA ) fprintf( dfile, "\t.data\n" );
	else fprintf( dfile, "\t.bss\n" );
	}

sectfull( sec, n ) long n; { /* do n more bytes not fit into the section sec */
	return( 0xfffeL - sectsize[sec] < n ? 1 : 0 );
	}

sectadd( sec, n ) long n; { /* n more bytes in the section sec */
	if( sectfull( sec, n ) ) uerror( "section overflow" );
	else sectsize[sec] += n;
	}

bstart( off, regs, fregs ) long off; { /* the head of a function body in the .I file */
	++ftnno;
	putc( '[', outfile );
	fprintf( outfile, "%ld\t", off );
	fprintf( outfile, "%d\t", regs );
	fprintf( outfile, "%d\t", fregs );
	putc( '\n', outfile );
	}

bend(){ /* the end of a function in the .I file */
	putc( '}', outfile );
	putc( '\n', outfile );
	}

defalign(n) { /* cause the alignment to become a multiple of n */
	register al;

	al = n/SZCHAR;
	if( curloc != PROG && al > 1 ){
		if( sectfull( DATA, 1L ) ) newsect( DATA );
		else if( sectsize[DATA] & 1 ) ++sectsize[DATA];
		fprintf( dfile, "    .even\n" );
		}
	}

getlab(){
	return( crslab++ );
	}

locctr( l ) register l; {
	register temp;
	/* l is PROG, ADATA, DATA, STRNG, ISTRNG, or STAB */

	if( !sectnew && blevel <= 1 && l != curloc && l == PROG && curloc != ISTRNG && curloc != STRNG )
		fprintf( dfile, "\t.code\n" );
	if( l == curloc ) return( l );
	temp = curloc;
	curloc = l;
	return( temp );
	}

deflab( n ) register n; {
	fprintf( outfile, "L%d\t\n", n );
	}

efcode(){
	/* code for the end of a function */
	register struct symtab *p;
	register int i;
	long size;

	deflab( retlab );
	if( strftn ){  /* a static area for the returned structure */
		p = curftn;
		i = getlab();
		curftn->sfld8 = i;
		size = tsize( DECREF(p->stype), p->dimoff, p->sizoff ) / SZCHAR;
		if( sectfull( ADATA, size ) ) newsect( ADATA );
		else fprintf( dfile, "\t.bss\n" );
		sectadd( ADATA, size );
		fprintf( dfile, "L%d:\t.=.+%ld\n", i, size );
		fprintf( dfile, "\t.data\n" );
		}
	bend();
	lineid();
	}

bfcode( a, n ) struct symtab *a[]; {
	/* code for the beginning of a function; a is an array of
		the entries of the arguments; n is the number */
	register i;
	register temp;
	register r;
	register struct symtab *p;
	OFFSZ off;
	int words, flags, lab;

	lineid();
	locctr( PROG );
	p = curftn;
	if( Fflag ) fprintf( Ffile, "-%s\n", &ntab[p->sname] );
	temp = p->stype;
	temp = DECREF(temp);
	strftn = (temp==STRTY) || (temp==UNIONTY);
	retlab = getlab();
	if( pflag ){
		lab = getlab();
		fprintf( dfile, "L%d:\n\t.addr\t0\n", lab );
		curftn->offset = lab;
		}
	putc( '{', outfile );
	fprintf( outfile, "%d\t", curftn->symno );
	ftnpos = ftell( outfile );
	fprintf( outfile, "0000\t00000\t0\t\n" );

	off = ARGINIT;

	for( i=0; i<n; ++i ){
		p = a[i];
		words = (tsize( p->stype, p->dimoff, p->sizoff ) + 15) / 16;
		flags = p->sfldb;
		if( p->sclass == REGISTER ){
			temp = p->offset;  /* save register number */
			r = p->stype == DOUBLE ? temp - 16 : temp;
			if( p->sfldc == 2 ){
				p->sclass = PARAM;  /* forget that it is a register */
				p->offset = NOOFFSET;
				oalloc( p, &off );
				p->offset = temp;  /* remember register number */
				p->sclass = REGISTER;   /* remember that it is a register */
				}
			}
		else if( p->sfldc != 1 && oalloc( p, &off ) ) cerror( "bad argument" );
		else if( p->sfldc == 1 ) p->sclass = PARAM;
		}
	}

bccode(){ /* called just before the first executable statment */
	SETOFF( autooff, SZINT );
	bstart( autooff, minrvar, fminrvar );
	}

bfend(){ /* "]" into the .I file */
	fprintf( outfile, "]\n" );
	}

defnam( p ) register struct symtab *p; {
	/* define the current location as the name p->sname */

	if( p->sclass == STATIC && p->slevel>1 ) fprintf( dfile, "L%d:\n", (int)p->offset );
	else if( p->sclass == EXTDEF ) fprintf( dfile, "%s::\n", exname( p->sname ) );
	else fprintf( dfile, "%s:\n", exname( p->sname ) );
	}

zecode( n ) long n; {
	/* n integer words of zeros */
	OFFSZ temp;
	register i;

	if( n <= 0 ) return;
	if( n == 1 ) fprintf( dfile, "\t.word\t0\n" );
	else fprintf( dfile, "\t.=.+%ld\n", n*2 );
	temp = n;
	inoff += temp*SZINT;
	}

fldal( t ) unsigned t; { /* return the alignment of field of type t */
	uerror( "illegal field type" );
	return( ALINT );
	}

lineid(){ /* a .line directive for the current line of a .c file (-l) */
	register char *cp;
	int len;

	if( lflag ){
		len = strlen( ftitle );
		cp = &ftitle[len-1];
		if( *cp == '"' ) --cp;
		if( *cp-- == 'c' && *cp == '.' ) fprintf( dfile, "\t.line %d %s \n", lineno, ftitle );
		}
	}

genswitch(p,n) register struct sw *p; {
	/*	p[0] holds the switch expression, p[1] the default label (>=0 if there is one),
		p[2] to p[n+1] are the cases */
	register i;
	register CONSZ j, range;
	register dlab;
	int swlab;
	long size;
	NODE *q;
	int tlab;
	NODE *s, *t, *r;

	range = p[++n].sval-p[2].sval;
	q = *(NODE **)p;

	if( range>0 && range <= 3*n && n>=5 ){ /* implement a direct switch */
		dlab = p[1].slab >= 0 ? p[1].slab : getlab();
		q->lval = -1;
		r = p[2].sval ? block( MINUS, q, bcon( p[2].sval ), INT, 0, INT ) : q;
		t = block( DESC, NIL, NIL, 0, 0, 0 );
		t->lval = range;
		swlab = getlab();
		t->rval = swlab;
		t = block( CM, bcon( -dlab ), t, 0, 0, 0 );
		ecomp( block( SWTCH, r, t, 0, 0, 0 ) );
		fprintf( outfile, "C %d\t%d\t", swlab, dlab );
		locctr( 4 );
		defalign( ALINT );
		size = n+n;
		if( sectfull( DATA, size ) ) newsect( DATA );
		sectadd( DATA, size );
		for( i=2, j=p[2].sval; i<=n; ++j ){
			fprintf( outfile, "%d\t", j==p[i].sval ? p[i++].slab : dlab );
			}
		fprintf( outfile, "\n" );
		locctr( PROG );
		if( p[1].slab < 0 ) deflab( dlab );
		return;
		}

	if( n >= 4 ){ /* a search in a table of the values */
		dlab = p[1].slab >= 0 ? p[1].slab : getlab();
		tlab = getlab();
		swlab = getlab();
		q->lval = -1;
		s = block( DESC, NIL, NIL, 0, 0, 0 );
		s->lval = n-1;
		s->rval = tlab;
		s = block( SEARCH, q, s, INT, 0, INT );
		t = block( DESC, NIL, NIL, 0, 0, 0 );
		t->lval = n-1;
		t->rval = swlab;
		r = bcon( -dlab );
		t = block( CM, r, t, 0, 0, 0 );
		r = block( SWTCH, s, t, 0, 0, 0 );
		ecomp( r );
		locctr( 4 );
		defalign( ALINT );
		size = (unsigned)(n*SZLONG/SZCHAR);
		if( sectfull( DATA, size ) ) newsect( DATA );
		sectadd( DATA, size );
		fprintf( dfile, "L%d:\n", tlab );
		for( i=2; i<=n; ++i ){
			fprintf( dfile, "\t.word\t" );
			fprintf( dfile, "%ld", p[i].sval );
			putc( '\n', dfile );
			}
		fprintf( outfile, "C %d\t%d\t", swlab, dlab );
		defalign( ALINT );
		for( i=2; i<=n; ++i ) fprintf( outfile, "%d\t", p[i].slab );
		fprintf( outfile, "\n" );
		locctr( PROG );
		if( p[1].slab < 0 ) deflab( dlab );
		return;
		}

	for( i=2; i<=n; ++i ){ /* a compare for each case */
		tlab = (int) bcon( p[i].sval );
		s = bcon( p[i].slab );
		t = r = talloc();
		ncopy( r, q );
		t->lval = n-i;
		t = block( NE, t, (NODE *) tlab, INT, 0, INT );
		r = block( CBRANCH, t, s, INT, 0, INT );
		ecomp( r );
		}
	tfree( q );
	if( p[1].slab >= 0 ) branch( p[1].slab );
	}
