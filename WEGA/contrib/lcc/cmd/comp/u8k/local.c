# include "mfile1.h"
#include <storclass.h>

/*	the following used to be on the old local.c file */
/* the register variables are %d2 through %d7 and %a2 through %a5 */
/* everything is saved on the stack as a 4 byte quantity */
/* the machine-independent part maintains a quantity regvar that is */
/* saved and restored across block boundaries */
/* if cisreg likes the type, it calls rallc to get a register number */
/* rallc puts the register number into nextrvar, and updates regvar */
/* rallc returns 0 if there are no free regs of that type, else 1 */

#define EXPSHIFT 7
#define	EXPMASK	(0xff << EXPSHIFT)	/* mask to access exponent on VAX */
#define EXPLORD ( 1L << EXPSHIFT )	/* low order exponent bit on VAX */
#define SIGNMASK ( 1L << 15 )		/* Vax sign bit */

/* Berechnen der Datensegmente */

char datanm[8] = "data   ";
static unsigned short datasiz[32];
xxx(i)
register unsigned i;
{
	register int j;
	register long l;

	for (j = 0; j < 32; j++)
	{
		l = datasiz[j];
		l += i;
		if ( l >= 0xfff8l )
			continue;
		datasiz[j] = l;
		if (j)
			sprintf(datanm, "modul%d", j+3);
		else
			strcpy(datanm, "data");
		return(0);
	}
	uerror( "data segments exhausted");
}

fincode( d, sz )
double d;
{
	/* initialization of floats */
	union {
		float  f;
		double d;
		struct { long l,l2;	 } lg;
		struct { short slo, shi; } sh;
	} x;
	long z;
	register long exponent,fract1,fract2,sign;

#ifndef ONEFLOAT
	if ( sz == SZDOUBLE )
	{
	   x.d = d;
	   printf("	.long	%%%lx\t// %e \n", x.lg.l, d );
	   printf("	.long	%%%lx\n", x.lg.l2);
	}
	else
#endif
	{
	   x.f = d;
	   printf("	.long	%%%lx\t// %e */\n", x.lg.l, d );
	}
	inoff += sz;
}

rallc( t )
register TWORD t;
{
	register int i;
	if ( ISPTR(t) || t == LONG || t == ULONG)
	{
		if (regvar & 0xc00)
		{
			if (regvar & 0x300)
				return(0);
			else
			{
				regvar |= 0x300;
				nextrvar = 8;
			}
		}
		else
		{
			regvar |= 0xc00;
			nextrvar = 10;
		}
		return(1);
	}
	else
	{
		for (i = 11; i >= 8; i--)
		{
			if (regvar & (1<<i)) continue;
			nextrvar = i;
			regvar |= (1<<i);
			return(1);
		}
	}
	return(0);
}

NODE *
clocal(p)
register NODE *p;
{
	return(p);
}

myinit( p, sz )
register NODE *p;
{
	register TWORD t;
	register NODE *q;
	if( p->in.left->tn.op != ICON ) yyerror( "illegal initializer" );
	t = p->tn.type;
	q = p->in.left;
	inoff += sz;
	if( q->tn.rval != NONAME )	/* do real pointers in pass 2 */
	{
	    ecode( p );
	    return;
	}

/* NOTE:  assembler will complain about constants that are too big
** for the type of item that is being initialized (like "byte 0x1000").
** The compiler should produce a message about this some day.
*/

	switch( (int)t )
	{
	case CHAR:
	case UCHAR:
		printf( "\t.byte\t%%%lx\n", q->tn.lval );
		return;

	case INT:
	case UNSIGNED:
		printf( "\t.word\t%%%x\n",(unsigned short) (q->tn.lval) );
		return;

	default:
		if( !ISPTR(t) )
			werror( "initialization error probable");
		/* FALLTHRU */

	case LONG:
	case ULONG:
		printf( "\t.long\t%%%lx\n", q->tn.lval );
		return;
	}
}

