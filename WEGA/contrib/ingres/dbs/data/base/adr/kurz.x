## shared adr

## char		NAME[51];
## char		TITEL[51];
## char		KURZ[51];
## char		LANG[51];
## char		ABT[51];
## char		STR[51];
## char		ORT[51];
## char		PLZ[51];

amx()
{
	register char	*p;

##	open adr
	fprintf(stderr, "ADRESSENDRUCK\n\n");

	while (enter("\nKURZ", KURZ))
	{
##		retrieve adr $kurz = KURZ
##		(
##			NAME = $name,
##			TITEL = $titel,
##			KURZ = $kurz,
##			LANG = $lang,
##			ABT = $abt,
##			STR = $str,
##			ORT = $ort,
##			PLZ = $plz
##		)
##		{
			printf("\n\n\n\t%s\n", KURZ);
			if (*LANG != ' ')
				printf("\t%s\n", LANG);
			if (*ABT != ' ')
				printf("\t%s\n", ABT);
			if (*TITEL != ' ')
				printf("\t%s %s\n", TITEL, NAME);
			printf("\t%s\n", STR);
			printf("\t%s\n", ORT);
			printf("\t------------------------\n");
			printf("\t%s\n", PLZ);
##		}
	}
##	close adr
}


enter(msg, buf)
register char	*msg;
register char	*buf;
{
	register char	*p;
	register int	c;

	fprintf(stderr, "%s:\t", msg);
	p = buf;
	while ((c = getchar()) >= 0)
	{
		if (c == '\n')
			break;
		*p++ = c;
	}
	*p = 0;
	return (p - buf);
}
