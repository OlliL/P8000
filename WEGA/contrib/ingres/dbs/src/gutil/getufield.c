/*
**  GETUFIELD -- extract field from AA_USERS
**
**	A buffer returned as the result of a getuser() (or getpw())
**	call is scanned for the indicated parameter, numbered from
**	zero.  A pointer to the parameter is returned.
*/
char	*getufield(p, num)
register char	*p;
register int	num;
{
	register int	c;
	register char	*r;

	/* skip other fields */
	for ( ; num > 0; num--)
		while (c = *p++)
			if (c == ':')
				break;

	/* save result pointer */
	r = p;

	/* null-terminate this field */
	while (c = *p++)
		if (c == ':')
			break;
	*--p = 0;

	return (r);
}
