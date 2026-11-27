# include "mfile1"
#include <string.h>
#include <float.h>
#include <math.h>

static char sccsid[] __attribute__((unused)) = "@[$]local1.c\t2.7  08/24/83 14:27:54 Copr. 1981 87wega3.2.";

char *dignames[] = { "0", "1", "2" };	/* a name that starts with one of these gets $ (exname; name is a guess) */

NODE *cast(NODE *p, TWORD t)
{
	/* cast node p to type t */

	p = buildtree( CAST, block( NAME, NIL, NIL, t, 0, (int)t ), p );
	p->in.left->in.op = FREE;
	p->in.op = FREE;
	return( p->in.right );
	}
NODE *clocal(NODE *p)
{

	/* this is called to do local transformations on
	   an expression tree preparitory to its being
	   written out in intermediate code.
	*/

	/* WEGA: labels of statics, float and unsigned compares, conversions that change nothing,
	   pointer arithmetic with long operands, char operands of divisions and compares */

	int o;
	int ml, m;
	register struct symtab *q;
	register NODE *r, *l;
	int i;

	l = p->in.left;
	r = p->in.right;
	o = p->in.op;

	switch( o ){

	case NAME:
		q = STP(p->tn.rval);
		switch( q->sclass ){

		case LABEL:
		case ULABEL:
			if( q->slevel ){
				p->in.op = LAB;
				p->tn.lval = 0;
				p->tn.rval = -q->offset;
				}
			}
		break;

	case EQ:
	case NE:
		if( l->in.type == DOUBLE || l->in.type == FLOAT || r->in.type == DOUBLE || r->in.type == FLOAT ){
		flt:
			p->in.op += FEQ-EQ;
			}
		break;

	case LT:
	case LE:
	case GT:
	case GE:
		if( l->in.type == DOUBLE || l->in.type == FLOAT || r->in.type == DOUBLE || r->in.type == FLOAT ) goto flt;
		if( ISPTR(l->in.type) || ISPTR(r->in.type) ) p->in.op += ULE-LE;
		break;

	case PCONV:
		m = l->in.type;
#ifdef SEG
		if( (m == CHAR || m == UCHAR || m == INT || m == UNSIGNED || m == SHORT || m == USHORT) && l->in.op != ICON ){
			p->in.op = o = SCONV;
			break;
			}
		if( l->in.op == ICON ) l->tn.lval &= 0x7f00ffffL;
	paint:
		l->in.type = p->in.type;
		l->fn.cdim = p->fn.cdim;
		l->fn.csiz = p->fn.csiz;
		p->in.op = FREE;
		p = l;
		return( p );
#else
		if( (m == CHAR || m == UCHAR || m == LONG || m == ULONG) && l->in.op != ICON ){
			p->in.op = o = SCONV;
			break;
			}
		goto paint;
#endif

	case SCONV:
		ml = p->in.type == FLOAT || p->in.type == DOUBLE;
		m = l->in.type == FLOAT || l->in.type == DOUBLE;
		if( ml ) break;
		if( m ) break;
		while( l->in.op == SCONV && tbytes( l->in.type ) > tbytes( p->in.type ) &&
				l->in.left->in.type != DOUBLE && l->in.left->in.type != FLOAT ){
			l->in.op = FREE;
			p->in.left = l = l->in.left;
			}
		ml = p->in.type;
		m = l->in.type;
		if( l->in.op == ICON ){ /* fold the conversion into the constant */
			int32_t val;

			val = l->tn.lval;
#ifdef SEG
			if( ml == INT && ISPTR(m) ){
				l->tn.lval = (int16_t) val;
				break;
				}
			if( ml == UNSIGNED && ISPTR(m) ){
				l->tn.lval = val & 0177777;
				break;
				}
			if( ml == SHORT && ISPTR(m) ){
				l->tn.lval = (int16_t) val;
				break;
				}
			if( ml == USHORT && ISPTR(m) ){
				l->tn.lval = val & 0177777;
				break;
				}
#endif
			switch( ml ){

			case CHAR:
				l->tn.lval = (char) val;
				break;

			case UCHAR:
				l->tn.lval = val & 0377;
				break;

			case USHORT:
			case UNSIGNED:
				l->tn.lval = val & 0177777;
				break;

			case SHORT:
			case INT:
				l->tn.lval = (int16_t) val;
				break;
				}
		settype:
			l->in.type = ml;
			}
		else if( l->in.op == MUL && szty( ml ) - szty( m ) == 1 ) goto settype;	/* the product is long */
		else if( tbytes( ml ) != tbytes( m ) ) break;
#ifdef SEG
		else if( ISPTR(ml) ) break;
#endif
#ifdef SEG
		if( tbytes( ml ) == tbytes( m ) ) goto paint;
#else
		if( tbytes( ml ) == tbytes( m ) ){
		paint:
			l->in.type = p->in.type;
			l->fn.cdim = p->fn.cdim;
			l->fn.csiz = p->fn.csiz;
			}
#endif
		p->in.op = FREE;
		p = l;
		return( p );

	case ASSIGN:
		if( r->in.op == SCONV ){
			ml = r->in.type;
			m = r->in.left->in.type;
			}
		break;

	case PVCONV:
		if( nncon( r ) && (i = ispow2( r->tn.lval )) > 0 ){
			p->in.op = FREE;
			r->tn.lval = i;
			p = buildtree( RS, l, r );
			return( p );
			}
	/* fall through */
	case PMCONV:
		if( r->in.op != ICON ) cerror( "bad conversion", 0 );
		p->in.op = FREE;
		p = buildtree( o == PMCONV ? MUL : DIV, l, r );
		return( p );

	case PLUS:
	case MINUS:
	case MUL:
	case LS:	/* WEGA: pointer arithmetic is done in words */
		if( ISPTR(p->in.type) || ISARY(p->in.type) ){
			if( l->in.type == LONG || l->in.type == ULONG ) l = p->in.left = cast( l, INT );
			if( r->in.type == LONG || r->in.type == ULONG ) r = p->in.right = cast( r, INT );
			}
		break;

	case DIV:
	case MOD:	/* WEGA: a char operand is used without its conversion */
		if( l->in.op == SCONV && bsize( l->in.type ) == SZINT && bsize( l->in.left->in.type ) == SZCHAR ){
			l->in.op = FREE;
			p->in.left = l = l->in.left;
			}
		break;

	case UNARY MINUS:
		if( p->in.type == UCHAR ) p->in.type = CHAR;
		break;
		}

	/* WEGA: a compare of a char with a constant that fits */
	if( logop( p->in.op ) && r && r->in.op == ICON && l->in.op == SCONV && (l->in.left->in.type == CHAR || l->in.left->in.type == UCHAR) &&
			r->tn.lval >= -256 && r->tn.lval <= 255 ){
		l->in.op = FREE;
		l = p->in.left = l->in.left;
		r->in.type = l->in.type;
		if( !logop( p->in.op ) ) p->in.type = l->in.type;
		}

	return(p);
	}