cisreg( t )
register TWORD t;
{
	/* is an automatic variable of type t OK for a register variable */

	switch( t )
	{
	default:
		if( !ISPTR(t) ) return( 0 );
		/* FALLTHRU */
	case SHORT:
	case USHORT:
	case INT:
	case UNSIGNED:
	case LONG:
	case ULONG:
/*
	case CHAR:
	case UCHAR:
	case FLOAT:
no characters and floats in registers (of) */
#ifdef ONEFLOAT
	case DOUBLE:
#endif
		return(rallc(t));
	}
}

static int	inwd;	/* current bit offset in word */
static long	word;	/* word being built from fields */

incode( p, sz )
register NODE *p;
{
	register long v;

	/* generate initialization code for assigning a constant c
	** to a field of width sz 
	*/
	/* we assume that the proper alignment has been obtained */
	/* inoff is updated to have the proper final value */
	/* go to some pain to handle the left-to-right fields, and defend
	/* against possible long bitfield initialization */
	/* ordinary (int sized) fields are
	/* left to right for the 68000 */

	v = p->tn.lval & ((1<<sz)-1);
	if( SZLONG-inwd-sz ) v <<= (SZLONG-inwd-sz);
	word |= v;
	inwd += sz;
	inoff += sz;
	if( inoff%ALSHORT == 0 )
	{
		if( inwd == SZSHORT )
/*			printf( "	short	0x%.4x\n", */
			printf( "\t.word\t%%%.4x\n",
				(unsigned short)(word>>SZSHORT));
		else
/*			printf( "	long	0x%.8lx\n", word); */
			printf( "\t.long\t%%%.8lx\n", word );
		word = inwd = 0;
	}
}

vfdzero( n )
register n;
{
	/* define n bits of zeros in a vfd */

	if( n <= 0 ) return;
	inwd += n;
	inoff += n;
	if( inoff%ALSHORT == 0 )
	{
		if( inwd == SZSHORT )
			printf( "\t.word\t%%%.4x\n",
				(unsigned short)(word>>SZSHORT));
		else
			printf( "\t.long\t%%%.8lx\n", word);
		word = inwd = 0;
	}
}

char *
exname( p )
char *p;
{
	return( p );
}

commdec( id )
{
	/* make a common declaration for id, if reasonable */
	register struct symtab *q;
	register OFFSZ sz;

	q = &stab[id];
	sz = tsize( q->stype, q->dimoff, q->sizoff )/SZCHAR;
	if (sz < 0)
	{
		int val;

		val = sz;
		if (val == sz)
		{
			if (nerrors)
				nerrors--;
			sz = (unsigned)val;
		}
	}

# ifndef NOSYMB
	if ( q->slevel == 0)
		    prdef(q, 0);	/* output debug info */
# endif
	if( q->sclass==STATIC ) 
	{
		xxx((int)sz);
		if( q->slevel )
			printf("\t.psec\t%s\n\t.data\n\t.even\nL%d:\n",
					datanm,(int)(q->offset) );
		else
			printf( "\t.even\n_%s:\n", exname( q->sname ) );
		printf( "\t.=.+%ld\n", (sz%2)? sz+1: sz);
		if( q->slevel )
			printf( "\t.psec\n\t.code\n" );
	}
	else {
		printf( "\t.comm\t%ld,\t_%s;\n", sz, exname( q->sname) );
	}
}

/* the following is the stuff on the old code.c file */

branch( n )
{
	/* output a branch to label n */
	if( !reached ) return;
	printf( "\tjpr\tL%d\n", n );
}

deflab( n )
{
	/* output something to define the current position as label n */
	printf( "L%d:\n", n );
}

defalign(n)
register n;
{
	/* cause the alignment to become a multiple of n */
	n /= SZCHAR;
	if( curloc != PROG && n > 1 ) printf( "\t.even\n" );
}

