# include	"AA_liba.h"

AAi2_f8(s, d)
register char	*s;
register char	*d;
{
	short	is;
	double	id;

	AAbmove(s, &is, sizeof is);
	id = is;
	AAbmove(&id, d, sizeof id);
}
