# include "mfile1"
# include <signal.h>

extern int cdebug;
int eprint();
int csavlab, enterlab;
int cretlab;
int lineswitch;
int proflag;
int strftn = 0;	/* is the current function one which returns a value */
FILE *tmpfile;
FILE *outfile = stdout;


int lastloc = PROG;




int crslab = 10;












/*
bycode( t, i ){
	i &= 0xf;
	if( t < 0 ) {
		if( i != 0 ) fprintf( outfile, "\n" );
	} else {
		if( i == 0 ) fprintf( outfile, "	.byte	" );
		else fprintf( outfile, "," );
		fprintf( outfile, "%d", t );
		if( i == 0xf ) fprintf( outfile, "\n" );
	}
}





char *tmpname = "/tmp/pcXXXXXX";

main( argc, argv ) char *argv[]; {
	int dexit();
	register int c;
	register int i;
	int r;

	for( i=1; i<argc; ++i )
		if( argv[i][0] == '-' && argv[i][1] == 'X' && argv[i][2] == 'p' ) {
			proflag = 1;
			}

	mktemp(tmpname);
	if(signal( SIGHUP, SIG_IGN) != SIG_IGN) signal(SIGHUP, dexit);
	if(signal( SIGINT, SIG_IGN) != SIG_IGN) signal(SIGINT, dexit);
	if(signal( SIGTERM, SIG_IGN) != SIG_IGN) signal(SIGTERM, dexit);
	tmpfile = fopen( tmpname, "w" );

	r = mainp1( argc, argv );

	tmpfile = freopen( tmpname, "r", tmpfile );
	if( tmpfile != NULL )
		while((c=getc(tmpfile)) != EOF )
			putchar(c);
	else cerror( "Lost temp file" );
	unlink(tmpname);
	return( r );
	}

/* f61d0: not written yet (0x61d0..0x6348 of /lib/cparse) */
f61d0()
{
}

/* f6348: not written yet (0x6348..0x63b2 of /lib/cparse) */
f6348()
{
}

/* f63b2: not written yet (0x63b2..0x6484 of /lib/cparse) */
f63b2()
{
}

/* f6484: not written yet (0x6484..0x64c2 of /lib/cparse) */
f6484()
{
}

/* f64c2: not written yet (0x64c2..0x65c4 of /lib/cparse) */
f64c2()
{
}

/* f65c4: not written yet (0x65c4..0x65e8 of /lib/cparse) */
f65c4()
{
}

/* f65e8: not written yet (0x65e8..0x6620 of /lib/cparse) */
f65e8()
{
}

/* f6620: not written yet (0x6620..0x66c2 of /lib/cparse) */
f6620()
{
}

/* f66c2: not written yet (0x66c2..0x6726 of /lib/cparse) */
f66c2()
{
}

/* f6726: not written yet (0x6726..0x6786 of /lib/cparse) */
f6726()
{
}

/* f6786: not written yet (0x6786..0x6790 of /lib/cparse) */
f6786()
{
}

/* f6790: not written yet (0x6790..0x67ec of /lib/cparse) */
f6790()
{
}

/* f67ec: not written yet (0x67ec..0x680a of /lib/cparse) */
f67ec()
{
}

efcode(){
	/* code for the end of a function */

	if( strftn ){  /* copy output (in r0) to caller */
		register struct symtab *p;
		register int stlab;
		register int count;
		int size;

		p = &stab[curftn];

		deflab( retlab );

		stlab = getlab();
		printf( "	ld	r6,$L%d\n", stlab );
		size = tsize( DECREF(p->stype), p->dimoff, p->sizoff ) / SZCHAR;
		count = size/2;
		printf( "	ld	r5,$%d\n", count );
		printf( "	ldir	*r6,*r7,r5\n" );
		printf( "	ld	r7,$L%d\n", stlab );
		printf( "	.bss\nL%d:	.=.+%d\n	.text\n", stlab, size );
		/* turn off strftn flag, so return sequence will be generated */
		strftn = 0;
		}
/* CCF mods for two pass operation */
#ifdef ONEPASS
	p2bend();
#endif
#ifndef ONEPASS
printf("]...\n");
#endif
	}

bfcode( a, n ) OFFSZ a[]; {
	/* code for the beginning of a function; a is an array of
		indices in stab for the arguments; n is the number */
	register i;
	register temp;
	register struct symtab *p;
	OFFSZ off;

	locctr( PROG );
	p = &stab[curftn];
	defnam( p );
	printf( "~~%.8s:\n", p->sname );
	temp = p->stype;
	temp = DECREF(temp);
	strftn = (temp==STRTY) || (temp==UNIONTY);

	for(i = 0;i<paramno;i++){
		p = &stab[paramstk[i]];
		printf( "\t~%.8s = %ld\n", p->sname, p->offset/SZCHAR );
	}

	retlab = getlab();
	/* routine prolog */

	/* adjust stack for autos */
	/* do adjust at routine exit, depend on optimizer */
	enterlab = getlab();
	csavlab = getlab();
	cretlab = getlab();
	branch(csavlab);
	deflab(enterlab);

	off = ARGINIT;

	for( i=0; i<n; ++i ){
		p = &stab[a[i]];
		if( p->sclass == REGISTER ){
			temp = p->offset;  /* save register number */
			p->sclass = PARAM;  /* forget that it is a register */
			p->offset = NOOFFSET;
			oalloc( p, &off );
			printf( "	ld	r%d,%ld(r14)\n", temp, p->offset/SZCHAR );
			p->offset = temp;  /* remember register number */
			p->sclass = REGISTER;   /* remember that it is a register */
			}
		else {
			if( oalloc( p, &off ) ) cerror( "bad argument" );
			}

		}
	}

