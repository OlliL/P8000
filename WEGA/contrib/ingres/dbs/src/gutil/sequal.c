/*
**	STRING EQUALITY TEST
**		null-terminated strings `a' and `b' are tested for
**			absolute equality.
**		returns one if equal, zero otherwise.
*/
AAsequal(a, b)
register char	*a;
register char	*b;
{
	while (*a || *b)
		if (*a++ != *b++)
			return (0);
	return (1);
}
