# include	"AA_liba.h"

AAf8_i1(s, d)
register char	*s;
register char	*d;
{
	double	is;

	AAbmove(s, &is, sizeof is);
	*d = is;
}
