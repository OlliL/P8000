# include "mfile2.h"
#ifndef NONAME
#define NONAME 040000
#endif

int rsmask;

/* the stack frame looks as follows:
** 
** incoming args
** saved PC (4 bytes)
** saved old FP (rr12) (4 bytes)
** saved regs (rr10, rr8)
** autos (maxboff keeps track of this)
** temps (maxtemp keeps track of this)
** 2 byte field for offset to saved FP
** SP,FP->
** the first temp begins at FP + 2
** the last temp ends at FP + ~L1
** the first auto begins at FP + ~L1
** the last auto ends at FP + ~L2 - 12
** rr8 is saved at FP + ~L2 - 12
** rr10 is saved at FP + ~L2 - 8
** rr12 (the old FP) is saved at FP + ~L2 - 4
** the first arg begins at FP + ~L2 + 4
*/

static char rnm[5];
#define HIGHREG 0x40
#define NFLG 0x7


/* retun number of bytes for type t */
typlen(t)
{
	if(t & (TLONG |TULONG| TPOINT | TFLOAT))
		return(4);
	else if (t & (TUCHAR | TCHAR))
		return(1);
	else if ( t & TDOUBLE)
		return(8);
	else
		return(2);
}

char *
prnames(typ, val, lab)
register typ, val;
register lab;
{
	char *s;
	register int errorf;
if(rdebug > 2)
	printf("typ = 0x%x val = %d lab = %d\n",typ,val,lab);
		errorf = 0;
		s = rnm;
		*s = '?';
		*(s+1) = '\0';
		switch(typlen(typ))
		{
		case 1:
			if((val & 0xf) > 7)
				errorf = 1;
			if(HIGHREG & val)
				sprintf(s, "rh%d",(val & 0xf));
			else sprintf(s, "rl%d",val);
			break;
		case 2:
			if(lab==2)
				goto lll;
			if(lab==4)
				goto ddd;
sss:
			if(val > 11)
				errorf = 1;
			sprintf(s, "r%d",val);
			break;
		case 4:
			if(lab==4)
				goto ddd;
			if(lab==1)
				goto sss;
lll:
			if(val & 1 || val > 11)
				errorf = 1;
			sprintf(s, "rr%d",val);
			break;
		case 8:
			if(lab==2)
				goto lll;
			if(lab==1)
				goto sss;
ddd:
			if(val & 3 || val > 8)
				errorf = 1;
			sprintf(s, "rq%d",val);
			break;
		default:
			werror("bad register type (length = %d)",typlen(typ));
		}
		if(errorf)
			werror("bad register value %d, length = %d",val,
								typlen(typ));
		return s;
}


lineid( l, fn )
char *fn;
{
	/* identify line l and file fn */
	printf( "%s  line %d, file %s\n", COMMENTSTR, l, fn );
}

eobl2()
{
	register off;
	register rsave;
	register i, m;
	/* count bits in rsmask */
	for( rsave=i=0, m=1; i<16; ++i, m<<=1 )
	{
		if( m&rsmask ) rsave += 4;
	}
	maxboff /= SZCHAR;
	maxtemp /= SZCHAR;
	maxarg /= SZCHAR;

	off = maxtemp + 2;
		/* space for temps + 2 bytes for offset to old frame pointer */
	printf("\t~L1 := %d\n", off);
	off += maxboff;		/* space for automatics */
	off += 12;		/* 12 bytes for saved registers */
	printf("\t~L2 := %d\n\t}\n", off);
	rsmask = 0;
	eobl2dbg();			/* very end debug code */
}

special( sh, p )
NODE *p;
register sh;
{
	register int val;

	sh &= SVMASK;		/* clear all but the special code */
	switch (sh)
	   {
	   case 1:		/* power of 2 */
	   case 2:		/* sum of 2 powers of 2 */
	   case 3:		/* difference of 2 powers of 2 */

		if (p->tn.op != ICON || p->tn.name != (char *) 0)
		   return(0);
		val = p->tn.lval;
		if (sh == 1)	/* power of 2 */
		   return (val > 0 && (val & (val-1)) == 0);
		else if (sh == 2 && val > 0)	/* sum of powers of 2 */
		   {
		   /* (word) multiply faster than shift by >12 on 68010 */
/* shift is always faster
		   if (!(p->tn.type & (TLONG | TULONG)) && val > 4096)
			return(0);
*/
		   return ((val &= (val-1)) != 0 && (val & (val-1)) == 0);
		   }
		else if (sh == 3 && val > 0)	/* diff of powers of 2 */
		   {
		   /* (word) multiply faster than shift by >12 on 68010 */
/*
		   if (!(p->tn.type & (TLONG | TULONG)) && val > 4096)
			return(0);
*/
		   while ((val & 1) == 0)	/* get to '00..011...1' */
		      val >>= 1;
		   return ((val & (val+1)) == 0);
		   }
		else 
		   return(0);
		break;

	   default:
		e2print(p);
		cerror( "special called!" );
		return( INFINITY );
	   }
}

