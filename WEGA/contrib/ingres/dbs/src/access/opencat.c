# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/bs.h"
# include	"../h/access.h"

int			Files[MAXFILES];

/*
**  OPENCATALOG: -- open system catalog
**
**	This routine opens a system catalog into a predetermined
**	cache.  If the catalog is already open, it is not reopened.
**
**	The 'Desxx' struct defines which relations may be opened
**	in this manner and is defined in h/aux.h.
**
**	The relation should not be closed after use (except in
**	special cases); however, it should be AAnoclose'd after use
**	if the number of tuples in the catalog may have changed.
**
**	The Desxx structure has an alias field which
**	is the address of the 'AAdmin' structure cache which holds
**	the relation descriptor.  Thus, an AArelopen need never actually
**	occur.
**
**	The actual desxx structure definition is in the file
**		catalog_desc.c
**
**	which defines which relations can be cached and if any
**	alias descriptors exist for the relations. That file
**	can be redefined to include various caching.
**
**	Parameters:
**		n    -- the name of the relation to open.  It must
**			match one of the names in the Desxx
**			structure.
**		mode -- just like 'mode' to AArelopen.  If zero, it
**			is opened read-only; if two, it is opened
**			read/write.  In fact, the catalog is always
**			opened read/write, but the flags are set
**			right for concurrency to think that you are
**			using it as you have declared.
*/
opencatalog(n, mode)
register char	*n;
int		mode;
{
	register DESC			*d;
	register struct desxx		*p;
	register int			i;
	extern struct desxx		Desxx[];

	/* find out which descriptor it is */
	for (p = Desxx; p->cach_relname; p++)
		if (AAsequal(n, p->cach_relname))
			break;
	if (!p->cach_relname)
		AAsyserr(10014, n);

	/* if it's already open, just return */
	if ((d = p->cach_desc)->relopn)
		AAam_clearkeys(d);
	else
	{
		/* not open, open it */
		if (p->cach_alias)
		{
			AAam_start();
			AAbmove(p->cach_alias, d, sizeof (*d));
		}
		else if (i = AArelopen(d, 2, n))
			AAsyserr(10015, i, n);

		/* mark it as an open file */
		i = d->relfp;
		Files[i] = 0;
	}

	/* determine the mode to mark it as for concurrency */
	switch (mode)
	{
	  case 0:	/* read only */
		d->relopn = RELROPEN(d->relfp);
		break;

	  case 2:	/* read-write */
		d->relopn = RELWOPEN(d->relfp);
		break;

	  default:
		AAsyserr(10016, mode, n);
	}
}
