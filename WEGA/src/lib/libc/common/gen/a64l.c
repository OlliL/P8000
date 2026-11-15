/* @[$] a64l.c	2.1  09/12/83 11:18:54 - 87wega3.2. */

/*
 * convert base 64 ascii to long int
 * char set is [./0-9A-Za-z]
 */

long
a64l(s)
register char *s;
{
	register i, c;
	long l;

	i = 0;
	l = 0;
	while ((c = *s++) != '\0') {
		if (c >= 'a')
			c -= 'a'-'Z'-1;
		if (c >= 'A')
			c -= 'A'-'9'-1;
		l |= (long)(c - '.') << i;
		i += 6;
	}
	return l;
}
