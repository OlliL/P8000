# include	<stdio.h>

/*							*/
/*	Call:						*/
/*		if (AAtTf(m, n)) ...			*/
/*		Tests bit n of trace flag m		*/
/*		(or any bit of m if n < 0)		*/
/*							*/
AAtTf(m, n)
register int	m;
register int	n;
{
	register int	i;
	extern int	AAtTany;
	extern short	AAtT[];

	if (!AAtTany)
		return (0);

	if (i = n < 0? AAtT[m]: (AAtT[m] >> n) & 01)
		fflush(stdout);
	return (i);
}
