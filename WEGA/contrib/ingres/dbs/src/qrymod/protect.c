# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/bs.h"
# include	"../h/catalog.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"
# include	"qrymod.h"

/*
**  PROTECT -- protection algorithm
**
**	This module performs the DBS protection algorithm
**
**	The basic algorithm is as follows:
**
**	The algorithm is applied once to each variable used in the
**	query.  Each variable has an initial check performed to
**	determine applicability -- if the current user owns the
**	relation, or if the relation is specially marked as being
**	"all access to everyone", then the algorithm is skipped,
**	thereby having effectively no restriction.
**
**	The set of all such variables is computed in 'protect',
**	and then 'dopro' is called to process each of those.  This
**	is so the protection algorithm does not get applied recursively
**	to constraints which define more than one variable.  Notice
**	that this could in itself be a protection violation, if it
**	were acceptable to reference a relation you do not own in a
**	PERMIT statement.
**
**	The effective query mode for this variable is then computed.
**	This is the same as the query mode of the whole query if
**	the variable in question is the result variable, otherwise
**	it is "retrieve" mode.
**
**	The next step is to scan the query tree and create sets of
**	domains referenced.  Four sets are created:
**		uset -- the set of domains updated (actually,
**			referenced in the target list -- on a
**			retrieve, this will be the set of domains
**			retrieved to the user).
**		rset -- the set of domains retrieved in some
**			context other than the left hand side of
**			an equal sign.
**		aset -- the set of domains aggregated.  This only
**			includes domains aggregated with a simple
**			aggregate (not an aggregate function) with
**			no qualification, since it may be possible
**			to come up with too much information other-
**			wise.
**		qset -- the set of domains retrieved for use in
**			a qualification, but never stored.  This
**			includes domains in a qualification of an
**			aggregate or aggregate function.
**	For more details of domains in each of these sets, look at
**	the routine 'makedset'.
**
**	If we had a retrieve operation in the first place, we will
**	then merge 'uset' into 'rset' and clear 'uset', so that
**	now 'uset' only contains domains which are actually updated,
**	and 'rset' contains all domains which are retrieved.
**
**	Now that we know what is referenced, we can scan the AA_PROTECT
**	catalog.  We scan the entire catalog once for each variable
**	mentioned in the query (except as already taken care of as
**	described above).
**
**	We must create a set of all operations on this variable which
**	are not yet resolved, that is, for which no PERMIT statements
**	which qualify have been issued.  We store this set in the
**	variable "noperm".  As PERMIT statements are found, bits will
**	be cleared.  If the variable is not zero by the end of the
**	scan of the AA_PROTECT catalog, then we reject the query,
**	saying that we don't have permission -- giving us default
**	to deny.
**
**	For each tuple in the AA_PROTECT catalog for this relation,
**	we call "proappl" to see if it applies to this query.  This
**	routine checks the user, terminal, time of day, and so forth
**	(in fact, everything which is query-independent) and tells
**	whether this tuple might apply.
**
**	If the tuple passes this initial check, we then do the query-
**	dependent checking.  This amounts to calling "prochk" once
**	for each operation (and domain set) in the query.  What we
**	get back is a set of operations which this tuple applies to.
**	If zero, the tuple doesn't apply at all; otherwise, it
**	applies to at least one operation.  If it applies to some-
**	thing, we call it "interesting".
**
**	For "interesting" tuples, we now get the corresponding
**	qualification (if one exists), and disjoin it to a set of
**	protection constraints held in "pqual".  Also, we mark
**	any operations we found as having been done, by clearing
**	bits in "noperm".
**
**	When we have completed scanning the AA_PROTECT catalog,
**	we check "noperm".  If it is non-zero, then we have some
**	operation for which a PERMIT statement has not been issued,
**	and we issue an error message.  If this variable is ok,
**	then we go on and try the next variable.
**
**	When all variables have been accounted for, we check to
**	see if we have any qualifications collected from the
**	protection algorithm.  If so, we conjoin them to the
**	query tree.
**
**	Finally, we return the root of the modified tree.  This
**	tree is guaranteed to have no authorization violations,
**	and may be run as a regular query.
**
**	Parameters:
**		root -- the root of the tree.
**
**	Returns:
**		The root of the modified and authorized tree.
**
**	Side Effects:
**		A possible non-local return on access violation.
**
**	Defines:
**		Proopmap -- a mapping from query modes to operation
**			set bits in the AA_PROTECT catalog.
**		protect -- the driver for the protection algorithm.
**		dopro -- the guts of the algorithm.
**		makedset -- the routine to make the domain sets.
**		proappl -- to check if a protection tuple applies.
**		prochk -- to check what operations it handles.
**		pr_set -- debugging set print.
*/

