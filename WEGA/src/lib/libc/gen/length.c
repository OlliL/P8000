/* @[$] length.c	2.2  09/12/83 11:26:18 - 87wega3.2 */

_length(str) 
register char *str;
{
	register char *tmp;
	register int i;

	i=0;
	do {
		if(*str++)
			break;
		i++;
	} while (i<=16384);

	return(i);
}
