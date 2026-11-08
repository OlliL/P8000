# include "mfile1"
# include <signal.h>

static char sccsid[] __attribute__((unused)) = "@[$]code1.c\t2.7  08/24/83 14:23:01 Copr. 1981 87wega3.2.";

int lflag = 0;	/* -l: write .line directives into the .D file */
int pflag = 0;	/* -p: profiling, a counter word for each function */
int strftn = 0;	/* is the current function one which returns a value */
int sectpre = 5;	/* length of the name prefix kept when a section is renamed */
char sectname[3][24] = { "modul_p", "modul_d", "modul_b" };	/* the names of the sections */
long sectsize[3] = { 0, 0, 0 };	/* bytes used in each section */

FILE *Ffile;	/* the file of -F */
long ftnpos;	/* position of the function head in the .I file (rewritten by ftnend) */
FILE *outfile, *sfile;	/* the .I and the .S file */
FILE *dfile;	/* the .D file */
char ifname[50], dfname[50], sfname[50];	/* the names of the output files */
char *ipath, *dpath, *spath;
#define BUFSZ 512	/* BUFSIZ of the stdio.h of the WEGA (setbuf) */
char ibuf[BUFSZ], dbuf[BUFSZ], sbuf[BUFSZ];
extern int crslab, curloc, ftnno, retlab, regflag;
extern struct symtab *curftn;

