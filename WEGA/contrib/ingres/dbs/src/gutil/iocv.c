/*
**  INTEGER OUTPUT CONVERSION
**
**	The integer `ix' is converted to ascii and put
**	into the static buffer `buf'.  The address of `buf' is
**	returned.
*/
char	*AA_iocv(ix)
register int	ix;
{
	register char	*a;
	register int	i;
	static char	buf[7];

	if ((i = ix) < 0)
		i = -i;

	a = &buf[6];
	*a-- = '\0';

	do
	{
		*a-- = i % 10 + '0';
		i /= 10;
	} while (i);

	if (ix < 0)
		*a-- = '-';

	return (++a);
}
