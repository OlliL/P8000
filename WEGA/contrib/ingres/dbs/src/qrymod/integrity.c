# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/catalog.h"
# include	"../h/access.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"qrymod.h"

extern DESC			Intdes;

/*
**  INTEGRITY -- Integrity Constraint Processor
**
**	This module contains the integrity constraint processor.  This
**	processor modifies the query to add integrity constraints.
**
**	Currently only single-variable aggregate-free constraints are
**	handled.  Thus the algorithm is reduced to scanning the tree
**	for each variable modified and appending the constraints for
**	that variable to the tree.
**
**	Returns:
**		The root of the modified tree.
**
**	Side Effects:
**		Modifies the tree in place, so the previous one is lost.
*/
QTREE	*integrity(root)
QTREE	*root;
{
	register QTREE		*r;
	short			dset[8];
	register QTREE		*p;
	register int		i;
	QTREE			*iqual;
	struct integrity	inttup;
	struct integrity	intkey;
	struct tup_id		hitid;
	struct tup_id		lotid;
	extern QTREE		*gettree();
	extern QTREE		*norml();

#	ifdef xQTR1
	AAtTfp(40, -1, "INTEGRITY\n");
#	endif

	r = root;

	/*
	**  Check to see if we should apply the integrity
	**  algorithm.
	**
	**  This means checking to insure that we have an update
	**  and seeing if any integrity constraints apply.
	*/

	if (Qmode == (int) mdRETR || (Rangev[Resultvar].rstat & S_INTEG) == 0)
	{
#		ifdef xQTR2
		AAtTfp(40, 0, "INTEGRITY: no integ\n");
#		endif
		return (r);
	}

	/*
	**  Create a set of the domains updated in this query.
	*/

	for (i = 0; i < 8; i++)
		dset[i] = 0;
	for (p = r->left; p != NULL && p->sym.type != TREE; p = p->left)
	{
#		ifdef xQTR3
		if (p->sym.type != RESDOM)
			AAsyserr(19037, p->sym.type);
#		endif
		lsetbit(((RES_NODE *) p )->resno, dset);
	}

#	ifdef xQTR3
	if (p == NULL)
		AAsyserr(19038);
#	endif
#	ifdef xQTR1
	if (AAtTf(40, 1))
		pr_set(dset, "dset");
#	endif

	/*
	**  Scan AA_INTEG catalog for possible tuples.  If found,
	**  include them in the integrity qualification.
	*/

	iqual = NULL;
	opencatalog(AA_INTEG, 0);
	AAam_setkey(&Intdes, &intkey, Rangev[Resultvar].relid, INTRELID);
	AAam_setkey(&Intdes, &intkey, Rangev[Resultvar].rowner, INTRELOWNER);
	AAam_find(&Intdes, EXACTKEY, &lotid, &hitid, &intkey);

	while ((i = AAam_get(&Intdes, &lotid, &hitid, &inttup, TRUE)) == 0)
	{
		if (AAkcompare(&Intdes, &intkey, &inttup) != 0)
			continue;

#		ifdef xQTR1
		if (AAtTf(40, 2))
			AAprtup(&Intdes, &inttup);
#		endif

		/* check for some domain set overlap */
		for (i = 0; i < 8; i++)
			if ((dset[i] & inttup.intdomset[i]) != 0)
				break;
		if (i >= 8)
			continue;

		/* some domain matches, include in integrity qual */
		i = Resultvar;
		p = gettree(Rangev[i].relid, Rangev[i].rowner, mdINTEG, inttup.inttree, FALSE);
#		ifdef xQTR1
		if (AAtTf(40, 3))
			treepr(p, "int_qual");
#		endif

		/* trim off (null) target list */
		p = p->right;

		/* merge the 'integrity' var into the Resultvar */
		i = inttup.intresvar;
		if (Remap[i] >= 0)
			i = Remap[i];
		mergevar(i, Resultvar, p);

		/* remove old integrity var */
		Rangev[i].rused = FALSE;

		/* add to integrity qual */
		if (iqual == NULL)
			iqual = p;
		else
			appqual(p, iqual);
	}
	if (i < 0)
		AAsyserr(19039, i);

	/*
	**  Clean up the integrity qualification so that it will merge
	**  nicely into the tree, and then append it to the user's
	**  qualification.
	*/

	if (iqual != NULL)
	{
		/* replace VAR nodes by corresponding user afcn */
		subsvars(&iqual, Resultvar, r->left, Qmode);

		/* append to tree and normalize */
		appqual(iqual, r);
#		ifdef xQTR3
		if (AAtTf(40, 8))
			treepr(r, "Unnormalized tree");
#		endif
		r->right = norml(trimqlend(r->right));
	}

#	ifdef xQTR1
	if (AAtTf(40, 15))
		treepr(r, "INTEGRITY->");
#	endif

	return (r);
}
