# include	"AA_liba.h"

AAi4_f8(s, d)
register char	*s;
register char	*d;
{
	long	is;
	double	id;

	AAbmove(s, &is, sizeof is);
	id = is;
	AAbmove(&id, d, sizeof id);
}