static int	toff;
static int	toffbase;

popargs( size )
register size;
{
	/* pop arguments from the stack */
	toff -= size/2;
	if( toff == 0 )
	{
		size -= 2*toffbase;
		toffbase = toff = 0;
	}
	if(!size)
		return;

/*
	printf( "	add.l	&%d,%%sp\n", size );
*/
	if (size <= 16)
		printf("\tinc\tsp,#%%%x\n", size);
	else
		printf("\tadd\tsp,#%%%x\n", (unsigned short)size);
}


/* print the address of p+d */
next_address( p, d, typ, flg)
register NODE *p;
register int d;	/* the displacement */
register int typ;
{
       register NODE *ptemp,*r,*l;

       if (d) {
	  ptemp = talloc();
	  *ptemp= *p;
	  if(d == NFLG) d -= NFLG;
	  ptemp->tn.type = typ;
	  switch ( ptemp->in.op )
	  {
	    case REG:
		ptemp->bn.label = typlen(typ)>>1;
		if((!d)&&(typ == TCHAR))
			ptemp->tn.rval |= HIGHREG;	
	       ptemp->tn.rval += (d / 2);
	       break;
	    case TEMP: 	/* temps may be in registers */
	       if (ptemp->tn.rval) 
		  goto bad;
	       /* else FALLTHRU */
	    case ICON:
		if(p->in.name == (char *)0 && p->tn.rval == NONAME) {
			if(typlen(typ)==2) {
				if(!d)
					ptemp->tn.lval >>= 16;
				ptemp->tn.lval &= 0xffff;
				break;
			} else if (typlen(typ)==1) {
				if(!d)
					ptemp->tn.lval >>= 8;
				ptemp->tn.lval &= 0xff;
				break;
			}
		}
	    case NAME:
	    case VAUTO:
	    case VPARAM:
	       ptemp->tn.lval += d;
	       break;
	    case STAR:
	       p = p->in.left;
	       switch( p->in.op )
	       {
	       case REG:
				   /* form ( STAR (PLUS REG ICON(4) ) */
		  l = ptemp->in.left = talloc();
		  *l = *p;		/* copy type and other info */
		  l->tn.op = PLUS;
		  l->in.left = talloc();	/* copy REG node */
		  *(l->in.left )  = *p;	/* copy REG node */
		  r = l->in.right = talloc();	/* node for icon */
		  *r = *p;				/* copy type and other info */
		  r->tn.op = ICON;
		  r->tn.lval = d;
		  break;

	       case MINUS:
		  if (p->in.right->in.op != ICON)
			goto bad;
		  d = -d;
		  /* FALL THRU */
	       case PLUS:
		  if (p->in.right->in.op == ICON)
		     {
				   /* Make copy of address tree with ICON */
				   /* modified by adding d */
		     l = ptemp->in.left = talloc();
		     *l = *p;		/* copy PLUS node */
		     r = l->in.right = talloc();
		     *r= *(p->in.right);	/* copy ICON node */
		     if ( r->in.op != ICON ) goto bad;
		     r->tn.lval += d;
		     l->in.left = talloc();
		     l = l->in.left;
		     p = p->in.left;
		     *l = *p;		/* make copy of either REG or REG+REG */
		     if( l->in.op == REG ) break;	/* Done with copy */
		     if( l->in.op != PLUS) goto bad;	/* Bad address tree */
		     l->in.right=talloc(); /* Copy REG */
		     *(l->in.right) = *(p->in.right); /* Copy REG */
		     l->in.left = talloc();
		     l = l->in.left;
		     p = p->in.left;
		     *l  = *p;				/* Copy REG or CONV */
		     if( l->in.op == REG ) break;	/* Done with copy */
		     if( l->in.op != CONV) goto bad;	/* Bad address tree */
		     l->in.left=talloc();  /* Copy REG */
		     *(l->in.left)  = *(p->in.left);  /* Copy REG */
		     break;

		     }
		  else if (p->in.right->in.op == REG)
		     {
				   /* Make copy of address tree with ICON */
				   /* adding d */
		     l = ptemp->in.left = talloc();
		     *l = *p;		/* copy PLUS node */
		     r = l->in.right = talloc();
		     *r = *p;
		     r->tn.op = ICON;
		     r->tn.lval = d;
		     l->in.left = talloc();
		     l = l->in.left;
		     *l = *p;		/* copy PLUS node */
		     r = l->in.right = talloc();
		     *r= *(p->in.right);	/* copy REG node */
		     l->in.left = talloc();
		     *(l->in.left) = *(p->in.left); /* make copy of REG */
		     break;
		     }

	       default: goto bad;
	       };
	       break;
	    default:
	       goto bad;
	  } /* switch */
	
	if(flg)
		nadrput(ptemp);
	else
		adrput( ptemp );
	tfree( ptemp );
       }
       else {
		if(flg)
			nadrput(p);
		else
			adrput(p);
	}
       return;
bad:
       e2print( ptemp );
       cerror("Illegal byte address (ZB)\n");
       return(-1);
}

