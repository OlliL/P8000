# include	<stdio.h>

## char		TITEL[61];
## int		LINE;
## char		TYP[61];
## char		AU[61];
## long		ID;
## long		NID;

main()
{
	register char	*p;
	register int	cnt;

##	ing_db ingres
##	range of d is doc
##	range of a is au

	ID = -1;
	for ( ; ; )
	{
		cnt = 0;
##		retrieve
##		(
##			NID   = d.id,
##			LINE  = d.line,
##			TITEL = d.titel,
##			TYP   = d.typ
##		)
##		where d.id > ID
##		{
			if (cnt++ != LINE)
				break;

			ID = NID;
			format(TITEL, 59);
			if (LINE == 0)
			{
				format(TYP, 59);
				printf("%s * University of California, Berkeley\n", TYP);
			}
			printf("\t%s\n", TITEL);
##		}

		if (cnt == 0)
			break;

		cnt = 0;
##		retrieve (AU = a.au) where a.id = ID
##		{
			if (cnt++ == 0)
				putchar('\t');
			else
				printf(", ");
			format(AU, 49);
			printf("%s", AU);
##		}
		printf("\n\n");
	}
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


format(msg, len)
register char	*msg;
register int	len;
{
	register char	*p;

	p = &msg[len];
	while (*p == ' ')
		--p;
	p[1] = 0;
}
