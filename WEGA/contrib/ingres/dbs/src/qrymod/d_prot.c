# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/bs.h"
# include	"../h/catalog.h"
# include	"../h/access.h"
# include	"../h/tree.h"
# ifdef BC
# define	QLEND		21	/* NULL branch at end of QUAL	*/
# define	RESDOM		11	/* RESULT DOMAIN NUMBER		*/
# define	TREE		25	/* SIGNALS BEGINNING of TREE	*/
# define	VAR		12	/* VARIABLE			*/
# define	mdDEFINE	14
# define	mdPROT		22
# else
# include	"../h/symbol.h"
# endif
# include	"../h/lock.h"
# include	"qrymod.h"

extern struct admin	AAdmin;
extern DESC		Prodes;
extern DESC		Reldes;
extern QTREE		*Treeroot;

struct downame
{
	char	*dow_name;
	int	dow_num;
};

static struct downame	Dowlist[] =
{
	"sun",		0,
	"sunday",	0,
	"mon",		1,
	"monday",	1,
	"tue",		2,
	"tues",		2,
	"tuesday",	2,
	"wed",		3,
	"wednesday",	3,
	"thu",		4,
	"thurs",	4,
	"thursday",	4,
	"fri",		5,
	"friday",	5,
	"sat",		6,
	"saturday",	6,
	(char *) 0
};

