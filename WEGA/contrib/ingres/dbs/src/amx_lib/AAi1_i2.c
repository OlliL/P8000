# include	"AA_liba.h"

AAi1_i2(s, d)
register char	*s;
register char	*d;
{
	short	id;

	id = ctoi(*s);
	AAbmove(&id, d, sizeof id);
}
