# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/scanner.h"

/*
** COMMENT
** scans comments (as delimited by the tokens 'Tokens.bgncmnt'
** and 'Tokens.endcmnt') and removes them from the query text.
*/
comment()
{
	register int		i;
	register int		l;
	register struct optab	*op;
	char			buf[3];

	/* find the end_of_comment operator */
	for (op = Optab; op->term; op++)
		if (op->token == Tokens.endcmnt)
			break;
	if (!op->term)
		AAsyserr(20000);

	/* scan for the end of the comment */
	l = AAlength(op->term);
	if (l > sizeof buf - 1)
		AAsyserr(20001, l, op->term, sizeof buf - 1);

	for (i = 0; i < l; i++)		/* set up window on input */
		if ((buf[i] = gtchar()) <= 0)
			/* non-terminated comment */
			yyerror(COMMTERM, (char *) 0);
	while (!AAbequal(buf, op->term, l))
	{
		/* move window on input */
		for (i = 0; i < l - 1; i++)
			buf[i] = buf[i + 1];
		if ((buf[l - 1] = gtchar()) <= 0)
			/* non terminated comment */
			yyerror(COMMTERM, (char *) 0);
	}
	return (0);
}