bccode(){ /* called just before the first executable statment */
		/* by now, the automatics and register variables are allocated */
	SETOFF( autooff, SZINT );
	/* set aside store area offset */
/* CCF mods for two pass operation */
#ifdef ONEPASS
	p2bbeg( autooff, regvar );
#endif
#ifndef ONEPASS
printf("[%d\t%ld\t%d\t\n",ftnno,autooff,regvar);
#endif
	}

/* f6ab6: not written yet (0x6ab6..0x6ac4 of /lib/cparse) */
f6ab6()
{
}

defnam( p ) register struct symtab *p; {
	/* define the current location as the name p->sname */

	if( p->sclass == EXTDEF )
		printf( "	.globl	%.8s\n",  exname(p->sname) );
	else if( p->slevel <= 1 )
		printf( "	.globl\n"); /* for the optimizer */
	if( p->sclass == STATIC && p->slevel>1 ) deflab( (int) p->offset );
	else printf( "%s:\n", exname( p->sname ) );

	}

/* f6b2c: not written yet (0x6b2c..0x6b86 of /lib/cparse) */
f6b2c()
{
}

fldal( t ) unsigned t; { /* return the alignment of field of type t */
	uerror( "illegal field type" );
	return( ALINT );
	}

/* f6b9c: not written yet (0x6b9c..0x6bf0 of /lib/cparse) */
f6b9c()
{
}