int cisreg(TWORD t)
{ /* is an automatic variable of type t OK for a register variable */

	return( t==INT || t==UNSIGNED || t==LONG || t==ULONG || ISPTR(t) || t==ENUMTY ||
			t==DOUBLE || t==FLOAT || t==SHORT || t==USHORT );
	}
NODE *offcon(OFFSZ off, TWORD t, int d, int s)
{
	(void) t;
	(void) d;
	(void) s;

	/* return a node, for structure references, which is suitable for
	   being added to a pointer of type t, in order to be off bits offset
	   into a structure */

	register NODE *p;

	/* t, d, and s are the type, dimension offset, and sizeoffset */
	/* in general they  are necessary for offcon, but not on H'well */

	p = bcon(0);
	p->tn.lval = off/SZCHAR;
	return(p);

	}

int inwd = 0;	/* current bit offsed in word */
int16_t word = 0;	/* word being built from fields (16 bits: a negative word prints with 4 digits) */

/* a shift of a 32 bit value; a negative count shifts the other way (sdll/sdal of the Z8000) */
static int32_t shl32(int32_t v, int n)
{
	if( n >= 0 ) return( n >= 32 ? 0 : (int32_t) ((uint32_t) v << n) );
	n = -n;
	return( n >= 32 ? (v < 0 ? -1 : 0) : v >> n );
	}