static int	Proopmap[MAXPROQM + 1] =
{
	PRO_RETR,		/* 0 -- mdRETTERM */
	PRO_RETR,		/* 1 -- mdRETR */
	PRO_APP,		/* 2 -- mdAPP */
	PRO_REPL,		/* 3 -- mdREPL */
	PRO_DEL,		/* 4 -- mdDEL */
};

extern DESC		Prodes;
extern char		Terminal[PROTERM + 1];
extern QTREE		*gettree();

QTREE	*protect(r)
register QTREE	*r;
{
	register int	vn;
	register int	qmode;
	register int	varset;

#	ifdef xQTR1
	AAtTfp(50, -1, "PROTECT>\n");
#	endif

	varset = 0;

	/*
	**  Scan the range table and create a set of all variables
	**  which are 'interesting', that is, on which the protection
	**  algorithm should be performed.
	*/

	for (vn = 0; vn < MAXVAR + 1; vn++)
	{
		if (!Rangev[vn].rused)
			continue;

		/* if owner, accept any query */
		if (AAbequal(Rangev[vn].rowner, AAusercode, 2))
			continue;

		/* check for "no restriction" bit asserted (= clear) */
		if ((Rangev[vn].rstat & S_PROTALL) == 0)
			continue;
		if ((Rangev[vn].rstat & S_PROTRET) == 0 &&
		    (Qmode == (int) mdRETR || Qmode == (int) mdRET_UNI))
			continue;

		varset |= 1 << vn;
	}

	/*
	**  For each variable specified in varset (that is, for each
	**  variable in the initial query), do the real algorithm.
	*/

	for (vn = 0; vn < MAXVAR + 1; vn++)
	{
		if ((varset & (1 << vn)) == 0)
			continue;

#		ifdef xQTR1
		AAtTfp(50, 1, "vn=%d: %.12s\n", vn, Rangev[vn].relid);
#		endif

		/*
		**  Determine the query mode for this variable.  This
		**  is not the query mode of the original query,
		**  unless the variable is the result variable.
		*/

		qmode = Qmode;
		if (vn != Resultvar || qmode == (int) mdRET_UNI)
			qmode = (int) mdRETTERM;

#		ifdef xQTR3
		if (qmode == (int) mdRETR || qmode > (int) mdDEL || qmode < 0)
			AAsyserr(19041, qmode);
#		endif

		/* do the interesting part of the algorithm */
		dopro(vn, r, qmode, (short *) 0);
	}

	/* return the (authorized) tree */
#	ifdef xQTR1
	if (AAtTf(50, 15))
		treepr(r, "PROTECT->");
#	endif
	return (r);
}


