/*
**  PRARGS -- print argument list
**
**	Takes an argument list such as expected by any main()
**	or the DBU routines and prints them on the standard
**	output for debugging purposes.
**
**	Parameters:
**		n -- parameter count.
**		p -- parameter vector (just like to main()).
**
*/

AAprargs(n, p)
register int	n;
register char	**p;
{
	register int	c;
	register char	*q;

	printf("#args=%d:\n", n);
	for ( ; n-- > 0; p++)
	{
		q = *p;
		while (c = *q++)
			AAxputchar(c);
		putchar('\n');
	}
}
