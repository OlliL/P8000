# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/scanner.h"

/* TWO CHARACTER STACK FOR 'UNGETC' BACKUP */
char	Pchar[2];
int	Pctr;

/*
** GTCHAR
** If 'Lcase' is set, all upper-case alphabetics are
** mapped into lower-case.
** The correct return is > 0; a return = 0 indicates end-of-file.
*/
gtchar()
{
	extern int	yyline;
	register int	chr;

	/* USE STACKED CHARACTERS IF POSSIBLE */
	if (Pctr)
		chr = Pchar[--Pctr];
	else
		chr = getscr(0);

	/* UPDATE LINE COUNTER */
	if (chr == '\n')
		yyline++;

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
** BACKUP
** saves the character argument in the global stack 'Pchar'
**/
backup(chr)
char	chr;
{
	extern int	yyline;

	if (Pctr == 2)
		AAsyserr(20002, yyline, Pchar[0], Pchar[1], chr);
	Pchar[Pctr++] = chr;
	if (chr == '\n')
		yyline--;
}
