/*
** ---  RG_TIME.C ------- GET AND PRINT TIME  ---------------------------------
**
**	Version: @(#)rg_time.c		4.0	02/05/89
**
*/

# include	"rg.h"


extern long	time();
static long	rg_time;
extern char	*ctime();
static char	*rg_ctime;
extern int	*localtime();
static int	*rg_localtime;


rg_prtime(tflag)
register int	tflag;
/*		tflag == FALSE:  DATE  */
/*			 TRUE:   TIME  */
{
	register char	*t;
	register char	*p;
	extern char	*AAbmove();

	if (!rg_time)
	{
		time(&rg_time);
# ifdef ESER_VMX
		rg_ctime = ctime(rg_time);
		rg_localtime = localtime(rg_time);
# else
		rg_ctime = ctime(&rg_time);
		rg_localtime = localtime(&rg_time);
# endif
	}
	t = rg_ctime;
	p = SYS_col;

	if (tflag)
		SYS_col = AAbmove(&t[11], p, 8);
	else
	{
		p = AAbmove(&t[8], p, 2);
		*p++ = '-';
		p = AAbmove(&t[4], p, 3);
		*p++ = '-';
		SYS_col = AAbmove(&t[22], p, 2);
	}
}


rg_getime(which)
/*----------------------------------------------*/
register int	which;
/*----------------------------------------------*/
/*	SECOND	==  0				*/
/*	MINUTE	==  1				*/
/*	HOUR	==  2				*/
/*	M_DAY	==  3	month day		*/
/*	MONTH	==  4				*/
/*	YEAR	==  5				*/
/*	W_DAY	==  6	week day 		*/
/*	Y_DAY	==  7	year day 		*/
/*	IS_DST	==  8	day light saving flag	*/
/*----------------------------------------------*/
{
	if (!rg_time)
	{
		time(&rg_time);
# ifdef ESER_VMX
		rg_ctime = ctime(rg_time);
		rg_localtime = localtime(rg_time);
# else
		rg_ctime = ctime(&rg_time);
		rg_localtime = localtime(&rg_time);
# endif
	}
	return (rg_localtime[which]);
}
