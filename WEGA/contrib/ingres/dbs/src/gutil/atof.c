/*
**  ASCII TO FLOATING CONVERSION
**
**	Converts the ping 'str' to floating point and stores the
**	result into the cell pointed to by 'val'.
**
**	Returns zero for ok, negative one for syntax error, and
**	positive one for overflow.
**	(actually, it doesn't check for overflow)
**
**	The syntax which it accepts is pretty much what you would
**	expect.  Basically, it is:
**		{<sp>} [+|-] {<sp>} {<digit>} [.{digit}] {<sp>} [<exp>]
**	where <exp> is "e" or "E" followed by an integer, <sp> is a
**	space character, <digit> is zero through nine, [] is zero or
**	one, and {} is zero or more.
**
*/
AA_atof(p, val)
register char	*p;
double		*val;
{
	double		v;
	double		fact;
	int		minus;
	register int	c;
	short		expon;
	register int	gotmant;
	extern double	pow();

	v = 0.0;
	minus = 0;

	/* skip leading blanks */
	while (c = *p)
	{
		if (c != ' ')
			break;
		p++;
	}

	/* handle possible sign */
	switch (c)
	{
	  case '-':
		minus++;

	  case '+':
		p++;
	}

	/* skip blanks after sign */
	while (c = *p)
	{
		if (c != ' ')
			break;
		p++;
	}

	/* start collecting the number to the decimal point */
	gotmant = 0;
	for ( ; ; )
	{
		c = *p;
		if (c < '0' || c > '9')
			break;
		v = v * 10.0 + (c - '0');
		gotmant++;
		p++;
	}

	/* check for fractional part */
	if (c == '.')
	{
		fact = 1.0;
		for ( ; ; )
		{
			c = *++p;
			if (c < '0' || c > '9')
				break;
			fact *= 0.1;
			v += (c - '0') * fact;
			gotmant++;
		}
	}

	/* skip blanks before possible exponent */
	while (c = *p)
	{
		if (c != ' ')
			break;
		p++;
	}

	/* test for exponent */
	if (c == 'e' || c == 'E')
	{
		p++;
		if (AA_atoi(p, &expon))
			return (-1);
		if (!gotmant)
			v = 1.0;
		fact = expon;
		v *= pow(10.0, fact);
	}
	else
	{
		/* if no exponent, then nothing */
		if (c)
			return (-1);
	}

	/* store the result and exit */
	if (minus)
		v = -v;
	*val = v;
	return (0);
}
