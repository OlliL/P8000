# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"
# include	"../h/pipes.h"
# include	"IIglobals.h"

/*
**	IIflushtup is called to syncronize the data pipe
**	after a retrieve.
*/
IIflushtup(file_name, line_no)
register char	*file_name;
register int	line_no;
{
	struct retcode		ret;

	if (IIproc_name = file_name)
		IIline_no = line_no;

#	ifdef xETR1
	if (IIdebug)
		printf("FLUSHTUP: IIerrflag %d\n", IIerrflag);
#	endif

	if (IIerrflag < 2000)
	{
		/* flush the data pipe */
		IIrdpipe(P_SYNC, &IIeinpipe, IIr_front);
		IIrdpipe(P_PRIME, &IIeinpipe);

		/* flush the control pipe */
		if (IIrdpipe(P_NORM, &IIinpipe, IIr_down, &ret, sizeof (ret)) == sizeof (ret))
		{
			/* there was a tuple count field */
			IItupcnt = ret.rc_tupcount;
		}
		IIrdpipe(P_SYNC, &IIinpipe, IIr_down);
		IIrdpipe(P_PRIME, &IIinpipe);
	}
	IIin_retrieve = 0;
	IIndoms = 0;
	IIdoms = 0;
}
