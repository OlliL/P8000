# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/symbol.h"
# include	"../h/pipes.h"
# include	"IIglobals.h"

/*
**	IIsetup is called to mark the start of a retrieve.
*/
IIsetup()
{
#	ifdef xETR1
	if (IIdebug)
		printf("SETUP\n");
#	endif

	IIin_retrieve = 1;
	IIr_sym.type = 0;
	IIr_sym.len = 0;
	IIdoms = 0;
}
