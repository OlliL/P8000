# include	"amx.h"

static int	C_code_switch;

/*
**		Each call to lex() returns a token for the next
**		syntactic unit.  If the object is of type  LCONST
**		RCONST or SCONST that object will also be entered
**		in the symbol table,  indexed by `yylval'. If the
**		object is not one of these types, `yylval' is the
**		opcode field of the  operator or  keyword tables.
**		The end_of_file `EOF' token is zero.
*/
lex()
{
	register int		chr;
	register int		rtval;

	rtval = -1;

	do
	{
		/* GET NEXT TOKEN */
		while (!(chr = if_c_code()))
			copy_c_code();

		C_code_switch++;

		if (chr <= 0)
		{
			rtval = 0;
			break;
		}

		Newline = chr == '\n';

		switch (CMAP(chr))
		{
		  case ALPHA:
			rtval = name(chr);
			break;

		  case NUMBR:
			rtval = number(chr);
			break;

		  case OPATR:
			if (!(rtval = operator(chr)))
				rtval = -1;
			break;

		  case CNTRL:
		  case PUNCT:
			continue;
		}
	} while (rtval == -1);

	return (rtval);
}


/*
** this routine copies a sinle line of c code from input to output.
** if it finds an end of file it returns a Newline character,
** although this is a situation which theoretically should never occur.
*/
copy_c_code()
{
	register int		lcase;
	register int		chr;

	lcase = Lcase;
	Lcase = 0;

	while ((chr = gtchar()) > 0)
	{
		putc(chr, FILE_tmp);
		if (chr == '\n')
			break;
	}

	Lcase = lcase;
}


if_c_code()
{
	register int		chr;
	register int		pre;

new_line:
	if (!(chr = gtchar()))
		return (-1);
	if (!Newline)
		return (chr);

	if (chr != '#')
	{
		pre = 0;
		goto c_code;
	}

	if (!(chr = gtchar()))
		return (-1);
	else if (chr != '#')
	{
		pre = 1;
c_code:
		if (C_code_switch && chr != '\n')
		{
			C_code_switch = 0;
			fprintf(FILE_tmp,
# ifdef P8000
				"AA_CONTROL %3d \"%s\"\n",
# else
				"# line %4d \"%s\"\n",
# endif
				Line, File_src);
		}
		if (pre)
			putc('#', FILE_tmp);
		putc(chr, FILE_tmp);
		if (chr == '\n')
			goto new_line;
		return (0);
	}

	if (!(chr = gtchar()))
		return (-1);
	return (chr);
}
