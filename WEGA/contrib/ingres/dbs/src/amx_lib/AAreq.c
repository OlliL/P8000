# include	"AA_liba.h"


AArequest(d, rel)
register DESCXX		*d;
register char		*rel;
{
# ifdef AMX_TRACE
	AAtTfp(11, 0,
# ifdef MSDOS
		"retrieve\t`%.10s'\n",
# else
		"retrieve\t`%.14s'\n",
# endif
		rel);
# endif

	errno = 0;
	if (!d)
		AAerror(106, rel);

	if (AA_NOLOCKS)
		return;

	do AAmark(d); while (d = d->xx_index);
}


static	AAmark(d)
register DESCXX		*d;
{
# ifdef AMX_TRACE
	AAtTfp(29, 0,
# ifdef MSDOS
		"lock\t\t`%.10s' (%d) mod 0%o\n",
# else
		"lock\t\t`%.14s' (%d) mod 0%o\n",
# endif
		d->xx_desc.reldum.relid, d->xx_locks, M_SHARE);
# endif

	if (d->xx_locks++)
		return;

	AA = 0;
	if (AA_TA || AAsetrll(A_SLP, d->xx_desc.reltid, AA = M_SHARE) < 0)
		AAerror(15, d->xx_desc.reldum.relid);
	AA_LOCKS++;
	d->xx_refresh = 0;
	d->xx_excl = M_SHARE;
}


AArelease(d)
register DESCXX		*d;
{
# ifdef AMX_TRACE
	AAtTfp(12, 0,
# ifdef MSDOS
		"end retrieve\t`%.10s'\n",
# else
		"end retrieve\t`%.14s'\n",
# endif
		d->xx_desc.reldum.relid);
# endif

	if (AA_NOLOCKS)
		return;

	do AAfree(d); while (d = d->xx_index);
}


static	AAfree(d)
register DESCXX		*d;
{
# ifdef AMX_TRACE
	AAtTfp(30, 0,
# ifdef MSDOS
		"unlock\t\t`%.10s' (%d)\n",
# else
		"unlock\t\t`%.14s' (%d)\n",
# endif
		d->xx_desc.reldum.relid, d->xx_locks);
# endif

	if (d->xx_locks <= 0)
		AAerror(23, d->xx_desc.reldum.relid);

	if (--d->xx_locks)
		return;

	AAnoclose(d);
	AA = 0;
	if (AA_TA || (AA = AAunlrl(d->xx_desc.reltid)) < 0)
		AAerror(16, d->xx_desc.reldum.relid);

	d->xx_refresh = 0;
	d->xx_excl = 0;
	--AA_LOCKS;
}
