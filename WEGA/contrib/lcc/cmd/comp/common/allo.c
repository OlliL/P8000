# include "mfile2.h"
#ifndef NHALF
#define NHALF 0x800
#endif
#ifndef RKLET
#define RKLET 0x1000
#endif
#ifndef RXLET
#define RXLET 0x2000
#endif

NODE resc[NRGS];

int busy[NRGS];

# define TBUSY 0100

allo0()
{
	/* free everything */
	register i;

	for( i=0; i<NRGS; ++i )
	{
		busy[i] = 0;
	}
}
allo( p, q )
register NODE *p; 
register struct optab *q; 
{
	register n, i, j;
	register NODE *presc;
	int k, rw;

	n = q->needs;
	rw = q->rewrite;
	if( rdebug > 1 ) printf( "allo( %d, %d ), needs = 0x%x\n", p-node, q->stinline, q->needs );

	i = 0;
	while( n & ( QCOUNT | PCOUNT | NCOUNT ) )
	{
		if ( n & QCOUNT )
		{
			j = freequad ( p, n ,rw );
			if(rw & RXLET)
				rw &= ~RXLET;
			busy[j] |= TBUSY;
			busy[j+1] |= TBUSY;
			busy[j+2] |= TBUSY;
			busy[j+3] |= TBUSY;
			n -= 0x100;
			k = 4;
		}
		else if( n & PCOUNT ) 
		{
			j = freepair( p, n , rw);
			if(rw & RXLET)
				rw &= ~RXLET;
			busy[j] |= TBUSY;
			busy[j+1] |= TBUSY;
			n -= 0x10;
			k = 2;
		}
		else if ( n & NCOUNT )
		{
			j = freereg( p, n );
			busy[j] |= TBUSY;
			n -= 1;
			k = 1;

		}
		presc = &resc[i];
		presc->in.op = REG;
		presc->tn.rval = j;
	/*	presc->tn.type = ( k == 4 ) ? TDOUBLE : ( k == 2 ) ? TLONG : TSHORT;  */
		presc->tn.type =  p->in.type;
		presc->tn.lval = 0;
		presc->bn.label =  k;
		++i;
	}

	/* turn off "temporarily busy" bit */
	for( j=0; j<NRGS; ++j )
	{
		busy[j] &= ~TBUSY;
	}
	if( rdebug > 1 )
	{

		for( j=0; j<i; ++j )
		{
			if( resc[j].tn.op == REG )
				printf( ", RESC%d = REG(%d+%d) type = 0x%x", j+1 ,resc[j].tn.rval, resc[j].bn.label , resc[j].in.type);
			else
				printf( ", RESC%d = TEMP(%ld)  ", j+1 ,resc[j].tn.lval );
		}
		putchar( '\n' );
	}
}

OFFSZ tmpoff;  /* offset of next temp to be allocated */

int rdebug = 0;

OFFSZ
freetemp( k )
register OFFSZ k;
{
	/* allocate k integers worth of temp space
	** we also make the convention that, if the number of words is more than 1,
	** it must be aligned for storing doubles... 
	*/

# ifndef BACKTEMP
	OFFSZ t;

	if( k>1 )
	{
		SETOFF( tmpoff, ALDOUBLE );
	}

	t = tmpoff;
	tmpoff += k*SZINT;
	if( tmpoff > maxtemp ) maxtemp = tmpoff;
	return(t);

# else
	tmpoff += k*SZINT;
	if( k>1 ) 
	{
		SETOFF( tmpoff, ALDOUBLE );
	}
	if( tmpoff > maxtemp ) maxtemp = tmpoff;
	return( -tmpoff );
# endif
}

# ifdef STACK
/* for stack machines, totally disable the register allocation */
freereg( p, n )
NODE *p; 
{
	return( 0 );
}

freepair( p, n )
NODE *p; 
{
	cerror( "pairs on a stack machine?" );
}

rbusy( r, t )
TWORD t; 
{
}
rfree( r, t )
TWORD t; 
{
}

