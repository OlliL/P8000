# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"

/*
**	AAam_insert - add a new tuple to a relation
**
**	AAam_insert puts a given tuple into a relation in
**	the "correct" position.
**
**	If AAam_insert is called with checkdups == TRUE then
**	the tuple will not be inserted if it is a duplicate
**	of some already existing tuple. If the relation is a
**	heap then checkdups is made false.
**
**	Tid will be set to the tuple id where the
**	tuple is placed.
**
**	returns:
**		<0  fatal eror
**		0   success
**		1   tuple was a duplicate
*/
AAam_insert(d, tid, tuple, checkdups)
register DESC		*d;
register struct tup_id	*tid;
char			*tuple;
int			checkdups;
{
	register int			i;
	register int			need;

#	ifdef xATR1
	if (AAtTfp(94, 0, "INSERT: %.14s,", d->reldum.relid))
	{
		AAdumptid(tid);
		AAprtup(d, tuple);
	}
#	endif

	/* determine how much space is needed for tuple */
	need = AAcanonical(d, tuple);

	/* find the "best" page to place tuple */
	if (i = AAfindbest(d, tid, tuple, need, checkdups))
		return (i);

	/* put tuple in position "tid" */
	AAput_tuple(tid, AAcctuple, need);

	d->reladds++;

	return (0);
}
