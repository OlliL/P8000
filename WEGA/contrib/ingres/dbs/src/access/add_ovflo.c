# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"
# include	"../h/aux.h"
# include	"../h/lock.h"

/* ADD_OVFLO -- Allocates an overflow page which will be */
/* 	attached to current page.  TID must be for current page. */
/* 	TID will be updated to id of new overflow page. */
AAadd_ovflo(d, tid)
register DESC		*d;
struct tup_id		*tid;
{
	register ACCBUF			*b;
	register int			lk;
	register int			i;
	long				mpage;
	long				newpage;
	struct tup_id			tidx;
	extern ACCBUF			*AAchoose_buf();

	/* save main page pointer so that it may be used when */
	/* setting up new overflow page. */
	mpage = AAcc_head->mainpg;

	if (lk = (AAcclock && (d->relopn < 0 )))
	{
		if (AAsetcsl(AAcc_head->rel_tupid) < 0)
			return (AMTA_ERR);
		AAcclock = FALSE;
	}

	/* Determine last page of the relation */
	AAlast_page(d, &tidx, AAcc_head);
	AApluck_page(&tidx, &newpage);
	newpage++;

	/* choose an available buffer as victim for setting up */
	/* overflow page. */
	if (!(b = AAchoose_buf(d, newpage)))
	{
		if (lk)
		{
			AAcclock = TRUE;
			AAunlcs(AAcc_head->rel_tupid);
		}
		return (-1);
	}

	/* setup overflow page */
	b->mainpg = mpage;
	b->ovflopg = 0;
	b->thispage = newpage;
	b->linetab[LINETAB(0)] = b->firstup - (char *) b;
	b->nxtlino = 0;
	b->bufstatus |= BUF_DIRTY;
	if (AApageflush(b))
		return (-2);

	/* now that overflow page has successfully been written, */
	/* get the old current page back and link the new overflow page */
	/* to it. */
	/* If the relation is a heap then don't leave the old main */
	/* page around in the buffers. This is done on the belief */
	/* that it will never be accessed again. */
	if (AAgetpage(d, tid))
		return (-3);

	AAcc_head->ovflopg = newpage;
	AAcc_head->bufstatus |= BUF_DIRTY;
	i = AApageflush(AAcc_head);
	if (lk)
	{
		AAcclock = TRUE;
		AAunlcs(AAcc_head->rel_tupid);
	}
	if (i)
		return (-4);

	if (abs(ctoi(d->reldum.relspec)) == M_HEAP)
		AAresetacc(AAcc_head);	/* no error is possible */

	/* now bring the overflow page back and make it current. */
	/* if the overflow page is still in AM cache, then this will not */
	/* cause any disk activity. */
	AAstuff_page(tid, &newpage);
	if (AAgetpage(d, tid))
		return (-5);

	return (0);
}
