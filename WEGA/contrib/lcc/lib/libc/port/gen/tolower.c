/*
 * If arg is upper-case, return the lower-case, else return the arg.
 */

int
tolower(c)
register int c;
{
	if(c >= 'A' && c <= 'Z')
		c -= 'A' - 'a';
	return(c);
}
