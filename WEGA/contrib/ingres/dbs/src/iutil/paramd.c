# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/catalog.h"
# include	"../h/access.h"

/*
**	get access parameters of a relation from its descriptor and return
**	them in struct pointed to by "ap".
*/
AAdparam(d, ap)
register DESC			*d;
register struct accessparam	*ap;
{
	register int			i;
	register int			p;

	ap->mode = AAgetmode(ctoi(d->reldum.relspec));
	ap->sec_index = FALSE;	/* indicate that this isn't the index-rel */

	for (i = 0; i < MAXDOM + 1; i++)
		ap->keydno[i] = 0;

	for (p = 1; p <= d->reldum.relatts; p++)
		if (i = d->relxtra[p])
			ap->keydno[i - 1] = p;
	return (0);
}


AAiparam(d, ap)
register struct index		*d;
register struct accessparam	*ap;
{
	ap->mode = AAgetmode(ctoi(d->irelspeci));
	ap->sec_index = TRUE;	/* this is an index */

	AAbmove(d->idom, ap->keydno, MAXKEYS);
	ap->keydno[MAXKEYS] = 0;
	return (0);
}


AAgetmode(spec)
register int	spec;
{
	switch (abs(spec))
	{
	  case M_HEAP:
		return (NOKEY);

	  case M_ISAM:
		return (LRANGEKEY);

	  case M_HASH:
		return (EXACTKEY);

	  default:
		AAsyserr(15018, spec);
	}
	/*NOTREACHED*/
}
