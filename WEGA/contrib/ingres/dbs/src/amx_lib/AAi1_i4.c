# include	"AA_liba.h"

AAi1_i4(s, d)
register char	*s;
register char	*d;
{
	long	id;

	id = ctoi(*s);
	AAbmove(&id, d, sizeof id);
}
