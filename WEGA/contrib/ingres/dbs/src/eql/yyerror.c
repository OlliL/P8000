# include	<stdio.h>
# include	"gen.h"
# include	"constants.h"
# include	"globals.h"

/*
**  YYERROR -- Error reporting routines
**
**	Defines:
**		yyerror()
**		yyserror()
**		yysemerr()
**
**
**
**  YYERROR -- Yacc error reporting routine.
**	Yyerror reports on syntax errors encountered by
**	the yacc parser.
**
**	Parameters:
**		s -- a string explaining the error
*/
yyerror(s)
register char	*s;
{
	extern char	*yysterm[];

	file_spec();
	if (!yychar)
		printf("EOF = ");
	else
		printf("%s = ", yysterm[yychar - 256]);

	if (yylval)
		printf("'%s' : ", ((struct disp_node *) yylval)->d_elm);

	printf("line %d, %s\n", yyline, s);
}


/*
**  YYSEMERR -- scanner error reporter
**
**	Parameters:
**		s -- string explaining the error
**		i -- if !0 a string which caused the error
*/
yysemerr(s, i)
register char	*s;
register char	*i;
{
	extern char	*yysterm[];

	file_spec();
	if (i)
		printf("'%s' : ", i);
	printf("line %d, %s\n", yyline, s);
}


/*
**  YYSERROR -- Semantic error reportin routine
**	reports on an error on an entry in the symbol space,
**	using the line number built into the entry.
**
**	Parameters:
**		s -- a string explaining the error
**		d -- a symbol space node
*/
yyserror(s, d)
register char			*s;
register struct disp_node	*d;
{
	file_spec();
	printf("'%s' : line %d, %s\n", d->d_elm, d->d_line, s);
}


/*
**  FILE_SPEC -- If in an included file, specify the name of that file.
*/
file_spec()
{
	if (Inc_file)
		printf("** %s : ", Input_file_name);
}
