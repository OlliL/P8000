# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/access.h"

/*
**	AAam_get - get a single tuple
**
**	AAam_get either gets the next sequencial tuple after
**	"tid" or else gets the tuple specified by tid.
**
**	If getnxt == TRUE, then tid is incremented to the next
**	tuple after tid. If there are no more, then AAam_get returns
**	1. Otherwise AAam_get returns 0 and "tid" is set to the tid of
**	the returned tuple.
**
**	Under getnxt mode, the previous page is reset before
**	the next page is read. This is done to prevent the previous
**	page from hanging around in the am's buffers when we "know"
**	that it will not be referenced again.
**
**	If getnxt == FALSE then the tuple specified by tid is
**	returned. If the tuple was deleted previously,
**	AAam_get retuns 2 else get returns 0.
**
**	If getnxt is true, limtid holds the the page number
**	of the first page past the end point. Limtid and the
**	initial value of tid are set by calls to FIND.
**
**	returns:
**		<0  fatal error
**		0   success
**		1   end of scan (getnxt=TRUE only)
**		2   tuple deleted (getnxt=FALSE only)
*/
AAam_get(d, tid, limtid, tuple, getnxt)
register DESC		*d;
register struct tup_id	*tid;
struct tup_id		*limtid;
int			getnxt;
char			*tuple;
{
	register int		i;
	long			pageid;
	long			lpageid;

#	ifdef xATR1
	if (AAtTfp(93, 0, "GET: %.14s\n", d->reldum.relid))
	{
		AAdumptid(tid);
		printf("lim");
		AAdumptid(limtid);
	}
#	endif

	if (AAgetpage(d, tid))
		return (AMREAD_ERR);

	if (getnxt)
	{
		AApluck_page(limtid, &lpageid);
		do
		{
			while (ctou(++(tid->line_id)) >= AAcc_head->nxtlino)
			{
				tid->line_id = SCANTID;
				pageid = AAcc_head->ovflopg;
				AAstuff_page(tid, &pageid);
				if (!pageid)
				{
					pageid = AAcc_head->mainpg;
					AAstuff_page(tid, &pageid);
					if (!pageid || pageid == lpageid + 1)
						return (1);
				}
				if (i = AAresetacc(AAcc_head))
					return (i);
				if (i = AAgetpage(d, tid))
					return (i);
			}
		} while (!AAcc_head->linetab[LINETAB(ctou(tid->line_id))]);
	}
	else if (i = AAinvalid(tid))
		return (i);

	AAget_tuple(d, tid, tuple);

#	ifdef xATR2
	if (AAtTfp(93, 1, "GET: "))
		AAprtup(d, tuple);
#	endif

	return (0);
}
