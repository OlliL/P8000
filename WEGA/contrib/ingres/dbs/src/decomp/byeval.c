# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"../h/pipes.h"
# include	"decomp.h"

/*
**	BYEVAL - process aggregate function
**
**	Byeval is passed the root of the original query
**	tree and the root of the aggregate function to
**	be processed.
**
**	It first creates a temporary relation which will
**	hold the aggregate result. The format of the relation
**	is:
**	SYSNAMExxxxxaa(count, by-dom1, ... , by-domn, ag1, ... , agm)
**
**	The relation is moved into the range table and will become
**	a part of the query.
**
**	If there are any occurences of the variables
**	from the by-domains, anywhere in the original query tree,
**	the aggregate relation is linked on all by-domains in the
**	original query tree.
**
**	If the aggregate is unique, multivariable, or has a
**	qualification, then special processing is done.
**
**	If the aggregate is qualified then the by-domains are
**	projected into the result relation. This guarantees that
**	every value of the by-domains will be represented in the
**	aggregate result.
**
**	If the aggregate is unique or multivariable, then another
**	temporary relation is created and the values which will be
**	aggregated; along with the by-domains, are retrieved into
**	the temporary relation.
**
**	If unique, then duplicates are removed from the temporary relation.
**
**	Next the result relation for the aggregate is modified
**	to hash in order to speed up the processing of the aggregate
**	and guarantee that there are no duplicates in the bylist.
**
**	The aggregate is then run, and if a temporary relation was
**	created (eg. unique or multivar aggregate) then it is destroyed.
**
*/


QTREE		*byeval(root, aghead, agvar)
QTREE			*root;		/* root of orig query */
QTREE			*aghead;	/* root of ag fcn sub-tree */
int			agvar;		/* variable number assigned to this aggregate */
{
	register QTREE			*q;
	register QTREE			*ag;
	register QTREE			*resdom;
	register QTREE			*r;
	int				temp_relnum;
	int				i;
	int				filled;
	QTREE				*lnodv[MAXDOM + 2];
	QTREE				*save_node[MAXDOM + 2];
	char				agbuf[AGBUFSIZ];
	char				nums[2];
	int				relnum;
	register QTREE			*byhead;
	register QTREE			**alnp;
	int				bydoms;
	int				bymap;
	int				primeag;
	int				srcmap;
	extern int			derror();
	extern QTREE			*copytree();
	extern QTREE			*makavar();
	extern QTREE			*makresdom();
	extern QTREE			*makroot();

# ifdef xDTR1
	AAtTfp(7, -1, "BYEVAL\n");
# endif

	ag = aghead;
	byhead = ag->left;

	/* first create the aggregate result relation */
	/* params for create */

	initp();	/* init globals for setp */
	setp("0");	/* initial relstat field */
	relnum = rnum_alloc();
	setp(rnum_convert(relnum));
	setp("count");		/* domain 1 - count field per BY value */
	setp("i4");		/* format of count field */

	i = bydoms = lnode(byhead->left, lnodv, 0);
	lnodv[i] = 0;
	alnp = &lnodv[++i];
	i = lnode(byhead->right, lnodv, i);
	lnodv[i] = 0;

	domnam(lnodv, "by");	/* BY list domains */
	domnam(alnp, "ag");	/* aggregate value domains */

	call_dbu(mdCREATE, FALSE);

	Rangev[agvar].relnum = relnum;
# ifdef xDTR1
	AAtTfp(7, 7, "agvar=%d,rel=%s\n", agvar, rnum_convert(relnum));
# endif

	bymap = varfind(byhead->left, (QTREE *) 0);

	/*
	** Find all variables in the tree in which you are nested.
	** Do not look at any other aggregates in the tree. Just in
	** case the root is an aggregate, explicitly look at its
	** two descendents.
	*/
	srcmap = varfind(root->left, ag) | varfind(root->right, ag);
# ifdef xDTR1
	AAtTfp(7, 8, "bymap=0%o,srcmap=0%o\n", bymap, srcmap);
# endif

	if (bymap & srcmap)
		modqual(root, lnodv, srcmap, agvar);

	/* if aggregate is unique or there is a qualification
	** or aggregate is multi-var, then special processing is done */

	temp_relnum = NORESULT;
	filled = FALSE;
	primeag = prime(byhead->right);
	if (ag->right->sym.type != QLEND || ((ROOT_NODE *) ag)->tvarc > 1 || primeag)
	{
		/* init a buffer for new tree components */
		initbuf(agbuf, AGBUFSIZ, AGBUFFULL, derror);

		/* make a root for a new tree */
		q = makroot(agbuf);

		/*
		** Create a RESDOM for each by-domain in the original
		** aggregate. Rather than using the existing by-domain
		** function, a copy is used instead. This is necessary
		** since that subtree might be needed later (if modqual())
		** decided to use it. Decomp does not restore the trees
		** it uses and thus the by-domains might be altered.
		*/
		for (i = 0; r = lnodv[i]; i++)
		{
			resdom = makresdom(agbuf, r);
			((RES_NODE *) resdom)->resno = i + 2;
			resdom->right = copytree(r->right, agbuf);
			resdom->left = q->left;
			q->left = resdom;
		}
		mapvar(q, 0);	/* make maps on root */
# ifdef xDTR1
		if (AAtTf(7, 2))
			printree(q, "by-domains");
# endif

		/* if agg is qualified, project by-domains into result */
		if (ag->right->sym.type != QLEND)
		{
			filled = TRUE;
			i = Sourcevar;	/* save value */
			decomp(q, mdRETR, relnum);
			Sourcevar = i;	/* restore value */
		}

		/* if agg is prime or multivar, compute into temp rel */
		if (((ROOT_NODE *) ag)->tvarc > 1 || primeag)
		{
			q->right = ag->right;	/* give q the qualification */
			ag->right = Qle;	/* remove qualification from ag */

			/* put aop resdoms on tree */
			for (i = bydoms + 1; r = lnodv[i]; i++)
			{
				resdom = makresdom(agbuf, r);
				resdom->right = r->right;
				resdom->left = q->left;
				q->left = resdom;

				/* make aop refer to temp relation */
				r->right = makavar(resdom, FREEVAR, i);
			}

			/* assign result domain numbers */
			for (resdom = q->left; resdom->sym.type != TREE; resdom = resdom->left)
				((RES_NODE *) resdom)->resno = --i;

			/*
			** change by-list in agg to reference new source rel.
			** Save the old bylist to be restored at the end of
			** this aggregate.
			*/
			for (i = 0; resdom = lnodv[i]; i++)
			{
				save_node[i] = resdom->right;
				resdom->right = makavar(resdom, FREEVAR, i + 1);
			}

			mapvar(q, 0);
# ifdef xDTR1
			if (AAtTf(7, 3))
				printree(q, "new ag src");
# endif

			/* create temp relation */
			temp_relnum = mak_t_rel(q, "a", -1);
			decomp(q, mdRETR, temp_relnum);
			Rangev[FREEVAR].relnum = temp_relnum;
			Sourcevar = FREEVAR;
			if (primeag)
				removedups(FREEVAR);
# ifdef xDTR1
			if (AAtTf(7, 4))
				printree(ag, "new agg");
# endif
		}
	}

	/* set up parameters for modify to hash */
	initp();
	setp(rnum_convert(relnum));
	setp("hash");		/* modify the empty rel to hash */
	setp("num");		/* code to indicate numeric domain names */
	nums[1] = '\0';
	for (i = 0; i < bydoms; i++)
	{
		nums[0] = i + 2;
		setp(nums);
	}
	setp("");

	/* set up fill factor information */
	setp("minpages");
	if (filled)
	{
		setp("1");
		setp("fillfactor");
		setp("100");
	}
	else
		setp("10");
	specclose(relnum);
	call_dbu(mdMODIFY, FALSE);


	Newq = 1;
	Newr = TRUE;
	call_ovqp(ag, mdRETR, relnum);

	Newq = 0;
	/* if temp relation was used, destroy it */
	if (temp_relnum != NORESULT)
	{
		for (i = 0; resdom = lnodv[i]; i++)
			resdom->right = save_node[i];
		dstr_rel(temp_relnum);
	}
}




