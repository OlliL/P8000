/* @[$] strlen.c	2.1  09/12/83 11:31:59 - 87wega3.2 */

/*
 * Returns the number of
 * non-NULL bytes in string argument.
 */

strlen(s)
register char *s;
{
	register n;

	n = 0;
	while (*s++)
		n++;
	return(n);
}
