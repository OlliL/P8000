# include	"AA_liba.h"

# define	DRIVER_LOCKED	-4

static int	AA_2_TA;

AAlock(d, rel, mod)
register DESCXX		*d;
char			*rel;
register int		mod;
{
	register DESCXX		*i;

	errno = 0;

	if (!AA_DESC)
		AAerror(111, "use");

# ifdef AMX_TRACE
	AAtTfp(14, 0,
# ifdef MSDOS
		"use\t\t`%.10s' (%d) mod 0%o\n",
# else
		"use\t\t`%.14s' (%d) mod 0%o\n",
# endif
		rel, d->xx_locks, mod);
# endif

	if (!d)
		AAerror(106, rel);

	if (AA_NOLOCKS)
		return;

	if (AA_LOCKS)
		AAerror(107, "TA-begin");

	if (mod < 0)
	{
		if (AA_2_TA)
			goto concur;
		mod = -mod;
		AA_2_TA++;
	}
	else if (!AA_2_TA)
	{
concur:
		AAerror(107, "second TA");
	}

	for (i = d; i = i->xx_index; )
		AArel_lock(i, mod);
	AArel_lock(d, mod);
}


static	AArel_lock(d, mod)
register DESCXX		*d;
register int		mod;
{
	register int		i;

# ifndef MSDOS
	if (ctoi(d->xx_desc.reldum.relindxd) == SECINDEX)
		mod &= ~M_TE;
# endif

# ifdef AMX_TRACE
	AAtTfp(31, 0,
# ifdef MSDOS
		"lock\t\t`%.10s' (%d) mod 0%o\n",
# else
		"lock\t\t`%.14s' (%d) mod 0%o\n",
# endif
		d->xx_desc.reldum.relid, d->xx_locks, mod);
# endif
	AA = errno = 0;

	if (d->xx_ta)
		AAerror(17, d->xx_desc.reldum.relid);

	d->xx_refresh = 0;

	if ((d->xx_excl = mod) & M_EXCL)
	{
		/* restore relation of descriptor d to read/write mode */
		d->xx_desc.relopn = RELWOPEN(d->xx_desc.relfp);
	}

	if (AA_TA)
		AA_TE->xx_ta = d;
	else
		AA_TA = d;
	AA_TE = d;

restart:
	errno = 0;
	for ( ; d; d = d->xx_ta)
	{
# ifdef AMX_TRACE
		AAtTfp(31, 1,
# ifdef MSDOS
			".... %s (%d)\t`%.10s'\n",
# else
			".... %s (%d)\t`%.14s'\n",
# endif
			(d == AA_TA)? "slp": "rtn", d->xx_excl, d->xx_desc.reldum.relid);
# endif
		if (AAsetrll((d == AA_TA)? A_SLP: A_RTN, d->xx_desc.reltid, d->xx_excl) < 0)
		{
			/* the lock request fails */
			AAunlock(i = errno == DRIVER_LOCKED);
			if (i)
				AAerror(109, "aborted");
			d = AA_TA;
			goto restart;
		}
		d->xx_locks++;
	}
}


AAunlock(really)
register int	really;
{
	register DESCXX		*next;
	register DESCXX		*d;

# ifdef AMX_TRACE
	AAtTfp(15, 0, "commit\t\t(%d)\n", really);
# endif

	if (!AA_DESC)
		AAerror(111, "commit");

	if (AA_NOLOCKS)
		return;

	if (!(d = AA_TA))
		AAerror(107, "TA-end");

	for ( ; d; d = next)
	{
# ifdef AMX_TRACE
	AAtTfp(15, 1,
# ifdef MSDOS
		"unlock\t\t`%.10s' (%d)\n",
# else
		"unlock\t\t`%.14s' (%d)\n",
# endif
		d->xx_desc.reldum.relid, d->xx_locks);
# endif
		next = d->xx_ta;
		if (really)
		{
			d->xx_excl = 0;
			d->xx_ta = (DESCXX *) 0;
			AAnoclose(d);
			/* make believe relation is read only	*/
			/* for concurrency that means,		*/
			/* readed pages are not locked		*/
			if (!bitset(S_CATALOG, d->xx_desc.reldum.relstat))
				d->xx_desc.relopn = RELROPEN(d->xx_desc.relfp);
		}
		if ((AA = d->xx_locks) && AAunlrl(d->xx_desc.reltid) < 0)
			AAerror(18, d->xx_desc.reldum.relid);
		d->xx_locks = 0;
	}
	if (really)
	{
		sync();
		AA_TE = AA_TA = (DESCXX *) 0;
		AA_2_TA = 0;
		AAdaemon = 0;
	}
}
