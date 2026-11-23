/* toupper and tolower in one module (System III toupper.c, tolower.c), as in /bin/ex (07/82); ccv0 */

toupper(c)
register int c;
{
	if(c >= 'a' && c <= 'z')
		c += 'A' - 'a';
	return c;
}

tolower(c)
register int c;
{
	if(c >= 'A' && c <= 'Z')
		c -= 'A' - 'a';
	return c;
}
