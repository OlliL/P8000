# include	"AA_liba.h"

static DESCXX		*AA_primary;
extern char		*AAusercode;

/*
**	AArefresh - update relation descriptor from system catalog AA_REL
*/
AArefresh(d)
register DESCXX		*d;
{
	if (AA_NOLOCKS || d->xx_refresh)
		return;

	d->xx_refresh++;

# ifdef AMX_TRACE
	AAtTfp(37, 0,
# ifdef MSDOS
		"refresh rel\t`%.10s'\n",
# else
		"refresh rel\t`%.14s'\n",
# endif
		d->xx_desc.reldum.relid);
# endif

	/* make believe catalog AA_REL is read only for concurrency	*/
	/* that means, readed pages are not locked			*/
	AA_rel->relopn = RELROPEN(AA_rel->relfp);

	if (AA = AAgetpage(AA_rel, &d->xx_desc.reltid))
		AAerror(24, d->xx_desc.reldum.relid);

	AAget_tuple(AA_rel, &d->xx_desc.reltid, d);
	AAresetacc(AAcc_head);	/* No error is possible */

	/* restore catalog AA_REL to read/write mode */
	AA_rel->relopn = RELWOPEN(AA_rel->relfp);
}


AAopenr(desc, prel)
register DESCXX 	**desc;
register char		*prel;
{
	register DESCXX 	*d;

# ifdef AMX_TRACE
	AAtTfp(0, 0, "open rel\t`%s'\n", prel);
# endif

	AAopen(desc, prel);

	/* check for valid relation */
	d = *desc;
	if (bitset(S_INTEG, d->xx_desc.reldum.relstat))
		AAerror(103, prel);

	if
	(
		!AAbequal(AAusercode, d->xx_desc.reldum.relowner, 2)
		&& bitset(S_PROTALL, d->xx_desc.reldum.relstat)
		&& bitset(S_PROTRET, d->xx_desc.reldum.relstat)
	)
		AAerror(104, prel);

	if (bitset(S_VIEW, d->xx_desc.reldum.relstat))
		AAerror(105, prel);

	AA_primary = d;

# ifndef MSDOS
	if (AA_NOLOCKS || bitset(S_CATALOG, d->xx_desc.reldum.relstat)
	   || bitset(S_INDEX, d->xx_desc.reldum.relstat))
		return;

	if (AAsetusl(A_RTN, d->xx_desc.reltid, AA = M_SHARE) < 0)
		AAerror(108, prel);
# endif
}


AAopeni(desc, irel, idom)
register DESCXX 	**desc;
register char		*irel;
register char		*idom;
{
# ifdef AMX_TRACE
	AAtTfp(1, 0, "open index\t`%s'\n", irel);
# endif

	AAopen(desc, irel);
	(*desc)->xx_indom = idom;
	AA_primary->xx_index = *desc;
	AA_primary = *desc;
}


static int	AAopen(desc, rel)
DESCXX		**desc;
register char	*rel;
{
	register DESCXX 	*d;
	register DESCXX 	*next;
	extern DESCXX		*malloc();

	errno = 0;

	if (!AA_DESC)
		AAerror(111, "open");

	if (*desc)
		AAerror(101, rel);

	if (!(d = malloc(sizeof (DESCXX))))
		AAerror(102, rel);

	if (AA = AArelopen(&d->xx_desc, 2, rel))
		AAerror(0, rel);
	/* make believe relation rel is read only for concurrency	*/
	/* that means, readed pages are not locked			*/
	if (!bitset(S_CATALOG, d->xx_desc.reldum.relstat))
		d->xx_desc.relopn = RELROPEN(d->xx_desc.relfp);

	d->xx_index = (DESCXX *) 0;
	d->xx_indom = (char *) 0;
	d->xx_ta = (DESCXX *) 0;
	d->xx_refresh = 0;
	d->xx_excl = 0;
	d->xx_locks = 0;

	d->xx_next = (DESCXX *) 0;
	for (next = AA_DESC; next->xx_next; next = next->xx_next)
		continue;
	next->xx_next = d;
	*desc = d;
}


long	AAtupcount(d)
register DESCXX 	*d;
{
	if (!AA_DESC)
		AAerror(111, "tuplecount");

	AArefresh(d);
	return (d->xx_desc.reladds + d->xx_desc.reldum.reltups);
}


AAtuplength(d)
register DESCXX 	*d;
{
	if (!AA_DESC)
		AAerror(111, "tuplelength");

	return (d->xx_desc.reldum.relwid);
}
