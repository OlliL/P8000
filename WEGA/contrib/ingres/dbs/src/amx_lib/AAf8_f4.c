# include	"AA_liba.h"

# ifndef NO_F4
AAf8_f4(s, d)
register char	*s;
register char	*d;
{
	double	is;
	float	id;

	AAbmove(s, &is, sizeof is);
	id = is;
	AAbmove(&id, d, sizeof id);
}
# endif