void incode(NODE *p, int sz)
{

	/* generate initialization code for assigning a constant c
		to a field of width sz */
	/* we assume that the proper alignment has been obtained */
	/* inoff is updated to have the proper final value */
	/* we also assume sz  < SZINT */

	word |= shl32( p->tn.lval, SZINT-sz-inwd );
	if( sz+inwd <= SZINT ) inwd += sz;
	inoff += sz;
	if( inwd == 8 ) fprintf( dfile, "\t.byte\t%%%x\n", (unsigned) (uint16_t) (word>>8) );
	else if( sz == SZINT ) fprintf( dfile, "\t.word\t%%%x\n", (unsigned) (uint16_t) word );
	else if( sz == SZLONG ) fprintf( dfile, "\t.long\t%%%lx\n", (unsigned long) (uint32_t) p->tn.lval );
	else return;
	inwd = 0;
	word = 0;
	}

void fincode(double d, int sz)
{
	/* output code to initialize space of size sz to the value d */
	/* the proper alignment has been obtained */
	/* inoff is updated to have the proper final value */
	/* on the target machine, write it out in octal! */

	float f;
	uint64_t du;
	uint32_t fu;

	/* the words of the number as the Z8000 holds it: IEEE, big endian, the first word has the sign */
	if( sz == SZDOUBLE ){
		memcpy( &du, &d, sizeof(du) );
		fprintf( dfile, "\t.word\t%%%x\n\t.word\t%%%x\n\t.word\t%%%x\n\t.word\t%%%x\n",
			(unsigned) (du >> 48) & 0xFFFF, (unsigned) (du >> 32) & 0xFFFF, (unsigned) (du >> 16) & 0xFFFF, (unsigned) du & 0xFFFF );
		}
	else {
		f = d;
		memcpy( &fu, &f, sizeof(fu) );
		fprintf( dfile, "\t.word\t%%%x\n\t.word\t%%%x\n", (unsigned) (fu >> 16) & 0xFFFF, (unsigned) fu & 0xFFFF );
		}
	inoff += sz;
	}

void cinit(NODE *p, int sz)
{
	/* arrange for the initialization of p into a space of size sz */
	struct symtab *q;
	char *s;

	switch( p->in.left->in.op ){

	case ICON:
		incode( p->in.left, sz );
		return;

	case NAME:
		q = STP(p->in.left->tn.rval);
		if( q->sclass == STATIC && q->slevel > 0 ) fprintf( dfile, "\t.addr\tL%ld\n", q->offset );
		else {
			s = exname( q->sname );
			fprintf( dfile, "\t.addr\t%s\n", s );
			}
		inoff += sz;
		return;

	case LAB:
		fprintf( dfile, "\t.addr\tL%d\n", (int) -p->in.left->tn.rval );
		break;

	default:
		if( ISPTR(p->in.type) ) fprintf( dfile, "\t.addr\t" );
		else fprintf( dfile, "\t.word\t" );
		cprint( p );
		putc( '\n', dfile );
		}
	inoff += sz;
	}

