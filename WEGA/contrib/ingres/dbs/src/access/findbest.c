# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/access.h"

/*
**	AAfindbest - find the "best" place to put a tuple.
**	AAfindbest does not actually put the tuple but rather
**	returns and allocates the tid for the tuple.
**
**	The initial part of the algorithm depends on whether
**	the relation is a heap or not.
**
**	If the relation is a heap, if there is a current page
**	with room for the tuple, that page is used. Otherwise
**	the last page of the heap is considered.
**
**	If the relation is hash or isam, then "find" is used
**	to determine the primary page for the tuple.
**
**	If necessary, AAfindbest will allocate an overflow page
**	if there is not sufficient room for the tuple otherwise.
**
**	If checkdups is TRUE and the relation is not a heap,
**	AAfindbest will check for duplicates.
**
**	Returns:
**
**		0 tuple not a duplicate, tid allocated
**		1 tuple a duplicate of the tuple at tid
*/
AAfindbest(d, tid, tuple, need, checkdups)
register DESC		*d;
register struct tup_id	*tid;
char			*tuple;
int			need;
int			checkdups;
{
	register int			i;
	struct tup_id			temptid;

	if (abs(ctoi(d->reldum.relspec)) == M_HEAP)
		checkdups = FALSE;
	else if (i = AAam_find(d, FULLKEY, tid, tid, tuple))
	{
		/* find a suitable page for isam or hash */
		/* determine primary page */
		return (i);	/* fatal error */
	}
	/* If we are not checking for duplicates then take any
	** convenient page linked to the main page current indicated
	** in "tid"
	*/
	if (!checkdups)
		AAfind_page(d, tid, need);

	/* search the chain of pages looking for a spot */
	for ( ; ; )
	{
		if (i = AAgetpage(d, tid))
			break;		/* fatal error */

		/* if tuple is duplicate, drop out */
		if (checkdups && AAdup_check(d, tid, tuple))
		{
			i = 1;
			break;
		}

		/* is there space on this page */
		if (AAspace_left(AAcc_head) >= need)
			break;	/* found a page to use */

		/* no space yet. look on next overflow page */
		if (AAcc_head->ovflopg)
		{
			AAstuff_page(tid, &AAcc_head->ovflopg);
			continue;
		}

		/* no space. allocate new overflow page */
		if (i = AAadd_ovflo(d, tid))
			break;		/* fatal error */
	}

	/* check for dups on remaining overflow pages */
	/* check only if there hasn't been a dup or a page error */
	if (!i && checkdups && AAcc_head->ovflopg)
	{
		AAstuff_page(&temptid, &AAcc_head->ovflopg);
		if (i = AAscan_dups(d, &temptid, tuple))
			AAbmove(&temptid, tid, sizeof(temptid));	/* tid of duplicate */
	}

	/* if tuple isn't a duplicate, allocate a line number */
	if (!i)
		tid->line_id = AAnewlino(need);

#	ifdef xATR1
	if (AAtTfp(88, 2, "FINDBEST: ret %d,", i))
		AAdumptid(tid);
#	endif

	return (i);
}


/*
** AAfind_page
**	Find an appropriate page to put a tuple.
**	If HEAP then any page with room will do. If none
**	can be found, then use the last page.
**	If it is a user relation and a page was found but
**	was full, use it anyway. This can happen only on a
**	modify (which has checkdups turned off).
**
**	For ISAM or HASH look for a page on the same mainpage
**	chain. Duplicate checking must not be enforced.
**
**	The first page to use will be returned in tid in either
**	case.
*/
AAfind_page(d, tid, need)
register DESC		*d;
struct tup_id		*tid;
int			need;
{
	register ACCBUF			*b;
	register ACCBUF			*maxbf;
	register int			heap;
	long				mainpg;

	maxbf = (ACCBUF *) 0;
	heap = abs(ctoi(d->reldum.relspec)) == M_HEAP;
	AApluck_page(tid, &mainpg);
	mainpg++; /* mainpage in buffer points to next higher mainpage */

	/* scan all current buffers looking for one belonging to this relation */
	for (b = AAcc_head; b; b = b->modf)
	{
		if (d->reltid == b->rel_tupid && !(b->bufstatus & BUF_DIRECT)
			&& (heap || (b->mainpg == mainpg)))
		{
			if (AAspace_left(b) >= need)
			{
				/* use this page */
				AAstuff_page(tid, &b->thispage);
				return;
			}

			/* save buffer of largest page */
			if (!maxbf || maxbf->thispage < b->thispage)
				maxbf = b;
		}
	}

	if (heap)
		AAlast_page(d, tid, maxbf);
	else
	{
		/* if we found a full page of a user's relation,use it */
		if (maxbf && !(d->reldum.relstat & S_CATALOG))
			AAstuff_page(tid, &maxbf->thispage);
	}
}
