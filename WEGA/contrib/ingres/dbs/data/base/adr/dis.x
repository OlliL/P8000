## shared adr

## char		KURZ[51];
## char		PC[51];
## short	STD;
## short	TAG;
## short	MON;
## short	JAHR;
## char		DBS[2];
## char		PQR[2];
## char		AMX[2];

amx()
{
	register int	cnt;

##	open dis
	fprintf(stderr, "DISTRIBUTION\n\n");

	while (enter("\n        Institution", KURZ))
	{
		cnt = 0;
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
			cnt++;
			tuple("EXIST");
##		}
		if (cnt)
			continue;
		if (enter("       Computer-Typ", PC) == 0)
			continue;
		if (i_enter("                Tag", &TAG) == 0)
			continue;
		if (i_enter("              Monat", &MON) == 0)
			continue;
		if (i_enter("               Jahr", &JAHR) == 0)
			continue;
		if (i_enter("             Stunde", &STD) == 0)
			continue;
		if (enter("ING_DB System [y/n]", DBS) == 0)
			continue;
		if (enter("   PQR System [y/n]", PQR) == 0)
			continue;
		if (enter("   AMX System [y/n]", AMX) == 0)
			continue;
##		use (+dis)
##		append dis
##		(
##			$kurz = KURZ,
##			$pc = PC,
##			$std = STD,
##			$tag = TAG,
##			$mon = MON,
##			$jahr = JAHR,
##			$dbs = DBS,
##			$pqr = PQR,
##			$amx = AMX
##		)
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
