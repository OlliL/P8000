/*
**  ASCII CHARACTER STRING TO 32-BIT INTEGER CONVERSION
**
**	`a' is a pointer to the character string, `i' is a
**	pointer to the doubleword which is to contain the result.
**
**	The return value of the function is:
**		zero:	succesful conversion; `i' contains the integer
**		+1:	numeric overflow; `i' is unchanged
**		-1:	syntax error; `i' is unchanged
**
**	A valid string is of the form:
**		<space>* [+-] <space>* <digit>* <space>*
*/
AA_atol(a, i)
register char	*a;
long		*i;
{
	register int	c;
	register int	sign;	/* flag to indicate the sign */
	long		x;	/* holds the integer being formed */

	sign = 0;
	/* skip leading blanks */
	while (*a == ' ')
		a++;
	/* check for sign */
	switch (*a)
	{
	  case '-':
		sign = -1;

	  case '+':
		while (*++a == ' ')
			continue;
	}

	/* at this point everything had better be numeric */
	x = 0;
	while ((c = *a) <= '9' && c >= '0')
	{
		/* the code below is the contortion for: */
		/* if (x > 2147483647 / 10) */
		if (x > 214748364)	/* check if mult by 10 will overflow */
			return (1);
		x = x * 10 + (c - '0');
		if (x < 0)	/* check if new digit caused overflow */
			return (1);
		a++;
	}

	/* eaten all the numerics; better be all blanks */
	while (c = *a++)
		if(c != ' ')			/* syntax error */
			return (-1);
	*i = sign? -x: x;
	return (0);		/* successful termination */
}
