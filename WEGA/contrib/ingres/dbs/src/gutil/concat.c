/*
**  STRING CONCATENATE
**
**	The strings `s1' and `s2' are concatenated and stored into
**	`dst'.  It is ok for `s1' to equal `dst', but terrible things
**	will happen if `s2' equals `dst'.  The return value is is a
**	pointer to the end of `dst' field.
*/
char	*AAconcat(s1, s2, dst)
register char	*s1;
register char	*s2;
register char	*dst;
{
	while (*s1)
		*dst++ = *s1++;
	while (*s2)
		*dst++ = *s2++;
	*dst = 0;
	return (dst);
}
