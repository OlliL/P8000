# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/symbol.h"
# include	"../h/access.h"

/*
**	Clr_tuple initializes all character domains
**	to blank and all numeric domains to zero.
*/
clr_tuple(d, tuple)
register DESC		*d;
char			*tuple;
{
	register char			*tup;
	register int			i;
	register int			j;
	register int			pad;

	for (i = 1; i <= d->reldum.relatts; i++)
	{
		pad = (d->relfrmt[i] == CHAR)? ' ': 0;
		tup = &tuple[d->reloff[i]];
		j = ctou(d->relfrml[i]);

		while (j--)
			*tup++ = pad;
	}
}
