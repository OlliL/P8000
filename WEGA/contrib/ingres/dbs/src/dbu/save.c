# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/bs.h"
# include	<sys/timeb.h>

# ifdef ESER_PSU
# define	PRIME_DATE	1980
# else
# define	PRIME_DATE	1970
# endif

# ifdef DYSIZE
# define	dysize(y)	((!(y%4) && y%100 || !(y%400))? 366: 365)
# endif

static int	Dmsize[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

struct monthtab
{
	char	*code;
	int	month;
};

static struct monthtab	Monthtab[] =
{
	"jan",		1,
	"feb",		2,
	"mar",		3,
	"apr",		4,
	"may",		5,
	"jun",		6,
	"jul",		7,
	"aug",		8,
	"sep",		9,
	"oct",		10,
	"nov",		11,
	"dec",		12,
	"january",	1,
	"february",	2,
	"march",	3,
	"april",	4,
	"june",		6,
	"july",		7,
	"august",	8,
	"september",	9,
	"october",	10,
	"november",	11,
	"december",	12,
	(char *) 0
};

/*
**  SAVE RELATION UNTIL DATE
**
**	This function arranges to save a named relation until a
**	specified date.
**
**	Parameters:
**		0 -- relation name
**		1 -- month (1 -> 12 or "jan" -> "dec" or a variety
**			of other codes)
**		2 -- day (1 -> 31)
**		3 -- year (PRIME_DATE -> ?)
**
**	Returns:
**		zero on success
**		nonzero otherwise.
**
**	Side Effects:
**		Change expiration date for some relation in
**		relation relation.
*/
save(parmc, parmv)
int	parmc;
char	**parmv;
{
	long				date;
	register int			i;
	extern DESC			Reldes;
	struct tup_id			tid;
	struct relation			relk;
	struct relation			relt;
	short				day;
	short				month;
	short				year;
	struct timeb			timeb;
	extern char			*AAusercode;
	extern char			*ctime();
# ifndef DYSIZE
	extern int			dysize();	/* defined in ctime(3) */
# endif

	/* validate parameters */
	if (AA_atoi(parmv[3], &year) || year < PRIME_DATE)
		return (error(5603, parmv[3], (char *) 0));	/* bad year */
	if (monthcheck(parmv[1], &month))
		return (error(5601, parmv[1], (char *) 0));	/* bad month */
	if (AA_atoi(parmv[2], &day) || day < 1 || day > monthsize(--month, year))
		return (error(5602, parmv[2], (char *) 0));	/* bad day */

	/* convert date */
	/* "date" will be # of days from PRIME_DATE for a while */
	date = 0;

	/* do year conversion */
	for (i = PRIME_DATE; i < year; i++)
	{
		date += dysize(i);
	}

	/* do month conversion */
	for (i = 0; i < month; i++)
		date += Dmsize[i];
	/* once again, allow for leapyears */
	if (month >= 2 && dysize(year) == 366)
		date += 1;

	/* do day conversion */
	date += day - 1;

	/* we now convert date to be the # of hours since PRIME_DATE */
	date *= 24;

	/* do daylight savings computations */
	/*  <<<<< none now >>>>> */

	/* convert to seconds */
	date *= 60 * 60;

	/* adjust to local time */
	ftime(&timeb);
	date += ((long) timeb.timezone) * 60;

#	ifdef xZTR1
# ifdef ESER_VMX
	AAtTfp(4, 1, "%s", ctime(date));
# else
	AAtTfp(4, 1, "%s", ctime(&date));
# endif
#	endif

	/* let's check and see if the relation exists */
	opencatalog(AA_REL, 2);
	AAam_clearkeys(&Reldes);
	AAam_setkey(&Reldes, &relk, parmv[0], RELID);
	AAam_setkey(&Reldes, &relk, AAusercode, RELOWNER);
	if (AAgetequal(&Reldes, &relk, &relt, &tid))
		return (error(5604, parmv[0], (char *) 0));	/* relation not found */

	/* check that it is not a system catalog */
	if (relt.relstat & S_CATALOG)
		return (error(5600, parmv[0], (char *) 0));	/* cannot save sys rel */
	/* got it; lets change the date */
	relt.relsave = date;

#	ifdef xZTR2
	if (AAtTf(4, 2))
		AAprtup(&Reldes, &relt);
#	endif

	if ((i = AAam_replace(&Reldes, &tid, &relt, 0)) < 0)
		AAsyserr(11158, i);

	/* that's all folks.... */
	return (0);
}


monthcheck(input, output)
char	*input;
short	*output;
{
	register struct monthtab	*p;
	short				month;

	/* month can be an integer, or an alphanumeric code */
	if (!AA_atoi(input, &month))
	{
		*output = month;
		return (month < 1 || month > 12);
	}
	for (p = Monthtab; p->code; p++)
	{
		if (AAsequal(input, p->code))
		{
			*output = p->month;
			return (0);
		}
	}
	return (1);
}


/*
**  MONTHSIZE -- determine size of a particular month
*/
monthsize(month, year)
int	month;
int	year;
{
	register int	size;
	extern int	Dmsize[12];
# ifndef DYSIZE
	extern int	dysize();	/* defined in ctime(3) */
# endif

	size = Dmsize[month];
	if (month == 1 && dysize(year) == 366)
		/* This is February of a leap year */
		size++;

	return (size);
}
