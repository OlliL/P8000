# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"../h/pipes.h"
# include	"ovqp.h"

/*
** TYPECHECK
**
**	Performs typechecking and conversions where appropriate .
**	Prohibits mixed type expressions.
*/
typecheck(pp1, pp2, opval)
SYMBOL		*pp1, *pp2;
int		opval;
{
	register SYMBOL		*p1;
	register SYMBOL		*p2;
	register int		i;

	p1 = pp1;
	p2 = pp2;

	i = (((SYMBOL *) p1)->type == CHAR && ((SYMBOL *) p2)->type == CHAR);
	switch (opval)
	{
	  case opCONCAT:
		if (!i)		/* numeric in a char operator */
			ov_err(NUMERIC);
		return;	/* else no further checking is needed */

	  case opADD:
	  case opSUB:
	  case opMUL:
	  case opDIV:
	  case opPOW:
	  case opMOD:
		if (i)	/* arithmetic operation on two character fields */
			ov_err(BADCHAR);
	}

	/* first check for identical types of symbols */
	if (((SYMBOL *) p1)->type == ((SYMBOL *) p2)->type)
	{
		if (((SYMBOL *) p1)->len == ((SYMBOL *) p2)->len)
			return;
		/* lengths are different. make p2 point to the smaller one */
		if (((SYMBOL *) p1)->len < ((SYMBOL *) p2)->len)
		{
			p2 = p1;
			p1 = pp2;
		}

		switch (((SYMBOL *) p2)->type)
		{
		  case INT:
			if (((SYMBOL *) p1)->len == sizeof (long))
				i2toi4(p2);

		  case CHAR:
			return;	/* done if char or int */

# ifndef NO_F4
		  case FLOAT:
			f8tof4(p2);
			return;
# endif
		}
	}

	/* at least one symbol is an INT or FLOAT. The other can't be a CHAR */
	if (((SYMBOL *) p1)->type == CHAR || ((SYMBOL *) p2)->type == CHAR)
		ov_err(BADMIX);	/* attempting binary operation on one CHAR field with a numeric */

	/* one symbol is an INT and the other a FLOAT */
	if (((SYMBOL *) p2)->type == INT)
	{
		/* exchange so that p1 is an INT and p2 is a FLOAT */
		p1 = p2;
		p2 = pp1;
	}

	/* p1 is an INT and p2 a FLOAT */
	itof8(p1);
# ifndef NO_F4
	if (((SYMBOL *) p2)->len == sizeof (float))
		f8tof4(p2);
# endif
}


/*
**	Coerce the top of stack symbol to the
**	specified type and length. If the current
**	value is a character then it must be converted
**	to numeric. A user error will occure is the
**	char is not syntaxtically correct.
*/
typecoerce(tos, ntype, nlen)
register struct stacksym	*tos;
int				ntype;
int				nlen;
{
	register char			*cp;
	register char			*val;
	register int			ret;
	char				temp[MAXFIELD + 1];

	if (((SYMBOL *) tos)->type == CHAR)
	{
		val = (char *) ((SYMBOL *) tos)->value;
		cp = temp;
		AAbmove(((STRINGP *) val)->stringp, cp, ctou(((SYMBOL *) tos)->len));
		cp[ctou(((SYMBOL *) tos)->len)] = '\0';
		if (ntype == FLOAT)
		{
			ret = AA_atof(cp, val);
# ifndef NO_F4
			if (nlen == sizeof (float))
				((F4TYPE *) val)->f4type = ((F8TYPE *) val)->f8type;
# endif
		}
		else
		{
			if (nlen == sizeof (long))
				ret = AA_atol(cp, val);
			else
				ret = AA_atoi(cp, val);
		}
		if (ret < 0)
			ov_err(CHARCONVERT);
		((SYMBOL *) tos)->type = ntype;
		((SYMBOL *) tos)->len = nlen;
	}
	else
		rcvt(tos, ntype, nlen);
}


i2toi4(p)
register SYMBOL		*p;
{
	register char		*val;

	val = (char *) ((SYMBOL *) p)->value;
	((I4TYPE *) val)->i4type = ((I2TYPE *) val)->i2type;
	((SYMBOL *) p)->len = sizeof (long);
}


i4toi2(p)
register SYMBOL		*p;
{
	register char		*val;

	val = (char *) ((SYMBOL *) p)->value;
	((I2TYPE *) val)->i2type = ((I4TYPE *) val)->i4type;
	((SYMBOL *) p)->len = sizeof (short);
}


itof8(p)
register SYMBOL		*p;
{
	register char		*val;

	val = (char *) ((SYMBOL *) p)->value;
	if  (((SYMBOL *) p)->len == sizeof (long))
		((F8TYPE *) val)->f8type = ((I4TYPE *) val)->i4type;
	else
		((F8TYPE *) val)->f8type = ((I2TYPE *) val)->i2type;
	((SYMBOL *) p)->type = FLOAT;
	((SYMBOL *) p)->len = sizeof (double);
}


# ifndef NO_F4
itof4(p)
register SYMBOL		*p;
{
	register char		*val;

	val = (char *) ((SYMBOL *) p)->value;
	if  (((SYMBOL *) p)->len == sizeof (long))
		((F4TYPE *) val)->f4type = ((I4TYPE *) val)->i4type;
	else
		((F4TYPE *) val)->f4type = ((I2TYPE *) val)->i2type;
	((SYMBOL *) p)->type = FLOAT;
	((SYMBOL *) p)->len = sizeof (float);
}
# endif


ftoi2(p)
register SYMBOL		*p;
{
	register char		*val;

	val = (char *) ((SYMBOL *) p)->value;
	((I2TYPE *) val)->i2type = ((F8TYPE *) val)->f8type;
	((SYMBOL *) p)->type = INT;
	((SYMBOL *) p)->len = sizeof (short);
}


ftoi4(p)
register SYMBOL		*p;
{
	register char		*val;

	val = (char *) ((SYMBOL *) p)->value;
	((I4TYPE *) val)->i4type = ((F8TYPE *) val)->f8type;
	((SYMBOL *) p)->type = INT;
	((SYMBOL *) p)->len = sizeof (long);
}


# ifndef NO_F4
f8tof4(p)
register SYMBOL		*p;
{
	register char		*val;

	val = (char *) ((SYMBOL *) p)->value;
	((F4TYPE *) val)->f4type = ((F8TYPE *) val)->f8type;
	((SYMBOL *) p)->len = sizeof (float);
}
# endif
