/*
** ---  RC_OP.C --------- OPERATOR SCANNER  -----------------------------------
**
**	An operator is defined as any 1-3 character sequence of
**	non-alphanumerics.  It looks up each operator in 'Optab'
**	and returns the appropriate token.
**
**	Version: @(#)rc_op.c		4.0	02/05/89
**
*/

# include	"rc.h"


rc_operator(chr)
register int	chr;
{
	register struct optab	*op;
	register int		toktyp;
	register int		save;
	char			buf[4];
	extern char		*rc_syment();
	extern char		*yylval;

	save = Lcase;
	Lcase = 0;

	/* get lookahead character */
	buf[0] = chr;
	buf[1] = toktyp = rc_gtchar();
	buf[2] = rc_gtchar();
	buf[3] = '\0';

	/* is it a floating fraction without leading zero ? */
	/* or an integer < 0				    */
	if ((buf[0] == '.' || buf[0] == '-') && CMAP(toktyp) == NUMBR)
	{
		Lcase = save;
		rc_backup(buf[2]);
		rc_backup(toktyp);
		return (rc_number(chr));
	}

	/* three character operator ? */
	for (op = &Optab[0]; op->term; op++)
		if (AAsequal(op->term, buf))
			break;
	if (!op->term)
	{
		/* two character operator ? */
		rc_backup(buf[2]);
		buf[2] = '\0';
		for (op = &Optab[0]; op->term; op++)
			if (AAsequal(op->term, buf))
				break;
		if (!op->term)
		{
			rc_backup(toktyp);
			buf[1] = '\0';
			for (op = &Optab[0]; op->term; op++)
				if (AAsequal(op->term, buf))
					break;
			if (!op->term)
			{
				/* invalid operator */
				rc_error(13, buf);
				return (1);
			}
		}
	}
	Lcase = save;
	if(op->token == Tokens.bgncmnt)
		return (rc_comment());
	if(op->token == Tokens.sconst)
		return (rc_string(op));
	yylval = (char *) op->opcode;
# ifdef RC_TRACE
	if (TR_LEX)
		printf("OPERATOR: op=%s\n", op->term);
# endif
	return (op->token);
}
