# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"monitor.h"

/*
**  Clear Input Line
**
**	This routine removes the newline following a monitor command
**	(such as \t, \g,  etc.)  Any other characters are processed.
**	Hence, \t\g\t will work.  It also maintains the
**	Newline flag on command lines.  It will make certain that
**	the current line in the query buffer ends with a newline.
**
**	The flag 'noprompt' if will disable the prompt character if set.
**	Otherwise, it is automatically printed out.
*/
clrline(noprompt)
register int	noprompt;
{
	register int	c;

	if (!Newline)
		putc('\n', Qryiop);
	Newline = TRUE;

	/* if char following is a newline, throw it away */
	if (!(Prompt = (c = getch()) == '\n'))
		Peekch = c;
	else if (!noprompt)
		prompt((char *) 0);
}
