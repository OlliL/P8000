# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/symbol.h"
# include	"../h/access.h"
# include	"../h/lock.h"

/*
**	Scan for duplicates. Start with the page
**	specified by tid and
**	continue until there are no more pages
**
**	AAscan_dups guarantees that the same page will
**	be in the buffer on entry and exit
**
**	returns:
**		<0 if fatal error
**		0  if not duplicate
**		1  if duplicate
*/
AAscan_dups(d, tid, tuple)
register DESC			*d;
register struct tup_id		*tid;
char				*tuple;
{
	register int		dup;
	register int		pagerr;
	struct tup_id		savetid;

	AAstuff_page(&savetid, &AAcc_head->thispage);

	AAcclock = FALSE;	/* turn concurrency off */
	dup = 0;	/* assume success */

	do
	{
		if (pagerr = AAgetpage(d, tid))
			break;	/* fatal error */

		if (dup = AAdup_check(d, tid, tuple))
			break;	/* found duplicate */

		AAstuff_page(tid, &AAcc_head->ovflopg);
	} while (AAcc_head->ovflopg);

	if (pagerr || (pagerr = AAgetpage(d, &savetid)))
		dup = pagerr;	/* return fatal page error */

	AAcclock = TRUE;		/* turn concurency on */
	return (dup);
}


/*
**	Check current page for a duplicate of tuple;
**
**	returns:
**		0 if not duplicate
**		1 if duplicate
*/
AAdup_check(d, tid, tuple1)
register DESC		*d;
struct tup_id		*tid;
char			*tuple1;
{
	register int			i;
	register short			*lp;
	int				tups_equal;
	register int			dom;
	register int			len;
	register int			lastline;
	char				*tup1;
	char				*tup2;
	char				tuple2[MAXTUP];
	extern char			*AAgetint_tuple();

	/* determine starting and ending points */
	lastline = AAcc_head->nxtlino;
	lp = &AAcc_head->linetab[LINETAB(0)];

	/* check each tuple for duplication */
	for (i = 0; i < lastline; i++)
	{
		/* is this line used? */
		if (*lp--)
		{
			/* yes. get tuple and check it */
			tid->line_id = i;
			tup2 = AAgetint_tuple(d, tid, tuple2);
			tup1 = tuple1;
			tups_equal = TRUE;	/* assume equal */

			/* now check each domain for duplication */
			for (dom = 1; dom <= d->reldum.relatts; dom++)
			{
				len = ctou(d->relfrml[dom]);
				if (d->relfrmt[dom] == CHAR)
					tups_equal = !AAscompare(tup1, len, tup2, len);
				else
					tups_equal = AAbequal(tup1, tup2, len);
				if (!tups_equal)	/* not duplicates */
					break;
				tup1 += len;
				tup2 += len;
			}
			if (tups_equal)
				return (1);	/* duplicate */
		}
	}
	return (0);	/* no duplicate */
}