/*
**  D_PROT -- define protection constraint
**
**	A protection constraint as partially defined by the last tree
**	defined by d_tree is defined.
**
**	The stuff that comes through the pipe as parameters is complex.
**	It comes as a sequence of strings:
**	 # The operation set, already encoded in the parser into a
**	   bit map.  If the PRO_RETR permission is set, the PRO_TEST
**	   and PRO_AGGR permissions will also be set.
**	 # The relation name.
**	 # The relation owner.
**	 # The user name.  This must be a user name as specified in
**	   the AA_USERS, or the keyword 'all', meaning all users.
**	 # The terminal id.  Must be a string of the form 'ttyx' or
**	   the keyword 'all'.
**	 # The starting time of day, as minutes-since-midnight.
**	 # The ending time of day.
**	 # The starting day-of-week, with 0 = Sunday.
**	 # The ending dow.
**
**	The domain reference set is build automatically from the
**	target list of the tree.  Thus, the target list must exist,
**	but it is not inserted into the tree.  The target list must
**	be a flat sequence of RESDOM nodes with VAR nodes hanging
**	of the rhs; also, the VAR nodes must all be for Resultvar.
**	If there is no target list on the tree, the set of all var-
**	iables is assumed.
**
**	The relstat field in the relation relation is updated to
**	reflect any changes.
**
**	It only makes sense for the DBA to execute this command.
**
**	If there is one of the special cases
**		permit all to all
**		permit retrieve to all
**	it is caught, and the effect is achieved by diddling
**	relstat bits instead of inserting into the AA_PROTECT catalog.
*/
d_prot()
{
	struct protect	protup;
	struct tup_id	protid;
	struct protect	prokey;
	struct protect	proxtup;
	register int	i;
	short		ix;
	register QTREE	*t;
	struct relation	reltup;
	struct relation	relkey;
	struct tup_id	reltid;
	int		relstat;
	int		all_pro;

	/*
	**  Check for valid tree:
	**	There must be a tree defined, and all variables
	**	referenced must be owned by the current user; this
	**	is because you could otherwise get at data by
	**	mentioning it in a permit statement; see protect.c
	**	for a better explanation of this.
	*/

	if (Treeroot == (QTREE *) 0)
		AAsyserr(19009);
	for (i = 0; i < MAXVAR + 1; i++)
	{
		if (!Rangev[i].rused)
			continue;
		if (!AAbequal(Rangev[i].rowner, AAusercode, 2))
			far_error(3592, -1, i, (char *) 0);
	}

	/* test for dba */
	if (!AAbequal(AAusercode, AAdmin.adhdr.adowner, 2))
		far_error(3595, -1, Resultvar, (char *) 0);

	clr_tuple(&Prodes, &protup);

	all_pro = fillprotup(&protup);

	/* get domain reference set from target list */
	/* (also, find the TREE node) */
	t = Treeroot->left;
	if (t->sym.type == TREE)
	{
		for (i = 0; i < 8; i++)
			protup.prodomset[i] = -1;
	}
	else
	{
		for (i = 0; i < 8; i++)
			protup.prodomset[i] = 0;
		for ( ; t->sym.type != TREE; t = t->left)
		{
			if (t->right->sym.type != VAR ||
			    t->sym.type != RESDOM ||
			    ((VAR_NODE *) t->right)->varno != Resultvar)
				AAsyserr(19010);
			lsetbit(((VAR_NODE *) t->right)->attno, protup.prodomset);
		}
		all_pro = FALSE;
	}

	/* trim off the target list, since it isn't used again */
	Treeroot->left = t;

	/*
	**  Check out the target relation.
	**	We first save the varno of the relation which is
	**	getting the permit stuff.  Also, we check to see
	**	that the relation mentioned is a base relation,
	**	and not a view, since that tuple would never do
	**	anything anyway.  Finally, we clear the Resultvar
	**	so that it does not get output to the AA_TREE catalog.
	**	This would result in a 'AAsyserr' when we tried to
	**	read it.
	*/

	protup.proresvar = Resultvar;
#	ifdef xQTR3
	if (Resultvar < 0)
		AAsyserr(19011, Resultvar);
#	endif

	/* check for the resultvariable being a real relation */
	if (Rangev[Resultvar].rstat & S_VIEW)
		far_error(3593, -1, Resultvar, (char *) 0);	/* is a view */

	/* and not an index */
	if (Rangev[Resultvar].rstat & S_INDEX)
		far_error(3596, -1, Resultvar, (char *) 0);	/* is an index */

	/* clear the (unused) Qmode */
#	ifdef xQTR3
	if (Qmode != mdDEFINE)
		AAsyserr(19012, Qmode);
#	endif
	Qmode = -1;

	/*
	**  Check for PERMIT xx to ALL case.
	**	The relstat bits will be adjusted as necessary
	**	to reflect these special cases.
	**
	**	This is actually a little tricky, since we cannot
	**	afford to turn off any permissions.  If we already
	**	have some form of PERMIT xx to ALL access, we must
	**	leave it.
	*/

	relstat = Rangev[Resultvar].rstat;
	if (all_pro && (protup.proopset & PRO_RETR))
	{
		if (ctoi(protup.proopset) == -1)
			relstat &= ~S_PROTALL;
		else
		{
			relstat &= ~S_PROTRET;
			if ((protup.proopset & ~(PRO_RETR|PRO_AGGR|PRO_TEST)))
			{
				/* some special case: still insert prot tuple */
				all_pro = FALSE;
			}
		}
	}
	else
		all_pro = FALSE;	/* insert tuple if not special case */

	/* see if we are adding any tuples */
	if (!all_pro)
		relstat |= S_PROTUPS;

	/*
	**  Change relstat field in AA_REL catalog
	*/

	opencatalog(AA_REL, 2);
	AAam_setkey(&Reldes, &relkey, Rangev[Resultvar].relid, RELID);
	AAam_setkey(&Reldes, &relkey, Rangev[Resultvar].rowner, RELOWNER);
	if (i = AAgetequal(&Reldes, &relkey, &reltup, &reltid))
		AAsyserr(19013, i);
	reltup.relstat = relstat;
	if (AAsetusl(A_SLP, reltid, M_EXCL) < 0)
		AAsyserr(5000);
	if (AAam_replace(&Reldes, &reltid, &reltup, FALSE) < 0)
		AAsyserr(19014);
	AAunlus(reltid);
	if (AAnoclose(&Reldes))
		AAsyserr(19015);

	Resultvar = -1;

	if (!all_pro)
	{
		/*
		**  Output the created tuple to the AA_PROTECT catalog
		**  after making other internal adjustments and deter-
		**  mining a unique sequence number (with the
		**  AA_PROTECT catalog locked).
		*/

		if (Treeroot->right->sym.type != QLEND)
			protup.protree = puttree(Treeroot, protup.prorelid, protup.prorelown, mdPROT);
		else
			protup.protree = -1;

		/* compute unique permission id */
		opencatalog(AA_PROTECT, 2);
		if (AAsetrll(A_SLP, Prodes.reltid, M_EXCL) < 0)
			AAsyserr(5000);
		AAam_setkey(&Prodes, &prokey, protup.prorelid, PRORELID);
		AAam_setkey(&Prodes, &prokey, protup.prorelown, PRORELOWN);
		for (ix = 2; ; ix++)
		{
			AAam_setkey(&Prodes, &prokey, &ix, PROPERMID);
			i = AAgetequal(&Prodes, &prokey, &proxtup, &protid);
			if (i < 0)
				AAsyserr(19016);
			else if (i > 0)
				break;
		}
		protup.propermid = ix;

		/* do actual insert */
		i = AAam_insert(&Prodes, &protid, &protup, FALSE);
		if (i < 0)
			AAsyserr(19017);
		if (AAnoclose(&Prodes))
			AAsyserr(19018);

		/* clear the lock */
		AAunlrl(Prodes.reltid);
	}
	Treeroot = (QTREE *) 0;
}


/*
**  CVT_DOW -- convert day of week
**
**	Converts the day of the week from string form to a number.
**
**	Parameters:
**		sdow -- dow in string form.
**
**	Returns:
**		0 -> 6 -- the encoded day of the week.
**		-1 -- error.
*/
cvt_dow(sdow)
char	*sdow;
{
	register struct downame	*d;
	register char		*s;

	s = sdow;

	for (d = Dowlist; d->dow_name; d++)
		if (AAsequal(d->dow_name, s))
			return (d->dow_num);
	return (-1);
}
