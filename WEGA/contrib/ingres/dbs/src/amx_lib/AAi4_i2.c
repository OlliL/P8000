# include	"AA_liba.h"

AAi4_i2(s, d)
register char	*s;
register char	*d;
{
	long	is;
	short	id;

	AAbmove(s, &is, sizeof is);
	id = is;
	AAbmove(&id, d, sizeof id);
}