void cprint(NODE *p)
{ /* print the constant expression p into the .D file (name is a guess) */
	int o;
	struct symtab *q;
	char *s;

	if( (o = optype(p->in.op)) == LTYPE ){
		switch( p->in.op ){

		case ICON:
			fprintf( dfile, "%d", p->tn.lval );
			break;

		case NAME:
			q = STP(p->tn.rval);
			if( q->sclass == STATIC && q->slevel > 0 ) fprintf( dfile, "L%ld", q->offset );
			else if( q->sclass == MOS ) fprintf( dfile, "%d", (int) (q->offset/SZCHAR) );
			else {
				s = exname( q->sname );
				fprintf( dfile, "%s", s );
				}
			break;

		case LAB:
			fprintf( dfile, "L%d", (int) -p->tn.rval );
			break;
			}
		return;
		}
	if( o == UTYPE ){
		cprint( p->in.left );
		return;
		}
	cprint( p->in.left );
	switch( p->in.op ){

	case PLUS:
	case STREF:
	case SUBSR:
		putc( '+', dfile );
		break;

	case MINUS:
		putc( '-', dfile );
		break;
		}
	cprint( p->in.right );
	}

void vfdzero(int n)
{ /* define n bits of zeros in a vfd */

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

char *exname(int s)
{ /* make a name look like an external name in the local machine */
	register char *p, *q;
	int i;
	char c;

	p = &ntab[s];
	q = extext;
	*q++ = ( (c = *p) == *dignames[0] || *dignames[1] == c || *dignames[2] == c ) ? '$' : '_';
	for( i=1; *p; ++i ){
		if( i > ncharsig ) break;
		*q++ = *p++;
		}
	*q = '\0';
	if( extext[0] == '_' ) extext[ncharsig] = '\0';
	else extext[ncharsig+1] = '\0';
	return( extext );
	}

int ctype(TWORD type)
{ /* map types which are not defined on the local machine */
	switch( BTYPE(type) ){

	case SHORT:
		MODTYPE(type,INT);
		break;

	case USHORT:
		MODTYPE(type,UNSIGNED);
		}
	return( type );
	}

int noinit(void)
{ /* curid is a variable which is defined but
	is not initialized (and not a function );
	This routine returns the stroage class for an uninitialized declaration */

	return(EXTERN);

	}

void commdec(struct symtab *q)
{ /* make a common declaration for q */
	OFFSZ sz;
	char *s;

	lineid();
	sz = tsize( q->stype, q->dimoff, q->sizoff )/SZCHAR;
	s = exname( q->sname );
	fprintf( dfile, ".comm\t%ld,\t%s;\n", sz, s );
	if( sz > 0xfffe ) uerror( "size too large" );
	}

/* The atof of the WEGA libc (src/lib/libc/nonseg/gen/atof.s, pow10.s), which the original reads its floating constants with:
   up to 19 significant digits as an integer M (the BCD operand of fldbcd), the exponent E normalized as atof.s does
   (E > 0: M times 10 while its 19th digit is 0; E < 0: M by 10 while its last digit is 0), then in the extended format of
   the EPU (64-bit mantissa, round to nearest) M times 10^E or M by 10^-E, 10^n made as __pow10 makes it, and rounded to
   double at the end. The host's long double (x87) has the same format: the double rounding of the original, which does not
   always give the nearest double (0.1013863514358673989967045588e10 of libm/j1.c: ...e907, the nearest is ...e908). */
#if LDBL_MANT_DIG == 64
typedef long double XF;

static XF xf_ext(unsigned short se, unsigned short m0, unsigned short m1, unsigned short m2, unsigned short m3)
{
	unsigned long long m = (unsigned long long) m0 << 48 | (unsigned long long) m1 << 32 | (unsigned long long) m2 << 16 | m3;
	unsigned char b[16] = { 0 };
	long double x;

	memcpy( b, &m, 8 );		/* x87: the mantissa, then sign and exponent (little endian) */
	memcpy( b + 8, &se, 2 );
	memcpy( &x, b, sizeof x < sizeof b ? sizeof x : sizeof b );
	return( x );
	}

static XF xf_u64(unsigned long long m) { volatile long double f = (long double) m; return( f ); }
static XF xf_mul(XF a, XF b) { volatile long double f = a * b; return( f ); }
static XF xf_div(XF a, XF b) { volatile long double f = a / b; return( f ); }
static double xf_double(XF f) { return( (double) f ); }
#else
/* No x87 (arm64: long double is a double): the extended format in software. Value = m / 2^63 * 2^e, m with its top bit set;
   multiplication and division round to nearest even to 64 bits, the conversion to double again (denormals included). */
typedef struct { int inf, zero; unsigned long long m; int e; } XF;

static XF xf_ext(unsigned short se, unsigned short m0, unsigned short m1, unsigned short m2, unsigned short m3)
{
	XF x = { 0, 0, 0, 0 };

	if( (se & 0x7fff) == 0x7fff ){ x.inf = 1; return( x ); }
	x.m = (unsigned long long) m0 << 48 | (unsigned long long) m1 << 32 | (unsigned long long) m2 << 16 | m3;
	x.e = (se & 0x7fff) - 16383;
	return( x );
	}

static XF xf_u64(unsigned long long m)
{
	XF x = { 0, 0, 0, 63 };

	if( m == 0 ){ x.zero = 1; return( x ); }
	while( !(m >> 63) ){ m <<= 1; --x.e; }
	x.m = m;
	return( x );
	}

static XF xf_round(unsigned long long hi, unsigned long long lo, int e)
{ /* hi has its top bit set; lo the 64 bits behind it */
	XF x = { 0, 0, 0, 0 };

	if( lo > 0x8000000000000000ULL || (lo == 0x8000000000000000ULL && (hi & 1)) )
		if( ++hi == 0 ){ hi = 0x8000000000000000ULL; ++e; }
	if( e > 16383 ){ x.inf = 1; return( x ); }
	if( e < -16382 ){ x.zero = 1; return( x ); }
	x.m = hi;
	x.e = e;
	return( x );
	}

static XF xf_mul(XF a, XF b)
{
	unsigned __int128 p;
	int e;

	if( a.inf || b.inf ){ a.inf = 1; a.zero = 0; return( a ); }
	if( a.zero || b.zero ){ a.zero = 1; return( a ); }
	p = (unsigned __int128) a.m * b.m;
	e = a.e + b.e;
	if( p >> 127 ) ++e;
	else p <<= 1;
	return( xf_round( (unsigned long long) (p >> 64), (unsigned long long) p, e ) );
	}

static XF xf_div(XF a, XF b)
{
	unsigned __int128 q, r;
	int e = a.e - b.e;

	if( b.inf ){ a.inf = 0; a.zero = 1; return( a ); }
	if( a.inf || a.zero ) return( a );
	if( a.m >= b.m ) q = ((unsigned __int128) a.m << 63) / b.m, r = ((unsigned __int128) a.m << 63) % b.m;
	else q = ((unsigned __int128) a.m << 64) / b.m, r = ((unsigned __int128) a.m << 64) % b.m, --e;
	/* q has its top bit at 63; r / b.m is the part behind it: as the 64 bits "lo" 2r / b.m rounds the same way */
	r <<= 1;
	return( xf_round( (unsigned long long) q, r > b.m ? 0x8000000000000001ULL : r == b.m ? 0x8000000000000000ULL : 0, e ) );
	}

static double xf_double(XF f)
{
	unsigned long long hi, rem, half;
	int sh;

	if( f.inf ) return( HUGE_VAL );
	if( f.zero ) return( 0.0 );
	if( f.e > 1023 ) return( HUGE_VAL );
	sh = 11;
	if( f.e < -1022 ) sh += -1022 - f.e;
	if( sh > 64 ) return( 0.0 );
	if( sh == 64 ){ hi = 0; rem = f.m; }
	else{ hi = f.m >> sh; rem = f.m & ((1ULL << sh) - 1); }
	half = 1ULL << (sh - 1);
	if( rem > half || (rem == half && (hi & 1)) ) ++hi;
	if( f.e < -1022 ) return( ldexp( (double) hi, -1074 ) );
	return( ldexp( (double) hi, f.e - 52 ) );
	}
#endif

static XF pow10x(int n)
{ /* __pow10 of pow10.s: n >= 5000 overflows; above 27 the constants of MAP, the rest by squaring 10 */
	static const unsigned short map[8][6] = {
		{ 3456, 0x6cd7, 0xbfce, 0x0f5a, 0xb8a6, 0x761e }, { 1728, 0x555d, 0x9cb0, 0x0bfd, 0x6f02, 0x5339 },
		{ 864, 0x4b25, 0x8d9e, 0x89d1, 0x1346, 0xbda5 }, { 432, 0x459a, 0x86a3, 0x364e, 0xa62c, 0x672d },
		{ 216, 0x42cc, 0xb9a7, 0x4a06, 0x37ce, 0x2ee1 }, { 108, 0x4165, 0xda01, 0xee64, 0x1a70, 0x8dea },
		{ 54, 0x40b2, 0xa70c, 0x3c40, 0xa64e, 0x6c52 }, { 27, 0x4058, 0xcecb, 0x8f27, 0xf420, 0x0f3a } };
	XF f0 = xf_u64( 1 ), f1 = xf_u64( 1 ), f2 = xf_u64( 10 );
	int i;

	if( n >= 5000 ) return( xf_ext( 0x7fff, 0x8000, 0, 0, 0 ) );
	if( n > 27 )
		for( i = 0; i < 8; ++i )
			if( n >= map[i][0] ){
				f0 = xf_mul( f0, xf_ext( map[i][1], map[i][2], map[i][3], map[i][4], map[i][5] ) );
				n -= map[i][0];
				}
	while( n ){
		if( n & 1 ) f1 = xf_mul( f1, f2 );
		f2 = xf_mul( f2, f2 );
		n >>= 1;
		}
	return( xf_mul( f0, f1 ) );
	}

static double wega_atof(char *s)
{
	unsigned long long m = 0;
	int nd = 0, e = 0, x = 0, xs = 1, neg = 0, seen = 0;
	XF f;

	while( *s == ' ' || *s == '\t' ) ++s;
	if( *s == '-' ){ neg = 1; ++s; }
	else if( *s == '+' ) ++s;
	for( ; *s >= '0' && *s <= '9'; ++s ){
		if( *s == '0' && !seen ) continue;
		seen = 1;
		if( nd < 19 ){ m = m * 10 + (*s - '0'); ++nd; }
		else ++e;			/* a digit of the integer part behind the 19th: the exponent */
		}
	if( *s == '.' )
		for( ++s; *s >= '0' && *s <= '9'; ++s ){
			if( *s == '0' && !seen ){ --e; continue; }
			seen = 1;
			if( nd < 19 ){ m = m * 10 + (*s - '0'); ++nd; --e; }
			}
	if( *s == 'e' || *s == 'E' ){
		++s;
		if( *s == '-' ){ xs = -1; ++s; }
		else if( *s == '+' ) ++s;
		while( *s >= '0' && *s <= '9' ) x = x * 10 + (*s++ - '0');
		}
	e += xs * x;
	if( e > 0 )
		while( e && m < 1000000000000000000ULL ){ m *= 10; --e; }
	else
		while( e && m % 10 == 0 ){ m /= 10; ++e; }
	f = xf_u64( m );
	if( e < 0 ) f = xf_div( f, pow10x( -e ) );
	else f = xf_mul( pow10x( e ), f );
	return( neg ? -xf_double( f ) : xf_double( f ) );
	}

int isitfloat(char *s)
{
	dcon = wega_atof(s);
	return( FCON );
	}

void ecode(NODE *p)
{

	/* walk the tree and write out the nodes.. */

	if( nerrors ) return;
	if( p->in.op == CBRANCH && p->in.left->in.op == FREE ){
		p->in.right->in.op = FREE;
		p->in.op = FREE;
		}
	if( p->in.op == FREE ) return;
	putc( '.', outfile );
	fprintf( outfile, "%d\t%s\n", lineno, ftitle );
	prtree( p );
	}
