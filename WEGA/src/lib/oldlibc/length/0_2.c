_length(str) 
register char *str;
{
	register int i;

	i=0;
	do {
		if(*str++ == 0)
			break;
		i++;
	} while (i<=512);

	return(i);
}
