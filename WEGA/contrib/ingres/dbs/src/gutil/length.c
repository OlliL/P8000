/*
**  FIND STRING LENGTH
**
**	The length of string `s' (excluding the null byte which
**		terminates the string) is returned.
*/
AAlength(s)
register char	*s;
{
	register int	l;

	l = 0;
	while (*s++)
		l++;
	return (l);
}
