# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/symbol.h"
# include	"../h/access.h"
# include	"../h/lock.h"

# define	SEARCHFUDGE	6	/* # of linenos to do a binary search */
/*
**  NDXSEARCH -- search an ISAM directory
**
**	Looks for an available page, if this would force out a page from
**	the relation it adds its own.  Searches each level of the dirctory
**	for the lowest (highest) page on which the key could occur if mode
**	is < (>) 0.  At each level the number of keys on the page is checked
**	if it is <= SEARCHFUDGE a linear sharch is done, otherwize a binary
**	search is preformed.  If the full key matches exactly the seach
**	can stop.  Note that the keys tell the minimum value on that page.
**
**	Parameters:
**		d	- pointer to descriptor of open relation
**		tid	- returns tid of page to start (end) looking
**		key	- to look for
**		mode	- < 0 lowkey, > 0 hikey
**		keyok	- true if all keys are assumed to be set
**
**	Returns:
**		-2	- AApageflush failure
**		-1	- AAgetpage failure
**		0	- success
*/
AAndxsearch(d, tid, tuple, mode, keyok)
DESC			*d;
register struct tup_id	*tid;
char			tuple[];
int			mode;
int			keyok;
{
	register int			i;
	register int			bottom;
	register int			top;
	register int			j;
	register int			keylen;
	register int			dno;
	register int			partialkey;
	long				pageid;
	char				*p;
	int				pagerr;
	struct accessparam		relparm;
	extern char			*AAgetaddr();

	pagerr = 0;
	AAdparam(d, &relparm);	/* get domains in key order */

	/* assume fullkey is given */
	partialkey = FALSE;

	/* remove any key domains not given by the user */
	if (!keyok)
		for (i = 0; dno = relparm.keydno[i]; i++)
		{
			if (d->relgiven[dno])
				continue;
			relparm.keydno[i] = 0;
			partialkey = TRUE;
			break;
		}

	/* if the first key isn't given, just return else search directory */
	if (relparm.keydno[0])
	{
		/* The actual ISAM directory search must be performed */
		pageid = d->reldum.relprim;
		AAstuff_page(tid, &pageid);

		AAcclock = FALSE;

		do
		{
			if (pagerr = AAgetpage(d, tid))
				break;	/* fatal error */
			AAcc_head->bufstatus |= BUF_DIRECT;  /* don't get confused */
			bottom = 0;
			top = AAcc_head->nxtlino - 1;
			if (top >= SEARCHFUDGE)  /* we are past breakeven */
				while (top != bottom)	/* binary search */
				{
					tid->line_id = ((1 + top - bottom) >> 1) + bottom;	/* get to half way point */
					p = AAgetaddr(tid);
					for (j = 0; dno = relparm.keydno[j]; j++)
					{
						keylen = ctou(d->relfrml[dno]);
						if (i = AAicompare(&tuple[d->reloff[dno]], p, d->relfrmt[dno], keylen))
							break;
						p += keylen;
					}
					/* if key is below directory, then we are in the bottom half */
					if (i < 0 || (!i && partialkey && mode < 0))
						top = ctou(tid->line_id) - 1;

					else if (!i && !partialkey)
					{
						bottom = ctou(tid->line_id);
						break;
					}
					else
						bottom = ctou(tid->line_id);
				}

			else	/* do a linar search */
				for (tid->line_id = 0; ctou(tid->line_id) < AAcc_head->nxtlino; tid->line_id++)
				{
					p = AAgetaddr(tid);
					for (i = 0; dno = relparm.keydno[i]; i++)
					{
						keylen = ctou(d->relfrml[dno]);
						if (j = AAicompare(&tuple[d->reloff[dno]], p, d->relfrmt[dno], keylen))
							break;
						p += keylen;
					}
					/* if key is below directory, then we have passed the position */
					if (j < 0)
						break;

					/* if lowkey search on partial key matches, then done */
					if (!j && partialkey && mode < 0)
						break;
					bottom = ctou(tid->line_id);
					if (!j && mode < 0)
						break;
				}
			pageid = AAcc_head->ovflopg + bottom;
			AAstuff_page(tid, &pageid);
		} while (AAcc_head->mainpg);  /* if at level zero then done */
		AAcclock = TRUE;

	}
	tid->line_id = SCANTID;
	return (pagerr);
}
