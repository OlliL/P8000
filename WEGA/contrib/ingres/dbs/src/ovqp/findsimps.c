# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"
# include	"../h/pipes.h"
# include	"ovqp.h"
# include	"strategy.h"

int		Nsimp;		/* Current no. entries in Simp vector	*/
struct simp	Simp[NSIMP];

/*
**	Findsimps searches the qualification for
**	occurances of simple clauses. In its
**	current crude implementation it only finds
**	clauses of the form:
**
**	var relop constant  or  constant relop var
**
**	it does not use simple clauses with 'OR's
**	nor does it accept clauses of the form
**
**	var relop constant + constant    etc.
**
**	Findsimps knows about pattern matching characters
**	and treats char constants containing pm chars
**	specially. For example
**		var >= "xx*"  --> var >= "xx"
**		var <= "xx*"  --> var <= "xx\0177"
**		var =  "xx*"  --> var >= "xx" and var <= "xx\0177"
**	If the first char is a pm char then the clause is not
**	considered as a simple clause. Also notice that the conversion
**	is done only once. If the next time Newq = FALSE, then findsimps()
**	isn't called. This works because a pm char can only come from
**	the user and not from a relation. Thus during tuple substition
**	a constant with a pm will never change.
*/

findsimps()
{
	register SYMBOL		*c;
	register int		t;
	int			length;
	register SYMBOL		**q;
	int			attno;
	int			rel;
	int			found;
	SYMBOL			*xc;
	extern SYMBOL		*cpsym();

# ifdef xOTR1
	AAtTfp(21, 0, "FINDSIMPS\n");
# endif
	Nsimp = 0;
	found = FALSE;
	q = Qlist;	/* q holds pointer to qualification */

	if (!q)
		return (0);


	for (t = ctou((*q)->type); t != QLEND; t = ctou((*++q)->type))
	{
		switch (t)
		{
		  case VAR:
			attno = ctou(*(*q)->value);	/* save att number */
			if ((t = ctou((*++q)->type)) == INT
			   || t == FLOAT || t == CHAR || t == S_VAR)
			{
				c = *q;	/* save pointer to value symbol */
				t = ctou((*++q)->type);
				if ((rel = relop(*q, FALSE)) >= 0
				   && (t = ctou((*++q)->type)) == AND)
				{
					/* found a simple clause */
					found = TRUE;
				}
			}
			break;

		  case S_VAR:
		  case INT:
		  case FLOAT:
		  case CHAR:
			c = *q++;
			if ((t = ctou((*q)->type)) == VAR)
			{
				attno = ctou(*(*q)->value);
				t = ctou((*++q)->type);
				if ((rel = relop(*q, TRUE)) >= 0 && (t = ctou((*++q)->type)) == AND)
				{
					/* found a simple clause */
					found = TRUE;
				}
			}
		}
		if (found)
		{
			/* a simple clause has been found.
			** Check that the constant contains
			** at least one char before any pattern
			** matching char. If there is a pattern
			** matching char then special processing
			** must be done.
			*/

			found = FALSE;
			if (length = check(c))
			{

				/*
				** If length is zero then the first char was
				** a pattern matching char. If length < 0 then
				** no pattern matching char, and finally
				** if length > 0 then length is the number of
				** chars before the first pattern matching char
				*/
				if (length > 0)
				{
					switch (rel)
					{

					  case opEQ:
						/*
						** Create two simple clauses:
						** One below the value and the
						** other above the value.
						*/
						xc = cpsym(c, length, opLTLE);
						add_simp(xc, opLTLE, attno);
						rel = (int) opGTGE;
						/* fall through to GTGE case */

					  case opGTGE:
						c = cpsym(c, length, opGTGE);
						break;

					  case opLTLE:
						c = cpsym(c, length, opLTLE);
						break;
					}
				}

				if (add_simp(c, rel, attno))
					break;	/* no more room in simps */
			}
		}
		while (t != AND)	/* skip to next AND */
			t = ctou((*++q)->type);
	}
# ifdef xOTR1
	AAtTfp(21, 2, "FINDSIMPS: rets %d\n", Nsimp);
# endif
	return (Nsimp);
}