char *locnames[] = 
{
	/* location counter names for PROG, ADATA, DATA, ISTRNG, STRNG */
	"\t.psec\n\t.code\n",
	0,
	"\t.psec\t%s\n\t.data\n",
	0,
	"\t.psec\tstr%s\n\t.data\n",
};

eccode(blevel)
int blevel;
{
	;			/* called at end of every block */
}

efcode()
{
	/* code for the end of a function */

	deflab( retlab );  /* define the return location */

	efdebug();		/* end of function debug info */

	if( strftn )
	{
		  /* copy output to caller */
		printf( "\tldl\trr2,#___StRet\n" );
	}

	printf("\tadd\tsp,#~L2-%%c\n\tldm\tr8,@SP,#6\n\tinc\tsp,#12\n");
	printf("\tret\n");
}

bfcode( a, n )
int a[];
{

	/* code for the beginning of a function; a is an array of
	** indices in stab for the arguments; n is the number 
	*/

#ifdef	STKCK
	register	spgrow_lbl;
	extern int stkckflg;
#endif

#ifdef	PROFILE
	extern int proflg;

	/*
	** code to provide profiling via prof(1)
	**
	*/
	if( proflg )
	{
		register int i;

		i = getlab();
		printf("	mov.l	&L%%%d,%%a0\n", i);
		printf("	jsr	mcount%%\n");
		printf("	data\n");
		printf("	even\n");
		printf("L%%%d:\n", i);
		printf("	long	0\n");
		printf("	text\n");
	}
#endif	PROFILE

#ifdef	STKCK

	/*
	** code needed for M68000 to check stack growth
	** against current splimit%.  if we are about to
	** exceed current limit, ask system for more stack.
	**
	*/

	if( stkckflg ) 			/* if stack checking requested */
	{
		spgrow_lbl = getlab();
		printf( "	lea.l	F%%%d-256(%%sp),%%a0\n", ftnno );
		printf( "	cmp.l	%%a0,splimit%%\n" );
		printf( "	bhi.b	L%%%d\n", spgrow_lbl );
		printf( "	jsr	spgrow%%\n" );
		deflab( spgrow_lbl );
	}
#endif


	retlab = getlab();

	/* routine prolog */

	printf("\t{\n\tdec\tsp,#12\n\tldm\t@SP,r8,#6\n\tsub\tsp,#~L2-%%c\n");
	printf("\tld\tfp,sp\n\tld\t@SP,#~L2-4\n");
/*	printf( "	movm.l	&M%%%d,S%%%d(%%fp)\n", ftnno, ftnno ); */

	bfdebug( a, n );		/* do debug info at fcn begin */
}

defnam( p )
register struct symtab *p;
{
	/* define the current location as the name p->sname
	 * first give the debugging info for external definitions
	 */
	if( p->slevel == 0 )	/* make sure it's external */
		prdef(p,0);

	if( p->sclass == EXTDEF )
		printf( "_%s::\n", exname( p->sname ) );
/*		printf( "	global	%s\n", exname( p->sname ) ); */
	else
		printf( "_%s:\n", exname( p->sname ) );
}

/*static char	*xtab[] = 
{
	"0x00", "0x01", "0x02", "0x03", "0x04", "0x05", "0x06", "0x07",
	"'\\b", "'\\t", "'\\n", "'\\v", "'\\f", "'\\r", "0x0e", "0x0f"
}; */
bycode( t, i )
register t,i;
{
	/* put byte i+1 in a string */

	i &= 07;
	if( t < 0 )
	{
		 /* end of the string */
		if( i != 0 ) printf( "\n" );
	}
	else 
	{
		 /* stash byte t into string */
		if( i == 0 )
			printf( "\t.byte\t" );
		else
			printf( "," );
/*		if((t=='\\') || (t=='\''))
			printf("'\\%c",t);
		else if((t > ' ') && (t <= '~'))
			printf( "'%c",t);
		else if(t <= 0x0f)
			printf("%s",xtab[t]);
		else
			printf( "0x%x", t );
*/
		printf( "%%%x", t );
		if( i == 07 )
			printf( "\n" );
	}
}

