# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"

/*
** AAinpclose - close an input relation
** 
** The relation must have been opened by AArelopen with
**	mode 0 (read only)
** 
** return values:
**	<0 fatal error
**	 0 success
**	 1 relation was not open
**	 2 relation was opened in write mode
*/
AAinpclose(d)
register DESC	*d;
{
	register int			i;

#	ifdef xATR1
	AAtTfp(90, 9, "AAinpclose: %.14s\n", d->reldum.relid);
#	endif

	if (abs(d->relopn) != RELROPEN(d->relfp))
		return (1);	/* relation not open */

	if (d->relopn < 0)
		return (2);	/* relation open in write mode */

	i = AAam_flush_rel(d, TRUE);	/* flush and reset all pages */

	if (close(d->relfp))
		i = AAam_error(AMCLOSE_ERR);
	d->relopn = 0;
	return (i);
}
