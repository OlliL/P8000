# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/scanner.h"
# include	"parser.h"

/*
** OPERATOR
** an operator is defined as any 1-3 character sequence of
** non-alphanumerics.  It looks up each operator in 'Optab'
** and returns the appropriate token.
*/
operator(chr)
char	chr;
{
	extern char		*yylval;
	register struct optab	*op;
	register int		toktyp;
	register int		save;
	char			buf[4];

	/* get lookahead characer */
	save = Lcase;
	Lcase = 0;
	buf[0] = chr;
	buf[1] = toktyp = gtchar();
	buf[2] = gtchar();
	buf[3] = '\0';

	/* is it a floating fraction without leading zero ? */
	if (buf[0] == '.' && CMAP(toktyp) == NUMBR)
	{
		Lcase = save;
		backup(buf[2]);
		backup(toktyp);
		return (number(chr));
	}

	/* three character operator ? */
	for (op = &Optab[0]; op->term; op++)
		if (AAsequal(op->term, buf))
			break;
	if (!op->term)
	{
		/* two character operator ? */
		backup(buf[2]);
		buf[2] = '\0';
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
				Lcase = save;
				/* invalid operator */
				yyerror (BADOP, 0);
			}
		}
	}
	Lcase = save;
	if(op->token == Tokens.bgncmnt)
		return (comment());
	if(op->token == Tokens.sconst)
		return (string(op));
	Lastok.tok = op->term;
	Lastok.toktyp = Tokens.sconst;
	yylval = CAST(op->opcode);
	return (op->token);
}
