# include	"AA_liba.h"

/* FIND with AP_FULLKEY on HASH */
AAhash(d, lowtid, hightid, key)
register DESCXX		*d;
register struct tup_id	*lowtid;
register long		*hightid;
char			*key;
{
	long		pageid;
	extern long	AArhash();

# ifdef AMX_TRACE
	AAtTfp(23, 0,
# ifdef MSDOS
		"hash\t\t`%.10s' (%d)\n",
# else
		"hash\t\t`%.14s' (%d)\n",
# endif
		d->xx_desc.reldum.relid, AA_PAT);
# endif
	errno = 0;

	if (AA_PAT)
	{
		/* found a pattern in key */
		AAbmove(&AA_LOW, lowtid, sizeof *lowtid);
		*hightid = AA_HIGH;
	}
	else
	{
		pageid = AArhash(d, key);
		AAstuff_page(lowtid, &pageid);
		lowtid->line_id = SCANTID;
		*hightid = *((long *) lowtid);
	}

# ifdef AM_TRACE
	AAtTfp(23, 10, "##AM_RHASH\n");
	if (AAtTfp(23, 11, "##\t\toldkey"))
		AAprkey(d, key);
	if (AAtTfp(23, 12, "##\t\tlo_"))
		AAdumptid(lowtid);
	if (AAtTfp(23, 13, "##\t\thi_"))
		AAdumptid(hightid);
# endif

}


/* FIND with AP_FULLKEY on ISAM */
AAisam(d, lowtid, hightid, given, key)
register DESCXX		*d;
register long		*lowtid;
register long		*hightid;
register char		*given;
register char		*key;
{
# ifdef AMX_TRACE
	AAtTfp(24, 0,
# ifdef MSDOS
		"isam\t\t`%.10s' (%d)\n",
# else
		"isam\t\t`%.14s' (%d)\n",
# endif
		d->xx_desc.reldum.relid, AA_PAT);
# endif
	errno = 0;

	if (AA_PAT)
	{
		/* found a pattern in key */
		AAlisam(d, lowtid, given, key);
		AAhisam(d, hightid, given, key);
	}
	else
	{
		AA = AAndxsearch(d, lowtid, key, -1, 1);

# ifdef AM_TRACE
	AAtTfp(24, 10, "##AM_=_SEARCH\t%d\n", AA);
		if (AAtTfp(24, 11, "##\t\toldkey"))
			AAprkey(d, key);
		if (AAtTfp(24, 12, "##\t\tnew"))
			AAdumptid(lowtid);
# endif

		if (AA)
			AAerror(8, d->xx_desc.reldum.relid);
		*hightid = *lowtid;
	}
}


/* FIND with AP_LOWKEY or AP_RANGEKEY on ISAM */
AAlisam(d, tid, given, lowkey, highkey)
register DESCXX		*d;
long			*tid;
char			*given;
char			*lowkey;
char			*highkey;
{
	register int		i;
	register char		*dom;
	register int		len;
	register int		g;
	register char		*k;
	register char		*t;
	struct accessparam	ap;
	char			tuple[MAXTUP];

# ifdef AMX_TRACE
	AAtTfp(25, 0,
# ifdef MSDOS
		"low isam\t`%.10s' (%d)\n",
# else
		"low isam\t`%.14s' (%d)\n",
# endif
		d->xx_desc.reldum.relid, AA_PAT);
# endif
	errno = 0;

	/* clear out key conditions */
	for (i = d->xx_desc.reldum.relatts; i; --i)
		d->xx_desc.relgiven[i] = 0;

	AAdparam(d, &ap);
	dom = ap.keydno;

	while (i = *dom++)
	{
		if (bitset(AP_EQ | AP_GT, g = given[i - 1]))
		{
			d->xx_desc.relgiven[i] = AP_GT;
			len = d->xx_desc.reloff[i];
			k = len + (bitset(AP_LT, g)? highkey: lowkey);
			t = len + tuple;
			len = ctou(d->xx_desc.relfrml[i]);
			if (bitset(bitset(AP_LT, g)? AP_P2: AP_P1, g))
				AApatmove(k, t, len, -1);
			else
				AAbmove(k, t, len);
		}
	}

	AA = AAndxsearch(d, tid, tuple, -1, 0);

# ifdef AM_TRACE
	AAtTfp(25, 10, "##AM_<_SEARCH\t%d\n", AA);
	if (AAtTfp(25, 11, "##\t\toldkey"))
		AAprkey(d, tuple);
	if (AAtTfp(25, 12, "##\t\tlo_"))
		AAdumptid(tid);
# endif

	if (AA)
		AAerror(9, d->xx_desc.reldum.relid);

	/* restore key conditions */
	AAbmove(given, &d->xx_desc.relgiven[1], d->xx_desc.reldum.relatts);
}


