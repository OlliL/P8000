# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"

/*
**	AAtup_len finds the amount of space occupied by the
**	tuple specified by "tid"
*/
AAtup_len(tid)
struct tup_id	*tid;
{
	register short	*lp;
	register short	nextoff;
	register short	off;
	register short	lineoff;
	register int	i;

	/* point to line number table */
	lp = &AAcc_head->linetab[LINETAB(0)];

	/* find offset for tid */
	i = ctou(tid->line_id);
	lineoff = lp[-i];

	/* assume next line number follows lineoff */
	i = AAcc_head->nxtlino;
	nextoff = lp[-i];

	/* look for the line offset following lineoff */
	for (i = 0; i < AAcc_head->nxtlino; i++)
	{
		off = *lp--;

		if (off <= lineoff)
			continue;

		if (off < nextoff)
			nextoff = off;
	}

#	ifdef xATR3
	AAtTfp(89, 1, "TUP_LEN: ret %d\n", nextoff - lineoff);
#	endif

	return (nextoff - lineoff);
}