/*
**  DOPRO -- actually do the protection algorithm
**
**	This is the guts of it, broken off because it must be called
**	recursively on aggregates.  The algorithm is as discussed
**	in the module header.
**
**	Parameters:
**		varno -- the variable number of interest.
**		root -- the root of the tree to modify.
**		qmode -- the effective query mode for this relation.
**		byset -- if non-NULL, a set of domains passed back
**			which gets bound out of the aggregate func,
**			in other words, the by list.
**
**	Side Effects:
**		The tree pointed at by 'root' gets modified.
**		Quite possibly 'Rangev' and 'Remap' get clobbered.
*/
dopro(varno, root, qmode, byset)
int	varno;
QTREE	*root;
int	qmode;
short	byset[8];
{
	short		qset[8];
	short		uset[8];
	short		aset[8];
	short		rset[8];
	short		zeros[8];
	QTREE		*p;
	QTREE		*pqual;
	register int	i;
	register int	vn;
	register QTREE	*t;
	int		noperm;
	int		noqual;
	struct protect	prokey;
	struct protect	protup;
	struct tup_id	lotid;
	struct tup_id	hitid;
	struct qvect
	{
		QTREE	*q_qual;
		int	q_mode;
	};
	struct qvect	quals[4];
	int		j;
	extern QTREE	*trimqlend();
	extern QTREE	*tree();
	extern QTREE	*norml();


	t = root;
	vn = varno;

	/* create domain usage sets */
	for (i = 0; i < 8; i++)
	{
		zeros[i] = uset[i] = rset[i] = qset[i] = aset[i] = 0;
		if (byset != (short *) 0)
			byset[i] = 0;
	}

	/*
	**  Create domain usage set for target list side.  There are
	**  two general cases: this is the root of the tree, or this
	**  is the head of an aggregate.
	*/

	switch (t->sym.type)
	{
	  case AGHEAD:
		/*
		**  An aggregate head falls into two classes: simple
		**  aggregate and aggregate function.  In an aggregate
		**  function, care must be taken to bind the variables
		**  in the by-list outside of the aggregate.  We use
		**  'rset' as a temporary here.
		*/

		if (t->left->sym.type == BYHEAD)
		{
			/* make by-list set */
			makedset(vn, t->left->left, (short *) 0, rset, aset, qset);

			/* merge by-list set into qualification set */
			for (i = 0; i < 8; i++)
			{
				if (byset != (short *) 0)
					byset[i] |= rset[i];
				qset[i] |= rset[i];
				rset[i] = 0;
			}

			/* make aggregate list set */
			makedset(vn, t->left->right->right, (short *) 0, rset, aset, qset);
		}
		else
		{
			/* simple aggregate */
#			ifdef xQTR3
			if (t->left->sym.type != AOP)
				AAsyserr(19042, t->left->sym.type);
#			endif

			/* check for qualification */
			if (t->right->sym.type == QLEND)
			{
				/* simple, unqualified aggregate */
				makedset(vn, t->left->right, (short *) 0, aset, aset, qset);
			}
			else
			{
#				ifdef xQTR3
				if (t->right->sym.type != AND)
					AAsyserr(19043, t->right->sym.type);
#				endif
				makedset(vn, t->left->right, (short *) 0, rset, aset, qset);
			}
		}
		break;

	  case ROOT:
		makedset(vn, t->left, uset, rset, aset, qset);
		break;
	}

	/* scan qualifcation */
	makedset(vn, t->right, (short *) 0, qset, aset, qset);

	/* if retrieval, drop the 'update' set		*/
	/* if delete or append, force an appernt update */
	switch (qmode)
	{
# ifdef ENUM_INT
	  case mdRETTERM:
# else
	  case (int) mdRETTERM:
# endif
		for (i = 0; i < 8; i++)
			uset[i] = 0;
		break;

# ifdef ENUM_INT
	  case mdDEL:
	  case mdAPP:
# else
	  case (int) mdDEL:
	  case (int) mdAPP:
# endif
		for (i = 0; i < 8; i++)
			uset[i] = -1;
		break;
	}

#	ifdef xQTR1
	if (AAtTfp(51, 2, "qmode %d\n", qmode))
	{
		pr_set(uset, "uset");
		pr_set(rset, "rset");
		pr_set(aset, "aset");
		pr_set(qset, "qset");
	}
#	endif

	/* create a bit map of all referenced operations */
	noperm = 0;
	if (!AAbequal(uset, zeros, sizeof zeros))
		noperm |= Proopmap[qmode];
	if (!AAbequal(rset, zeros, sizeof zeros))
		noperm |= PRO_RETR;
	if (!AAbequal(aset, zeros, sizeof zeros))
		noperm |= PRO_AGGR;
	if (!AAbequal(qset, zeros, sizeof zeros))
		noperm |= PRO_TEST;

	/*
	** if no oper, then the query was probably just aggr's
	**	if (noperm == 0)
	**		return;
	*/

	/* initialize qualification portion */
	for (i = 0; i < 4; )
		quals[i++].q_qual = (QTREE *) 0;
	noqual = FALSE;

	/* check the AA_PROTECT catalog */
	opencatalog(AA_PROTECT, 0);
	AAam_setkey(&Prodes, &prokey, Rangev[vn].relid, PRORELID);
	AAam_setkey(&Prodes, &prokey, Rangev[vn].rowner, PRORELOWN);
	AAam_find(&Prodes, EXACTKEY, &lotid, &hitid, &prokey);

	while ((i = AAam_get(&Prodes, &lotid, &hitid, &protup, TRUE)) == 0)
	{
		if (AAkcompare(&Prodes, &prokey, &protup) != 0)
			continue;

#		ifdef xQTR2
		if (AAtTfp(51, 4, "<PROTECT "))
			AAprtup(&Prodes, &protup);
#		endif

		/* check if this is the correct user, terminal, etc */
		if (!proappl(&protup))
			continue;

		/* alright, let's check the operation */
		i = 0;
		if (qmode != (int) mdRETTERM)
			i = quals[0].q_mode = prochk(Proopmap[qmode], uset, &protup);
		i |= quals[1].q_mode = prochk(PRO_RETR, rset, &protup);
		i |= quals[2].q_mode = prochk(PRO_AGGR, aset, &protup);
		i |= quals[3].q_mode = prochk(PRO_TEST, qset, &protup);

#		ifdef xQTR2
		AAtTfp(51, 5, "Satisfies operations 0%o\n", i);
#		endif

		/* see if this tuple is "interesting" */
		if (i == 0)
			continue;

		/* it is!  get the qualification (if any) */
		if (protup.protree >= 0)
		{
			p = gettree(Rangev[vn].relid, Rangev[vn].rowner, mdPROT, protup.protree, FALSE);
#			ifdef xQTR2
			if (AAtTf(51, 6))
				treepr(p, "Protection Clause");
#			endif
			p = trimqlend(p->right);
#			ifdef xQTR3
			/* check for a non-NULL qualification */
			if (p == (QTREE *) 0)
				AAsyserr(19044);
#			endif

			/* translate to the interesting variable */
			j = protup.proresvar;
			if (Remap[j] >= 0)
				j = Remap[j];
			mergevar(j, varno, p);

			/* disjoin the protection qual to real qual */
			for (j = 0; j < 4; j++)
			{
				if (quals[j].q_mode == 0)
					continue;
				if (quals[j].q_qual == (QTREE *) 0)
					quals[j].q_qual = p;
				else
					quals[j].q_qual = tree(quals[j].q_qual, p, OR, 0);
			}
		}
		else
			noqual = TRUE;

		/* mark this operation as having been handled */
		noperm &= ~i;
	}

	/* test 'get' return code */
	if (i < 0)
		AAsyserr(19045);

#	ifdef xQTR1
	AAtTfp(51, 12, "No perm on 0%o\n", noperm);
#	endif

	/* see if no tuples applied for some operation */
	if (noperm != 0)
		far_error(3500, Qmode, vn, (char *) 0);

	/* see if we want to modify the query at all */
	if (!noqual)
	{
		/* conjoin the qualification */
		pqual = (QTREE *) 0;
		for (i = 0; i < 4; i++)
			if (quals[i].q_qual != (QTREE *) 0)
				if (pqual == (QTREE *) 0)
					pqual = quals[i].q_qual;
				else
					pqual = tree(pqual, quals[i].q_qual, AND, 0);
		pqual = tree(pqual, tree((QTREE *) 0, (QTREE *) 0, QLEND, 0), AND, 0);
		appqual(pqual, t);

		/* normalize the tree */
		t->right = norml(trimqlend(t->right));
	}
}


