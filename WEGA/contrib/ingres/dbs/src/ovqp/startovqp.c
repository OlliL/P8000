# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"
# include	"../h/pipes.h"
# include	"../h/aux.h"
# include	"ovqp.h"


/*
**	startovqp is called at the beginning of
**	the execution of ovqp.
*/
startovqp()
{
	extern			flptexcep();
	extern struct pipfrmt	Eoutpipe;

	if (Eql)	/* prime the write pipe to EQL */
		wrpipe(P_PRIME, &Eoutpipe, 0, (char *) 0, 0);

	Tupsfound = 0;	/* counts the number of tuples which sat the qual */
	Retrieve = Bopen = FALSE;
	/* catch floating point signals */
	signal(8, flptexcep);
}

/*
**	Give a user error for a floating point exceptions
*/
flptexcep()
{
	ov_err(FLOATEXCEP);
}
