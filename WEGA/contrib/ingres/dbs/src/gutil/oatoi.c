/*
**  OCTAL ASCII TO INTEGER CONVERSION
**
**	The ascii string 'a' which represents an octal number
**	is converted to binary and returned.  Conversion stops at any
**	non-octal digit.
**
**	Note that the number may not have a sign, and may not have
**	leading blanks.
**
**	(Intended for converting the status codes in users(FILE))
*/
AA_oatoi(a)
register char	*a;
{
	register int	r;
	register int	c;

	r = 0;

	while ((c = *a++) >= '0' && c <= '7')
		r = (r << 3) | (c &= 7);

	return (r);
}