int main(int argc, char *argv[])
{
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

void openfiles(int argc, char *argv[])
{ /* -E base: create base.I, base.D and base.S */
	register int i, fd;
	register char *cp;

	for( i=1; i<argc; ++i ){
		cp = argv[i];
		if( *cp == '-' && *++cp == 'E' ){
			++i;
			ipath = (char *) strcpy( ifname, argv[i] );
			ipath = (char *) strcat( ipath, ".I" );
			if( (fd = creat( ipath, 0660 )) < 0 ) syserr( ipath );
			if( (outfile = fdopen( fd, "w" )) == NULL ) syserr( ipath );
			setvbuf( outfile, ibuf, _IOFBF, BUFSZ );
			dpath = (char *) strcpy( dfname, argv[i] );
			dpath = (char *) strcat( dpath, ".D" );
			if( (fd = creat( dpath, 0660 )) < 0 ) syserr( dpath );
			if( (dfile = fdopen( fd, "w" )) == NULL ) syserr( dpath );
			setvbuf( dfile, dbuf, _IOFBF, BUFSZ );
			spath = (char *) strcpy( sfname, argv[i] );
			spath = (char *) strcat( spath, ".S" );
			if( (fd = creat( spath, 0660 )) < 0 ) syserr( spath );
			if( (sfile = fdopen( fd, "w" )) == NULL ) syserr( spath );
			setvbuf( sfile, sbuf, _IOFBF, BUFSZ );
			}
		}
	}

void fnfile(char *s)
{ /* -F s: the names go to the file s */
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

int fltlab(double d)
{ /* the label of the floating constant d; crslab if it is new */
	register struct fcon *p;
	int h;

	h = fhashv( (char *) &d, 61 );
	for( p = fhash[h]; p; p = p->fnext ){
		if( d == p->fval ) return( p->flab );
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

int fhashv(char *p, int n)
{ /* hash of the double at p */
	char *q;
	union { long l; char c[4]; } u;

	q = p;
	u.c[0] = 0;
	u.c[1] = q[3];
	u.c[2] = q[1];
	u.c[3] = q[2];
	return( u.l % n );
	}

void newsect(int sec)
{ /* start a new section sec (DATA or ADATA) */
	char *cp, *q;
	char buf[4];
	int k, n, len;

	if( sec <= 0 || sec >= 3 ) cerror( "declaring improper section" );
#ifdef SEG
	if( nsect >= 127 ) uerror( "too many sections" );
#else
	if( nsect >= 3 ) uerror( "too many sections" );
#endif
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
	for( ; *cp; ++q, ++cp ) *q = *cp;
	*q = 0;
	sectnew = 0;
	sectsize[sec] = 0;
#ifdef SEG
	fprintf( dfile, "\t.psec %s\n", sectname[sec] );
#endif
	if( sec == DATA ) fprintf( dfile, "\t.data\n" );
	else fprintf( dfile, "\t.bss\n" );
	}

int sectfull(int sec, long n)
{ /* do n more bytes not fit into the section sec */
	return( 0xfffeL - sectsize[sec] < n ? 1 : 0 );
	}

void sectadd(int sec, long n)
{ /* n more bytes in the section sec */
	if( sectfull( sec, n ) ) uerror( "section overflow" );
	else sectsize[sec] += n;
	}

void bstart(long off, int regs, int fregs)
{ /* the head of a function body in the .I file */
	++ftnno;
	putc( '[', outfile );
	fprintf( outfile, "%ld\t", off );
	fprintf( outfile, "%d\t", regs );
	fprintf( outfile, "%d\t", fregs );
	putc( '\n', outfile );
	}

void bend(void)
{ /* the end of a function in the .I file */
	putc( '}', outfile );
	putc( '\n', outfile );
	}

void defalign(int n)
{ /* cause the alignment to become a multiple of n */

	n /= SZCHAR;
	if( curloc != PROG && n > 1 ){
		if( sectfull( DATA, 1L ) ) newsect( DATA );
		else if( sectsize[DATA] & 1 ) ++sectsize[DATA];
		fprintf( dfile, "    .even\n" );
		}
	}

int getlab(void)
{
	return( crslab++ );
	}

int locctr(int l)
{
	int temp;
	/* l is PROG, ADATA, DATA, STRNG, ISTRNG, or STAB */

	if( !sectnew && blevel <= 1 && l != curloc && l == PROG && curloc != ISTRNG && curloc != STRNG )
#ifdef SEG
		{
		fprintf( dfile, "\t.psec\n" );
		fprintf( dfile, "\t.code\n" );
		}
#else
		fprintf( dfile, "\t.code\n" );
#endif
	if( l == curloc ) return( l );
	temp = curloc;
	curloc = l;
	return( temp );
	}

void deflab(int n)
{
	fprintf( outfile, "L%d\t\n", n );
	}

void efcode(void)
{
	/* code for the end of a function */
	register int i;
	register struct symtab *p;
	long size;

	deflab( retlab );
	if( strftn ){  /* a static area for the returned structure */
		p = curftn;
		i = getlab();
		curftn->sfld8 = i;
		size = tsize( DECREF(p->stype), p->dimoff, p->sizoff ) / SZCHAR;
		if( sectfull( ADATA, size ) ) newsect( ADATA );
		else {
#ifdef SEG
			fprintf( dfile, "\t.psec %s\n", sectname[ADATA] );
#endif
			fprintf( dfile, "\t.bss\n" );
			}
		sectadd( ADATA, size );
		fprintf( dfile, "L%d:\t.=.+%ld\n", i, size );
		fprintf( dfile, "\t.data\n" );
		}
	bend();
	lineid();
	}

void bfcode(struct symtab *a[], int n)
{
	/* code for the beginning of a function; a is an array of
		the entries of the arguments; n is the number */
	int i;
	int temp;
	int r;
	register struct symtab *p;
	OFFSZ off;
	int words, flags, lab;

	(void) r;
	(void) words;
	(void) flags;
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

void bccode(void)
{ /* called just before the first executable statment */
	SETOFF( autooff, SZINT );
	bstart( autooff, minrvar, fminrvar );
	}

void bfend(void)
{ /* "]" into the .I file */
	fprintf( outfile, "]\n" );
	}

void defnam(struct symtab *p)
{
	/* define the current location as the name p->sname */

	if( p->sclass == STATIC && p->slevel>1 ) fprintf( dfile, "L%d:\n", (int)p->offset );
	else if( p->sclass == EXTDEF ) fprintf( dfile, "%s::\n", exname( p->sname ) );
	else fprintf( dfile, "%s:\n", exname( p->sname ) );
	}

void zecode(long n)
{
	/* n integer words of zeros */
	OFFSZ temp;

	if( n <= 0 ) return;
	if( n == 1 ) fprintf( dfile, "\t.word\t0\n" );
	else fprintf( dfile, "\t.=.+%ld\n", n*2 );
	temp = n;
	inoff += temp*SZINT;
	}

int fldal(unsigned t)
{
	(void) t; /* return the alignment of field of type t */
	uerror( "illegal field type" );
	return( ALINT );
	}

void lineid(void)
{ /* a .line directive for the current line of a .c file (-l) */
	register char *cp;
	int len;

	if( lflag ){
		len = strlen( ftitle );
		if( *(cp = &ftitle[len-1]) == '"' ) --cp;
		if( *cp-- == 'c' && *cp == '.' ) fprintf( dfile, "\t.line %d %s \n", lineno, ftitle );
		}
	}

void genswitch(struct sw *p, int n)
{
	/*	p[0] holds the switch expression, p[1] the default label (>=0 if there is one),
		p[2] to p[n+1] are the cases */
	int i;
	register CONSZ j, range;
	int dlab;
	int swlab;
	long size;
	NODE *q;
	int tlab;
	NODE *tn;
	NODE *s, *t, *r;

	range = p[++n].sval-p[2].sval;
	q = *(NODE **)p;

#ifdef OLD_D6D
	if( range>0 && range <= 3*(n-1) && n>=5 ){ /* implement a direct switch (v0: 3 values per case, n counts one too many) */
#else
	if( range>0 && range <= 3*n && n>=5 ){ /* implement a direct switch */
#endif
		dlab = p[1].slab >= 0 ? p[1].slab : getlab();
		q->tn.lval = -1;
#ifdef OLD_D6D
		if( p[2].sval ){
			t = bcon( 0 );
			t->tn.lval = p[2].sval;
			r = block( MINUS, q, t, INT, 0, INT );
			}
		else r = q;
#else
		r = p[2].sval ? block( MINUS, q, bcon( p[2].sval ), INT, 0, INT ) : q;
#endif
		t = block( DESC, NIL, NIL, 0, 0, 0 );
		t->tn.lval = range;
		t->tn.rval = swlab = getlab();
		t = block( CM, bcon( -dlab ), t, 0, 0, 0 );
		ecomp( block( SWTCH, r, t, 0, 0, 0 ) );
		fprintf( outfile, "C %d\t%d\t", swlab, dlab );
		locctr( 4 );
		defalign( ALINT );
#ifdef SEG
		if( sectfull( DATA, size = n*4 ) ) newsect( DATA );
#else
		if( sectfull( DATA, size = n+n ) ) newsect( DATA );
#endif
		sectadd( DATA, size );
		for( i=2, j=p[2].sval; i<=n; ++j ){
			fprintf( outfile, "%d\t", j==p[i].sval ? p[i++].slab : dlab );
			}
		fprintf( outfile, "\n" );
		locctr( PROG );
		if( p[1].slab < 0 ) deflab( dlab );
		return;
		}

#ifdef OLD_D6C
	if( n >= 5 ){ /* a search in a table of the values (v0: from 4 cases on, up to 3 compares) */
#else
	if( n >= 4 ){ /* a search in a table of the values */
#endif
		dlab = p[1].slab >= 0 ? p[1].slab : getlab();
		tlab = getlab();
		swlab = getlab();
		q->tn.lval = -1;
		s = block( DESC, NIL, NIL, 0, 0, 0 );
		s->tn.lval = n-1;
		s->tn.rval = tlab;
		s = block( SEARCH, q, s, INT, 0, INT );
		t = block( DESC, NIL, NIL, 0, 0, 0 );
		t->tn.lval = n-1;
		t->tn.rval = swlab;
		r = bcon( -dlab );
		t = block( CM, r, t, 0, 0, 0 );
		r = block( SWTCH, s, t, 0, 0, 0 );
		ecomp( r );
		locctr( 4 );
		defalign( ALINT );
#ifdef SEG
		if( sectfull( DATA, size = n*(SZPOINT+SZINT)/SZCHAR ) ) newsect( DATA );
#else
		if( sectfull( DATA, size = n*SZLONG/SZCHAR ) ) newsect( DATA );
#endif
		sectadd( DATA, size );
#ifdef OLD_D6T
		/* v0: the values go to codgen with the targets, which writes the two lists together */
		fprintf( outfile, "V %d\t%d\t%d\t", swlab, dlab, tlab );
		for( i=2; i<=n; ++i ) fprintf( outfile, "%ld\t", p[i].sval );
		fprintf( outfile, "\n" );
#else
		fprintf( dfile, "L%d:\n", tlab );
		for( i=2; i<=n; ++i ){
			fprintf( dfile, "\t.word\t" );
			fprintf( dfile, "%ld", (long) p[i].sval );
			putc( '\n', dfile );
			}
#endif
		fprintf( outfile, "C %d\t%d\t", swlab, dlab );
		defalign( ALINT );
		for( i=2; i<=n; ++i ) fprintf( outfile, "%d\t", p[i].slab );
		fprintf( outfile, "\n" );
		locctr( PROG );
		if( p[1].slab < 0 ) deflab( dlab );
		return;
		}

	for( i=2; i<=n; ++i ){ /* a compare for each case */
		tn = bcon( p[i].sval );
		s = bcon( p[i].slab );
		r = t = talloc();
		ncopy( r, q );
		t->tn.lval = n-i;
		t = block( NE, t, tn, INT, 0, INT );
		r = block( CBRANCH, t, s, INT, 0, INT );
		ecomp( r );
		}
	tfree( q );
	if( p[1].slab >= 0 ) branch( p[1].slab );
	}