zzzcode( p, ppc , q)
register NODE *p;
register char **ppc;
OPTAB *q;
{
	register int c;
	register int ii;
	register NODE *newp, *tnode;
	register int displ,t;
	TWORD ht;

	static int labels[10];	/* labels for use in patterns */

	switch( c= *++(*ppc) )
	{
/*
	case 'p':
		popargs( 4 );
		return;
	case 'q':
		popargs( 8 );
		return;
*/
	case 'c':
		popargs((short)(p->stn.argsize/SZCHAR));
		return;
/*
	case '1':
		if( toff )
		{
			putchar( '-' );
		}
		printf( "(%%sp)" );
		if( !toff ) toffbase = 1;
		++toff;
		return;

	case '2':
		if( toff )
		{
			putchar( '-' );
		}
		printf( "(%%sp)" );
		if( !toff ) toffbase = 2;
		toff+=2;
		return;

	case '0':
		/* like 2 but doesn't print -(%%sp) */
		/* used for pea. leaves toffbase at 0, also */
/*		toff += 2;
		return;
*/
	case 's':  /* like s, but needs value afterwards */
	case 'S':  /* structure assignment */
		if( p->in.op == STASG )
		{
			stas( p, p->stn.stsize, c);
		}
		else if( p->in.op == STARG )
		{
			/* store an arg onto the stack */
			star( p->in.left, p->stn.stsize ,q);
		}
		else
		{
			e2print(p);
			cerror( "ZS bad" );
		}
		break;

	case 'z':  /* adjust register on right */
		{
		register long adj = p->stn.stsize;
		if( adj == SZSHORT || adj == SZLONG ) return;
		adj /= SZCHAR;
		if( adj > 16)
			printf( "\tsub\t%s,#%%%x\n",
				prnames(TINT, p->in.right->tn.rval+1, 0), (unsigned short)adj);
		else if( adj)
			printf( "\tdec\t%s,#%%%x\n",
				prnames(TINT, p->in.right->tn.rval+1,0), adj);
		}
		return;

	case 'I':
		cbgen( p->bn.lop, (int)(p->bn.label), c );
		return;

	case 'i':
		++*ppc;
		p = getadr(p,ppc);
		printf("%s",prnames(TINT,p->tn.rval,0));

		/* c = p->in.left->in.left->tn.rval;  /* register number */
		/* dbccgen( p->bn.lop, p->bn.label, c );*/
		return;

	case 'b':
		++*ppc;
		p = getadr(p,ppc);
		if((ii = ispow2(p->tn.lval))>=0)
		{
			printf( "#%%%x", ii );
			break;
		}
		cerror( "Zb but not power of 2" );

	case 'n':
/*		++*ppc;
		next_address( getadr(p,ppc), 4 );
*/
		displ = ((*++(*ppc)) - '0');
		if(!displ) displ += NFLG; /* da 'B' auch next_address nutzt */
		switch(*++(*ppc)) { 
			case 'c': t = TCHAR; break;
			case 's': t = TINT; break;
			case 'l': t = TLONG; break;
			case 'd': t = TDOUBLE; break;
			}
		++*ppc;
		ii = 0;
		if(**ppc == 'B') {
			**ppc = 'A';
			ii = 1;
		}
		next_address( getadr(p,ppc), displ, t , ii);
		break;

	case 'y':	/* CONV reg to shorter reg */
			/* CONV for regvar in char only in scatchregs */
		{ char qt,zt;
			qt = *++(*ppc);
			zt = *++(*ppc);
			++*ppc;
			tnode = getadr(p,ppc);
			if(zt == 'c') {
				++*ppc;
				newp = getadr(p,ppc);
				if(newp->tn.type&TCHAR) 
					newp->tn.type=(~TCHAR&newp->tn.type)|
					TSHORT;
				else
				if(newp->tn.type&TUCHAR)
					newp->tn.type=(~TUCHAR&newp->tn.type)|
					TUSHORT;
				else cerror(" bad 'CONV' registertype ");
				ht = newp->tn.type;
				if((tnode->tn.rval & 0xf) > 7 ) {
					printf("\tld\t");
					adrput(newp);
					putchar(',');
					if(qt == 'l') {
						tnode->tn.rval++;
						tnode->tn.type = TINT;
							}	
					adrput(tnode);
					putchar('\n');
					}
				else {
					*newp = *tnode;
					if(qt == 'l') {
						newp->tn.rval++;	
						}
				newp->tn.type = ht;
				newp->bn.label = 1;	
					}
			}
			if(zt == 's') {
				if(tnode->tn.type&TLONG) 
					tnode->tn.type=(~TLONG&tnode->tn.type)|
					TSHORT;
				else
				if(tnode->tn.type&TULONG)
					tnode->tn.type=(~TULONG&tnode->tn.type)|
					TUSHORT;
				else
				if(tnode->tn.type&TPOINT)
					tnode->tn.type =
					((~TPOINT & tnode->tn.type) & ~BTMASK)|
					TSHORT;
				else cerror(" bad 'CONV' registertype ");
				rfree(tnode->tn.rval,1);
				tnode->tn.rval++;
				tnode->bn.label = 1;	
				}
		}
		break;


	case 'f':	/* shift by number of lowest bit on */
		++*ppc;
		newp = getadr(p, ppc);
		c = newp->tn.lval;
		if (c > 0)
		   {
		   for (ii = 0; (c & 1) == 0; ii++)
			   c >>= 1;
		   doshift(ii, ppc, p);
		   }
		else
		   cerror( "Zf on zero or negative value" );
		break;

	case 'd':	/* shift by diff. of powers of 2 (for sums of powers) */
		++*ppc;
		newp = getadr(p, ppc);
		c = newp->tn.lval;
		if (c > 0)
		   {
		   while ((c & 1) == 0)
			   c >>= 1;
		   if ((c >>= 1) != 0 && (c & (c-1)) == 0) /* 1 bit left */
		      {
		      for (ii = 1; (c & 1) == 0; ii++)
			      c >>= 1;
		      doshift(ii, ppc, p);
		      break;
		      }
		   }
		cerror( "Zd on improper value" );
		break;

	case 'D':	/* shift by diff. of powers of 2 (for diff.) */
		++*ppc;
		newp = getadr(p, ppc);
		c = newp->tn.lval;
		if (c > 0)
		   {
		   while ((c & 1) == 0)	/* get to '00...011...1' */
			   c >>= 1;
		   if (c != 0 && (c & (c+1)) == 0) /* string of 1's */
		      {
		      for (ii = 0; (c & 1); ii++) /* count 1's */
			      c >>= 1;
		      doshift(ii, ppc, p);
		      break;
		      }
		   }
		cerror( "ZD on improper value" );
		break;

	case 'B':	/* byte address for a bset, bclr */
		++*ppc;
		newp = getadr(p, ppc);
		if((ii = ispow2(newp->tn.lval)) < 0)
		   {
		   cerror( "ZB but not power of 2" );
		   return;
		   }
		if(typlen(newp->tn.type)==1)
			ii&=7;
		printf("#%%%x", ii);	/* output the bit number (mod 8) */
		break;

	case 't':	/* take address of temp */
		++*ppc;
		newp = getadr(p, ppc);		/* the temp */
		++*ppc;
		p = getadr(p, ppc);		/* the result reg */
		if (newp->tn.rval == 0)		/* memory temp? */
		   {
			printf("\tldl\t");
		   adrput(p);
			printf(",FP\n\tadd\t");
			next_address(p, 2, TINT, 0);
			printf(",#%%%x\n", (unsigned short)(2+(int)newp->tn.lval));
		   break;
		   }
		tnode = talloc();		/* the memory temp */
		*tnode = *newp;			/* copy stuff */
		tnode->tn.lval = freetemp((OFFSZ)1);	/* get a memory temp */
		tnode->tn.lval = BITOOR(tnode->tn.lval);
		tnode->tn.rval = 0;		/* not in register */
		if( typlen(tnode->tn.type) >= 4)
		   c = 'l';			/* move long */
		else if(typlen(tnode->tn.type)==2)
		   c = ' ';			/* move word */
		else 
		   c = 'b';			/* move byte */
		printf("\tld%c\t", c);
		adrput(tnode);
		putchar(',');
		adrput( newp );
		if(typlen(tnode->tn.type) == 8) {
			printf("\tldl\t");
			next_address(tnode, 4, TLONG, 0);
			putchar(',');
			next_address(newp, 4, TLONG, 0);
		}
		printf("\tldl\t");
		adrput(p);
		printf(",FP\n\tadd\t");
		next_address(p, 2, TINT, 0);
		printf(",#%%%x\n", (unsigned short)(2+(int)tnode->tn.lval));
		tfree( tnode );
		break;

	case 'L':	/* first use of label in pattern */
	case 'l':	/* other uses of label in pattern */
		ii = *++(*ppc) - '0';	/* label number */
		if (ii < 0 || ii > 9)
			cerror( "illegal %c code: %c", c, ii+'0');
		if (c == 'L')		/* define it */
			labels[ii] = getlab();
		printf("L%d", labels[ii]);
		break;

	default:
		e2print(p);
		cerror( "illegal zzzcode: %c", c );
	}
}

