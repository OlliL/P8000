# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"
# include	"../h/access.h"
# include	"../h/batch.h"
# include	"../h/pipes.h"
# include	"ovqp.h"

copyreturn()
{
	/* copy information from decomp back to parser */
	rdpipe(P_PRIME, &Inpipe);
	wrpipe(P_PRIME, &Outpipe, EXEC_PARSER, (char *) 0, 0);
	copypipes(&Inpipe, R_down, &Outpipe, W_up);
}
