/* @[$] strspn.c	2.1  09/12/83 11:37:08 - 87wega3.2 */

/*
 * Return the number of characters in the maximum leading segment
 * of string which consists solely of characters from charset.
 */
int
strspn(string, charset)
char	*string;
register char	*charset;
{
	register char *p, *q;

	for(q=string; *q != '\0'; ++q) {
		for(p=charset; *p != '\0' && *p != *q; ++p)
			;
		if(*p == '\0')
			break;
	}
	return(q-string);
}
