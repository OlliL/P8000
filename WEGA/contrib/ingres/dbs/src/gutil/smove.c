/*
**  STRING MOVE
**
**	The string `a' is moved to the string `b'.  The length
**	of the string is returned.  `a' must be null terminated.
**	There is no test for overflow of `b'.
*/
AAsmove(a, b)
register char	*a;
register char	*b;
{
	register int	l;

	l = 0;
	while (*a)
	{
		*b++ = *a++;
		l++;
	}
	*b = '\0';
	return (l);
}
