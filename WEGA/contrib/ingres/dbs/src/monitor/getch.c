# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"monitor.h"

/*
**  GET CHARACTER
**
**	This routine is just a getchar, except it allows a single
**	backup character ("Peekch").  If Peekch is nonzero it is
**	returned, otherwise a getchar is returned.
*/
getch()
{
	register int	c;

	if (c = Peekch)
		Peekch = 0;
	else
		c = getc(Input);
	if (c < 0)
		c = 0;

	/* deliver EOF if newline in Oneline mode */
	if (c == '\n' && Oneline)
	{
		Peekch = c;
		c = 0;
	}

#	ifdef xMTR1
	AAtTfp(12, 0, "GETCH: %c (0%o)\n", c, c);
#	endif
	return (c);
}


/*
**  GETCHAR -- get character from standard input
**
**	This routine is redefined to do unbuffered reads.
**
**	Returns:
**		next character from standard input.
**		EOF if at end of file
*/
fgetchar()
{
	static char	x;

	if (read(0, &x, sizeof (char)) <= 0)
		return (EOF);
	return (x);
}
