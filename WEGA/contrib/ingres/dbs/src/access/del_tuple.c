# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"

/*
** Delete the specified tuple from the
** current page.
** The space occupied by the tuple is
** compacted and the effected remaining
** tuple line offsets are adjusted.
*/
AAdel_tuple(tid, width)
struct tup_id	*tid;
int		width;
{
	register char	*startpt;
	register char	*midpt;
	register int	i;
	char		*endpt;
	register int	cnt;
	register int	offset;
	register int	nextline;
	register int	linenum;
	extern char	*AAgetaddr();

	linenum = ctou(tid->line_id);
	offset = AAcc_head->linetab[LINETAB(linenum)];
	nextline = AAcc_head->nxtlino;

	startpt = AAgetaddr(tid);
	midpt = startpt + width;
	endpt = (char *) AAcc_head + AAcc_head->linetab[LINETAB(nextline)];

	cnt = endpt - midpt;

	/* delete tuple */
	AAcc_head->linetab[LINETAB(linenum)] = 0;

	/* update affected line numbers */
	for (i = 0; i <= nextline; i++)
		if (AAcc_head->linetab[LINETAB(i)] > offset)
			AAcc_head->linetab[LINETAB(i)] -= width;

	/* compact the space */
	while (cnt--)
		*startpt++ = *midpt++;

	AAcc_head->bufstatus |= BUF_DIRTY;
}
