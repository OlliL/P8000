# include	"AA_liba.h"

AAgetup(d, given, low_tid, high_tid, low_key, high_key)
register DESCXX		*d;
char			*given;
register long		*low_tid;
register long		*high_tid;
char			*low_key;
char			*high_key;
{
	register int	i;

	errno = 0;

# ifdef AMX_TRACE
	if (AAtTfp(9, 0,
# ifdef MSDOS
		"get tuple\t'%.10s' ",
# else
		"get tuple\t'%.14s' ",
# endif
		d->xx_desc.reldum.relid))
			AAdumptid(low_tid);
# endif

	if (!AA_NOLOCKS && !d->xx_excl)
		AAerror(19, d->xx_desc.reldum.relid);

	AArefresh(d);

	/* pop key conditions */
	AAbmove(given, &d->xx_desc.relgiven[1], d->xx_desc.reldum.relatts);

	for ( ; ; )
	{
		AA = AAam_get(d, low_tid, high_tid, AA_STUP, 1);

# ifdef AM_TRACE
		AAtTfp(9, 10, "##AM_GETUP\t%d\n", AA);
		if (AAtTfp(9, 11, "##\t\tlo_"))
			AAdumptid(low_tid);
		if (AAtTfp(9, 12, "##\t\thi_"))
			AAdumptid(high_tid);
		if (AAtTfp(9, 13, "##\t\tnewtup"))
			AAprtup(d, AA_STUP);
# endif

		if (AA || AAtup_compare(d, low_key, high_key))
			break;
	}

	if (AA < 0)
		AAerror(11, d->xx_desc.reldum.relid);

	/* get value of tidp for AAgetid if indexed access path */
	if (ctoi(d->xx_desc.reldum.relindxd) == SECINDEX)
	{
		i = d->xx_desc.reldum.relatts;
		i = d->xx_desc.reloff[i];
		AAbmove(&AA_STUP[i], &AA_TID, sizeof AA_TID);
	}

# ifdef AMX_TRACE
	AAtTfp(9, 1, "... ret %d\n", !AA);
# endif

	return (!AA);
}


AAgetid(d, tid, given, low_key, high_key)
register DESCXX		*d;
register long		*tid;
char			*given;
char			*low_key;
char			*high_key;
{
	register int	i;

	errno = 0;
	*tid = AA_TID;

# ifdef AMX_TRACE
	if (AAtTfp(10, 0,
# ifdef MSDOS
		"get tid\t\t'%.10s'",
# else
		"get tid\t\t'%.14s'",
# endif
		d->xx_desc.reldum.relid))
			AAdumptid(tid);
# endif

	if (!AA_NOLOCKS && !d->xx_excl)
		AAerror(19, d->xx_desc.reldum.relid);

	AArefresh(d);

	/* pop key conditions */
	AAbmove(given, &d->xx_desc.relgiven[1], d->xx_desc.reldum.relatts);

	AA = AAam_get(d, tid, tid, AA_STUP, 0);

# ifdef AM_TRACE
	AAtTfp(10, 10, "##AM_GETID\t%d\n", AA);
	if (AAtTfp(10, 11, "##\t\tnewtup"))
		AAprtup(d, AA_STUP);
# endif

	if (AA)
		AAerror(12, d->xx_desc.reldum.relid);

	i = AAtup_compare(d, low_key, high_key);

# ifdef AMX_TRACE
	AAtTfp(10, 1, "... ret %d\n", i);
# endif

	return (i);
}


AAget(d, tid)
register DESCXX		*d;
register long		*tid;
{
	register int	i;

	errno = 0;
# ifdef AMX_TRACE
	if (AAtTfp(43, 0,
# ifdef MSDOS
		"get tid tuple\t'%.10s' ",
# else
		"get tid tuple\t'%.14s' ",
# endif
		d->xx_desc.reldum.relid))
			AAdumptid(tid);
# endif

	if (!AA_NOLOCKS && !d->xx_excl)
		AAerror(19, d->xx_desc.reldum.relid);

	AArefresh(d);

	AA = AAam_get(d, tid, tid, AA_STUP, 0);

# ifdef AM_TRACE
	AAtTfp(43, 10, "##AM_GET\t%d\n", AA);
	if (AAtTfp(43, 11, "##\t\tnewtup"))
		AAprtup(d, AA_STUP);
# endif

	if (AA < 0)
		AAerror(25, d->xx_desc.reldum.relid);

# ifdef AMX_TRACE
	AAtTfp(43, 1, "... ret %d\n", !AA);
# endif

	return (!AA);
}


static	AAtup_compare(d, low_key, high_key)
register DESCXX		*d;
char			*low_key;
char			*high_key;
{
	register int	dom;
	register int	cond1;
	register int	cond2;

	for (dom = d->xx_desc.reldum.relatts; dom; --dom)
	{
		if (cond1 = d->xx_desc.relgiven[dom])
		{
			cond2 = 0;
			if (bitset(AP_NE, cond1) && bitset((AP_LT | AP_GT), cond1))
			{
				clrbit(AP_NE, cond1);
				cond2 = AP_NE;
			}
			else if (bitset(AP_LT, cond1) && bitset(AP_GT, cond1))
			{
				clrbit(AP_GT, cond1);
				cond2 = AP_GT;
				if (bitset(AP_GE, cond1))
				{
					clrbit(AP_GE, cond1);
					cond2 |= AP_GE;
				}
			}

			if (AAdom_compare(d, dom, cond1, low_key, AP_P1))
				return (0);

			if (cond2 && AAdom_compare(d, dom, cond2, high_key, AP_P2))
				return (0);
		}
	}

	return (1);
}


