# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/tree.h"
# include	"../h/pipes.h"
# include	"../h/symbol.h"
# include	"parser.h"

/*
** XDOT
**	add to attribute stash any missing attributes in the
**	source relation and then build tree with all attribs
**	in the 'attid' order.  This algorithm assumes that
**	the function 'attadd' insert attributes into the list
**	in 'attid' order from 1 -> N.
*/
QTREE			*xdot(rptr)
struct rngtab		*rptr;
{
	struct attribute		tuple;
	register struct attribute	*ktuple;
	struct attribute		ktup;
	struct tup_id			tid;
	struct tup_id			limtid;
	QTREE				*tempt;
	register QTREE			*vnode;
	int				ik;
	register struct atstash		*aptr;
	extern QTREE			*tree();
	extern QTREE			*addresdom();

#	ifdef	xPTR2
	AAtTfp(15, 0, "ALL being processed for %s\n", rptr->relnm);
#	endif

	/* if attstash is missing any attribs then fill in list */
	if (rptr->ratts != attcount(rptr))
	{
		/* get all entries in attrib relation */
		AAam_clearkeys(&Desc);
		ktuple = &ktup;
		AAam_setkey(&Desc, ktuple, rptr->relnm, ATTRELID);
		AAam_setkey(&Desc, ktuple, rptr->relnowner, ATTOWNER);
		if (ik = AAam_find(&Desc, EXACTKEY, &tid, &limtid, ktuple))
			AAsyserr(18024, ik);
		while (!AAam_get(&Desc, &tid, &limtid, &tuple, 1))
			if (!AAkcompare(&Desc, &tuple, ktuple))
				/* add any that are not in the stash */
				if (!attfind(rptr, tuple.attname))
					attadd(rptr, &tuple);
	}

	/* build tree for ALL */
	tempt = (QTREE *) 0;
	aptr = rptr->attlist;
	while (aptr != 0)
	{
		vnode = tree((QTREE *) 0, (QTREE *) 0, VAR, VAR_SIZ, CAST(rptr->rentno), aptr);
		Trname = aptr->atbname;
		tempt = addresdom(tempt, vnode);
		aptr = aptr->atbnext;
	}
	return (tempt);
}