/* put a shift left of size s of register A? from string "sA?" */
doshift (n, ppc, p)
	register int n;
	register char **ppc;
	register NODE *p;
	{
	register char size;
	register unsflg = 0;

	size= *++(*ppc);
	if(size == 'u') {
		size = *++(*ppc);
		unsflg++;
	}

	if(size == 's')
		size = ' ';
	++*ppc;
	p = getadr(p, ppc);
	if (n == 1)
	   {
	   printf("\tadd%c\t", size);
	   adrput(p);
	   putchar(',');
	   adrput(p);
	   putchar('\n');
	   }
	else if (n > 1)
	   {
	   if(unsflg)
		printf("\tsll%c\t",size);
	   else
		printf("\tsla%c\t",size);
	   adrput(p);
	   printf(",#%%%x\n", n);
/*
	   putchar('\n');
*/
	   }
	/* null for shift of 0 */
	}

star( p, sz , q)
register NODE *p;
OFFSZ sz;
OPTAB *q;
{
	/* put argument onto the stack */
	/* p is a REG */

	if( p->tn.op != REG ) uerror( "star of non-REG" );
/*
	sz /= (SZCHAR << 1);
*/
	sz /= SZCHAR;

/*
	if(sz > 2) { 
*/
	if(sz > 4) { /* mit ldir */
		char rn[4];
		char *dec = "dec";
		if(sz > 16)
			dec = "sub";
		strcpy(rn, prnames(TINT, resc[0].tn.rval,0));
		printf("\tld\t%s,#%%%x\n",rn,(unsigned short)sz);
		printf("\tsub\tsp,%s\n",rn);
		printf("\tld\t@SP,#0\n\tldirb\t@SP,@%s,%s\n",
			prnames(TLONG,p->tn.rval,0) ,rn);
		printf("\t%s\tsp,#%%%x\n",dec,(unsigned short)sz);
	} else
		printf("\tpush%c\t@SP,%s\n",((sz==4)?'l':' '),
			prnames((sz==4)?TLONG:TINT, p->tn.rval,0));

}

