/*
** ---  RC_NUM.C -------- NUMBER SCANNER  -------------------------------------
**
**	Scans numerical constants (both integer and floating).  Each
**	constant is converted from ascii to its numerical representation
**	and is entered into the symbol table, indexed by 'yylval'.
**	A token is returned for the number type.
**
**	Due to the current atof in the utility library, floating overflow
**	is not checked.
**
**	Version: @(#)rc_num.c		4.0	02/05/89
**
*/

# include	"rc.h"


rc_number(chr)
register int	chr;
{
	register char	*ptr;
	register int	lsave;
	register int	toktyp;
	double		ftemp;
	long		ltemp;
	char		buf[MAX_FIELD];
	extern char	*rc_syment();
	extern char	*yylval;

	lsave = Lcase;
	Lcase = 0;
	ptr = buf;

	if ((*ptr = toktyp = chr) != '.')
	{
		do
		{
			/* get integer portion */
			if ((ptr - buf) >= MAX_FIELD)
			{
				/* buffer overflow */
				rc_error(14);
				break;
			}
			*++ptr = toktyp = rc_gtchar();
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
			if ((ptr - buf) >= MAX_FIELD)
			{
				/* buffer overflow */
				rc_error(14);
				break;
			}
			*++ptr = toktyp = rc_gtchar();
		} while (CMAP(toktyp) == NUMBR);
		if (toktyp != 'e' && toktyp != 'E')
		{
			rc_backup(toktyp);
			*ptr = 0;
			goto convr;
		}

	  case 'e':
	  case 'E':
		if ((ptr - buf) >= MAX_FIELD)
		{
			/* buffer overflow */
			rc_error(14);
			goto convb;
		}
		*++ptr = toktyp = rc_gtchar();
		if (CMAP(toktyp) == NUMBR || toktyp == '-' || toktyp == '+')
		{
			do
			{
				/* get exponent */
				if ((ptr - buf) >= MAX_FIELD)
				{
					/* buffer overflow */
					rc_error(14);
					break;
				}
				*++ptr = toktyp = rc_gtchar();
			} while (CMAP(toktyp) == NUMBR);
		}
		rc_backup(toktyp);
		*ptr = 0;
convr:
		if (AA_atof(buf, &ftemp))
		{
			rc_error(15, buf);	/* floating conversion error */
convb:
			ftemp = 0;
		}
		yylval = rc_syment(&ftemp, sizeof (double));
		toktyp = Tokens.rconst;
# ifdef RC_TRACE
		if (TR_LEX)
			printf("NUMBER: rconst=%10.3f\n", ftemp);
# endif
		break;

	  default:
		/* integer */
		rc_backup(*ptr);
		*ptr = 0;
		if (AA_atol(buf, &ltemp))
		{
			rc_error(16, buf);	/* integer conversion error */
			ltemp = 0;
		}
		if (ltemp < MIN_I2 || ltemp > MAX_I2)
		{
			yylval = rc_syment(&ltemp, sizeof (long));
			toktyp = Tokens.lconst;
# ifdef RC_TRACE
			if (TR_LEX)
				printf("NUMBER: lconst=%ld\n", ltemp);
# endif
		}
		else
		{
			yylval = (char *) ltemp;
			toktyp = Tokens.iconst;
# ifdef RC_TRACE
			if (TR_LEX)
				printf("NUMBER: iconst=%ld\n", ltemp);
# endif
		}
		break;
	}
	Lcase = lsave;
	return (toktyp);
}
