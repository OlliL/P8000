# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"../h/lock.h"

/*
** REINIT -- reinitialize decomp upon end of query, error, or interrupt.
**	All open relations are closed, temp relations destroyed,
**	and relation locks released.
*/

reinit()
{
	closers();
	/* release all relation locks */
	AAunlall();
}
