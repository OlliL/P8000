# include	"AA_liba.h"

# ifndef NO_F4
AAf4_i2(s, d)
register char	*s;
register char	*d;
{
	float	is;
	short	id;

	AAbmove(s, &is, sizeof is);
	id = is;
	AAbmove(&id, d, sizeof id);
}
# endif
