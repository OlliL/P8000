# include	"AA_liba.h"

AAi4_i1(s, d)
register char	*s;
register char	*d;
{
	long	is;

	AAbmove(s, &is, sizeof is);
	*d = is;
}