/*
**  MAKEDSET -- make domain reference sets
**
**	This routine creates some sets which reflect the usage of
**	domains for a particular variable.
**
**	The interesting nodes are 'case' labels in the large
**	switch statement which comprises most of the code.  To
**	describe briefly:
**
**	VAR nodes are easy: if they are for the current variable,
**		set the bit corresponding to the domain in the
**		'retrieval' set.  They can have no descendents,
**		so just return.
**	RESDOM nodes are also easy: they can be handled the same,
**		but the bit is set in the 'update' set instead.
**	AGHEAD nodes signal the beginning of an aggregate or
**		aggregate function.  In this case, we scan the
**		qualification first (noting that RESDOM and VAR
**		nodes are processed as 'qualification' sets
**		instead of 'retrieval' or 'update' sets).  Then,
**		if the aggregate has a WHERE clause or a BY list,
**		we treat it as a retrieve; otherwise, we call our-
**		selves recursively treating VAR nodes as 'aggregate'
**		types rather than 'retrieve' types.
**	BYHEAD nodes signal the beginning of a BY list.  The left
**		subtree (the actual BY-list) is processed with
**		RESDOM nodes ignored (since they are pseudo-domains
**		anyhow) and VAR nodes mapped into the 'qualification'
**		set.  Then we check the right subtree (which better
**		begin with an AOP node!) and continue processing.
**	AOP nodes must have a NULL left subtree, so we just drop
**		to the right subtree and iterate.  Notice that we
**		do NOT map VAR nodes into the 'aggregate' set for
**		this node, since this has already been done by the
**		AGHEAD node; also, this aggregate might be counted
**		as a retrieve operation instead of an aggregate
**		operation (as far as the protection system is con-
**		cerned) -- this has been handled by the AGHEAD
**		node.
**	All other nodes are processed recursively along both edges.
**
**	Parameters:
**		vn -- the variable number that we are currently
**			interested in.
**		tree -- the root of the tree to scan.  Notice that this
**			will in general be only one half of the tree --
**			makedset will be called once for the target
**			list and once for the qualification, with
**			different sets for the following parameters.
**		uset -- adjusted to be the set of all domains
**			updated.
**		rset -- adjusted to be the set of all domains
**			retrieved implicitly, that is, on the right-
**			hand-side of an assignment operator.
**		aset -- adjusted to be the set of all domains
**			aggregated.  Notice that this set is not
**			adjusted explicitly, but rather is passed
**			to recursive incarnations of this routine
**			as 'rset'.
**		qset -- adjusted to be the set of domains retrieved
**			implicitly in a qualification.  Like 'aset',
**			this is passed as 'rset' to recursive
**			incarnations.
*/
makedset(vn, t, uset, rset, aset, qset)
int		vn;
register QTREE	*t;
short		uset[8];
short		rset[8];
short		aset[8];
short		qset[8];
{
	register int	i;
	short		byset[8];

#	ifdef xQTR1
	if (AAtTfp(53, 0, "MAKEDSET>\n"))
	{
		pr_set(uset, "uset");
		pr_set(rset, "rset");
		pr_set(aset, "aset");
		pr_set(qset, "qset");
	}
#	endif

	while (t != (QTREE *) 0)
	{
		switch (t->sym.type)
		{
		  case VAR:
			if (((VAR_NODE *) t)->varno == vn)
				lsetbit(((VAR_NODE *) t)->attno, rset);
			break;

		  case AGHEAD:
			/* do protection on qualification */
			dopro(vn, t, -1, byset);

			/* merge by-list set into qualification set */
			for (i = 0; i < 8; i++)
				qset[i] |= byset[i];

			break;

		  case BYHEAD:
		  case AOP:
			AAsyserr(19046, t->sym.type);

		  case RESDOM:
			if (((RES_NODE *) t)->resno == 0)
			{
				/* tid -- ignore right subtree (and this node) */
				t = t->left;
				continue;
			}
			if (uset != (short *) 0)
				lsetbit(((RES_NODE *) t)->resno, uset);
			/* explicit fall-through to "default" case */

		  default:
			/* handle left subtree (recursively) */
			makedset(vn, t->left, uset, rset, aset, qset);

			/* handle right subtree (iteratively) */
			t = t->right;
			continue;
		}
		break;
	}

#	ifdef xQTR1
	if (AAtTfp(53, 15, "<MAKEDSET\n"))
	{
		pr_set(uset, "uset");
		pr_set(rset, "rset");
		pr_set(aset, "aset");
		pr_set(qset, "qset");
	}
#	endif

	return;
}


