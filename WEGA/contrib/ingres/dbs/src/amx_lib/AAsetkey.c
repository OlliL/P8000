# include	"AA_liba.h"


int	AA_PAT;		/* pattern matching flag */

AAclearkeys(d, rel)
register DESCXX		*d;
register char		*rel;
{
# ifdef AMX_TRACE
	AAtTfp(27, 0,
# ifdef MSDOS
		"clear keys\t`%.10s'\n",
# else
		"clear keys\t`%.14s'\n",
# endif
		rel);
# endif

	AA_PAT = 0;
	errno = 0;

	if (!d)
		AAerror(106, rel);

	AAam_clearkeys(d);
}


/*
**	hunts through a string and converts the pattern matching
**	characters and replaces with the corresponding cntrl chars
**
**	Searches a string up to a null byte for one of the pattern
**	matching characters '*', '?', '[', and ']'. It then converts
**	these characters to their internal control character equivalents.
*/
AAsetpat(d, key, val, dom, cond)
register DESCXX		*d;
char			*key;
register char		*val;
register int		dom;
int			cond;
{
	register char	*v;
	register int	pat;
	register int	c;
	char		val_buf[MAXFIELD];

# ifdef AMX_TRACE
	AAtTfp(28, 0,
# ifdef MSDOS
		"set pattern\t`%.10s' dom %d cond 0%o\n",
# else
		"set pattern\t`%.14s' dom %d cond 0%o\n",
# endif
		d->xx_desc.reldum.relid, dom, cond);
# endif

	for (pat = 0, v = val_buf; *val; val++)
	{
		if (*val == '\\')
		{
			*v++ = *++val;
			continue;
		}
		switch (*val)
		{
		  case '*':
			*v++ = PAT_ANY;
			goto pat_found;

		  case '?':
			*v++ = PAT_ONE;
			goto pat_found;

		  case '[':
			*v++ = PAT_LBRAC;
			goto pat_found;

		  case ']':
			*v++ = PAT_RBRAC;
pat_found:
			pat++;
			continue;
		}
		if (((c = *val & LAST_CHAR) < ' ' && c != '\n' && c != '\t') || c == LAST_CHAR)
			c = ' ';	/* convert to blank */
		*v++ = c;
	}
	*v = 0;

	AAsetkey(d, key, val_buf, dom, cond);

	if (!pat)
		return;

	if ((pat = d->xx_desc.relxtra[dom]) && (!AA_PAT || AA_PAT > pat))
		AA_PAT = pat;

	switch (cond)
	{
	  case AP_NE:
		if (bitset(AP_GT, cond))
			goto set_p2;
		/* fall through */

	  case AP_GT:
	  case AP_GT | AP_GE:
		if (bitset(AP_LT, cond))
		{
set_p2:
			d->xx_desc.relgiven[dom] |= AP_P2;
# ifdef AM_TRACE
			goto all_set;
# else
			return;
# endif
		}
	}
	/* all others */
	d->xx_desc.relgiven[dom] |= AP_P1;

# ifdef AM_TRACE
all_set:
	AAtTfp(28, 10, "##AM_SETPAT\tnewcond 0%o\n", d->xx_desc.relgiven[dom]);
# endif
}


/*
**	Searches a string up to a null byte for one of the pattern
**	matching characters PAT_ANY, PAT_ONE, PAT_LBRAC, and PAT_RBRAC.
*/
AAsetind(d, key, val, dom, cond)
register DESCXX		*d;
char			*key;
char			*val;
register int		dom;
int			cond;
{
	register char	*v;
	register int	len;
	register int	c;
	register int	pat;

# ifdef AMX_TRACE
	AAtTfp(36, 0,
# ifdef MSDOS
		"set index key\t`%.10s' dom %d cond 0%o\n",
# else
		"set index key\t`%.14s' dom %d cond 0%o\n",
# endif
		d->xx_desc.reldum.relid, dom, cond);
# endif

	pat = 0;
	if (d->xx_desc.relfrmt[dom] == CHAR)
	{
		len = ctou(d->xx_desc.relfrml[dom]);
		for (v = val; len; --len)
			if ((c = *v++) == PAT_ANY || c == PAT_ONE || c == PAT_LBRAC || c == PAT_RBRAC)
				pat++;
	}

	AAsetkey(d, key, val, dom, cond);

	if (!pat)
		return;

	if ((pat = d->xx_desc.relxtra[dom]) && (!AA_PAT || AA_PAT > pat))
		AA_PAT = pat;

	switch (cond)
	{
	  case AP_NE:
		if (bitset(AP_GT, cond))
			goto set_p2;
		/* fall through */

	  case AP_GT:
	  case AP_GT | AP_GE:
		if (bitset(AP_LT, cond))
		{
set_p2:
			d->xx_desc.relgiven[dom] |= AP_P2;
# ifdef AM_TRACE
			goto all_set;
# else
			return;
# endif
		}
	}
	/* all others */
	d->xx_desc.relgiven[dom] |= AP_P1;

# ifdef AM_TRACE
all_set:
	AAtTfp(36, 10, "##AM_SETIND\tnewcond 0%o\n", d->xx_desc.relgiven[dom]);
# endif
}


AAsetkey(d, key, val, dom, cond)
register DESCXX		*d;
char			*key;
char			*val;
register int		dom;
int			cond;
{
	if (dom <= 0 || dom > d->xx_desc.reldum.relatts)
	{
		AA = dom;
		AAerror(13, d->xx_desc.reldum.relid);
	}

# ifdef AMX_TRACE
	AAtTfp(21, 0,
# ifdef MSDOS
		"set key val\t`%.10s' dom %d cond 0%o\n",
# else
		"set key val\t`%.14s' dom %d cond 0%o\n",
# endif
		d->xx_desc.reldum.relid, dom, cond);
# endif

	switch (cond)
	{
	  case AP_EQ:
	  case AP_LT:
	  case AP_LT | AP_LE:
	  case AP_GT:
	  case AP_GT | AP_GE:
	  case AP_NE:
		cond |= d->xx_desc.relgiven[dom];
		AAam_setkey(d, key, val, dom);
		d->xx_desc.relgiven[dom] = cond;

# ifdef AM_TRACE
	AAtTfp(21, 10, "##AM_SETKEY\tnewcond 0%o\n", cond);
	if (AAtTfp(21, 11, "##\t\tnewkey"))
		AAprkey(d, key);
# endif

		return;
	}

	AA = cond;
	AAerror(14, d->xx_desc.reldum.relid);
}
