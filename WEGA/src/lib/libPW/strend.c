char *strend(p)
register char *p;
{
	while (*p++)
		;
	return(--p);
}