regrcl( p )
NODE *p; 
{
}
# else
freepair( p, n, rw )
register NODE *p; 
register n ;
int rw;
{
	/* allocate a register pair */
	/* p gives the type */
	register j;
	register t = optype( p->tn.op );

	if( callop(p->in.op) )
	{
		j = callreg(p);
		if( j&1 ) cerror( "callreg returns bad pair" );
		if( usable( p, n, j ) && usable( p, n, j+1) ) return( j );
		/* have allocated callreg first */
	}
	if ( rw & RXLET )
	{
		if ( usable ( p, n, 2) && usable (p, n, 3) ) return ( 2 );
		cerror ( "RXLET bad pair allocation");
	}
	if( n )
	{

		if( (n&LPREF) && (j = shared( getlt( p, t ) ) ) >= 0 ) {
			j = ( NHALF & rw ) ? j & 0xe : j;
			if ( j & 1 ) cerror("illegal pair LPREF");
		   	if( usable( p, n, j ) && usable( p, n, j+1) ) return( j );
		}
		if( (n&RPREF) && (j = shared( getrt( p, t ) ) ) >= 0 &&
		   usable( p, n, j ) && usable( p, n, j+1) ) return( j );
		for( j=2; j<NRGS; j+=2 ) {
			if( usable(p,n,j) && usable(p,n,j+1) )
			return( j );
		}
	}
	cerror( "pair register allocation fails, try simplify expression" );
	/* NOTREACHED */
}
freequad( p, n ,rw)
register NODE *p; 
register n;
int rw;
{
	/* allocate a register pair */
	/* p gives the type */
	register j;
	register t = optype ( p->tn.op ); 

	if( callop(p->in.op) )
	{
		j = callreg(p);
		if( j ) cerror( "callreg returns bad pair" );
		if( usable( p, n, j ) && usable( p, n, j+1) &&
		    usable(p,n, j+2 ) && usable( p, n, j+3) ) return( j );
		/* have allocated callreg first */
	}
	if ( rw & RXLET )
	{
		if( usable(p,n,0) && usable(p,n,1) &&
			usable(p,n,2) && usable (p,n,3) ) return ( 0 );
		cerror ("RXLET bad quad allocation");
	}
	if( n )
	{
		if( (n&LPREF) && (j = shared( getlt( p, t ) ) ) >= 0) {
			j = ( rw & NHALF ) ? j & 0xc : j;
			if ( j  & 0x3 ) cerror ( "illegal quad LPREF");
			if(  usable( p, n, j ) && usable ( p, n, j+1 ) &&
		   	     usable(p,n,j+2) && usable(p,n,j+3)    ) return( j );
			}
		if( (n&RPREF) && (j = shared( getrt( p, t ) ) ) >= 0 &&
		   usable( p, n, j ) && usable ( p, n, j+1 ) &&
		   usable(p,n,j+2) && usable(p,n,j+3)    ) return( j );
		for( j=0; j<NRGS; j+=4 ) 
			if( usable(p,n,j) && usable(p,n,j+1)  &&
			    usable(p,n,j+2) && usable(p,n,j+3))
			return( j );
	}
	cerror( "quad register allocation fails, try simplify exprssion" );
	/* NOTREACHED */
}

freereg( p, n )
register NODE *p; 
register n;
{
	/* allocate a register */
	/* p gives the type */

	register j;
	register int t = optype( p->tn.op );

	if( callop(p->in.op) )
	{
		j = callreg(p);
		if( usable( p, n, j ) ) return( j );
		/* have allocated callreg first */
	}
	if( n )
	{
		if( (n&LPREF) && (j = shared( getlt( p, t ) ) ) >= 0 &&
		   usable( p, n, j ) ) return( j );
		if( (n&RPREF) && (j = shared( getrt( p, t ) ) ) >= 0 &&
		   usable( p, n, j ) ) return( j );
		for( j=2; j<NRGS ; ++j ) if( usable(p,n,j) ) return( j );
	}
	cerror( "short register allocation fails, try simplify expression" );
	/* NOTREACHED */
}

shared( p )
register NODE *p; 
{
	/* simple, at present */
	/* try to find a register to share */
	register r, o;
	if( rdebug ) 
	{
		printf( "shared called on:\n" );
		e2print( p );
	}
	if( (o=p->tn.op) == REG ) 
	{
		r = p->tn.rval;
		if( r >= NRGS ) return( -1 );
		if( rdebug ) 
		{
			printf( "preference for R%d\n", r );
		}
		return( r );
	}
	/* we look for shared regs under unary-like ops */
	switch( optype( o ) ) 
	{

	case BITYPE:
		/* look for simple cases */
		/* look only on the left */
	case UTYPE:
		return( shared( p->in.left ) );
	}
	return( -1 );
}

