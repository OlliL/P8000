# include	"AA_liba.h"

AAcloser(desc, rel)
register DESCXX		**desc;
char			*rel;
{
	register DESCXX		*d;
	register DESCXX		*next;

	errno = 0;

	if (!AA_DESC)
		AAerror(111, "close");

	if (!(d = *desc))
		AAerror(106, rel);

	do
	{
		d->xx_desc.relopn = RELWOPEN(d->xx_desc.relfp);

# ifdef AMX_TRACE
	AAtTfp(2, 0,
# ifdef MSDOS
		"close\t\t`%.10s'\n",
# else
		"close\t\t`%.14s'\n",
# endif
		d->xx_desc.reldum.relid);
# endif


# ifndef MSDOS
		if (!(AA_NOLOCKS
		   || bitset(S_CATALOG, d->xx_desc.reldum.relstat)
		   || bitset(S_INDEX, d->xx_desc.reldum.relstat)))
			AAunlus(d->xx_desc.reltid);
# endif
		if (AA = AArelclose(d))
			AAerror(1, rel);

		for (next = AA_DESC; next->xx_next != d; next = next->xx_next)
			continue;
		next->xx_next = d->xx_next;
		free(d);

	} while (d = d->xx_index);

	d = *desc;
	*desc = (DESCXX *) 0;
}


/*
**	AAnoclose - update system catalogs for a relation
**	DESCRIPTION
**
**		function values:
**			<0  fatal error
**			 0  success
**			 1  relation was not open
*/
AAnoclose(d)
register DESCXX		*d;
{
	register int			i;
	struct relation			rel;

# ifdef AMX_TRACE
	AAtTfp(13, 0,
# ifdef MSDOS
		"flush\t\t`%.10s' (%ld)\n",
# else
		"flush\t\t`%.14s' (%ld)\n",
# endif
		d->xx_desc.reldum.relid, d->xx_desc.reladds);
# endif

	/* flush all pages associated with relation      */
	/* reset all the buffers so they can't be reused */
	i = AAam_flush_rel(d, 1);

	/* check to see if number of tuples has changed */
	if (d->xx_desc.reladds && !(i = AAgetpage(AA_rel, &d->xx_desc.reltid)))
	{
		/* yes, update the system catalogs */
		/* get tuple from catalog AA_REL   */
		AAget_tuple(AA_rel, &d->xx_desc.reltid, &rel);

		/* update the reltups field */
		rel.reltups += d->xx_desc.reladds;
		d->xx_desc.reldum.reltups = rel.reltups;

		/* put the tuple back */
		AAput_tuple(&d->xx_desc.reltid, &rel, AA_rel->reldum.relwid);
		i = AAresetacc(AAcc_head);
		d->xx_desc.reladds = 0;
	}

	return (i);
}