stas( p, sz, c )
register NODE *p;
OFFSZ sz;
register c;
{
	/* p is an STASG node; left and right are regs */
	/* all structures are aligned on int boundaries */

	register rn, ln, zz, scn;

	rn = p->in.right->tn.rval;
	if( c == 's' )
	{
		/* lhs is a temp moved into a reg: kluge! */
		ln = resc[0].tn.rval;  /* A1 */
		scn = resc[1].tn.rval;  /* A2 */
	}
	else {
		ln = p->in.left->tn.rval;
		scn = resc[0].tn.rval;  /* A1 */
		}

	zz = (sz /= SZCHAR) >> 1;
	if(zz > 2) { 
			char * dec = "dec";
			if(zz > 8) 
				dec = "sub";
			printf("\tld\t%s,#%%%x\n",prnames(TINT, scn,0), (unsigned short)zz);
			printf("\tldir\t@%s,",prnames(TLONG,ln,0));
			printf("@%s,",prnames(TLONG,rn,0));
			printf("%s\n",prnames(TINT,scn,0));
			if( (ln & 0xf) > 7)  /* regvar */
			/* restore clobbered register variable */
				printf( "\t%s\t%s,#%%%x\n",dec,
					prnames(TINT, ln+1,0), (unsigned short)sz);
			if( (rn & 0xf) > 7)  /* regvar */
			/* restore clobbered register variable */
				printf( "\t%s\t%s,#%%%x\n",dec,
				prnames(TINT, rn+1,0), (unsigned short)sz);
	} else if(zz == 2) {
		printf("\tldl\trx0,@r%s\n",prnames(TLONG, rn,0));
		printf("\tldl\t@r%s,rx0\n",prnames(TLONG, ln,0));
	} else  if (zz == 1) {
		printf("\tld\tx0,@r%s\n",prnames(TINT, rn,0));
		printf("\tld\t@r%s,x0\n",prnames(TINT, ln,0));
	} else	{
		e2print(p);
		cerror( "ZS sz %ld", sz );
	}
}

