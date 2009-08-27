/* @[$] toupper.c	2.1  09/12/83 11:37:53 - 87wega3.2 */

/*LINTLIBRARY*/

toupper(c)
register int c;
{
	if(c >= 'a' && c <= 'z')
		c += 'A' - 'a';
	return c;
}