usable( p, n, r )
register NODE *p; 
register n,r;
{
	/* decide if register r is usable in tree p to satisfy need n */

	if( r>= NRGS || (busy[r] & TBUSY) ) return( 0 );
	if( busy[r] > 1 ) 
	{
		/*
		** uerror( "register %d too busy", r );
		*/
		return( 0 );
	}
	if( busy[r] == 0 ) return(1);

	/* busy[r] is 1: is there chance for sharing */
	return( shareit( p, r, n ) );

}

shareit( p, r, n )
register NODE *p; 
register r,n;
{
	/* can we make register r available by sharing from p
	** given that the need is n 
	*/
	register int t = optype(p->tn.op);
	if( (n&LSHARE) && ushare( getlt( p, t ), r ) ) return(1);
	if( (n&RSHARE) && ushare( getrt( p, t ), r ) ) return(1);
	return(0);
}

ushare( p, r )
register NODE *p; 
register r;
{
	/* can we find register r to share in p */
	if( p->in.op == REG ) 
	{
/****
		if( typlen ( p->tn.type ) == 4 && r==(p->tn.rval+1) ) return( 1 );
		if( typlen ( p->tn.type ) == 8 && 
			 ( r > (p->tn.rval) && r <= (p->tn.rval+3) )
								    ) return( 1 );

		return( r == p->tn.rval );
********/
		if ( r >= p->tn.rval && r < p->tn.rval + p->bn.label )return(1);
	}
	switch( optype( p->tn.op ) )
	{
	case BITYPE:
		if( ushare( p->in.right, r ) ) return( 1 );
	case UTYPE:
		if( ushare( p->in.left, r ) ) return( 1 );
	}
	return(0);
}

regrcl( p )
register NODE *p; 
{
	/* free registers in the tree (or fragment) p */
	register r;
	if( !p ) return;
	r = p->tn.rval;
	if( p->in.op == REG ) rfree( r, p->bn.label );
	switch( optype( p->tn.op ) )
	{
	case BITYPE:
		regrcl( p->in.right );
		/* explict assignment to regs not accounted for */
		if( asgop(p->tn.op) && p->in.left->tn.op == REG ) break;
	case UTYPE:
		regrcl( p->in.left );
	}
}

rfree( r, n )
register int n; 
register r;
{
	register i;
	/* mark register r free, if it is legal to do so */
	/* t is the type */
	r &= 0xf;

#ifndef NODBG
	if( rdebug )
	{
		printf( "rfree( R%d , lenght=%d)\n", r, n);
	}
#endif
	if( istreg(r) )
	{
		for ( i = n ; i ; --i ){
			if( !istreg(r+i-1) ) cerror ( "register to big" ) ;
			if( --busy[r+i-1] < 0 ) cerror ( "register overfreed %d" ,r+i-1);
		}
	}
}

rbusy(r, n )
register r; 
register  n; 
{

register i;
	/* mark register r busy */
	r &= 0xf;

#ifndef NODBG
	if( rdebug )
	{
		printf( "rbusy( R%d, length=%d ) \n", r, n);
	}
#endif
	if( istreg(r) ) 
	{
		for ( i = n ; i ; --i ){
			if( !istreg(r+i-1) ) cerror ( "register to big" ) ;
			++busy[r+i-1] ;
		}
	}
}

# endif

rwprint( rw )
register rw;
{
	/* print rewriting rule */
	register i, flag;
	static char * rwnames[] = 
	{
		"RLEFT",
		"RRIGHT",
		"RESC1",
		"RESC2",
		"RESC3",
		"RESC4",
		"RESCC",
		"RNOP",
		"RNULL",
		"RHIGHT",
		"RLOW",
		0,
	};
	if( rw == RNULL )
	{
		printf( "RNULL" );
		return;
	}
	flag = 0;
	for( i=0; rwnames[i]; ++i )
	{
		if( rw & (1<<i) )
		{
			if( flag ) printf( "|" );
			++flag;
			printf( rwnames[i] );
		}
	}
	if( !flag ) printf( "?0x%lx", rw );
}

