## char		TITEL[61];
## int		LINE;
## char		TYP[61];
## char		VERLAG[61];
## char		ORT[61];
## char		JAHR[61];
## char		AU[61];
## int		ID;

main()
{
##	ing_db ingres
##	range of r is relation
##	retrieve (ID = r.reltups) where r.relid = "doc"
##	{
		printf("ENTER INGRES DOCUMENT\n\n");
##	}
	ID++;
	while (enter("\nJAHR", JAHR))
	{
		LINE = 0;
		*TYP = 0;
		*VERLAG = 0;
		*ORT = 0;
		*AU = 0;
		if (enter("TYP", TYP) == 0)
			continue;
		if (enter("VERLAG", VERLAG) == 0)
			continue;
		if (enter("ORT", ORT) == 0)
			continue;
		while (enter("TITEL", TITEL))
		{
##			append doc
##			(
##				line = LINE,
##				titel = TITEL,
##				typ = TYP,
##				verlag = VERLAG,
##				ort = ORT,
##				jahr = JAHR,
##				id = ID
##			)
			LINE++;
		}
		while (enter("AUTOR", AU))
		{
##			append au
##			(
##				au = AU,
##				id = ID
##			)
		}
		ID++;
	}
}


enter(msg, buf)
register char	*msg;
register char	*buf;
{
	register char	*p;
	register int	c;

	printf("%s: ", msg);
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
