# include	"amx.h"

/*
**	A name is defined to be a sequence of MAXNAME or fewer alphanumeric
**	characters, starting with an alphabetic (underscore "_" is considered
**	an alphabetic).  If it is not a keyword, each name is entered into
**	the symbol table, indexed by 'yylval'.  A token is then returned for
**	that name.
*/
name(chr)
int	chr;
{
	register int		hi;
	register int		lo;
	register int		curr;
	char			namebuf[MAXNAME + 1];
	extern char		*yylval;
	extern char		*syment();

	/* fill in the name */
	yylval = namebuf;
	*yylval = chr;
	do
	{
		*++yylval = gtchar();
		if ((yylval - namebuf) > MAXNAME)
		{
			/* name too long */
			*yylval = '\0';
			amxerror(81, namebuf);
			break;
		}

		curr = *yylval;
		curr = CMAP(curr);
	} while (curr == ALPHA || curr == NUMBR);
	backup(*yylval);
	*yylval = '\0';

	/* is it a keyword ? */
	lo = 0;
	hi = Keyent - 1;
	while (lo <= hi)
	{
		curr = (lo + hi) / 2;
		switch (AAscompare(Keyword[curr].term, MAXNAME, namebuf, MAXNAME))
		{
		  case 1:
			hi = curr - 1;
			continue;

		  case -1:
			lo = curr + 1;
			continue;

		  case 0:
			yylval = (char *) Keyword[curr].opcode;
			return (Keyword[curr].token);
		}
	}

	/* else, USER DEFINED NAME */
	yylval = syment(namebuf, AAlength(namebuf) + 1);
	return (Tokens.name);
}