modqual(root, lnodv, srcmap, agvar)
QTREE			*root;
QTREE			*lnodv[];
int			srcmap;
int			agvar;
{
	register QTREE			*and_eq;
	register QTREE			*afcn;
	register int			i;
	extern QTREE			*copytree();
	extern QTREE			*makavar();
	extern QTREE			*need();

# ifdef xDTR1
	AAtTfp(14, 5, "modqual 0%o\n", srcmap);
# endif

	for (i = 0; afcn = lnodv[i]; i++)
	{
		/*  'AND' node  */
		and_eq = need(Qbuf, QT_HDR_SIZ + AND_SIZ);
		and_eq->sym.type = AND;
		and_eq->sym.len = AND_SIZ;
		((ROOT_NODE *) and_eq)->tvarc = 0;
		((ROOT_NODE *) and_eq)->lvarc = 0;
		((ROOT_NODE *) and_eq)->lvarm = 0;
		((ROOT_NODE *) and_eq)->rvarm = 0;
		and_eq->right = root->right;
		root->right = and_eq;

		/* 'EQ' node  */
		and_eq->left = need(Qbuf, QT_HDR_SIZ + OP_SIZ);
		and_eq = and_eq->left;
		and_eq->sym.type = BOP;
		and_eq->sym.len = OP_SIZ;
		((OP_NODE *) and_eq)->opno = (int) opEQ;

		/* bydomain opEQ var */
		and_eq->right = copytree(afcn->right, Qbuf);	/* a-fcn (in Source) specifying BY domain */
		and_eq->left = makavar(afcn, agvar, i + 2);	/* VAR ref BY domain */
	}
}