zecode( n )
register n;
{
	/* n integer words of zeros */
	register OFFSZ temp;

	if( n <= 0 ) return;
	printf( "\t.=.+%d\n", (SZINT/SZCHAR)*n );
	temp = n;
	inoff += temp*SZINT;
}

main( argc, argv )
char *argv[]; 
{
	register r;
#ifdef	BLIT
	char errbuf[BUFSIZ];

	setbuf(stderr, errbuf);
#endif

	r = mainp1( argc, argv );
	return( r );
}


/*
**	generate switch/case construct
*/
genswitch(p, n)
register struct sw	*p;
{
	/*	p points to an array of structures, each consisting
	** of a constant value and a label.
	** The first is >=0 if there is a default label;
	** its value is the label number
	** The entries p[1] to p[n] are the nontrivial cases
	*/
	register	i;
	register long	j, range;
	register	dlab, swlab; /* default label, bottom-of-switch label */
	register	svallab;

	range = p[n].sval - p[1].sval;

	/* table is faster & shorter for density > 20% - JGH */
	/*    direct: overhead=2 (for br); increment: 5*n */
	/*    table:  overhead=15; increment: 1*(range+1) */

/*
	if(range+14 <= 5*n)
*/
	dlab = p->slab >= 0 ? p->slab : getlab();
	if(range <= 3 * n)

	{
		 /* implement a direct switch */

		if( p[1].sval)
		{
/*
			printf( "	sub.w	&%ld,%%d0\n", p[1].sval );
*/
		if ((unsigned)p[1].sval <= 16)
			printf( "	dec	r2,#%%%x\n", (unsigned short) p[1].sval );
		else
			printf( "	sub	r2,#%%%x\n", (unsigned short) p[1].sval );
		}

		/* note that this is a logical compare; it thus checks
		** for numbers below range as well as out of range.
		*/
/*
		printf( "	cmp.w	%%d0,&%ld\n", range );
*/
		printf( "	cp	r2,#%%%lx\n", range );

/*
		printf( "	bhi	L%%%d\n", dlab );

		printf( "	add.w	%%d0,%%d0\n" );
/* Assume "swbeg &n" takes 4 bytes:  2 for swbeg, 2 for # of cases */
/*
		printf( "	mov.w	10(%%pc,%%d0.w),%%d0\n" );
		printf( "	jmp	6(%%pc,%%d0.w)\n" );
		printf(	"	swbeg	&%d\n", range+1 );
				/* range runs from zero, so add 1 for cases */
		printf( "	jpr	ugt,L%d\n", dlab );

		printf( "	rl	r2,#2\n" );
		swlab = getlab();
		printf( "	ldl	rr4,L%d(r2)\n", swlab);
		printf( "	jp	@rr4\n" );
		xxx((range + 1) << 2);
		printf(	"\t.psec %s\n\t.data\n\t.even\nL%d:\n", datanm, swlab);
/*
		deflab( swlab = getlab() );
*/


		/* output table */

		for( i=1, j=p[1].sval;  i<=n;  ++j )
		{
/*
			printf( "	short	L%%%d-L%%%d\n",
				(j == p[i].sval) ? p[i++].slab : dlab, swlab );
*/
			printf( "\t.addr\tL%d\n",
				(j == p[i].sval) ? p[i++].slab : dlab);
		}
		printf("\t.psec\n\t.code\n");

		if( p->slab < 0 )
			deflab( dlab );
		return;
	}
	/* out for the moment
	** if( n >= 4 )
	**	werror( "inefficient switch: %d, %d", n, (int) (range/n) );
	*/

	/* simple switch code */

	if(n < 10) {
		for( i=1; i<=n; ++i )
		{
		/* already in 0 */
			if( p[i].sval )
/*
			printf( "	cmp.w	%%d0,&%ld\n", p[i].sval );
		else
			printf( "	tst.w	%%d0\n" );
*/
				printf( "\tcp\tr2,#%%%x\n", (unsigned short) p[i].sval );
			else
				printf( "\ttest\tr2\n" );
/*
		printf( "	beq	L%%%d\n", p[i].slab );
*/
		printf( "\tjpr\teq,L%d\n", p[i].slab );
	}

	if( p->slab >= 0 )
/*
		printf( "	br	L%%%d\n", p->slab );
*/
		printf( "\tjpr\tL%d\n", p->slab );
		return;
	}
	/* hashswitch */
	svallab = getlab();
	swlab = getlab();
	printf("\tldl\trr4,#L%d\n", svallab);	
	printf("\tld\tr3,#%%%x\n", n);
	printf("\tcpir\tr2,@rr4,r3,eq\n");
	printf("\tjpr\tne,L%d\n", dlab);
	printf("\tsll\tr3,#2\n");
	printf("\tneg\tr3\n");
	printf("\tldl\trr4,L%d+%%%x(r3)\n", swlab, (n - 1) << 2);
	printf("\tjp\t@rr4\n");
	xxx((n << 1) + (n << 2));
	printf("\t.psec %s\n\t.data\n\t.even\nL%d:\n", datanm, svallab);
	for(i = 1; i <= n; i++)
		printf("\t.word\t%%%x\n", (unsigned short) p[i].sval);
	deflab(swlab);
	for(i = 1; i <= n; i++)
		printf("\t.addr\tL%d\n", p[i].slab);
	printf("\t.psec\n\t.code\n");
	if(p->slab < 0)
		deflab(dlab);
}

