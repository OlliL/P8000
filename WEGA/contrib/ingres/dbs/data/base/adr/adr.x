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
##	open adr
	fprintf(stderr, "ADRESSEINGABE\n\n");

	while (enter("\nNAME", NAME))
	{
		if (enter("TITEL", TITEL) == 0)
			continue;
		if (enter("KURZ", KURZ) == 0)
			continue;
		*LANG = 0;
		enter("LANG", LANG);
		if (enter("ABT", ABT) == 0)
			continue;
		if (enter("STR", STR) == 0)
			continue;
		if (enter("ORT", ORT) == 0)
			continue;
		if (enter("PLZ", PLZ) == 0)
			continue;
##		use (+adr)
##		append adr
##		(
##			$name = NAME,
##			$titel = TITEL,
##			$kurz = KURZ,
##			$lang = LANG,
##			$abt = ABT,
##			$str = STR,
##			$ort = ORT,
##			$plz = PLZ
##		)
##		commit
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
