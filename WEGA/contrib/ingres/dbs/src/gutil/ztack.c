/*
**  LOCAL STRING CONCATENATE
**	Strings `a' and `b' are concatenated and left in an
**	internal buffer.  A pointer to that buffer is returned.
**
**	Ztack can be called recursively as:
**		AAztack(AAztack(AAztack(w, x), y), z);
*/
char	*AAztack(a, b)
register char	*a;
register char	*b;
{
	register char	*c;
	static char	buf[101];

	c = buf;

	while (*a)
		*c++ = *a++;

	while (*b)
		*c++ = *b++;

	*c = '\0';

	return (buf);
}