/* FIND with AP_HIGHKEY or AP_RANGEKEY on ISAM */
AAhisam(d, tid, given, key)
register DESCXX		*d;
long			*tid;
char			*given;
char			*key;
{
	register int		i;
	register char		*dom;
	register int		len;
	register int		g;
	register char		*k;
	register char		*t;
	struct accessparam	ap;
	char			tuple[MAXTUP];

# ifdef AMX_TRACE
	AAtTfp(26, 0,
# ifdef MSDOS
		"high isam\t`%.10s' (%d)\n",
# else
		"high isam\t`%.14s' (%d)\n",
# endif
		d->xx_desc.reldum.relid, AA_PAT);
# endif
	errno = 0;

	/* clear out key conditions */
	for (i = d->xx_desc.reldum.relatts; i; --i)
		d->xx_desc.relgiven[i] = 0;

	AAdparam(d, &ap);
	dom = ap.keydno;

	while (i = *dom++)
	{
		if (bitset(AP_EQ | AP_LT, g = given[i - 1]))
		{
			d->xx_desc.relgiven[i] = AP_LT;
			len = d->xx_desc.reloff[i];
			k = len + key;
			t = len + tuple;
			len = ctou(d->xx_desc.relfrml[i]);
			if (bitset(AP_P1, g))
				AApatmove(k, t, len, 1);
			else
				AAbmove(k, t, len);
		}
	}

	AA = AAndxsearch(d, tid, tuple, 1, 0);

# ifdef AM_TRACE
	AAtTfp(26, 10, "##AM_>_SEARCH\t%d\n", AA);
	if (AAtTfp(26, 11, "##\t\toldkey"))
		AAprkey(d, tuple);
	if (AAtTfp(26, 13, "##\t\thi_"))
		AAdumptid(tid);
# endif

	if (AA)
		AAerror(10, d->xx_desc.reldum.relid);

	/* restore key conditions */
	AAbmove(given, &d->xx_desc.relgiven[1], d->xx_desc.reldum.relatts);
}


/*
** AApatmove -- copy a symbol to a new buffer area.
**
**	AApatmove checks the symbol for
**	pattern matching characters.
**
**	Note that PAT_RBRAC need not be checked for
**	since it is not a pattern matching char unless
**	PAT_LBRAC appears before it.
**
**	If op is opLTLE then add a pad character
**	whose value is the largest possible char
**	value.
**
**	If any ranges of characters are found,
**	then the lowest/highest char is taken from
**	range.
*/
static	AApatmove(key, tuple, len, op)
register char	*key;
char		*tuple;
register int	len;
int		op;		/* op: LE = +1 */
				/*     GE = -1 */
{
	register int	c;
	register int	newc;
	register char	*tup;

	tup = tuple;
	tuple += len;

	while (len--)
	{
		/* copy chars processing ANY, ONE or LBRAC chars if any */
		if ((c = *key++) == PAT_ANY || c == PAT_ONE)
			break;

		if (c == PAT_LBRAC)
		{
			/* if string is empty, ignore it */
			if (!len--)
				break;

			if ((c = *key++) == PAT_RBRAC)
				continue;	/* empty [] */

			while (len-- && ((newc = *key++) != PAT_RBRAC))
			{
				/* ignore '-' */
				if (newc == '-')
					continue;

				/* check for char larger/smaller than 'c' */
				if (op * (newc - c) > 0)
					c = newc;
			}
		}

		*tup++ = c;	/* copy next char */
	}
	while (tup < tuple)
		*tup++ = (op > 0)? LAST_CHAR: 0;
}
