# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"

# define	SAMETUP		0
# define	SAMEKEYS	1
# define	DIFFTUP		2

/*
**	AAam_replace - replace an already existing tuple
**
**	AAam_replace will replace the tuple specified by TID
**	with the new tuple. An attempt is made to not
**	move the tuple if at all possible.
**
**	Three separate conditions are delt with. If the
**	new tuple is the same as the old tuple, a return
**	of zero occures and the page is not changed.
**
**	If the keys(if any) are the same and the canonical
**	tuple lengths are the same, then the new tuple will
**	be placed in the same location.
**
**	If the lengths or the keys are different, then the
**	tuple is deleted and the new tuple inserted
**
**	Checkdups specifies whether to check for duplicates.
**	If the new tuple is a duplicate of one already there,
**	then the tuple at TID is deleted
**	returns:
**		<0  fatal error
**		 0  ok
**		 1  new tuple was duplicate of returned tid
**		 2  tuple not found, already deleted (TID invalid)
**
**		If AAam_replace returns 1 then tid is set to the
**		duplicate tuple. This is necessary for updating
**		secondary indices.
*/
AAam_replace(d, tid, tuple, checkdups)
register DESC			*d;
register struct tup_id		*tid;
char				*tuple;
int				checkdups;
{
	register int			i;
	register int			same;
	register int			need;
	register int			len;
	register int			oldlength;
	char				*new;
	char				*old;
	char				*oldt;
	struct tup_id			primtid;
	long				primpage;
	char				oldtuple[MAXTUP];
	extern char			*AAgetint_tuple();

#	ifdef xATR1
	if (AAtTfp(95, 0, "REPLACE: %.14s,", d->reldum.relid))
	{
		AAdumptid(tid);
		printf("REPLACE: ");
		AAprtup(d, tuple);
	}
#	endif

	/* make tuple canonical */
	need = AAcanonical(d, tuple);

	/* if heap, no dup checking */
	if (abs(ctoi(d->reldum.relspec)) == M_HEAP)
		checkdups = FALSE;

	if (i = AAgetpage(d, tid))
		return (i);	/* fatal error */

	/* check if tid exists */
	if (i = AAinvalid(tid))
		return (i);	/* already deleted or AAinvalid */

	oldt = AAgetint_tuple(d, tid, oldtuple);
	oldlength = AAtup_len(tid);

	/* check whether tuples are the same, different lengths, different keys */
	same = DIFFTUP;	/* assume diff lengths or keys */
	if (oldlength == need)
	{
		/* same size. check for same domains */
		same = SAMETUP;	/* assume identical */
		new = tuple;
		old = oldt;
		for (i = 1; i <= d->reldum.relatts; i++)
		{
			len = ctou(d->relfrml[i]);
			if (AAicompare(new, old, d->relfrmt[i], len))
			{
				if (d->relxtra[i])
				{
					same = DIFFTUP;
					break;
				}
				same = SAMEKEYS;
			}
			old += len;
			new += len;
		}
	}

#	ifdef xATR2
	AAtTfp(95, 1, "REPLACE: same=%d\n", same);
#	endif

	switch (same)
	{
	  case SAMETUP:		/* new tuple same as old tuple */
		i = 1;	/* flag as duplicate */
		/* though character strings may compare equal,
		**  they can look different, so if they do look different
		**  go ahead and do the replace using AAput_tuple.  */
		if (!AAbequal(tuple, oldt, d->reldum.relwid))
			goto puttuple;
		break;

	  case SAMEKEYS:	/* keys the same, lengths the same, tuples different */
		if (checkdups)
		{
			/* This is either an ISAM or HASH file. If mainpg
			** is non-zero, then the primary page = mainpg - 1.
			** Otherwise, "find" must be called to determine
			** the primary page
			*/
			if (AAcc_head->mainpg)
			{
				primpage = AAcc_head->mainpg - 1;
				AAstuff_page(&primtid, &primpage);
			}
			else
			{
				if (i = AAam_find(d, FULLKEY, &primtid, &primtid, tuple))
					return (i);	/* fatal error */
				if (i = AAgetpage(d, tid))	/* restore page for tuple */
					return (i);
			}

			if (i = AAscan_dups(d, &primtid, tuple))
			{
				if (i == 1)
				{
					AAdel_tuple(tid, oldlength);	/* tuple a duplicate */
					d->reladds--;
					/* copy tid of duplicate tuple */
					AAbmove(&primtid, tid, sizeof(primtid));
				}
				break;
			}
		}
		goto puttuple;

	  case DIFFTUP:		/* keys different or lengths different */
		AAdel_tuple(tid, oldlength);

		/* find where to put tuple */
		if (i = AAfindbest(d, tid, tuple, need, checkdups))
		{
			d->reladds--;
			break;
		}

		/* place new tuple in page */
puttuple:
		AAput_tuple(tid, AAcctuple, need);
		i = 0;
	}

#	ifdef xATR1
	if (AAtTfp(95, 2, "REPLACE: rets %d,", i))
		AAdumptid(tid);
#	endif

	return (i);
}
