# include	"AA_liba.h"

AAi2_i4(s, d)
register char	*s;
register char	*d;
{
	short	is;
	long	id;

	AAbmove(s, &is, sizeof is);
	id = is;
	AAbmove(&id, d, sizeof id);
}
