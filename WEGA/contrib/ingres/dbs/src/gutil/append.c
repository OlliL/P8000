/*
**	BLOCK CONCATANATE
**		block `q' of length `n' is concatenated to block
**		`b2' of length `l2', giving `p'.
**
**		Returns the address of the next byte available after
**		the end of `p'.
*/
char	*AAappend(q, n, b2, l2, p)
register char	*q;
register int	n;
register char	*p;
register char	*b2;
register int	l2;
{
	while (n-- > 0)
		*p++ = *q++;
	n = l2;
	q = b2;
	while (n-- > 0)
		*p++ = *q++;
	return (p);
}