genswitch(p,n) register struct sw *p;{
	/*	p points to an array of structures, each consisting
		of a constant value and a label.
		The first is >=0 if there is a default label;
		its value is the label number
		The entries p[1] to p[n] are the nontrivial cases
		*/
	register i;
	register CONSZ j, range;
	register dlab, swlab;

	range = p[n].sval-p[1].sval;
	dlab = p->slab >= 0 ? p->slab : getlab();

	if( range>0 && range <= 3*n && n>=4 ){ /* implement a direct switch */

		printf( "	ld	r6,$L%d\n", swlab = getlab() );
		if (lineswitch) {
printf( "	ld	r5, 4(r6)" );	/* r5 = default code address */
printf( "	cp	r7, 2(r6)" );		
printf( "	jp	gt, *r5" );	/* out of case range on high end */
printf( "	sub	r7, *r6" );	
printf( "	jp	lt, *r5" );	/* out of case range on low end */
printf( "	add	r6, r7" );	
printf( "	add	r6, r7" );	/* r6 = index into case code addresses */
printf( "	ld	r5, 6(r6)" );	/* r5 = case code address */
printf( "	jp	*r5" );		/* jump to case */
		} else
		printf( "	jpr	switch1\n" );

		/* output table */

		locctr( ADATA );
		defalign( ALPOINT );
		deflab( swlab );

		printf( "	.word	%ld\n", p[1].sval );
		printf( "	.word	%ld\n", p[n].sval );
		if( p->slab < 0 )
			printf( "	.word	L%d\n", dlab );
		else
			printf( "	.word	L%d\n", p[0].slab );

		for( i=1,j=p[1].sval; i<=n; ++j ){
			printf( "	.word	L%d\n", ( j == p[i].sval ) ?
				p[i++].slab : dlab );
			}

		locctr( PROG );

		if( p->slab< 0 ) deflab( dlab );
		return;

		}

	/* debugging code */

	if( n >= 4 ) {
		printf( "	ld	r6,$L%d\n", swlab=getlab() );
		if (lineswitch) {
printf( "	ld	r5, *r6" );	/* r5 = number of cases + 1 */
printf( "	ld	r4, r5" );	/* r4 = number of cases + 1 */
printf( "	inc	r6, $2" );	/* r6 = ptr to case value list */
printf( "	cpir	r7, *r6, r5, eq" );	/* look for correct case value */
printf( "	add	r6, r4" );		
printf( "	add	r6, r4" );	/* r6 = address of correct code address */
printf( "	ld	r5, -4(r6)" );	/* r5 = correct case code address */
printf( "	jp	*r5" );		/* jump to case */
		} else
		printf( "	jpr	switch2\n" );
		locctr( ADATA );
		defalign( ALPOINT );
		deflab( swlab );
		printf( "	.word	%d\n", n+1);
		for (i=1; i<=n; i++)
			printf( "	.word	%ld\n", j = p[i].sval );
		for (i=1; i<=n; i++)
			printf( "	.word	L%d\n", p[i].slab );
		printf( "	.word	L%d\n", dlab );
		locctr( PROG );
		if( p->slab < 0 ) deflab( dlab );
		return;
		}

	/* simple switch code */

	for( i=1; i<=n; ++i ){
		/* already in r7 */

		printf( "	cp	r7,$" );
		printf( CONFMT, p[i].sval );
		printf( "\n	jpr	eq,L%d\n", p[i].slab );
		}

	if( p->slab>=0 ) branch( p->slab );
	}

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

	/* the major essential job is rewriting the
	   automatic variables and arguments in terms of
	   REG and OREG nodes */
	/* conversion ops which are not necessary are also clobbered here */
	/* in addition, any special features (such as rewriting
	   exclusive or) are easily handled here as well */

	register struct symtab *q;
	register NODE *r;
	register o;
	register m, ml;

	switch( o = p->op ){

	case NAME:
		if( p->rval < 0 ) { /* already processed; ignore... */
			return(p);
		}
		q = &stab[p->rval];
		switch( q->sclass ){

		case AUTO:
		case PARAM:
			/* fake up a structure reference */
			r = block( REG, NIL, NIL, PTR+STRTY, 0, 0 );
			r->lval = 0;
			r->rval = (q->sclass==AUTO?STKREG:ARGREG);
			p = stref( block( STREF, r, p, 0, 0, 0 ) );
			break;

		case ULABEL:
		case LABEL:
		case STATIC:
			if( q->slevel == 0 ) break;
			p->lval = 0;
			p->rval = -q->offset;
			break;

		case REGISTER:
			p->op = REG;
			p->lval = 0;
			p->rval = q->offset;
			break;

		}
		break;
	case LT:
	case LE:
	case GT:
	case GE:
		if( ISPTR( p->left->type ) || ISPTR( p->right->type ) ){
			p->op += (ULT-LT);
		}
		break;

	case PCONV:
		/* do pointer conversions for char and longs */
		ml = p->left->type;
		if( ml == FLOAT )
			goto pclobber;
		if( ( ml==CHAR || ml==UCHAR || ml==LONG || ml==ULONG )
		   && p->left->op != ICON ) break;

pclobber:
		/* pointers all have the same representation;
		   the type is inherited */
		p->left->type = p->type;
		p->left->cdim = p->cdim;
		p->left->csiz = p->csiz;
		p->op = FREE;
		return( p->left );

	case SCONV:
		if(cdebug)fwalk(p,eprint,0);
/*
 *		m = (p->type == FLOAT || p->type == DOUBLE );
 *		ml = (p->left->type == FLOAT || p->left->type == DOUBLE );
 *		if( m != ml ) break;
 */

		/* now, look for conversions downwards */

		m = p->type;
		ml = p->left->type;
		if( p->left->op == ICON ){ /* simulate the conversion here */
			CONSZ val;
			val = p->left->lval;
			switch( m ){
			case CHAR:
				p->left->lval = (char) val;
				goto clobber;
			case UCHAR:
				p->left->lval = val & 0XFF;
				goto clobber;
			case UNSIGNED:
				p->left->lval = val & 0XFFFFL;
				break;
			case INT:
				p->left->lval = (int)val;
				break;
			}
		} else if ( p->left->op != FCON )  {
			/* meaningful ones are conversion of int to char,
			   int to short,
			   and short to char, and unsigned version of them */
			if( m==CHAR || m==UCHAR ){
				if( ml != CHAR && ml != UCHAR ) break;
			}
			else if( m==INT || m==UNSIGNED ){
				if( ml != INT && ml != UNSIGNED ) break;
			}
			else if( m==LONG || m==ULONG ) {
				if( ml != LONG && ml != ULONG ) break;
			}
			else if( m==DOUBLE ) {
				if( ml != DOUBLE ) break;
			}
			else if( m==FLOAT ) {
				if( ml != FLOAT ) break;
			}
		}

		/* clobber conversion */
		p->left->type = m; /* inherit type */
	clobber:
		p->op = FREE;
		return( p->left );  /* conversion gets clobbered */

	case PVCONV:
	case PMCONV:
		if( p->right->op != ICON ) cerror( "bad conversion", 0);
		p->op = FREE;
		return( buildtree( o==PMCONV?MUL:DIV, p->left, p->right ) );

	case PLUS:
	case MINUS:
	case LS:
	case MUL:
		/* optimize address calculations with long indexes */
		if( ISPTR( p->type ) || ISARY( p->type ) ) {
			if( p->left->type==LONG || p->left->type==ULONG )
				p->left = cast( p->left, INT );
			if( p->right->type==LONG || p->right->type==ULONG )
				p->right = cast( p->right, INT );
		}
		break;

	}

	return(p);
}

cisreg( t ) TWORD t; { /* is an automatic variable of type t OK for a register variable */

	if( t==INT || t==UNSIGNED || ISPTR(t) ) return(1);
	return(0);
	}

NODE *
offcon( off, t, d, s ) OFFSZ off; TWORD t; {

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

