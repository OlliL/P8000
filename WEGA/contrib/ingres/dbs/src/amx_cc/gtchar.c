# include	"amx.h"

/* ONE CHARACTER STACK FOR 'UNGETC' BACKUP */
static int	Pchar;

/*
**	GTCHAR
**		If 'Lcase' is set, all upper-case alphabetics are
**		mapped into lower-case.
**		The correct return is > 0; a return = 0 indicates end-of-file.
*/
gtchar()
{
	register int		chr;
	extern int		errno;

	errno = 0;
	/* USE STACKED CHARACTERS IF POSSIBLE */
	if (chr = Pchar)
		Pchar = 0;
	else if ((chr = getc(FILE_inp)) < 0)
		chr = 0;
# ifdef AMX_SC
	if (!chr && !errno  && FILE_exp)
	{
		fclose(FILE_inp);
		FILE_inp = FILE_exp;
		FILE_exp = (FILE *) 0;
		Line = Line_exp;
		return (gtchar());
	}
# endif

	/* UPDATE LINE COUNTER */
	if (chr == '\n')
		Line++;

	if (Lcase && ((chr >= 'A'
# ifdef EBCDIC
	   && chr <= 'I') || (chr >= 'J'
	   && chr <= 'R') || (chr >= 'S'
# endif
	   && chr <= 'Z')))
		chr += 'a' - 'A';

	return (chr);
}


/*
**	BACKUP
**		Saves the character argument in the global stack 'Pchar'
*/
backup(chr)
register int	chr;
{
	if (Pchar)
		yyexit(6);
	Pchar = chr;
	if (chr == '\n')
		Line--;
}
