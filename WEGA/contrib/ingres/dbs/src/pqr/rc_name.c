/*
** ---  RC_NAME.C ------- NAME SCANNER  ---------------------------------------
**
**	A name is defined to be a sequence of MAX_NAME or fewer alphanumeric
**	characters, starting with an alphabetic (underscore "_" is considered
**	an alphabetic).  If it is not a keyword, each name is entered into
**	the symbol table, indexed by 'yylval'.  A token is then returned for
**	that name.
**
**	Version: @(#)rc_name.c		4.0	02/05/89
**
*/

# include	"rc.h"


rc_name(chr)
int	chr;
{
	register int		hi;
	register int		lo;
	register int		curr;
	register struct macro	*mac;
	char			namebuf[MAX_NAME + 1];
	extern char		*yylval;
	extern char		*rc_syment();

	/* fill in the name */
	yylval = namebuf;
	*yylval = chr;
	do
	{
		*++yylval = curr = rc_gtchar();
		if ((yylval - namebuf) > MAX_NAME)
		{
			/* name too long */
			*yylval = '\0';
			rc_error(17, namebuf);
			break;
		}

	} while (CMAP(curr) == ALPHA || CMAP(curr) == NUMBR);
	rc_backup(curr);
	*yylval = '\0';

	/* is it a keyword ? */
	lo = 0;
	hi = Keyent - 1;
	while (lo <= hi)
	{
		curr = (lo + hi) / 2;
		switch (AAscompare(Keyword[curr].term, MAX_NAME, namebuf, MAX_NAME))
		{
		  case 1:
			hi = curr - 1;
			continue;

		  case -1:
			lo = curr + 1;
			continue;

		  case 0:
			yylval = (char *) Keyword[curr].opcode;
# ifdef RC_TRACE
			if (TR_LEX)
				printf("NAME: keyword %s\n", namebuf);
# endif
			return (Keyword[curr].token);
		}
	}

	/* lookup MACRO's */
	for (curr = 0, mac = Macro; curr < Macro_no; curr++, mac++)
	{
		if (AAsequal(mac->templt, namebuf))
		{
# ifdef RC_TRACE
			if (TR_LEX)
				printf("MACRO: %s=%d\n", namebuf, mac->subst);
# endif
			yylval = (char *) mac->subst;
			return (Tokens.iconst);
		}
	}

	/* else, USER DEFINED NAME */
	yylval = rc_syment(namebuf, AAlength(namebuf) + 1);
# ifdef RC_TRACE
	if (TR_LEX)
		printf("NAME: %s\n", yylval);
# endif
	return (Tokens.name);
}
