## shared adr

## char		NAME[51];
## char		VERTRAG[8];
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
	char		outbuf[BUFSIZ];

	setbuf(stdout, outbuf);
	fprintf(stderr, "\t|---------------------------------------\n");
##	open adr
##	open dis

##	retrieve dis
##	(VERTRAG = $vertrag)
##	{
##		retrieve adr $name = $name and $kurz = $kurz
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
			pr_sadr();
			AAcat("q1");
			pr_adr("");
			printf("\t\t\t\t\t\t20. Juni 1989\n");
			printf("\n\n\n");
			printf("Betreff:  Vertrag %s zu ING_DB 6.3\n", VERTRAG);
			printf("--------  Auslieferung Release 4\n");
			printf("\n\n\n");
			printf("Werte(r) %s %s\n\n", TITEL, NAME);
			AAcat("q2");
			pr_adr("	|  ");
			AAcat("q3");
			putchar('\f');
##		}
##	}

##	close dis
##	close adr
	fflush(outbuf);
}


pr_adr(msg)
register char	*msg;
{
	printf("%s%s\n", msg, KURZ);
	if (*LANG)
		printf("%s%s\n", msg, LANG);
	if (*ABT)
		printf("%s%s\n", msg, ABT);
	if (*TITEL)
		printf("%s%s %s\n", msg, TITEL, NAME);
	printf("%s%s\n", msg, STR);
	printf("%s%s\n", msg, ORT);
	printf("%s%s\n", msg, PLZ);
}


pr_sadr()
{
	fprintf(stderr, "\t|\n");
	fprintf(stderr, "\t|  %s%s\n", KURZ);
	if (*LANG)
		fprintf(stderr, "\t|  %s\n", LANG);
	if (*ABT)
		fprintf(stderr, "\t|  %s\n", ABT);
	if (*TITEL)
		fprintf(stderr, "\t|  %s %s\n", TITEL, NAME);
	fprintf(stderr, "\t|  %s\n", STR);
	fprintf(stderr, "\t|  %s\n", ORT);
	fprintf(stderr, "\t|  %s\n", PLZ);
	fprintf(stderr, "\t|\n");
	fprintf(stderr, "\t|---------------------------------------\n");
}


AAcat(file)
register char	*file;
{
	register int	i;
	register FILE	*fd;
	char		buf[BUFSIZ];
	extern FILE	*fopen();

	fd = fopen(file, "r");
	setbuf(fd, buf);
	while ((i = getc(fd)) > 0)
		putchar(i);
	fclose(fd);
}
