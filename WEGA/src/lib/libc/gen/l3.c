/* @[$] l3.c	2.1  09/12/83 11:25:34 - 87wega3.2 */

/*
 * Convert longs to and from 3-byte disk addresses
 */
ltol3(cp, lp, n)
register char	*cp;
long	*lp;
register int	n;
{
	register char *b;

	b = (char *)lp;
	while (n--) {
		b++;
		*cp++ = *b++;
		*cp++ = *b++;
		*cp++ = *b++;
	}
}

l3tol(lp, cp, n)
long	*lp;
register char	*cp;
register int	n;
{
	register char *a;

	a = (char *)lp;
	while (n--) {
		*a++ = 0;
		*a++ = *cp++;
		*a++ = *cp++;
		*a++ = *cp++;
	}
}
