# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"monitor.h"

/*
**  GET FILE NAME
**
**	This routine collects a file name up to a newline and returns a
**	pointer to it.  Keep in mind that it is stored in a static
**	buffer.
*/
char	*getfilename()
{
	static char	filename[FILENAME];
	register int	c;
	register int	i;
	register char	*p;
	extern int	getch();

	Oneline = TRUE;
# ifdef MACRO
	macinit(getch, (char **) 0, FALSE);
# endif

	/* skip initial spaces */
# ifdef MACRO
	while ((c = macgetch()) == ' ' || c == '\t')
		continue;
# else
	while ((c = getch()) == ' ' || c == '\t')
		continue;
# endif

	for (i = 0, p = filename; c > 0; )
	{
		if (i++ <= FILENAME)
			*p++ = c;
# ifdef MACRO
		c = macgetch();
# else
		c = getch();
# endif
	}
	*p = 0;

#	ifdef xMTR2
	AAtTfp(17, 0, "GETFILENAME: `%s'\n", filename);
#	endif

	Prompt = Newline = TRUE;
	Oneline = FALSE;
	Peekch = 0;
	return (filename);
}