/*
**  PROAPPL -- check for protection tuple applicable
**
**	A given AA_PROTECT catalog tuple is checked in a query-
**	independent way for applicability.
**
**	This routine checks such environmental constraints as the
**	user, the terminal, and the time of day.  The code is
**	fairly straightforward, just take a look.
**
**	One note: the user and terminal codes contained in the
**	AA_PROTECT catalog are blank to mean 'any value' of the
**	corresponding field.
**
**	Parameters:
**		protup -- the protection tuple to compare against.
**
**	Returns:
**		TRUE -- this tuple applies to the current environment.
**		FALSE -- this tuple does not apply.
*/
proappl(protup)
struct protect	*protup;
{
	register struct protect	*p;
	long			tvect;
	register int		*tt;
	extern int		*localtime();
	register int		mtime;

	p = protup;

	/* check for correct user [insert clique code here] */
	if (!AAbequal("  ", p->prouser, 2))
	{
		if (!AAbequal(p->prouser, AAusercode, 2))
			return (FALSE);
	}

	/* check for correct terminal */
	if (*p->proterm != ' ')
	{
		if (!AAbequal(p->proterm, Terminal, PROTERM))
			return (FALSE);
	}

	/* check for correct time of day & week */
	time(&tvect);
# ifdef ESER_VMX
	tt = localtime(tvect);
# else
	tt = localtime(&tvect);
# endif
	mtime = tt[2] * 60 + tt[1];

