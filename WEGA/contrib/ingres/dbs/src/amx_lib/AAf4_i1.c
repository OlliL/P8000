# include	"AA_liba.h"

# ifndef NO_F4
AAf4_i1(s, d)
register char	*s;
register char	*d;
{
	float	is;

	AAbmove(s, &is, sizeof is);
	*d = is;
}
# endif
