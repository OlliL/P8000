# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"constants.h"
# include	"globals.h"

/*
**  NUMBER -- process a numeric token
**	Number gets a number as, either floating or integer,
**	with the QL format, from inside a QL statement,
**	and adds it to the symbol space.
**
**	Parameters:
**		chr -- the first character of the number
**
**	Returns:
**		The lexical code for the appropriate
**		type of number.
**
**	Side Effects:
**		Adds a token to the symbols space.
**		yylval is set to the node added.
*/
number(chr)
register int	chr;
{
	register char	*ptr;
	register int	toktyp;
	double		ftemp;
	long		ltemp;
	char		buf[MAXFIELD + 1];
	extern char	*yylval;
	extern char	*addsym();
	extern char	*salloc();

	ptr = buf;
	if ((*ptr = toktyp = chr) != '.')
	{
		do
		{
			/* get integer portion */
			if ((ptr - buf) > MAXFIELD)
			{
				/* buffer overflow
				 * return integer 0,
				 * and signal error.
				 */
bufovflo :
				*ptr = '\0';
				yysemerr("numeric too long", buf);
				yylval = addsym("0");
				return (Tokens.sp_i2const);
			}

			*++ptr = toktyp = getch();
		} while (CMAP(toktyp) == NUMBR);
	}

	/* do rest of type determination */
	switch (toktyp)
	{
	  case '.':
		/* floating point */
		do
		{
			/* fill into ptr with up to next non-digit */
			if ((ptr - buf) > MAXFIELD)
				goto bufovflo;
			*++ptr = toktyp = getch();
		} while (CMAP(toktyp) == NUMBR);
		if (toktyp != 'e' && toktyp != 'E')
		{
			backup(toktyp);
			*ptr = '\0';
			goto convr;
		}

	  case 'e':
	  case 'E':
		if ((ptr - buf) > MAXFIELD)
			goto bufovflo;
		*++ptr = toktyp = getch();
		if (CMAP(toktyp) == NUMBR || toktyp == '-' || toktyp == '+')
		{
			do
			{
				/* get exponent */
				if ((ptr - buf) > MAXFIELD)
					goto bufovflo;
				*++ptr = toktyp = getch();
			} while (CMAP(toktyp) == NUMBR);
		}
		backup(toktyp);
		*ptr = '\0';
convr:
		if (AA_atof(buf, &ftemp))
		{
			/* floating conversion error */
			yysemerr("numeric ofverflow", buf);
			yylval = addsym("0");
			return (Tokens.sp_f8const);
		}
		yylval = addsym(salloc(buf));
		toktyp = Tokens.sp_f8const;
		break;

	  default:
		/* integer */
		backup(toktyp);
		*ptr = '\0';

		/* long conversion error */
		if (AA_atol(buf, &ltemp) || ltemp > 32767 || ltemp < -32768)
			goto convr;
		yylval = addsym(salloc(buf));
		toktyp = Tokens.sp_i2const;
		break;
	}

	return (toktyp);
}
