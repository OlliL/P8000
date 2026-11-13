/* @[$] tolower.c	2.1  09/12/83 11:37:39 - 87wega3.2 */

/*LINTLIBRARY*/

tolower(c)
register int c;
{
	if(c >= 'A' && c <= 'Z')
		c -= 'A' - 'a';
	return c;
}