static	AAdom_compare(d, dom, cond, key, pat)
register DESCXX		*d;
register int		dom;
register int		cond;
char			*key;
int			pat;
{
	register int	frml;
	register int	off;
	register int	i;

	frml = ctou(d->xx_desc.relfrml[dom]);
	off = d->xx_desc.reloff[dom];
	key += off;
	i = bitset(pat, d->xx_desc.relgiven[dom])?
		AApcompare(key, frml, AA_STUP + off, frml):
		AAicompare(AA_STUP + off, key, d->xx_desc.relfrmt[dom], frml);

# ifdef AM_TRACE
	AAtTfp(32, 0, "##AM_DOM_COMP\tdom %d pat 0%o given 0%o cond 0%o rc %d\n",
		dom, pat, d->xx_desc.relgiven[dom], cond, i);
	if (AAtTfp(32, 1, "##\t\toldkey"))
		AAprkey(d, key - off);
	if (AAtTfp(32, 2, "##\t\toldtup"))
		AAprtup(d, AA_STUP);
# endif

	if (i < 0 && bitset((AP_LT | AP_NE), cond))
			return (0);
	else if (!i && bitset((AP_EQ | AP_LE | AP_GE), cond))
			return (0);
	else if (i > 0 && bitset((AP_GT | AP_NE), cond))
			return (0);

	return (1);
}


static	AApcompare(s1, l1, s2, l2)
register char	*s1;
int		l1;
char		*s2;
int		l2;
{
	register int	l;
	register char	*s;
	char		str1[MAXFIELD];
	char		str2[MAXFIELD];

	for (l = l1, s = str1; l; --l)
		if ((*s = *s1++) == ' ')
			--l1;
		else
			s++;

	for (l = l2, s1 = s2, s = str2; l; --l)
		if ((*s = *s1++) == ' ')
			--l2;
		else
			s++;

	return (AApattern(str1, l1, str2, l2));
}


/*
**	AApattern performs character comparisons between the two
**	strings s1 and s2. All blanks and null are ignored in
**	both strings. In addition pattern matching is performed
**	using the "shell syntax". Pattern matching characters
**	are converted to the pattern matching symbols PAT_ANY etc.
**	by the scanner.
**
**	Pattern matching characters can appear in string s1.
**
**	examples:
**
**	"Smith, Homer" = "Smith,Homer"
**
**	"abcd" < "abcdd"
**
**	"abcd" = "aPAT_ANYd"
**
**	returns	<0 if s1 > s2
**		 0 if s1 = s2
**		>0 if s1 < s2
*/
static	AApattern(s1, l1, s2, l2)
register char	*s1;
register int	l1;
register char	*s2;
register int	l2;
{
	register int	c1;
	register int	c2;

# ifdef AM_TRACE
	if (AAtTfp(33, 0, "##AM_PATTERN\t%d", l1))
	{
		AApratt(BINARY, l1, s1);
		printf("| %d", l2);
		AApratt(BINARY, l2, s2);
		printf("|\n");
	}
# endif

loop:
	while (l1--)
	{
		switch (c1 = *s1++)
		{
		  case PAT_ANY:
			return (AAany_match(s1, l1, s2, l2));

		  case PAT_LBRAC:
			return (AAset_match(s1, l1, s2, l2));

		  default:
			if (!l2--)
				return (-1);	/* s1 > s2 */
			if (c1 == (c2 = *s2++) || c1 == PAT_ONE)
				goto loop;
			return (c2 - c1);
		}
	}

	/* examine remainder of s2 for any characters */
	return (l2? 1: 0);
}


static	AAany_match(s1, l1, s2, l2)
register char	*s1;
register int	l1;
register char	*s2;
register int	l2;
{
	register int	c1;

# ifdef AM_TRACE
	if (AAtTfp(34, 0, "##AM_PAT_ANY\t%d", l1))
	{
		AApratt(BINARY, l1, s1);
		printf("| %d", l2);
		AApratt(BINARY, l2, s2);
		printf("|\n");
	}
# endif

	if (!l1)
		return (0);	/* a match if no more chars in p */

	/*
	** If the next character in "s1" is not another
	** pattern matching character, then scan until
	** first matching char and continue comparison.
	*/
	if ((c1 = *s1) != PAT_ANY && c1 != PAT_LBRAC && c1 != PAT_ONE)
	{
		while (l2--)
		{
			if (*s2 == c1 && !AApattern(s1, l1, s2, l2 + 1))
				return (0);
			s2++;
		}
	}
	else
	{
		while (l2)
			if (!AApattern(s1, l1, s2++, l2--))
				return (0);	/* match */
	}
	return (1);	/* no match */
}


static	AAset_match(s1, l1, s2, l2)
register char	*s1;
register int	l1;
register char	*s2;
register int	l2;
{
	register int	c1;
	register int	c2;
	register int	oldc;
	int		found;

# ifdef AM_TRACE
	if (AAtTfp(35, 0, "##AM_PAT_SET\t%d", l1))
	{
		AApratt(BINARY, l1, s1);
		printf("| %d", l2);
		AApratt(BINARY, l2, s2);
		printf("|\n");
	}
# endif

	c2 = *s2;
	while (l2--)
	{
		/* search for a match on 'c2' */
		found = 0;	/* assume failure */
		oldc = 0777;	/* make previous char large */

		while (l1--)
		{
			switch (c1 = *s1++)
			{
			  case PAT_RBRAC:
				return (found? AApattern(s1, l1, s2, l2): 1);

			  case '-':
				if (!l1--)
					return (1);	/* not found */
				c1 = *s1++;
				if (!found && oldc <= c2 && c2 <= c1)
				{
					s2++;
					found++;
				}
				break;

			  default:
				if (c2 == (oldc = c1))
				{
					s2++;
					found++;
				}
			}
		}
		return (1);	/* no match */
	}
	return (-1);
}
