## shared adr

## char		NAME[51];
## char		TITEL[51];
## char		KURZ[51];

amx()
{
	register char	*p;

##	open adr
	fprintf(stderr, "DRUCK adr\n\n");

##	retrieve adr
##	(
##		NAME = $name,
##		TITEL = $titel,
##		KURZ = $kurz
##	)
##	{
		printf("%-50s", KURZ);
		if (*TITEL != ' ')
			printf(" %s %s\n", TITEL, NAME);
##	}
##	close adr
}
