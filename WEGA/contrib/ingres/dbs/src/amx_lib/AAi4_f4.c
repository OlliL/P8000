# include	"AA_liba.h"

# ifndef NO_F4
AAi4_f4(s, d)
register char	*s;
register char	*d;
{
	long	is;
	float	id;

	AAbmove(s, &is, sizeof is);
	id = is;
	AAbmove(&id, d, sizeof id);
}
# endif
