# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/access.h"

/*
**  CLOSECATALOG -- close system catalog
**
**	This routine closes the sysetm relations opened by calls
**	to opencatalog.
**
**	The 'Desxx' struct defines which relations will be closed
**	in this manner and is defined in h/aux.h.
**
**	The actual desxx structure definition is in the file
** 		catalog_desc.c
**
**	which defines which relations can be cached and if any
**	alias descriptors exist for the relations. That file
**	can be redefined to include various caching.
**
**	Parameters:
** 		really - whether to actually close the relations
** 			or just update and flush them.
**
**	Side Effects:
** 		A relation is (may be) closed and its pages flushed
*/
closecatalog(really)
register int	really;
{
	register struct desxx		*p;
	extern struct desxx		Desxx[];

	for (p = Desxx; p->cach_relname; p++)
		if (really && !p->cach_alias)
		{
			if (AArelclose(p->cach_desc) < 0)
				AAsyserr(10009, p->cach_relname);
		}
		else
		{
			if (AAnoclose(p->cach_desc) < 0)
				AAsyserr(10009, p->cach_relname);
			if (really)
				p->cach_desc->relopn = 0;
		}
}
