# include	"gen.h"
# include	"../h/dbs.h"

/*
**  TRIM_RELNAME -- trim blanks from relation name for printing
**
**	A relation name (presumably in 'AAdbname' format: MAXNAME
**	characters long with no terminating null byte) has the
**	trailing blanks trimmed off of it into a local buffer, so
**	that it can be printed neatly.
**
**	Parameters:
**		name -- a pointer to the relation name
**
**	Returns:
**		a pointer to the trimmed relation name.
*/
char	*trim_relname(name)
register char	*name;
{
	static char	trimname[MAXNAME + 1];
	register char	*new;
	register int	i;

	new = trimname;
	i = MAXNAME;

	while (i--)
		if ((*new++ = *name++) == ' ')
		{
			new--;
			break;
		}

	*new = '\0';

	return (trimname);
}
