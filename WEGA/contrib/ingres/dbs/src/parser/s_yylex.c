# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/scanner.h"
# include	"parser.h"

struct lastok	Lastok;
int		Pars;		/* flag for call getcvar or not	*/

/*
** YYLEX
** This is the control program for the scanner (lexical analyzer).
** Each call to yylex() returns a token for the next syntactic unit.
** If the object is of type I2CONST, I4CONST, F8CONST, SCONST or NAME, that
** object will also be entered in the symbol table, indexed by 'yylval'.
** If the object is not one of these types, yylval is the opcode field of
** the operator or keyword tables.
** The end-of-file token is zero.
*/
yylex()
{
	register int	chr;
	register int	rtval;

	rtval = -1;
	Lastok.tokop = 0;
	/* GET NEXT TOKEN */
	do
	{
		if((chr = gtchar()) <= 0)
		{
#			ifdef	xPTR2
			AAtTfp(72, 8, "end-of-file\n");
#			endif
			rtval = 0;
			break;
		}
		switch (CMAP(chr))
		{
		  case ALPHA:
			rtval = name(chr);
			break;

		  case NUMBR:
			rtval = number(chr);
			break;

		  case OPATR:
			if ((rtval = operator(chr)) == 0)
				rtval = -1;
			break;

		  case PUNCT:
			continue;

		  case CNTRL:
			/* already converted number ? */
			if (Pars)
				switch (chr)
				{
				  case CVAR_I2:
					rtval = getcvar(Tokens.i2const, sizeof (short));
					break;

				  case CVAR_I4:
					rtval = getcvar(Tokens.i4const, sizeof (long));
					break;

				  case CVAR_F8:
					rtval = getcvar(Tokens.f8const, sizeof (double));
					break;

				  case CVAR_S:
					rtval = getcvar(Tokens.sconst, 0);
					break;

				  default:
					printf("Zeichen 0%o ignoriert\n", chr);
					continue;
				}
			break;
		  default:
			AAsyserr(20003);
		}
	} while (rtval == -1);
	if (rtval == 0)
	{
		Lastok.tokop = GOVAL;
		Lastok.tok = 0;
		Lastok.toktyp = 0;
	}
	return (rtval);
}


getcvar(type, len)
register int	type;
register int	len;
{
	register int	chr;
	register int	save;
	char		buf[MAXSTRING + 1];
	extern char	*yylval;
	extern char	*syment();

	save = Lcase;
	Lcase = 0;
	yylval = buf;
	if (len)
	{
		while ((yylval - buf) < len)
			*yylval++ = chr = gtchar();
	}
	else
	{
		do
		{
			*yylval = chr = gtchar();
			if ((yylval - buf) > MAXSTRING)
			{
				Lcase = save;
				yyerror(STRLONG, (char *) 0);
			}
			if (CMAP(chr) == CNTRL && chr != '\0')
			{
				Lcase = save;
				/* control char in string from EQL */
				yyerror(CNTRLCHR, (char *) 0);
			}
		} while (*yylval++);
		len = yylval - buf;
	}
	Lcase = save;
	yylval = syment(buf, len);
	Lastok.tok = yylval;
	Lastok.toktyp = type;
	return (type);
}
