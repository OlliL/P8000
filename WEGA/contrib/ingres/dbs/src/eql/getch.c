# include	<stdio.h>
# include	"gen.h"
# include	"constants.h"
# include	"globals.h"

char		*Line_pos;		/* position in input line	    */
char		Line_buf[MAXSTRING + 1];	/* buffer for input line    */

static int	Peekc[2] =	{-1,-1};

/*
**  GETCH.C -- I/O manipulation routines
**
**	Defines:
**		getch()
**		backup()
**		int Peekc[2]  -- 2 char stack, if[0]  != -1, the character to
**			be returned on the next call to getch().
**		char *Line_pos -- if != 0 *Line_pos is next
**			character that should be returned.
**		char Line_buf[MAXSTRING + 1] -- holds current
**			input line's text.
**
**
**
** initializes peek buffer to be empty
*/

/*
**  GETCH -- Get a character from the input stream
**
**	Returns:
**		the next character on the input stream, or
**		the backed up character if there is one.
**		EOF_TOK is returned on EOF.
**
**	Side Effects:
**		If a backed up character exists, unloads it.
**		*Line_pos is returned if Line_pos != 0
**		and no backup character exists. Line_buf
**		contains the line from which characters are being
**		returned. A call with Line_pos == 0 forces
**		reading of the next input line. Yyline is
**		incremented when a newline is returned.
**		If an EOF is in the middle of a line,
**		then a newline will be appended.
*/
getch()
{
	register char	*cp;
	register int	ch;
	static		eoflag;
	extern int	yyline;

	if (Peekc[0] >= 0)
	{
		/* have a backed up character */
		ch = Peekc[0];
		if (ch == '\n')
			yyline += 1;
		Peekc[0] = Peekc[1];
		Peekc[1] = -1;
	}
	else
	{
		for ( ; ; )
		{
			/* no lookahead character */
			if (!Line_pos)
			{
				if (eoflag)
				{
					eoflag = 0;

					/* try to restore previous file */
					if (!restoref())
						return (0);
#					ifdef xDEBUG
					if (Chardebug || Lex_debug)
						printf("include file - pop\n");
#					endif
				}
				for (cp = Line_buf; (*cp = getc(In_file)) != '\n'
						    && ctoi(*cp) != EOF; cp++)
				{
					if (cp - Line_buf > sizeof Line_buf - 1)
					{
						printf("WARNING : line %d too long, broken in two\n",
						yyline);
						break;
					}
				}
				if (ctoi(*cp) == EOF)
				{
					eoflag++;
					if (cp == Line_buf)
						/* EOF after '\n' */
						continue;
					/* EOF in middle of line */
					*cp = '\n';
				}
				Line_pos = Line_buf;

				/* test for a "#include" line */
				if (tst_include())
				{
					/* Force reloading Line_buf */
					Line_pos = 0;
					eoflag = 0;
#					ifdef xDEBUG
					if (Chardebug || Lex_debug)
						printf("include file - push\n");
#					endif
					continue;
				}
			}
			cp =  Line_pos;
			if (*cp == '\n')
			{
				Line_pos = 0;
				yyline += 1;
			}
			else
				Line_pos++;
			ch = *cp;
			break;
		}
	}
	ch = ctou(ch);

#	ifdef xDEBUG
	if (Chardebug)
		printf("getch - returning '%c'.\n", ch);
#	endif

	return (ch);
}


/*
**  BACKUP -- Back up a character on the input stream.
**	Backs up a single character into Peekc.
**
**	Parameters:
**		ch - character to back up
**
**	Side Effects:
**		pushes Peekc[0] to Peekc[1].
**		sets Peekc[0] to backed up character.
*/
backup(ch)
register int	ch;
{
	extern int	yyline;

#	ifdef xDEBUG
	if (Chardebug)
		printf("backed up : '%c'\n", ch);
#	endif

	if (Peekc[1] >= 0)
		AAsyserr(13002, yyline, Peekc[0], Peekc[1], ch);

	Peekc[1] = Peekc[0];
	if ((Peekc[0] = ctou(ch)) == '\n')
		--yyline;
}
