# include	"amx.h"

/*
**	A string is defined as any sequence of MAXFIELD or fewer characters,
**	surrounded by string delimiters.  New-line ;characters are purged
**	from strings unless preceeded by a '\'; QUOTE's must be similarly
**	prefixed in order to be correctly inserted within a string.  Each
**	string is entered in the symbol table, indexed by 'yylval'.  A
**	token or the error condition -1 is returned.
*/
string(op)
struct optab	*op;
{
	register int		esc;
	register int		toktyp;
	register char		*ptr;
	register int		save;
	char			buf[MAXFIELD + 1];
	extern char		*yylval;
	extern char		*syment();

	/* disable case conversion and fill in string */
	ptr = buf;
	save = Lcase;
	Lcase = 0;
	do
	{
		/* get next character */
		if (!(*ptr = gtchar()))
			goto non_term;

		/* handle escape characters */
		esc = (*ptr == '\\');
		if (*ptr == '\n')
		{
			if (!(*ptr = gtchar()))
				goto non_term;
		}
		if (esc == 1)
		{
			if (!(*++ptr = gtchar()))
			{
non_term:
				/* non terminated string */
				*ptr = 0;
				amxerror(93);
				break;
			}
			if (*ptr == *(op->term))
				*--ptr = *(op->term);
		}

		/* check length */
		if ((ptr - buf) > MAXFIELD - 1)
		{
			/* string too long */
			amxerror(92);
			break;
		}
		toktyp = *ptr;
		if (CMAP(toktyp) == CNTRL)
			*ptr = ' ';
	} while (*ptr++ != *(op->term) || esc == 1);

	/* restore case conversion and return */
	*--ptr = '\0';
	Lcase = save;
	yylval = syment(buf, (ptr - buf) + 1);
	return (Tokens.sconst);
}
