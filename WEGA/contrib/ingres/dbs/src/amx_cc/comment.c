# include	"amx.h"

/*
**	Scans comments (as delimited by the tokens 'Tokens.bgncmnt'
**	and 'Tokens.endcmnt') and removes them from the query text.
**
*/
comment()
{
	register int		i;
	register int		l;
	register struct optab	*op;
	char			buf[5];

	/* find the end_of_comment operator */
	for (op = Optab; op->term; op++)
		if (op->token == Tokens.endcmnt)
			break;

	/* scan for the end of the comment */
	l = AAlength(op->term);
	for (i = 0; i < l; i++)
	{
		/* set up window on input */
		if (!(buf[i] = gtchar()))
			goto non_term;
	}
	while (!AAbequal(buf, op->term, l))
	{
		/* move window on input */
		for (i = 0; i < l - 1; i++)
			buf[i] = buf[i + 1];
		if (!(buf[l - 1] = gtchar()))
		{
non_term:
			amxerror(50);
			return (0);
		}
	}
	return (0);
}
