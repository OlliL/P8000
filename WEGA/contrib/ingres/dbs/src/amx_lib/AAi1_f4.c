# include	"AA_liba.h"

# ifndef NO_F4
AAi1_f4(s, d)
register char	*s;
register char	*d;
{
	float	id;

	id = ctoi(*s);
	AAbmove(&id, d, sizeof id);
}
# endif
