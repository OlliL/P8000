# include	<stdio.h>
# include	"gen.h"
# include	"constants.h"
# include	"globals.h"

/*
**  STRING -- processes a string constant
**	Strings are kept internally exactly as their external
**	appearance, except for the outermost '"'.
**	A string may be at most MAXSTRING characters
**	long, and may have escaped newlines.
**
**	Parameters:
**		op -- pointer to string quote operator
**			table entry.
**
**	Returns:
**		SCONST
*/
string(op)
struct optab	*op;
{
	char		buf[MAXSTRING + 1];
	register int	c;
	register char	*cp;
	register int	escape;
	register int	error;
	extern char	*yylval;
	extern char	*addsym();
	extern char	*salloc();

	error = escape = 0;
	cp = buf;
	for ( ; ; )
	{
		switch (c = getch())
		{
		  case '\\' :
			if (!escape)
				escape = 2;
			goto regchar;

		  case '\n' :
			if (escape)
				goto regchar;
			*cp = '\0';
			yysemerr("non-terminated string", !error? buf :(char *) 0);
			break;

		  case EOF_TOK :
			backup(c);
			*cp = '\0';
			yysemerr("EOF in string", !error? buf :(char *) 0);
			break;

		  default :
regchar :
			if (c == *op->op_term && !escape)
			{
				/* end of string */
				*cp = '\0';
				break;
			}
			if (!error)
			{
				if (cp - buf < MAXSTRING)
				{
					if (CMAP(c) == CNTRL)
						yysemerr("control character in string eliminated", (char *) 0);
					else
						*cp++ = c;
				}
				else
				{
					yysemerr("string too long, rest discarded", (char *) 0);
					error = 1;
				}
			}
			if (escape)
				--escape;
			continue;
		}
		break;
	}
	yylval = addsym(salloc(buf));
	return (Tokens.sp_sconst);
}