conput( p )
register NODE *p;
{
	char *s;
	int typ;
	switch( p->in.op )
	{
	case ICON:
		acon( p );
		return;
	case REG:
		printf("%s", prnames(p->tn.type, p->tn.rval,p->bn.label));
		return;
	default:
		e2print(p);
		cerror( "illegal conput" );
	}
}

insput( p )
NODE *p;
{
	e2print(p);
	cerror( "insput" );
}

#ifdef UPPUT

upput( pp )
register NODE *pp;
{
	/* output the address of the second word in the
	** pair pointed to by p (for LONGs)
	*/
	register long v;
	register NODE *r, *l, *p;

	v=0;
	p = pp;
	if( p->tn.op == PLUS )
	{
/*
		v=0;
		r = p->in.right;
		l = p->in.left;
		if( r->tn.op == ICON &&  l->tn.op == PLUS )
		{
			v = r->tn.lval;
			p = l;
			r = p->in.right;
			l = p->in.left;
		}
		if( r->tn.op == REG )
		{
			l = p->in.left;
			if( l->tn.op == REG )
			{
				printf( "%%%lx(%%a%d,%%d%d.l)", v, r->tn.rval,
					l->tn.rval );
				return;
			}
			if( l->tn.op != CONV ) goto ill;
			l = l->in.left;
			if( l->tn.op != REG ) goto ill;
			printf("%%%lx(%%a%d,%%d%d.w)",v,r->tn.rval,l->tn.rval);
			return;
		}
not by Z8000 */
		if( r->tn.op != ICON ) goto ill;
		v = r->tn.lval;
		p = p->in.left;
	}
	else if( p->tn.op == MINUS )
	{
		r = p->in.right;
		if( r->tn.op != ICON || r->in.name != (char *) 0 ) goto ill;
		v = -r->tn.lval;
		p = p->in.left;
	}
	if( p->tn.op != REG ) goto ill;
	if( v )
/*
		printf( "%%%lx(%%a%s)", v, rnames[p->tn.rval] );
	else
		printf( "(%%a%s)", rnames[p->tn.rval] );
*/
		printf( "_stkseg+%%%x(%s)",(unsigned short) v, prnames(TLONG,p->tn.rval,0) );
	else
		printf( "@%s", prnames(TLONG,p->tn.rval,0) );
	return;
ill:
	e2print(pp);
	cerror( "illegal address: upput" );
}

#else
upput( pp )
register NODE *pp;
{
	/* output the address of the second word in the
	** pair pointed to by p (for LONGs)
	*/
	if( pp->tn.op == REG ) {
		printf( "@%s", prnames(TLONG,pp->tn.rval,0) );
		return;
	}
	e2print(pp);
	cerror( "illegal address: upput" );
}
#endif UPPUT

adrput( p )
register NODE *p;
{
	/* output an address, with offsets, from p */

	while( p->in.op == FLD || p->in.op == CONV )
	{
		p = p->in.left;
	}
	switch( p->in.op )
	{

	case NAME:
		acon( p );
		sideff = 0;
		return;

	case ICON:
		/* addressable value of the constant */
		printf( "#" );
		acon( p );
		sideff = 0;
		return;

	case REG:
		conput( p );
		sideff = 0;
		return;

	case STAR:
		upput( p->in.left );
		return;

	case VAUTO:
		if ((int)p->tn.lval < 0)
			printf( "_stkseg+~L1-%%%x(fp)", (unsigned short)(-(p->tn.lval)) );
		else
			printf( "_stkseg+~L1+%%%x(fp)", (unsigned short) (p->tn.lval) );
		sideff = 0;
		return;

	case VPARAM:
		if ((int)p->tn.lval < 0)
			printf( "_stkseg+~L2-%%%x(fp)", (unsigned short)(-(p->tn.lval)) );
		else
			printf( "_stkseg+~L2+%%%x(fp)", (unsigned short) p->tn.lval );
		sideff = 0;
		return;

	case TEMP:
		if (p->tn.rval)
		   {
		   register int i = (int)p->tn.lval;

			printf("%s", prnames(p->tn.type, i, 0));
		   }
		else
		   printf( "_stkseg+%%%x(fp)", (unsigned short)(2 + p->tn.lval));
		sideff = 0;
		return;

	default:
		e2print(p);
		cerror( "illegal address" );
		return;
	}
}

