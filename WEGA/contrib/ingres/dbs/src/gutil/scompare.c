/*
**  STRING COMPARE
**
**	The strings 'ap' and 'bp' are compared.  Blanks are
**	ignored.  The first string may be no longer than 'al'
**	bytes, and the second string may be no longer than 'bl'
**	bytes.  If either length is zero, it is taken to be very
**	long.  A null byte also terminates the scan.
**
**	Compares are based on the ascii ordering.
**
**	Shorter strings are less than longer strings.
**
**	Return value is positive one for a > b, minus one for a < b,
**	and zero for a == b.
**
**	Examples:
**		"abc" > "ab"
**		"  a bc  " == "ab  c"
**		"abc" < "abd"
*/
AAscompare(ap, al, bp, bl)
register char	*ap;
register int	al;
register char	*bp;
register int	bl;
{
	register int	a;
	register int	b;

	if (!al)
		al = 32767;
	if (!bl)
		bl = 32767;

	while (1)
	{
		/* supress blanks in both strings */
		while ((a = *ap) == ' ' && al > 0)
		{
			al--;
			ap++;
		}
		if (!al)
			a = 0;
		while (*bp == ' ' && bl > 0)
		{
			bl--;
			bp++;
		}
		if (!bl)
			b = 0;
		else
			b = *bp;

		/* do inequality tests */
		if (a < b)
			return (-1);
		if (a > b)
			return (1);
		if (!a)
			return (0);

		/* go on to the next character */
		ap++;
		al--;
		bp++;
		bl--;
	}
}