#ifdef TWOPASS
/*
 *	Code added from common/reader.c to make 2 pass version
 *
 */


#ifndef NODBG
e2print( p )
register NODE *p; 
{
	printf( "\n********* costs=(0,...,NRGS;EFF;TEMP;CC)\n" );
	e22print( p ,"T");
	printf("=========\n");
}

e22print( p ,s)
register NODE *p; 
char *s;
{
	static down=0;
	register ty;

	ty = optype( p->tn.op );
	if( ty == BITYPE )
	{
		++down;
		e22print( p->in.right ,"R");
		--down;
	}
	e222print( down, p, s );

	if( ty != LTYPE )
	{
		++down;
		e22print( p->in.left, "L" );
		--down;
	}
}

e222print( down, p, s )
NODE *p; 
char *s;
{
	/* print one node */
	int i, d;

	for( d=down; d>1; d -= 2 ) printf( "\t" );
	if( d ) printf( "    " );

	printf( "%s.%d) op= '%s'",s, (int)(p-node), opst[p->in.op] );
	switch( p->in.op ) 
	{
		 /* special cases */
	case REG:
		printf( " %s", p->tn.rval );  /* Changed form reader. */
		break;

	case ICON:
	case NAME:
	case VAUTO:
	case VPARAM:
	case TEMP:
		printf( " " );
		printf(" %s", p->in.name );
		break;

	case STCALL:
	case UNARY STCALL:
		printf( " args=%ld", p->stn.argsize );
	case STARG:
	case STASG:
		printf( " size=%ld", p->stn.stsize );
		printf( " align=%d", p->stn.stalign );
		break;

	case GENBR:
		printf( " %d (%s)", p->bn.label, opst[p->bn.lop] );
		break;

	case CALL:
	case UNARY CALL:
		printf( " args=%ld", p->stn.argsize );
		break;

	case GENUBR:
	case GENLAB:
		printf( " %d", p->bn.label );
		break;

	case FUNARG:
		printf( " offset=%d", p->tn.rval );

	}

	printf( ", %d\n", p->in.type );
	printf( "\n" );
}

# else
e2print( p )
NODE *p; 
{
	werror( "e2print not compiled" );
}
e222print( p, s )
NODE *p; 
char *s;
{
	werror( "e222print not compiled" );
}
# endif
/*
 *	End of code added from common/reader.c to make 2 pass version
 *
 */
#endif	TWOPASS
