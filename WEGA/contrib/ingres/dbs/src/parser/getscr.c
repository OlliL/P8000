# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/pipes.h"

/*
** GETSCR
**	returns a single character from the R_up pipe.
**	modes are:
**		0 = read normally
**		1 = prime the pipe
**		2 = sync (or flush) the pipe
*/
getscr(mode)
int	mode;
{
	extern int		Pctr;		/* vble for backup stack in scanner */
	static struct pipfrmt	a;
	register int		ctr;
	char			c;

	if (!mode)
	{
		ctr = rdpipe(P_NORM, &a, R_up, &c, sizeof (char));
		return (ctr ? c : 0);
	}
	if (mode == 1)
	{
		rdpipe(P_PRIME, &a);
		Pctr = 0;
		return (0);
	}
	if (mode == 2)
	{
#		ifdef xPTM
		if (AAtTf(76, 1))
			timtrace(2, 0);
#		endif
		rdpipe(P_SYNC, &a, R_up);
		return (0);
	}
	AAsyserr(18010, mode);
}
