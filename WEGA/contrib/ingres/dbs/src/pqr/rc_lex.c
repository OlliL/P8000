/*
** ---  RC_LEX.C -------- SCANNER  --------------------------------------------
**
**		Each call to rc_lex() returns a token for the next
**		syntactic unit.  If the object is of type  LCONST
**		RCONST or SCONST that object will also be entered
**		in the symbol table,  indexed by `yylval'. If the
**		object is not one of these types, `yylval' is the
**		opcode field of the  operator or  keyword tables.
**		The end_of_file `EOF' token is zero.
**
**	Version: @(#)rc_lex.c		4.0	02/05/89
**
*/

# include	"rc.h"

rc_lex()
{
	register int		chr;
	register int		rtval;

	rtval = -1;

	do	/* GET NEXT TOKEN */
	{
		if (!(chr = rc_gtchar()))
		{
# ifdef RC_TRACE
			if (TR_LEX)
				printf("YYLEX: EOF\n");
# endif
			rtval = 0;
			break;
		}
		switch (CMAP(chr))
		{
		  case ALPHA:
			rtval = rc_name(chr);
# ifdef RC_TRACE
			if (TR_LEX)
				printf("YYLEX: name ret %d\n", rtval);
# endif
			break;

		  case NUMBR:
			rtval = rc_number(chr);
# ifdef RC_TRACE
			if (TR_LEX)
				printf("YYLEX: number ret %d\n", rtval);
# endif
			break;

		  case OPATR:
			if (!(rtval = rc_operator(chr)))
				rtval = -1;
# ifdef RC_TRACE
			if (TR_LEX)
				printf("YYLEX: operator ret %d\n", rtval);
# endif
			break;

		  case CNTRL:
			printf("<\\0%o>", chr);
			continue;

		  case PUNCT:
			continue;
		}
	} while (rtval == -1);

# ifdef RC_TRACE
	if (TR_LEX)
		printf("YYLEX: ret %d\n", rtval);
# endif

	return (rtval);
}
