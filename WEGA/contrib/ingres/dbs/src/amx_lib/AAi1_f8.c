# include	"AA_liba.h"

AAi1_f8(s, d)
register char	*s;
register char	*d;
{
	double	id;

	id = ctoi(*s);
	AAbmove(&id, d, sizeof id);
}
