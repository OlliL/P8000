/*
**  Packed Move
**
**	Moves string `s' to storage area `b' of length `l' bytes.  If
**	`s' is too long, it is truncated, otherwise it is padded to
**	length `l' with character `c'.  `B' after the transfer is
**	returned.
*/
char	*AApmove(s, b, l, c)
register char	*s;
register char	*b;
register int	l;
register int	c;
{
	/* move up to `l' bytes */
	while (*s && l > 0)
	{
		*b++ = *s++;
		--l;
	}

	/* if we still have some `l', pad */
	while (l-- > 0)
		*b++ = c;

	return (b);
}
