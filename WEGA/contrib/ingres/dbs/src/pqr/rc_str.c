/*
** ---  RC_STR.C -------- STRING CONSTANT SCANNER  ----------------------------
**
**	A string is defined as any sequence of MAX_FIELD or fewer characters,
**	surrounded by string delimiters.  New-line ;characters are purged
**	from strings unless preceeded by a '\'; QUOTE's must be similarly
**	prefixed in order to be correctly inserted within a string.  Each
**	string is entered in the symbol table, indexed by 'yylval'.  A
**	token or the error condition -1 is returned.
**
**	Version: @(#)rc_str.c		4.0	02/05/89
**
*/

# include	"rc.h"


rc_string(op)
struct optab	*op;
{
	register int	esc;
	register int	toktyp;
	register char	*ptr;
	register int	save;
	char		buf[MAX_FIELD + 1];
	extern char	*rc_syment();
	extern char	*yylval;

	/* disable case conversion and fill in string */
	ptr = buf;
	save = Lcase;
	Lcase = 0;
	do
	{
		/* get next character */
		if (!(*ptr = rc_gtchar()))
		{
			/* non term string */
			rc_error(11);
			break;
		}

		/* handle escape characters */
		esc = (*ptr == '\\');
		if (*ptr == '\n')
		{
			if (!(*ptr = rc_gtchar()))
			{
				*ptr = 0;
				/* non term string */
				rc_error(11);
				break;
			}
		}
		if (esc == 1)
		{
			if (!(*++ptr = rc_gtchar()))
			{
				*ptr = 0;
				/* non term string */
				rc_error(11);
				break;
			}
			if (*ptr == *(op->term))
				*--ptr = *(op->term);
		}

		/* check length */
		if ((ptr - buf) > MAX_FIELD - 1)
		{
			/* string too long */
			rc_error(12);
			break;
		}
		toktyp = *ptr;
		if (CMAP(toktyp) == CNTRL)
		{
			printf("<\\0%o>", *ptr);
			*ptr = ' ';
		}
	} while (*ptr++ != *(op->term) || esc == 1);

	/* restore case conversion and return */
	*--ptr = '\0';
	Lcase = save;
	yylval = rc_syment(buf, (ptr - buf) + 1);
# ifdef RC_TRACE
	if (TR_LEX)
		printf("STRING: %s\n", yylval);
# endif
	return (Tokens.sconst);
}
