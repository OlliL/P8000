/*
**  BLOCK EQUALITY TEST
**
**	blocks `p' and `q', both of length `l', are tested
**		for absolute equality.
**	Returns one for equal, zero otherwise.
*/
AAbequal(p, q, l)
register char	*p;
register char	*q;
register int	l;
{
	for ( ; l > 0; --l)
		if (*p++ != *q++)
			return (0);
	return (1);
}
