# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/symbol.h"
# include	"../h/pipes.h"
# include	"IIglobals.h"

/*
**	IIwrite is used to write a string to the
**	QL parser
*/
IIwrite(str)
register char	*str;
{
	register int	i;

#	ifdef xETR1
	if (IIdebug)
		printf("WRITE: string \"%s\"\n", str);
#	endif
	if (!IIqlpid)
		IIsyserr("Es ist noch kein ## DBS-statement aktiv");
	if (IIin_retrieve)
		IIsyserr("QL-Statement innerhalb ## RETRIEVE");

	if (i = IIlength(str))
		if (IIwrpipe(P_NORM, &IIoutpipe, IIw_down, str, i) != i)
			IIsyserr("WRITE - VALUE len=%d", i);
}
