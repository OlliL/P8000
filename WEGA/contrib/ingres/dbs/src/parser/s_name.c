# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/scanner.h"
# include	"parser.h"

/*
** NAME
** A name is defined to be a sequence of MAXNAME or fewer alphanumeric
** characters, starting with an alphabetic (underscore "_" is considered
** an alphabetic).  If it is not a keyword, each name is entered into
** the symbol table, indexed by 'yylval'.  A token is then returned for
** that name.
*/
name(chr)
char	chr;
{
	extern char		*yylval;
	char			namebuf[MAXNAME + 1];
	register int		hi;
	register int		lo;
	register int		curr;
	extern char		*syment();

	/* fill in the name */
	yylval = namebuf;
	*yylval = chr;
	do
	{
		*++yylval = curr = gtchar();
		if ((yylval - namebuf) > MAXNAME)
		{
			/* name too long */
			*yylval = '\0';
			yyerror(NAMELONG, namebuf, (char *) 0);
		}

	} while (CMAP(curr) == ALPHA || CMAP(curr) == NUMBR);
	backup(curr);
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
			Lastok.toktyp = Tokens.sconst;
			Lastok.tok = Keyword[curr].term;
			Lastok.tokop = Keyword[curr].opcode;
			yylval = CAST(Keyword[curr].opcode);
#			ifdef	xPTR2
			AAtTfp(71, 0, "keyword: %s\n", namebuf);
#			endif
			return (Keyword[curr].token);
		}
	}

	/* else, USER DEFINED NAME */
#	ifdef	xPTR2
	AAtTfp(71, 0, "name: %s\n", namebuf);
#	endif
	yylval = syment(namebuf, AAlength(namebuf) + 1);
	Lastok.tok = yylval;
	Lastok.toktyp = Tokens.sconst;
	Lastok.tokop = 0;
	return (Tokens.name);
}
