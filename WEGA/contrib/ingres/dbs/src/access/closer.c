# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"

/*
** AArelclose - close a relation
**	is used to close a relation which was opened by AArelopen.
**	should always be called once for each AArelopen.
**
**	function values:
**
** 		<0  fatal error
** 		 0  success
** 		 1  relation was not open
*/
AArelclose(d)
register DESC	*d;
{
	register int			i;

#	ifdef xATR1
	AAtTfp(90, 8, "CLOSER: %.14s,%ld\n", d->reldum.relid, d->reladds);
#	endif

	if (i = AAnoclose(d))
		return (i);

	AAam_flush_rel(d, TRUE);	/* No error is possible since AAnoclose() */
				/* has already flushed any pages */

	if (close(d->relfp))	/*close the relation*/
		i = AAam_error(AMCLOSE_ERR);

	d->relopn = 0;
	return (i);
}
