# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"../h/pipes.h"
# include	"../h/bs.h"
# include	"decomp.h"


derror(eno)
{
	endovqp(NOACK);
	error(eno, (char *) 0);
	if (eno == QBUFFULL)
		clearpipe();	/* if overflow while reading pipe, flush the pipe */
	reinit();
	writeback(0);
	reset();
}



clearpipe()
{
	rdpipe(P_SYNC, &Inpipe, R_up);
}