/* lda ala OF */
padrput( p )
register NODE *p;
{
	/* output an address, with offsets, from p */

	switch( p->in.op )
	{

	case VAUTO:
	/* offset only !!! without FP */
		if ((int)p->tn.lval < 0)
			printf( "#~L1-%%%x", (unsigned short)(-(p->tn.lval)) );
		else
			printf( "#~L1+%%%x", (unsigned short)(p->tn.lval) );
		sideff = 0;
		return;

	case VPARAM:
	/* offset only !!! without FP */
		if ((int)p->tn.lval < 0)
			printf( "#~L2-%%%x", (unsigned short)(-(p->tn.lval)) );
		else
			printf( "#~L2+%%%x", (unsigned short)(p->tn.lval) );
		sideff = 0;
		return;

	default:
		e2print(p);
		cerror( "illegal address" );
		return;
	}
}

/* for BA-addressing in ld-Operations */
nadrput( p )
register NODE *p;
{
	/* output an address, with offsets, from p */

	switch( p->in.op )
	{

	case VAUTO:
		if ((int)p->tn.lval < 0)
			printf( "FP(#~L1-%%%x)", (unsigned short)(-(p->tn.lval)) );
		else
			printf( "FP(#~L1+%%%x)", (unsigned short) (p->tn.lval) );
		sideff = 0;
		return;

	case VPARAM:
		if ((int)p->tn.lval < 0)
			printf( "FP(#~L2-%%%x)", (unsigned short)(-(p->tn.lval)) );
		else
			printf( "FP(#~L2+%%%x)", (unsigned short) p->tn.lval );
		sideff = 0;
		return;

	}
	adrput( p );
}

acon( p )
register NODE *p;
{
	/* print out a constant */

	if( p->in.name != (char *) 0) {
		/* names */
		switch(p->tn.rval) {
			case 0x1000:
				printf("^s _%s", p->in.name );
				return;
			case 0x2000:
				printf("^o _");
				break;
			case 0x9000:
				printf("^s %s", p->in.name );
				return;
			case 0xa000:
				printf("^o ");
				break;
			case 0x3000:
				printf("_%s(",p->in.name);
				prnames(TINT,(int)p->tn.lval,0);
				putchar(')');
				return;
			default:
				if(p->tn.rval >= 0 || *(p->in.name)!='L')
					putchar('_');
		}
		printf("%s", p->in.name );
		if(p->tn.lval)
			putchar('+');
		else
			return;
	} else if(typlen(p->tn.type) >= 4 ) {
		printf( "%%%lx", p->tn.lval);
		return;
	}
	printf( "%%%x", (unsigned short)p->tn.lval);
}

/* alte version
acon( p )
register NODE *p;
{
	if( p->in.name == (char *) 0)
	{
		if(typlen(p->tn.type) < 4)
			printf( "%%%x", (unsigned short) p->tn.lval);
		else
			printf( "%%%lx", p->tn.lval);
	} else if( p->tn.rval == 0x2000 ) {
		printf("^o _%s",p->in.name);
	} else if( p->tn.rval == 0x1000 ) {
		printf( "^s _%s", p->in.name );
	} else if( p->tn.rval == 0xa000 ) {
		printf("^o %s",p->in.name);
	} else if( p->tn.rval == 0x9000 ) {
		printf( "^s %s", p->in.name );
	} else if( p->tn.rval == 0x3000 ) {
		printf("_%s(",p->in.name);
		prnames(TINT,(int)p->tn.lval,0);
		putchar(')');
	} else if( p->tn.lval == 0 ) {
		if(p->tn.rval < 0 && *(p->in.name)=='L')
			printf("%s",p->in.name);
		else
			printf( "_%s", p->in.name );
	}
	else
	{
		if(p->tn.rval < 0 && *(p->in.name)=='L')
			printf( "%s+%%%x", p->in.name , (unsigned short) p->tn.lval );
		else
			printf( "_%s+%%%x", p->in.name , (unsigned short) p->tn.lval );
	}
	if( p->tn.lval != 0 && (p->tn.rval == 0xa000 || p->tn.rval == 0x2000))
		printf( "+%%%x", (unsigned short) p->tn.lval );
}
*/
static char	*ccbranches[] =
{
/*
	"	beq	L%%%d\n",
	"	bne	L%%%d\n",
	"	ble	L%%%d\n",
	"	blt	L%%%d\n",
	"	bge	L%%%d\n",
	"	bgt	L%%%d\n",
	"	bls	L%%%d\n",
	"	blo	L%%%d\n",
	"	bhs	L%%%d\n",
	"	bhi	L%%%d\n",
*/
	"	jpr	eq,L%d\n",
	"	jpr	ne,L%d\n",
	"	jpr	le,L%d\n",
	"	jpr	lt,L%d\n",
	"	jpr	ge,L%d\n",
	"	jpr	gt,L%d\n",
	"	jpr	ule,L%d\n",
	"	jpr	ult,L%d\n",
	"	jpr	uge,L%d\n",
	"	jpr	ugt,L%d\n",
};

