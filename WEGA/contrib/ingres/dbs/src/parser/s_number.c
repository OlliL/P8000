# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/scanner.h"
# include	"parser.h"

/*
** NUMBER
**	scans numerical constants (both integer and floating).  Each
**	constant is converted from ascii to its numerical representation
**	and is entered into the symbol table, indexed by 'yylval'.
**	A token is returned for the number type.
**
**	due to the current atof in the utility library, floating overflow
**	is not checked.
*/
number(chr)
char	chr;
{
	extern char	*yylval;
	extern char	*syment();
	double		ftemp;
	long		ltemp;
	short		itemp;
	char		buf[MAXFIELD];
	register char	*ptr;
	register int	toktyp;
	register int	save;

	save = Lcase;
	Lcase = 0;
	ptr = buf;
	if ((*ptr = toktyp = chr) != '.')
	{
		do
		{
			/* get integer portion */
			if ((ptr - buf) > MAXFIELD)
				/* buffer overflow */
				yyerror(NUMBUFOFLO, (char *) 0);
			*++ptr = toktyp = gtchar();
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
				yyerror(NUMBUFOFLO, (char *) 0);	/* buf oflo */
			*++ptr = toktyp = gtchar();
		} while (CMAP(toktyp) == NUMBR);
		if (toktyp != 'e' && toktyp != 'E')
		{
			backup(toktyp);
			*ptr = 0;
			goto convr;
		}

	  case 'e':
	  case 'E':
		if ((ptr - buf) > MAXFIELD)
			yyerror(NUMBUFOFLO, (char *) 0);	/* buf oflo */
		*++ptr = toktyp = gtchar();
		if (CMAP(toktyp) == NUMBR || toktyp == '-' || toktyp == '+')
		{
			do
			{
				/* get exponent */
				if ((ptr - buf) > MAXFIELD)
					yyerror(NUMBUFOFLO, (char *) 0);	/* buf oflo */
				*++ptr = toktyp = gtchar();
			} while (CMAP(toktyp) == NUMBR);
		}
		backup(toktyp);
		*ptr = 0;
	convr:
		if (AA_atof(buf, &ftemp))
			yyerror(FCONSTERR, buf, (char *) 0);	/* floating conversion error */
		yylval = syment(&ftemp, sizeof (double));
		Lastok.toktyp = Tokens.f8const;
		break;

	  default:
		/* integer */
		backup(toktyp);
		*ptr = 0;
		if (AA_atol(buf, &ltemp))	/* long conversion error */
			goto convr;
		if (ltemp > 32767)
		{
			yylval = syment(&ltemp, sizeof (long));
			Lastok.toktyp = Tokens.i4const;
			break;
		}
		itemp = ltemp;
		yylval = syment(&itemp, sizeof (short));
		Lastok.toktyp = Tokens.i2const;
		break;
	}
	Lcase = save;
	Lastok.tok = yylval;
	Lastok.tokop = 0;
	return (Lastok.toktyp);
}