reclaim( p, rw, goal )
register NODE *p; 
register rw, goal;
{
	register NODE *q;
	register o;

	/* get back stuff */
	if( rdebug )
	{
		printf( "reclaim( %d, ", p-node );
		rwprint( rw );
		printf( ", " );
		prgoal( goal );
		printf( " )\n" );
	}

	if( !p ) return;
	if( rdebug > 1)
		printf("reclaim inputtype = 0x%x\n",p->tn.type);
	/* special cases... */
	if( (o=p->tn.op) == COMOP )
	{
		/* LHS has already been freed; don't free again */
		regrcl( p->in.right );
	}
	else regrcl( p );
	if( (o==FREE && rw==RNULL) || rw==RNOP ) return;
	if( callop(o) )
	{
		/* check that all scratch regs are free */
		callchk(p);  /* ordinarily, this is the same as allchk() */
	}
	if( rw == RNULL || (goal&FOREFF) )
	{
		/* totally clobber, leave nothing */
		tfree(p);
		return;
	}
	/* handle condition codes specially */
	if( (goal & FORCC) && (rw&RESCC)) 
	{
		/* result is CC register */
		tfree(p);
		p->in.op = CCODES;
		p->tn.lval = 0;
		p->tn.rval = 0;
		return;
	}
	q = 0;


	if ( rw&RRIGHT ) {
			q = getr( p );
			q->tn.type = p->tn.type;
			}
	else {
		int k;
		if( rw & RLEFT) { q = getl (p); q->tn.type = p->tn.type; k=1 ;}
		else if( rw&RESC1 ) q = &resc[0];
		else if( rw&RESC2 ) q = &resc[1];
		else if( rw&RESC3 ) q = &resc[2];
		else if( rw&RESC4 ) q = &resc[3];
		else cerror( "illegal reclaim, op %s", opst[p->tn.op]);
		if ( q->tn.op == REG ) {
			if( q->tn.rval < 8 ) k = q->bn.label / 2 ;
			if ( k == 0 && (rw & (RHIGHT | RLOW))) 
				cerror( "illegal half register");
			if ( k ) {
				if (rw&RHIGHT ) {
						q->bn.label = k;
				}
				if (rw&RLOW ) {
						q->tn.rval += k;
						if ( q->tn.rval < 8 )
							q->bn.label = k;
						if ( RKLET & rw  && k == 2 ) 
							q->bn.label = 1;
				}
			}
			/* Halbieren der Reclaims */
	
			if ( q->in.type &  TCHAR ) 
				q->in.type  = (~TCHAR & q->in.type) | TSHORT ;
			if ( q->in.type  == TUCHAR )
				q->in.type  = (~TUCHAR & q->in.type) | TUSHORT;
			if (( rw&RLOW) && (rw&RLEFT) )
				q->in.type  = TINT;
		}
		if ( rdebug > 1 ) {
			printf ( "reclaim type = 0x%x\n",q->in.type);
		}
	}
	if( o == STARG ) p = p->in.left;  /* STARGs are still STARGS */
	q = tcopy(q);
	tfree(p);
	*p = *q;  /* make the result replace the original */
	q->in.op = FREE;
}

NODE *
tcopy( p )
register NODE *p; 
{
	/* make a fresh copy of p */
	register NODE *q;
	register r;

	q=talloc();
	*q = *p;
	r = p->tn.rval;
	if( p->in.op == REG ) rbusy( r, p->bn.label );
	switch( optype(q->in.op) )
	{
	case BITYPE:
		q->in.right = tcopy(p->in.right);
	case UTYPE:
		q->in.left = tcopy(p->in.left);
	}
	return(q);
}

allchk()
{
	/* check to ensure that all register are free */
	register i;

	for( i=0; i<NRGS; ++i )
	{
		if( busy[i] )
		{
			cerror( "register allocation error");
		}
	}
}

/* this may not be the best place for this routine... */
OFFSZ
argsize( p )
register NODE *p; 
{
	/* size of the arguments */
	register OFFSZ t;
	t = 0;
	if( p->tn.op == CM )
	{
		t = argsize( p->in.left );
		p = p->in.right;
	}
	if( p->tn.type & (TDOUBLE|TFLOAT) )
	{
		SETOFF( t, ALDOUBLE );
		t += SZDOUBLE;
	}
	else if( p->tn.type & (TLONG|TULONG) )
	{
		SETOFF( t, ALLONG );
		t += SZLONG;
	}
	else if( p->tn.type & TPOINT )
	{
		SETOFF( t, ALPOINT );
		t += SZPOINT;
	}
	else if( p->tn.type & TSTRUCT )
	{
		SETOFF( t, p->stn.stalign );  /* alignment */
		t += p->stn.stsize;  /* size */
	}
	else 
	{
		SETOFF( t, ALINT );
		t += SZINT;
	}
	return( t );
}
