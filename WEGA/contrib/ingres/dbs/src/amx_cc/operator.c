# include	"amx.h"

/*
**	An operator is defined as any 1-2 character sequence of
**	non-alphanumerics.  It looks up each operator in 'Optab'
**	and returns the appropriate token.
*/
operator(chr)
register int	chr;
{
	register struct optab	*op;
	register int		save;
	register int		toktyp;
	char			buf[3];
	extern char		*yylval;

	save = Lcase;
	Lcase = 0;

	/* get lookahead character */
	buf[0] = chr;
	buf[1] = toktyp = gtchar();
	buf[2] = '\0';

	/* is it a floating fraction without leading zero ? */
	/* or an integer < 0				    */
	if ((chr == '.' || chr == '-') && CMAP(toktyp) == NUMBR)
	{
		Lcase = save;
		backup(toktyp);
		return (number(chr));
	}

	/* two character operator ? */
	for (op = &Optab[0]; op->term; op++)
		if (AAsequal(op->term, buf))
			break;
	if (!op->term)
	{
		backup(toktyp);
		buf[1] = '\0';
		for (op = &Optab[0]; op->term; op++)
			if (AAsequal(op->term, buf))
				break;
		if (!op->term)
		{
			/* invalid operator */
			amxerror(87, buf);
			return (1);
		}
	}
	Lcase = save;
	if(op->token == Tokens.bgncmnt)
		return (comment());
	if(op->token == Tokens.sconst)
		return (string(op));
	yylval = (char *) op->opcode;
	return (op->token);
}
