# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"
# include	"../h/pipes.h"
# include	"ovqp.h"


/*
**	INTERP1.C
**
**	symbol I/O utility routines for OVQP interpreter.
**
*/

/*
** GETSYMBOL
**
**	Gets (type, len, value) symbols from list
**	A ptr to the list is advanced after
**	call.  Symbols are moved to a target location
**	(typically a slot on the interpreter's Stack).
**	Legality of symbol type and length is checked.
**	Returns		1  if no more symbols in list
**			0 otherwise
**
*/

getsymbol(ts, p)
SYMBOL		*ts;		/* target location (on stack) */
SYMBOL		***p;		/* pointer to list */
{
	register char			*d;	/* ptr to data for target symbol */
	register char			*cp;	/* the item in the list */
	register struct stacksym	*tops;	/* target location on stack */
	register int			len;	/*length of target symbol*/
	register char			*val;
	extern char			*AAbmove();

	tops = (struct stacksym *) ts;	/* copy stack pointer */
	cp = (char *) **p;	/* get list pointer */
#	ifdef xOTR1
	if (AAtTfp(24, 0, "GETSYM:"))
		prsym(cp);
#	endif

	if (((SYMBOL *) cp)->type == VAR || ((SYMBOL *) cp)->type == S_VAR)
	{
		((SYMBOL *) tops)->type = ((SVAR_NODE *) cp)->vtype;
		len = ((SYMBOL *) tops)->len = ((SVAR_NODE *) cp)->vlen;
		d = (char *) (((SVAR_NODE *) cp)->vpoint);
		if (((SYMBOL *) tops)->type == INT && len == sizeof (char))
		{
			val = (char *) ((SYMBOL *) tops)->value;
			((I2TYPE *) val)->i2type = ctoi(((I1TYPE *) d)->i1type);
			/* advance Qvect sequencing pointer p */
			*p += 1;
			return (0);
		}
	}
	else
	{
		((SYMBOL *) tops)->type = ((SYMBOL *) cp)->type;
		len = ((SYMBOL *) tops)->len = ((SYMBOL *) cp)->len;
		len = ctou(len);
		d = (char *) ((SYMBOL *) cp)->value;
	}
	/* advance Qvect sequencing pointer p */
	*p += 1;

	val = (char *) ((SYMBOL *) tops)->value;
	switch (((SYMBOL *) tops)->type)
	{
	  case INT:
		switch (len)
		{
		  case sizeof (char):
			((I2TYPE *) val)->i2type = ((I2TYPE *) d)->i2type;
			break;

		  case sizeof (short):
		  case sizeof (long):
			AAbmove(d, val, len);
			break;

		  default:
			AAsyserr(17007, len);
		}
		break;

	  case FLOAT:
		AAbmove(d, val, len);
# ifndef NO_F4
		if (len == sizeof (float))	/* convert to f8 */
		{
			((F8TYPE *) val)->f8type = ((F4TYPE *) val)->f4type;
			((SYMBOL *) tops)->len = sizeof (double);
		}
		else
# endif
			if (len != sizeof (double))
				AAsyserr(17008, len);
		break;

	  case CHAR:
		((STRINGP *) val)->stringp = d;
		break;

	  case AOP:
	  case BOP:
	  case UOP:
	  case COP:
	  case RESDOM:
		/* all except aop are of length 1. aop is
		** length AOP_SIZ but the first byte is the aop value
		*/
		((I2TYPE *) val)->i2type = ((I2TYPE *) d)->i2type;
		break;

	  case AND:
	  case OR:
		break;

	  case AGHEAD:
	  case BYHEAD:
	  case ROOT:
	  case QLEND:
		return (1);	/* all these are delimitors between lists */

	  default:
		AAsyserr(17009, ((SYMBOL *) tops)->type);
	}
	return (0);
}



/*
*  TOUT
*
*	Copies a symbol value into the Output tuple buffer.
* 	Used to write target
*	list elements or aggregate values into the output tuple.
*/

tout(sp, offp, rlen)
SYMBOL		*sp;
char		*offp;
{
	register SYMBOL		*s;
	register int		i;
	register char		*p;
	int			slen;
	extern char		*AAbmove();

	s = sp;	/* copy pointer */

#	ifdef xOTR1
	if (AAtTfp(24, 3, "TOUT: s="))
	{
		prstack(s);
		printf("  offset=%d, rlen=%d\n", offp-Outtup, rlen);
	}
#	endif

	p = (char *) ((SYMBOL *) s)->value;
	if (((SYMBOL *) s)->type == CHAR)
	{
		slen = ctou(((SYMBOL *) s)->len);
		rlen = ctou(rlen);
		i = rlen - slen;	/* compute difference between sizes */
		if (i <= 0)
			AAbmove(((STRINGP *) p)->stringp, offp, rlen);
		else
		{
			p = AAbmove(((STRINGP *) p)->stringp, offp, slen);
			while (i--)
				*p++ = ' ';	/* blank out remainder */
		}
	}
	else if (rlen == sizeof (char))	/* ((SYMBOL *) s)->type == INT */
		*offp = ((I2TYPE *) p)->i2type;
	else
		AAbmove(p, offp, rlen);
}



rcvt(tosp, restype, reslen)
SYMBOL		*tosp;
int		restype;
int		reslen;
{
	register SYMBOL		*tos;
	register int		rtype;
	register int		rlen;
	register int		stype;
	register int		slen;

	tos = tosp;
	rtype = restype;
	rlen = reslen;
	stype = ((SYMBOL *) tos)->type;
	slen= ((SYMBOL *) tos)->len;
#	ifdef xOTR1
	if (AAtTfp(24, 6, "RCVT: rt=%d, rl=%d, tos=", rtype, rlen))
		prstack(tos);
#	endif

	if (rtype != stype)
	{
		if (rtype == CHAR || stype == CHAR)
			ov_err(BADCONV);	/* bad char to numeric conversion requested */
		if (rtype == FLOAT)
		{
# ifndef NO_F4
			if (rlen == sizeof (float))
				itof4(tos);
			else
# endif
				itof8(tos);
		}
		else
		{
			if (rlen == sizeof (long))
				ftoi4(tos);
			else
				ftoi2(tos);
		}
		((SYMBOL *) tos)->len = rlen;	/* handles conversion to i1 */
	}

	else
		if (rtype != CHAR && rlen != slen)
		{
			if (rtype == INT)
			{
				if (rlen == sizeof (long))
					i2toi4(tos);
				else if (slen == sizeof (long))
					i4toi2(tos);
			}
# ifndef NO_F4
			else	/* rtype == FLOAT && rlen == sizeof (float) */
				f8tof4(tos);
# endif
			((SYMBOL *) tos)->len = rlen;	/* handles conversion to i1 */
		}
}
