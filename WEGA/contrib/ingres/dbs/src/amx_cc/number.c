# include	"amx.h"

/*
**	Scans numerical constants (both integer and floating).  Each
**	constant is converted from ascii to its numerical representation
**	and is entered into the symbol table, indexed by 'yylval'.
**	A token is returned for the number type.
**
**	Due to the current atof in the utility library, floating overflow
**	is not checked.
*/
number(chr)
register int	chr;
{
	register char		*ptr;
	register int		lsave;
	double			ftemp;
	long			ltemp;
	char			buf[MAXFIELD];
	extern char		*yylval;
	extern char		*syment();

	lsave = Lcase;
	Lcase = 0;
	ptr = buf;

	if ((*ptr = chr) != '.')
	{
		do
		{
			/* get integer portion */
			if ((ptr - buf) >= MAXFIELD)
			{
				/* number too long */
				amxerror(84);
				break;
			}
			*(++ptr) = chr = gtchar();
		} while (CMAP(chr) == NUMBR);
	}

	/* do rest of type determination */
	switch (chr)
	{
	  case '.':
		/* floating point */
		do
		{
			/* fill into ptr with up to next non-digit */
			if ((ptr - buf) >= MAXFIELD)
			{
				/* number too long */
				amxerror(84);
				break;
			}
			*(++ptr) = chr = gtchar();
		} while (CMAP(chr) == NUMBR);
		if (chr != 'e' && chr != 'E')
		{
			backup(chr);
			goto convr;
		}

	  case 'e':
	  case 'E':
		if ((ptr - buf) >= MAXFIELD)
		{
			/* number too long */
			amxerror(84);
			goto convb;
		}
		*(++ptr) = chr = gtchar();
		if (CMAP(chr) == NUMBR || chr == '-' || chr == '+')
		{
			do
			{
				/* get exponent */
				if ((ptr - buf) >= MAXFIELD)
				{
					/* number too long */
					amxerror(84);
					break;
				}
				*(++ptr) = chr = gtchar();
			} while (CMAP(chr) == NUMBR);
		}
		backup(chr);
convr:
		*ptr = 0;
		if (AA_atof(buf, &ftemp))
		{
			/* floating conversion error */
			amxerror(85, buf);
convb:
			ftemp = 0;
		}
		yylval = syment(buf, AAlength(buf) + 1);
		chr = Tokens.rconst;
		break;

	  default:
		/* integer */
		backup(chr);
		*ptr = 0;
		if (AA_atol(buf, &ltemp))
		{
			/* integer conversion error */
			amxerror(86, buf);
			ltemp = 0;
		}
		if (ltemp < -32768 || ltemp > 32767)
		{
			yylval = syment(&ltemp, sizeof (long) + 1);
			chr = Tokens.lconst;
		}
		else
		{
			yylval = (char *) ltemp;
			chr = Tokens.iconst;
		}
		break;
	}
	Lcase = lsave;
	return (chr);
}
