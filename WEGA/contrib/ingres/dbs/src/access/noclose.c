# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/access.h"

/*
** AAnoclose - update system catalogs for a relation
**
**	function values:
** 
**		<0  fatal error
**		 0  success
**		 1  relation was not open
*/
AAnoclose(d)
register DESC		*d;
{
	register int			i;
	struct relation			rel;

#	ifdef xATR1
	AAtTfp(90, 8, "NOCLOSE: %.14s,%ld\n", d->reldum.relid, d->reladds);
#	endif

	/* make sure relation relation is read/write mode */
	if (abs(d->relopn) != RELROPEN(d->relfp))
		return (1);

	/* flush all pages associated with relation */
	/* if system catalog, reset all the buffers so they can't be reused */
	i = AAam_flush_rel(d, d->reldum.relstat & S_CATALOG);

	/* check to see if number of tuples has changed */
	if (d->reladds)
	{
		/* yes, update the system catalogs */
		/* get tuple from relation relation */
		AAdmin.adreld.relopn = RELWOPEN(AAdmin.adreld.relfp);
		if (i = AAgetpage(&AAdmin.adreld, &d->reltid))
			return (i);	/* fatal error */

		/* get the actual tuple */
		AAget_tuple(&AAdmin.adreld, &d->reltid, &rel);

		/* update the reltups field */
		rel.reltups += d->reladds;
		d->reldum.reltups = rel.reltups;

		/* put the tuple back */
		AAput_tuple(&d->reltid, &rel, AAdmin.adreld.reldum.relwid);
		i = AAresetacc(AAcc_head);
		d->reladds = 0;
	}

	return (i);
}