cbgen( o, lab, mode )
register o,lab;
{
	/* printf conditional and unconditional branches */
	if( o == 0 ) printf( "	jpr	L%d\n", lab );
	else if( o > UGT ) cerror( "bad conditional branch: %s", opst[o] );
	else printf( ccbranches[o-EQ], lab );
}

static char	*dbccops[] =
{
/*
	"	dbeq	%%d%d,L%%%d\n",
	"	dbne	%%d%d,L%%%d\n",
	"	dble	%%d%d,L%%%d\n",
	"	dblt	%%d%d,L%%%d\n",
	"	dbge	%%d%d,L%%%d\n",
	"	dbgt	%%d%d,L%%%d\n",
	"	dbls	%%d%d,L%%%d\n",
	"	dblo	%%d%d,L%%%d\n",
	"	dbhs	%%d%d,L%%%d\n",
	"	dbhi	%%d%d,L%%%d\n",
*/
	"ne", "eq", "gt", "ge", "lt", "le", "ugt", "uge", "ult", "ule",
};

dbccgen( o, lab, reg )
register o,reg,lab;
{
	/* printf conditional and unconditional branches */
	int l;
/*
	if( o == 0 ) printf( "	dbt	%%d%d,L%%%d\n", reg, lab );
	else if( o > UGT ) cerror( "bad conditional branch: %s", opst[o] );
	else printf( dbccops[o-EQ], reg, lab );
*/
	if( o == 0) printf("\tdec\t%s\n\tjpr\tpl,L%d\n",prnames(TINT,reg,0),lab);
	else if( o > UGT ) cerror( "bad conditional branch: %s", opst[o] );
	else {
		l = getlab();
		printf("\tjr\t%s,L%d\n\tdec\t%s\n\tjpr\tpl,L%d\nL%d:\n",
			dbccops[o-EQ],l,prnames(TINT,reg,0),lab,l);
	}
}

/* see if an unused register can be used for a temp */

/*#define DREGS (0xff & (0xff<<NRGS))	/* the available D registers */
/*#define AREGS (0x3f00 & (0xff00<<NRGS))	/* the available A registers */
#define MREGS 0xf00	/* 4 available registers 8...11 for scratch */

int regtmp = 0;

tempreg(rnum, nlongs, type)
	register int *rnum;	/* returns regiser number if one allocated */
	OFFSZ nlongs;	/* number of long words needed for the temp */
	register TWORD type;	/* type of the value to be put in the temp */
	{
	register int r, rbit, mask;
	extern int regvar;

	if (nlongs != 1 || ((type & TSTRUCT) && !(type & TPOINT)))
		return (0);	/* can't have > 1 reg or structures */
	mask = ~(regvar | regtmp);	/* consider all unallocated registers */

/*	if ((type & TPOINT) && (mask & AREGS))	*/
/*		mask &= AREGS;	/* prefer pointers in A registers */
/*				/* but can be put in D registers if no A's */
/*	else	*/
/*		mask &= DREGS;	/* non-pointers restricted to D registers */
	mask &= MREGS;

	if (mask == 0)
		return(0);	/* none available! */

	for (r = 8, rbit = 0x100; (mask & rbit) == 0; r++)
		rbit <<= 1;
	/* mark it used */
	regtmp |= rbit;
	SETREGS(rbit);
	*rnum = r;
	return (1);	/* success! */
	}
