# include	"AA_liba.h"

# ifndef NO_F4
AAf4_i4(s, d)
register char	*s;
register char	*d;
{
	float	is;
	long	id;

	AAbmove(s, &is, sizeof is);
	id = is;
	AAbmove(&id, d, sizeof id);
}
# endif
