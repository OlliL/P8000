# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"

/*
** AAam_delete - delete the tuple specified by tid
**
** AAam_delete removes the tuple specified by tid
** and reclaims the tuple space.
**
** returns:
**
** 	<0  fatal error
** 	0   success
** 	2   tuple specified by tid aleady deleted
*/
AAam_delete(d, tid)
register DESC		*d;
register struct tup_id	*tid;
{
	register int			i;

#	ifdef xATR1
	if (AAtTfp(91, 0, "DELETE: %.14s,", d->reldum.relid))
		AAdumptid(tid);
#	endif

	if (i = AAgetpage(d, tid))
		return (i);

	if (i = AAinvalid(tid))
		return (i);

	AAdel_tuple(tid, AAtup_len(tid));
	d->reladds--;

	return (0);
}
