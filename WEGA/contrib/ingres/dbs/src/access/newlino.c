# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"

/*
** AAnewlino finds a free line number which it returns and adjusts
**	the next line counter (Nxtlino) by the length of the tuple (len).
**	This routine is used to recover unused sections of the
**	line number table (AAcc_head->linetab).
*/
AAnewlino(len)
register int	len;
{
	register short	*lp;
	register short	newlno;
	register short	nextlno;

	nextlno = AAcc_head->nxtlino;
	lp = &AAcc_head->linetab[LINETAB(0)];
	for (newlno = 0; newlno < nextlno; newlno++)
	{
		if (!*lp)
		{
			/* found a free line number */
			*lp = AAcc_head->linetab[LINETAB(nextlno)];
			AAcc_head->linetab[LINETAB(nextlno)] += len;
			return (newlno);
		}
		lp--;
	}

	/* no free line numbers. use nxtlino */
	AAcc_head->linetab[LINETAB(nextlno + 1)] = *lp + len;
	AAcc_head->nxtlino++;
	return (nextlno);
}
