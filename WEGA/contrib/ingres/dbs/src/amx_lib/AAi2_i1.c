# include	"AA_liba.h"

AAi2_i1(s, d)
register char	*s;
register char	*d;
{
	short	is;

	AAbmove(s, &is, sizeof is);
	*d = is;
}
