# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/pipes.h"
# include	"../h/catalog.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"../h/lock.h"
# include	"qrymod.h"

extern QTREE			*Treeroot;
extern DESC			Intdes;
extern DESC			Reldes;
extern struct pipfrmt		Pipe;

/*
**  D_INTEG -- define integrity constraint
**
**	An integrity constraint (as partially defined by the last
**	tree defined by d_tree) is defined.
*/
d_integ()
{
	register int		i;
	register QTREE		*t;		/* definition tree */
	struct integrity	inttup;
	struct tup_id		tid;
	register int		rv;		/* result variable */
	struct retcode		*rc;
	extern struct retcode	*issueinvert();
	struct relation		relkey;
	struct relation		reltup;

	t = Treeroot;
	Treeroot = (QTREE *) 0;
	rv = Resultvar;

	/* pipe is not used, clear it out */
	rdpipe(P_SYNC, &Pipe, R_up);

	/*
	**  Check for valid environment.
	**	The tree must exist, have a qualification, and have
	**	no target list.  The query mode must be mdDEFINE.
	**
	**	User level stuff checks to see that this is single
	**	variable aggregate free, since that's all we know
	**	about thusfar.  Also, the relation in question must
	**	not be a view.
	*/

#	ifdef xQTR3
	if (!t)
		AAsyserr(19000);
	if ((i = t->right->sym.type) != AND)
		AAsyserr(19001, i);
	if ((i = t->left->sym.type) != TREE)
		AAsyserr(19002, i);
	if (Qmode != mdDEFINE)
		AAsyserr(19003, Qmode);
#	endif

	/* check for aggregates */
	if (aggcheck(t))
		far_error(3490, -1, rv, (char *) 0);	/* aggregates in qual */

	/* check for multi-variable */
	for (i = 0; i < MAXVAR + 1; i++)
	{
		if (!Rangev[i].rused)
			continue;
		if (i != rv)
		{
#			ifdef xQTR3
			AAtTfp(49, 1, "D_INTEG: Rv %d(%.14s) i %d(%.14s)\n",
				    rv, Rangev[rv].relid, i, Rangev[i].relid);
#			endif
			far_error(3491, -1, rv, (char *) 0);	/* too many vars */
		}
	}

	/* check for the resultvariable being a real relation */
	if (Rangev[rv].rstat & S_VIEW)
		far_error(3493, -1, rv, (char *) 0);	/* is a view */

	/* no catalog */
	if (Rangev[rv].rstat & S_CATALOG)
		far_error(3496, -1, rv, (char *) 0);	/* is a catalog */

	/* and not an index */
	if ((Rangev[rv].rstat & S_INDEX))
		far_error(3495, -1, rv, (char *) 0);	/* is an index */

	/* guarantee that you own this relation */
	if (!AAbequal(AAusercode, Rangev[rv].rowner, 2))
		far_error(3494, -1, rv, (char *) 0);	/* don't own reln */

	/*
	**  Guarantee that the integrity constraint is true now.
	**	This involves issuing a retrieve statement for the
	**	inverse of the qualification.  The target list is
	**	already null, so we will get nothing printed out
	**	(only a return status).
	*/

	Qmode = (int) mdRETR;
	Resultvar = -1;

	/* issue the invert of the query */
	rc = issueinvert(t);
	if (rc->rc_tupcount)
		far_error(3492, -1, rv, (char *) 0);	/* constraint not satisfied */

	/*
	**  Set up the rest of the environment.
	*/

	opencatalog(AA_INTEG, 2);
	clr_tuple(&Intdes, &inttup);
	Resultvar = -1;
	Qmode = -1;

	/*
	**  Set up integrity relation tuple.
	**	The qualification will be scanned, and a set of
	**	domains referenced will be created.  Other stuff
	**	is filled in from the range table and from the
	**	parser.
	**
	**	The tree is actually inserted into the AA_TREE catalog
	**	in this step.  Extra information is cleared here.
	*/

	inttup.intresvar = rv;
	AAbmove(Rangev[rv].relid, inttup.intrelid, MAXNAME);
	AAbmove(Rangev[rv].rowner, inttup.intrelowner, 2);
	makeidset(rv, t, inttup.intdomset);
	inttup.inttree = puttree(t, inttup.intrelid, inttup.intrelowner, mdINTEG);

	/*
	**  Insert tuple into AA_INTEG catalog.
	*/

	if (AAam_insert(&Intdes, &tid, &inttup, FALSE) < 0)
		AAsyserr(19004);
	if (AAnoclose(&Intdes))
		AAsyserr(19005);

	/*
	**  Update relstat S_INTEG bit.
	*/

	if (!(Rangev[rv].rstat & S_INTEG))
	{
		opencatalog(AA_REL, 2);
		AAam_setkey(&Reldes, &relkey, inttup.intrelid, RELID);
		AAam_setkey(&Reldes, &relkey, inttup.intrelowner, RELOWNER);
		if (AAgetequal(&Reldes, &relkey, &reltup, &tid))
			AAsyserr(19006);
		reltup.relstat |= S_INTEG;
		if (AAsetusl(A_SLP, tid, M_EXCL) < 0)
			AAsyserr(5000);
		if (AAam_replace(&Reldes, &tid, &reltup, FALSE))
			AAsyserr(19007);
		AAunlus(tid);
		if (AAnoclose(&Reldes))
			AAsyserr(19008);
	}
}


makeidset(varno, tree, dset)
int	varno;
QTREE	*tree;
short	dset[8];
{
	register int	vn;
	register QTREE	*t;

	vn = varno;
	t = tree;

	while (t)
	{
		if (t->sym.type == VAR && ((VAR_NODE *) t)->varno == vn)
			lsetbit(((VAR_NODE *) t)->attno, dset);

		/* handle left subtree recursively */
		makeidset(vn, t->left, dset);

		/* handle right subtree iteratively */
		t = t->right;
	}
}
