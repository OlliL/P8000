# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"

/*
**  MCALL -- call a macro
**
**	This takes care of springing a macro and processing it for
**	any side effects.  Replacement text is saved away in a static
**	buffer and returned.
**
**	Parameters:
**		mac -- the macro to spring.
**
**	Returns:
**		replacement text.
*/
char	*mcall(mac)
char	*mac;
{
	static char		buf[FILENAME + 1];
	register int		c;
	register char		*p;
	extern int		macsget();

#	ifdef xMTR2
	AAtTfp(18, -1, "MCALL: `%s'\n", mac);
#	endif

	/* set up to process the macro */
	macinit(macsget, &mac, FALSE);

	/* process it -- throw away result */
	for (p = buf; (c = macgetch()) > 0; )
	{
#		ifdef xMTR2
		if (AAtTf(18, 1))
			putchar(c);
#		endif
		if (p < &buf[sizeof buf])
			*p++ = c;
	}
	*p = 0;

#	ifdef xMTR2
	if (AAtTf(18, 1))
		putchar('\n');
#	endif

	return (buf);
}
