# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/catalog.h"

/*
**  GET_P_TID -- Get the primary tid for a relation for locking
**
**	Finds the correct tid for locking the relation. If the
**	relation is a primary relation, then the tid of the
**	relation is returned.
**
**	If the relation is a secondary index then the tid of the
**	primary relation is returned.
**
**	Parameters:
**		des - an open descriptor for the relation.
**		tidp - a pointer to a place to store the tid.
**
**	Side Effects:
**		alters the value stored in "tidp",
**		may cause access to the AA_INDEX relation
*/
get_p_tid(des, tidp)
DESC			*des;
struct tup_id		*tidp;
{
	register DESC			*d;
	register struct tup_id		*tp;
	register int			i;
	struct index			indkey;
	struct index			itup;
	DESC				ides;
	extern DESC			Inddes;

	d = des;
	tp = tidp;
	if (ctoi(d->reldum.relindxd) == SECINDEX)
	{
		/* this is a secondary index. lock the primary rel */
		opencatalog(AA_INDEX, 0);
		AAam_setkey(&Inddes, &indkey, d->reldum.relowner, IOWNERP);
		AAam_setkey(&Inddes, &indkey, d->reldum.relid, IRELIDI);
		if (AAgetequal(&Inddes, &indkey, &itup, tp))
			AAsyserr(11029, d->reldum.relid);

		if (i = AArelopen(&ides, -1, itup.irelidp))
			AAsyserr(11030, i, itup.irelidp);

		AAbmove(&ides.reltid, tp, sizeof (*tp));
	}
	else
		AAbmove(&d->reltid, tp, sizeof (*tp));
}
