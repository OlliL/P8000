# include	<stdio.h>
# include	"gen.h"
# include	"constants.h"
# include	"globals.h"

int	yyline;

/*
**  YYLEX -- Lexical analyzer
**	Yylex controls the return to the parser of lexemes,
**	and the copying out of C_CODE on the all to yylex
**	after yylex() returned the C_CODE.
**
**	Returns:
**		Lexical tokens.
**
**	Side Effects:
**		Copies C code out on call after seeing it.
**		Puts terminals in symbol space.
*/
yylex()
{
	register int		rtval;
	register int		chr;
	extern char		*yysterm[];

	/* get next token */
	rtval = CONTINUE;

	while (rtval == CONTINUE)
	{
		if (C_code_flag)
		{
			copy_c_code();
			Newline = 1;
		}
		Pre_proc_flag = 0;

		/* END OF FILE ? */
		if ((chr = getch()) == EOF_TOK)
		{
#			ifdef xDEBUG
			if (Lex_debug)
				printf("end of file\n");
#			endif
			return (0);
		}

		/* Test for a line of C code */
		if (Newline && if_c_code(chr))
		{
			if (C_code_flag)
				continue;
			rtval = Tokens.sp_c_code;
			C_code_flag = 1;
			break;
		}
		else
		{
			C_code_flag = 0;
			if (Newline)
			{
				Newline = 0;
				continue;
			}
		}

		/* CARRIAGE CONTROL ? */
		Newline = chr == '\n';

		switch (CMAP(chr))
		{

		  case PUNCT :
			continue;

		  case OPATR :
			rtval = operator(chr);
			break;

		  case NUMBR :
			rtval = number(chr);
			break;

		  case ALPHA :
			rtval = name(chr);
			break;

		}
	}
	if (Lex_debug)
		printf("YYLEX : %s: '%s'\n", yysterm[rtval - 256],
		yylval? ((struct disp_node *) yylval)->d_elm: "");
	return (rtval);
}


/*
**  COPY_C_CODE -- Copies out a line of C code
**	The test for Charcnt != 0 is beacuse if a C pre-processor
**	line follows an EQL line, and equate_lines() puts out no
**	newline, the initial '#' will not be on the beginning of
**	the line. As is, if this should happen, then the line
**	with the '#' will be one line in the output ahead of
**	where it should be.
*/
copy_c_code()
{
	register int	c;
	char		ch[2];

	ch[1] = 0;
	equate_lines();
	if (Pre_proc_flag)
	{
		if (Charcnt)
			w_raw("\n");
		w_raw("#");
	}
	do
	{
		if ((c = getch()) == EOF_TOK)
			return;
		*ch = c;
		w_raw(ch);
	} while (*ch != '\n');
}


/*
**  IF_C_CODE -- Test to see if a line is C code
**
**	Sees if a line begins with "##" to see if it is EQL.
**
**	Parameters:
**		chr -- first char of line
**
**	Returns:
**		0 -- QL line
**		1 -- C line
*/
if_c_code(chr)
register int	chr;
{
	for ( ; ; )
	{
		if (chr != '#')
		{
			backup(chr);
			return (1);
		}
		Pre_proc_flag = 1;
		if ((chr = getch()) == EOF_TOK)
		{
			return (0);
		}
		if (chr != '#')
		{
			backup(chr);
			return (1);
		}
		else
		{
			return (0);
		}
	}
}
