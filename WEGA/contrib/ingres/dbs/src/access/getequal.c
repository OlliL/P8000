# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"

/*
** AAgetequal - get the first tuple equal to the provided key
**
** AAgetequal is used to do a keyed retrieval of a single
** tuple in cases where the calling program knows the key to
** be unique.  AAam_setkey must be called first to set all desired
** domain values.  AAgetequal will return the first tuple with
** equality on all of the specified domains.
** The tuple is returned in TUPLE.
** 
**	function values:
** 
**		<0 fatal error
**		 0  success
**		 1  no match
*/
AAgetequal(d, key, tuple, tid)
register DESC		*d;
char			key[MAXTUP];
char			tuple[MAXTUP];
register struct tup_id	*tid;
{
	register int			i;
	struct tup_id			limtid;

#	ifdef xATR1
	if (AAtTfp(97, 0, "GETEQUAL: %.14s,", d->reldum.relid))
		AAprkey(d, key);
#	endif

	if (i = AAam_find(d, EXACTKEY, tid, &limtid, key))
		return (i);

	while (!(i = AAam_get(d, tid, &limtid, tuple, TRUE)))
	{
		if (!AAkcompare(d, key, tuple))
		{
#			ifdef xATR2
			if (AAtTfp(97, 1, "GETEQUAL: "))
			{
				AAdumptid(tid);
				printf("GETEQUAL: ");
				AAprtup(d, tuple);
			}
#			endif
			return (0);
		}
	}

#	ifdef xATR2
	AAtTfp(97, 2, "GETEQUAL: %d\n", i);
#	endif

	return (i);
}
