# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"monitor.h"

/*
**  Clear query buffer
**	Flag f is set if called explicitly (with \q) and is
**	clear if called automatically.
*/
clear(f)
register int	f;
{
	Autoclear = 0;

	/* TRUNCATE FILE & RETURN */
	if (!freopen(Qbname, "w", Qryiop))
		AAsyserr(16001);

	if (Nodayfile >= 0 && f)
		prompt(EMPTY);

	if (f)
		clrline(TRUE);

	Notnull = 0;
}
