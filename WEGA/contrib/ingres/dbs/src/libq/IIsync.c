# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/symbol.h"
# include	"../h/pipes.h"
# include	"IIglobals.h"

/*
**	IIsync is called to syncronize the running
**	of a query with the running of the EQL process.
**
**	The query is flushed and an EOP is written
**	to the QL parser.
**
**	The QL parser will write an end-of-pipe when
**	an operation is complete.
*/
IIsync(file_name, line_no)
register char	*file_name;
register int	line_no;
{
	if (IIproc_name = file_name)
		IIline_no = line_no;

#	ifdef xETR1
	if (IIdebug)
		printf("SYNC\n");
#	endif

	IIwrpipe(P_END, &IIoutpipe, IIw_down);
	IIwrpipe(P_PRIME, &IIoutpipe, IIw_down);

	IIerrflag = 0;	/* reset error flag. If an error occures, */
			/* IIerrflag will get set in IIerror      */
	IIrdpipe(P_SYNC, &IIinpipe, IIr_down);
	IIrdpipe(P_PRIME, &IIinpipe);
}
