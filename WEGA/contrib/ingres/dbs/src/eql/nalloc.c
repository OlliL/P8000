/*
**  NALLOC.C -- Dynamic core allocation routines
**
**	Defines:
**		nalloc()
**		xfree()
**		salloc()
**
**
**
**	NALLOC --
**		Dynamic allocation routine which
** 		merely calls alloc(III), returning
**		0 if no core and a pointer otherwise.
*/
char	*nalloc(size)
register int	size;
{
	extern char	*malloc();

	return (malloc(size));
}


/*
**	SALLOC -- allocate
**		place for string and initialize it,
**		return string or 0 if no core.
*/
char	*salloc(s)
register char	*s;
{
	register char	*string;
	extern char	*malloc();

	string = malloc(AAlength(s) + 1);
	if (!string)
		return ((char *) 0);

	AAsmove(s, string);
	return (string);
}

/*
**	XFREE -- Free possibly dynamic storage
**		checking if its in the heap first.
**
**		0 - freed
**		1 - not in heap
*/
xfree(cp)
char		*cp;
{
	register char		*lcp;
	register char		*lend;
	register char		*lacp;
	extern char		*end;

	lcp = cp;
	lacp = (char *) &cp;
	lend = (char *) &end;
	if (lcp >= lend && lcp < lacp)	/* make shure its in heap */
	{
		free(cp);
		return (0);
	}
	return (1);
}
