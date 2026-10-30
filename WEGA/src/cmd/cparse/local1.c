# include "mfile1"

extern int eprint();
int cdebug;


/*	this file contains code which is dependent on the target machine */







static inwd	/* current bit offsed in word */;
static word	/* word being built from fields */;

/* f7574: not written yet (0x7574..0x7614 of /lib/cparse) */
f7574()
{
}

fincode( d, sz ) double d; {
	/* output code to initialize space of size sz to the value d */
	/* the proper alignment has been obtained */
	/* inoff is updated to have the proper final value */
	/* on the target machine, write it out in octal! */

	register int *mi;
	float f;

	if( sz==SZDOUBLE ) {
		mi = (int *)&d;
		printf( "	.word	0x%x, 0x%x, 0x%x, 0x%x\n",
			mi[0], mi[1], mi[2], mi[3] );
	} else {
		f = d;
		mi = (int *)&f;
		printf( "	.word	0x%x, 0x%x\n", mi[0], mi[1] );
	}
	inoff += sz;
}

/* f7684: not written yet (0x7684..0x777c of /lib/cparse) */
f7684()
{
}

/* f777c: not written yet (0x777c..0x78cc of /lib/cparse) */
f777c()
{
}

vfdzero( n ){ /* define n bits of zeros in a vfd */

	if( n <= 0 ) return;

	inwd += n;
	inoff += n;
	if( inoff%ALINT ==0 ) {
		printf( "	.word	0%o\n", word );
		word = inwd = 0;
		}
	}

/* f7952: not written yet (0x7952..0x79d8 of /lib/cparse) */
f7952()
{
}

ctype( type ) TWORD type; { /* map types which are not defined on the local machine */
	switch( BTYPE(type) ){
	case SHORT:
		MODTYPE(type,INT);
		break;
	case USHORT:
		MODTYPE(type,UNSIGNED);
		}
	return( type );
	}

/* f7a1a: not written yet (0x7a1a..0x7a1e of /lib/cparse) */
f7a1a()
{
}

/* f7a1e: not written yet (0x7a1e..0x7a80 of /lib/cparse) */
f7a1e()
{
}

isitfloat( s ) char *s; {
	double atof();
	dcon = atof(s);
	return( FCON );
	}

/* f7a9e: not written yet (0x7a9e..0x7b28 of /lib/cparse) */
f7a9e()
{
}

/* f7b28: not written yet (0x7b28..0x7b4c of /lib/cparse) */
f7b28()
{
}

