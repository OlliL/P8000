# include	"AA_liba.h"

AAf8_i4(s, d)
register char	*s;
register char	*d;
{
	double	is;
	long	id;

	AAbmove(s, &is, sizeof is);
	id = is;
	AAbmove(&id, d, sizeof id);
}