	if (p->protodbgn <= p->protodend)
	{
		if (p->protodbgn > mtime || mtime > p->protodend)
			return (FALSE);
	}
	else if (p->protodbgn > mtime && mtime > p->protodend)
		return (FALSE);

	if (p->prodowbgn <= p->prodowend)
	{
		if (p->prodowbgn > tt[6] || tt[6] > p->prodowend)
			return (FALSE);
	}
	else if (p->prodowbgn > tt[6] && tt[6] > p->prodowend)
		return (FALSE);

	/* hasn't failed yet -- I guess it's ok */
	return (TRUE);
}


/*
**  PROCHK -- query-dependent protection tuple check
**
**	This routine does the query-dependent part of checking
**	the validity of a protection tuple.  Unlike proappl,
**	which looked at aspects of the environment but not the
**	query being run, this routine assumes that the environ-
**	ment is ok, and checks that if it applies to this tuple.
**
**	Two things are checked.  The first is if this tuple applies
**	to the operation in question (passed as 'inbit').  The
**	second is if the set of domains in the tuple contains the
**	set of domains in the query.  If either of these fail,
**	the return is zero.  Otherwise the return is the operation
**	bit.  In otherwise, the return is the operation to which
**	this tuple applies (if any).
**
**	As a special check, the domain set is checked for all
**	zero.  If so, no domains have been referenced for this
**	operation at all, and we return zero.  In other words, this
**	tuple might apply to this operation, but since we don't
**	use the operation anyhow we will ignore it.  It is important
**	to handle things in this way so that the qualification for
**	this tuple doesn't get appended if the variable is not
**	used in a particular context.
**
**	Parameters:
**		inbit -- the bit describing the operation to be
**			checked.  Note that only one bit should
**			be set in this word, although this is
**			not checked.
**		domset -- the set of domains actually referenced
**			in this query for the operation described
**			by 'inbit'.
**		protup -- the tuple in question.
**
**	Returns:
**		The operation (if any) to which this tuple applies.
*/
prochk(inbit, domset, protup)
int		inbit;
short		domset[8];
struct protect	*protup;
{
	register struct protect	*p;
	register short		*d;
	register int		i;

	p = protup;
	d = domset;

#	ifdef xQTR1
	if (AAtTfp(55, 0, "->PROCHK, inbit=0%o, proopset=0%o\n", inbit, p->proopset))
	{
		pr_set(d, "domset");
		pr_set(p->prodomset, "prodomset");
	}
#	endif

	/* check for NULL domain set, if so return zero */
	for (i = 0; i < 8; i++)
		if (d[i] != 0)
			break;
	if (i >= 8)
	{
#		ifdef xQTR2
		AAtTfp(55, 15, "PROCHK-> NULL set\n");
#		endif
		return (0);
	}

	/* see if this tuple applies to this operation */
	if ((inbit & p->proopset) == 0)
	{
#		ifdef xQTR2
		AAtTfp(55, 15, "PROCHK-> no op\n");
#		endif
		return (0);
	}

	/* check if domains are a subset */
	for (i = 0; i < 8; i++)
	{
		if ((d[i] & ~p->prodomset[i]) != 0)
		{
			/* failure */
#			ifdef xQTR2
			AAtTfp(55, 15, "PROCHK-> not subset\n");
#			endif
			return (0);
		}
	}

	/* this is hereby an "interesting" tuple */
#	ifdef xQTR2
	AAtTfp(55, 15, "PROCHK-> %d\n", inbit);
#	endif
	return (inbit);
}


# ifdef xQTR1
/*
**  PR_SET -- print set for debugging
**
**	This routine prints a 128-bit set for debugging.
**
**	Parameters:
**		xset -- the set to convert.
**		labl -- a label to print before the set.
*/
pr_set(xset, labl)
register short	*xset;
register char	*labl;
{
	register int	i;

	printf("\t%s: hex/", labl);
	if (xset == (short *) 0)
	{
		printf("NULL/\n");
		return;
	}
	for (i = 0; i < 8; i++)
		printf("%04x/", xset[i]);
	putchar('\n');
}
# endif
