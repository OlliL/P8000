## shared adr

## char		KURZ[51];
## short	STD;
## short	TAG;
## short	MON;
## short	JAHR;
## char		DBS[2];
## char		PQR[2];
## char		AMX[2];

amx()
{
	int		num;
	char		buf[51];

##	open dis
	fprintf(stderr, "UPDATE DISTRIBUTION\n\n");

	while (enter("\n        Institution", KURZ))
	{
##		use (+dis)
##		retrieve dis $kurz = KURZ
##		(
##			STD = $std,
##			TAG = $tag,
##			MON = $mon,
##			JAHR = $jahr,
##			DBS = $dbs,
##			PQR = $pqr,
##			AMX = $amx
##		)
##		{
			tuple("OLD");
			if (i_enter("                Tag", &num))
				TAG = num;
			if (i_enter("              Monat", &num))
				MON = num;
			if (i_enter("               Jahr", &num))
				JAHR = num;
			if (i_enter("             Stunde", &num))
				STD = num;
			if (enter("ING_DB System [y/n]", buf))
				AAsmove(buf, DBS);
			if (enter("   PQR System [y/n]", buf))
				AAsmove(buf, PQR);
			if (enter("   AMX System [y/n]", buf))
				AAsmove(buf, AMX);
##			replace
##			(
##				$std = STD,
##				$tag = TAG,
##				$mon = MON,
##				$jahr = JAHR,
##				$dbs = DBS,
##				$pqr = PQR,
##				$amx = AMX
##			)
			tuple("NEW");
##		}
##		commit
	}
##	close dis
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


i_enter(msg, num)
register char	*msg;
register int	*num;
{
	char		buf[51];

	if (enter(msg, buf))
	{
		*num = atoi(buf);
		return (1);
	}
	return (0);
}


tuple(msg)
register char	*msg;
{
	printf("\n%s: %2d.%2d.%4d %2d Uhr", msg, TAG, MON, JAHR, STD);
	if (*DBS == 'y')
		printf(" ING_DB");
	if (*PQR == 'y')
		printf(" PQR");
	if (*AMX == 'y')
		printf(" AMX");
	printf("\n\n");
}