/*
**	RELOP determines whether a symbol is a
**	usable relational operator ie. =,>,>=,<,<=
**
**	returns the type of the relational
**	operator if found, else it returns
**	-1
**
**	Items are normalized to be in the form:
**	var relop constant. If reverse is TRUE then
**	complement the sense of the relop. Reverse will
**	be TRUE is the simple clause was found in the
**	form constant relop var.
*/
relop(s, reverse)
register SYMBOL	*s;
register int	reverse;
{
	register int	v;

	v = -1;	/* assume failure */
	if (s->type == BOP)
	{
		switch (ctou(*s->value))
		{
		  case opEQ:
			v = (int) opEQ;
			break;

		  case opLT:
		  case opLE:
			v = (int) opLTLE;
			if (reverse)
				v = (int) opGTGE;
			break;

		  case opGT:
		  case opGE:
			v = (int) opGTGE;
			if (reverse)
				v = (int) opLTLE;
			break;
		}
	}
	return (v);
}



/*
**	CHECK checks the symbol for
**	pattern matching characters.
**	If any are found then check returns
**	the number of characters before the
**	first pattern matching character.
**
**	If no pattern matching chars are found
**	then check returns -1.
**
**	note that PAT_RBRAC need not be checked for
**	since it is not a pattern matching char unless
**	PAT_LBRAC appears before it.
**
**	PAT_LBRAC is treated specially in cpsym().
**	If any are detected, then length until the
**	first PAT_ANY or PAT_ONE is returned.
*/
check(s)
register SYMBOL	*s;
{
	register char		*cp;
	register int		len;
	register int		flag;

# ifdef xOTR1
	if (AAtTfp(21, 4, "CHECK: sym"))
		prsym(s);
# endif
	if (s->type == CHAR)
	{
		flag = FALSE;
		cp = (char *) s->value;	/* the string is a literal */
		len = ctou(s->len);
		while (len--)
		{
			switch (*cp++)
			{
			  case PAT_ANY:
			  case PAT_ONE:
				return ((ctou(s->len)) - len - 1);

			  case PAT_LBRAC:
				flag = TRUE;
			}
		}
		if (flag)
			return (ctou(s->len));	/* constant had PAT_LBRAC char */
	}
	return (-1);	/* ok */
}


# ifdef EBCDIC
# define	LAST_CHAR	0377
# else
# define	LAST_CHAR	0177
# endif

/*
** CPSYM -- copy a symbol to a new buffer area.
**	If op is opLTLE then add a pad character
**	whose value is the largest possible char
**	value.
**
**	If any ranges of characters are found,
**	then the lowest/highest char is taken from
**	range.
*/
SYMBOL	 *cpsym(konst, len, op)
SYMBOL		*konst;
int		len;
int		op;
{
	register SYMBOL		*s;
	register char		*cp;
	register int		i;
	register char		*sp;
	register int		c;
	register int		nc;
	extern char		*Ovqpbuf;
	extern char		*need();

	i = len;
	len += SYM_HDR_SIZ;
	if (op == (int) opLTLE)
		len++;
	s = (SYMBOL *) need(Ovqpbuf, len);
	s->type = CHAR;
	sp = (char *) s->value;
	cp = (char *) konst->value;

	while (i--)
	{
		/* copy chars processing LBRAC chars if any */
		if ((c = *cp++) == PAT_LBRAC)
		{
			/* if string is empty, ignore it */
			if (i == 0)
				break;

			c = *cp++;
			i--;

			if (c == PAT_RBRAC)
				continue;	/* empty [] */

			while (i-- && ((nc = *cp++) != PAT_RBRAC))
			{
				/* ignore '-' */
				if (nc == '-')
					continue;

				/* check for char larger/smaller than 'c' */
				if (op == (int) opLTLE)
				{
					if (nc > c)
						c = nc;
				}
				else
				{
					if (nc < c)
						c = nc;
				}
			}
		}

		*sp++ = c;	/* copy next char */
	}
	if (op == (int) opLTLE)
		*sp++ = LAST_CHAR;
	s->len = sp - (char *) s->value;

	return (s);
}


/*
** ADD_SIMP -- add a simple clause to the list of
**	simple clauses. As a side effect the Nsimp
**	is incremented. If there is no room return
**	TRUE else return FALSE
*/
add_simp(konst, rel, attno)
register SYMBOL	*konst;
register int	rel;
int		attno;
{
	register struct simp	*s;

	if (Nsimp == NSIMP)
		return (TRUE);	/* no more room */

	s = &Simp[Nsimp++];

	s->att = attno;
	s->konst = konst;
	s->relop = rel;

# ifdef xOTR1
	if (AAtTf(21, 3))
		prsimp(s);
# endif

	return (FALSE);
}


# ifdef xOTR1
prsimp(s)
register struct simp	*s;
{
	printf("simp:relop=%d,att=%d,val=", s->relop, s->att);
	prsym(s->konst);
}
# endif
