# include	"AA_liba.h"

/* THIS MODULE checks all PERMITS */

# ifndef MSDOS
/* check for correct user		*/
AAPuser()
{
	register int		uid;
	extern int		AAuid;

	uid= getuid();
# ifdef AMX_TRACE
	AAtTfp(38, 0, "PERMIT(user %d) found %d\n", AAuid, uid);
# endif
	if (AAuid != uid)
		AAerror(110, "user");
}


/* check for correct group		*/
AAPgroup()
{
	register int		gid;
	extern int		AAgid;

	gid = getgid();
# ifdef AMX_TRACE
	AAtTfp(39, 0, "PERMIT(group %d) found %d\n", AAgid, gid);
# endif
	if (AAgid != gid)
		AAerror(110, "group");
}


# ifdef	AMX_QRYMOD
/* check for correct terminal		*/
/* returns 0: violation			*/
/*         1: ok			*/
AAPterm(t)
register char	*t;
{
	register char		*tty;
	register int		i;
	char			term[PROTERM + 1];
	extern char		*ttyname();

	/* determine user's terminal for protection algorithm */
# ifdef BIT_32
	tty = "####";
# else
	tty = "##";
# endif
	for (i = 0; i < 3; i++)
		if (isatty(i))
		{
			tty = ttyname(i);
			break;
		}
	if (AAbequal(tty, "tty", 3))
		tty = &tty[3];
	else if (AAbequal(tty, "/dev/tty", 8))
		tty = &tty[8];
	AApmove(tty, term, PROTERM, ' ');
	term[PROTERM] = 0;
# ifdef AMX_TRACE
	AAtTfp(40, 0, "PERMIT(term %s) found %s\n", t, term);
# endif
	if (!AAbequal(t, term, PROTERM))
		return (0);
	return (1);
}


/* check for correct day of the week	*/
/* returns 0: violation			*/
/*         1: ok			*/
AAPday(f, t)
register int	f;
register int	t;
{
	register int	d;

	d = getdate(1);
# ifdef AMX_TRACE
	AAtTfp(41, 0, "PERMIT(day %d to %d) found %d\n", f, t, d);
# endif
	return (check(f, d, t));
}


/* check for correct time of the day	*/
/* returns 0: violation			*/
/*         1: ok			*/
AAPtime(f, t)
register int	f;
register int	t;
{
	register int	d;

	d = getdate(0);
# ifdef AMX_TRACE
	AAtTfp(42, 0, "PERMIT(time %d to %d) found %d\n", f, t, d);
# endif
	return (check(f, d, t));
}


static int	check(f, d, t)
register int	f;
register int	d;
register int	t;
{
	return (((f <= d) + (d <= t)) == ((f < t)? 2: 1));
}


static int	getdate(which)
register int	which;	/* which == 1: get day	*/
			/*       == 0: get time */
{
	register int	*local;
	long		long_time;
	extern long	time();
	extern int	*localtime();

	time(&long_time);
# ifdef ESER_VMX
	local = localtime(long_time);
# else
	local = localtime(&long_time);
# endif
/*----------------------------------------------------------------------*/
/* OFFSETS for local:							*/
/*----------------------------------------------------------------------*/
/*	SECOND	==  0				(0 -59)			*/
/*	MINUTE	==  1				(0 -59)			*/
/*	HOUR	==  2				(0 - 23)		*/
/*	M_DAY	==  3	month day		(1 - 31)		*/
/*	MONTH	==  4				(0 - 11)		*/
/*	YEAR	==  5				(year - 1900)		*/
/*	W_DAY	==  6	week day 		(0 - 6, 0 == sunday)	*/
/*	Y_DAY	==  7	year day 		(0 - 365)		*/
/*	IS_DST	==  8	day light saving flag	(!= 0: summer time)	*/
/*----------------------------------------------------------------------*/
	return (which? local[6]: local[1] + 60 * local[2]);
}
# endif
# endif
