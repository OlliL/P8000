# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"
# include	"../h/aux.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"../h/pipes.h"
# include	"decomp.h"

struct agglist
{
	QTREE	**father;	/* addr of pointer to you */
	QTREE	*agpoint;	/* pointer to aghead */
	QTREE	*agfather;	/* is your father an aggregate? */
	int	agvarno;	/* var # assigned to aggr fnct */
};

struct hitlist
{
	QTREE	**trepr;	/* position in tree to be changed */
	int	byno;		/* by-list position */
};

static struct agglist	*Aggnext;

aggregate(root)
QTREE			*root;
/*
**	AGGREGATE - replace aggregates with their values
**
**	Aggregate attempts to optimize aggregate processing
**	wherever possible. It replaces aggregates with their
**	values, and links aggregate functions which have
**	identical by-lists together.
**
**	Note that for the sake of this code, A "prime"
**	aggregate is one in which duplicates are removed.
**	These are COUNTU, SUMU, and AVGU.
**
**	Aggregate first forms a list of all aggregates in
**	the order they should be processed.
**
**	For each aggregate, it looks at all other aggregates
**	to see if there are two simple aggregates
**	or if there is another aggregate funtion with the
**	same by-list.
**
**	An attempt is made to run
**	as many aggregates as possible at once. This can be
**	done only if two or more aggregates have the same
**	qualifications and in the case of aggregate functions,
**	they must have identical by-lists.
**	Even then, certain combinations
**	of aggregates cannot occure together. The list is
**	itemized later in the code.
**
**	Aggregate calls BYEVAL or AGEVAL to actually process
**	aggregate functions or aggregates respectively.
**
*/
{
	struct agglist			alist[MAXAGG + 1];
	QTREE				*rlist[MAXAGG + 1];
	register struct agglist		*al;
	register QTREE			*agg;
	register QTREE			*aop1;
	register QTREE			*r;
	register struct agglist		*al1;
	register QTREE			*aop;
	register QTREE			*agg1;
	int				i;
	int				simple_agg;
	int				varmap;
	int				attcnt;
	int				anyagg;
	int				attoff;
	int				twidth;
	extern QTREE			*makavar();
	extern QTREE			*agspace();

	al = alist;
	Aggnext = al;

	findagg(&root, root);	/* generate list of all aggregates */
	Aggnext->agpoint = 0;	/* mark end of the list */
	anyagg = 0;

	varmap = ((ROOT_NODE *) root)->lvarm | ((ROOT_NODE *) root)->rvarm;

	/* process each aggregate */
	for ( ; agg = al->agpoint; al++)
	{

		/* is this still an aggregate? */
		if (agg->sym.type != AGHEAD)
			continue;
		mapvar(agg, 0);	/* map the aggregate tree */
		anyagg++;

		Sourcevar = bitpos(((ROOT_NODE *) agg)->lvarm | ((ROOT_NODE *) agg)->rvarm);
#		ifdef xDTR1
		AAtTfp(6, 4, "Sourcevar=%d,rel=%s\n", Sourcevar, rangename(Sourcevar));
#		endif

		simple_agg = (agg->left->sym.type == AOP);	/* TRUE if no bylist */
		aop = agg->left;	/* assume it was TRUE */
#		ifdef xDTR1
		AAtTfp(6, 0, "%s\n", simple_agg ? "agg" : "agg-func");
#		endif
		if (simple_agg)
		{
			/* simple aggregate */
			rlist[0] = agspace(aop);
			/* init width to the width of the aggregate */
			twidth = ctou(((VAR_NODE *) aop)->frml);
		}
		else
		{
			attoff = ((RES_NODE *) agg->left->left)->resno + 2;
			aop = aop->right;	/* find the AOP node */
			/* assign  new source variable for aggregate */
			al->agvarno = getrange(&varmap);
			/* compute width of bylist + count field */
			twidth = findwid(agg->left) + sizeof (long);

			/* make sure the aggregate does not exceed max dimensions */
			if (chkwidth(aop, &twidth, attoff))
				derror(AGFTOBIG);
		}
		attcnt = 1;	/* one aggregate so far */

		/* look for another identical aggregate */
		for (al1 = al + 1; agg1 = al1->agpoint; al1++)
		{

			/* if agg is nested inside agg1 then ignore it */
			if (al->agfather == agg1 || agg1->sym.type != AGHEAD)
				continue;

			/* split aggs and agg-func apart */
			/* check for identical aggregate */
			if (simple_agg)
			{
				aop1 = agg1->left;	/* find AOP node */

				if (aop1->sym.type != AOP)
					continue;	/* not a simple agg */

				/* make sure they can be run together */
				if (checkagg(agg, aop, agg1, aop1) == 0)
					continue;

				if ((i = sameagg(agg, aop1, attcnt)) >= 0)
				{
					/* found identical aggregate to rlist[i] */
					r = rlist[i];
				}
				else
				{
					/* put this agg in with the others */

					/* first make sure it won't exceed tuple length */
					if (chkwidth(aop1, &twidth, 0))
						continue;	/* can't be included */
					r = rlist[attcnt++] = agspace(aop1);

					/* connect into tree */
					aop1->left = agg->left;
					agg->left = aop1;
				}
			}
			else
			{
				/* aggregate function */
				if (!sameafcn(agg->left->left, agg1->left->left))
					continue;

				aop1 = agg1->left->right;	/* find AOP */


				if (checkagg(agg, aop, agg1, aop1) == 0)
				{
					/* same by-lists but they can't be run together */
					continue;
				}

				if ((i = sameagg(agg, aop1, attcnt)) < 0)
				{
					/* make sure there is room */
					if (chkwidth(aop1, &twidth, attcnt + attoff))
						continue;

					/* add aggregate into tree */
					i = attcnt++;

					aop1->left = agg->left->right;
					agg->left->right = aop1;
				}

				r = makavar(aop1, al->agvarno, i + attoff);
			}
			/* replace aggregate in original tree with its value */
			*(al1->father) = r;

			/* remove aggregate from local list */
			agg1->sym.type = -1;
#			ifdef xDTR1
			AAtTfp(6, 3, "including aghead 0%o\n", agg1);
#			endif
		}

		/* process aggregate */
		if (simple_agg)
		{
			rlist[attcnt] = 0;
			ageval(agg, rlist);	/* pass tree and result list */
			r = rlist[0];
		}
		else
		{
			opt_bylist(alist, agg);
			byeval(al->agfather, agg, al->agvarno);
			r = makavar(aop, al->agvarno, attoff);
		}
		/*
		** Link result into tree. al->father hold the address
		** &(tree->left) or &(tree->right).
		*/
		*(al->father) = r;
#		ifdef xDTR1
		if (AAtTf(6, 4))
			printree(*(al->father), "aggregate value");
#		endif
	}
	if (anyagg)
	{
		opt_bylist(alist, root);
		mapvar(root, 0);	/* remap main tree */
	}
}

findagg(nodep,  agf)
QTREE			**nodep;
QTREE			*agf;
/*
**	findagg builds a list of all aggregates
**	in the tree. It finds them by leftmost
**	innermost first.
**
**	The parameters represent:
**		nodep:	the address of the node pointing to you
**				eg. &(tree->left) or &(tree->right)
**		agf:	the root node. or if we are inside
**			a nested aggregate, the AGHEAD node
*/
{
	register QTREE			*q;
	register QTREE			*f;
	register struct agglist		*list;
	register int			agg;

	if (!(q = *nodep))
		return;

	f = agf;
	if (agg = (q->sym.type == AGHEAD))
		f = q;	/* this aggregate is now the father root */

	/* find all aggregates below */
	findagg(&(q->left), f);
	findagg(&(q->right), f);

	/* if this is an aggregate, put it on the list */
	if (agg)
	{
		list = Aggnext;
		list->father = nodep;
		list->agpoint = q;
		list->agfather = agf;
		Aggnext++;
	}
}



QTREE		 *agspace(aop)
QTREE			*aop;
/*
**	Agspace allocates space for the result of
**	a simple aggregate.
*/
{
	register QTREE			*a;
	register QTREE			*r;
	register int			length;
	extern QTREE			*need();

	a = aop;
	length = ctou(((VAR_NODE *) a)->frml);

	r = need(Qbuf, length + QT_HDR_SIZ);
	r->left = r->right = (QTREE *) 0;
	r->sym.type = ((VAR_NODE *) a)->frmt;
	r->sym.len = length;

	return (r);
}



chkwidth(aop, widthp, domno)
QTREE			*aop;
int			*widthp;
int			domno;

/*
** Chkwidth -- make sure that the inclusion of another aggregate will
**	not exceed the system limit. This means that the total width
**	cannot exceed MAXTUP and the number of domains cannot exceed MAXDOM-1
*/

{
	register int	width;

	width = *widthp;

#	ifdef xDTR1
	AAtTfp(6, 10, "agg width %d,dom %d\n", width, domno);
#	endif

	width += ctou(((VAR_NODE *) aop)->frml);

	if (width > AAmaxtup || domno > MAXDOM - 1)
		return (1);

	*widthp = width;
	return (0);
}


cprime(aop)
register QTREE		*aop;
/*
**	Determine whether an aggregate is prime
**	or a don't care aggregate. Returns TRUE
**	if COUNTU,SUMU,AVGU,MIN,MAX,ANY.
**	Returns false if COUNT,SUM,AVG.
*/
{
	register int	i;

	i = TRUE;
	switch (aop->sym.value[0])
	{
	  case opCNT:
	  case opSUM:
	  case opAVG:
		i = FALSE;
	}
	return (i);
}


getrange(varmap)
int	*varmap;
/*
**	Getrange find a free slot in the range table
**	for an aggregate function.
**
**	If there are no free slots,derror is called
*/
{
	register int	i;
	register int	map;
	register int	bit;

	map = *varmap;

	for (i = 0; i < MAXRANGE; i++)
	{
		/* if slot is used, continue */
		if ((bit = 1 << i) & map)
			continue;

		map |= bit;	/* "or" bit into the map */
		*varmap = map;

#		ifdef xDTR1
		AAtTfp(6, 10, "Assn var %d, map 0%o\n", i, map);
#		endif

		return (i);
	}
	derror(NODESCAG);
}


checkagg(aghead1, aop1, aghead2, aop2)
QTREE			*aghead1;
QTREE			*aop1;
QTREE			*aghead2;
QTREE			*aop2;
{
	register QTREE			*aop_1;
	register QTREE			*aop_2;
	register QTREE			*agg1;
	register int			ok;

	/* two aggregate functions can be run together
	** according to the following table:
	**
	**		prime	!prime	don't care
	**
	** prime	afcn?	never	afcn?
	** !prime	never	always	always
	** don't care	afcn?	always	always
	**
	** don't care includes: ANY, MIN, MAX
	** afcn? means only if a-fcn's are identical
	*/

	aop_1 = aop1;
	aop_2 = aop2;
	agg1 = aghead1;

	if (!prime(aop_1) && !prime(aop_2))
		ok = TRUE;
	else
		if (sameafcn(aop_1->right, aop_2->right))
			ok = cprime(aop_1) && cprime(aop_2);
		else
			ok = FALSE;
	/* The two aggregates must range over the same variables */
	if ((((ROOT_NODE *) agg1)->lvarm | ((ROOT_NODE *) agg1)->rvarm) != (((ROOT_NODE *) aghead2)->lvarm | ((ROOT_NODE *) aghead2)->rvarm))
		ok = FALSE;


	/* check the qualifications */
	if (ok)
		ok = sameafcn(agg1->right, aghead2->right);
	return (ok);
}


sameagg(aghead, newaop, agg_depth)
QTREE			*aghead;
QTREE			*newaop;
int			agg_depth;
{
	register QTREE			*agg;
	register QTREE			*newa;
	register int			i;

	agg = aghead;
	newa = newaop;
	agg = agg->left;
	if (agg->sym.type == BYHEAD)
		agg = agg->right;

	/* agg now points to first aggregate */
	for (i = 1; agg; agg = agg->left, i++)
		if (sameafcn(agg->right, newa->right) && agg->sym.value[0] == newa->sym.value[0])
		{
#			ifdef xDTR1
			AAtTfp(6, 6, "found identical aop 0%o\n", newa);
#			endif
			return (agg_depth - i);
		}

	/* no match */
	return (-1);
}


static struct hitlist	*Hnext;
static struct hitlist	*Hlimit;


opt_bylist(alist, root)
struct agglist		*alist;
QTREE			*root;
{
	register struct agglist		*al;
	register QTREE			*agg;
	register struct hitlist		*hl;
	register QTREE			**tpr;
	register QTREE			*tree;
	QTREE				*lnodv[MAXDOM+2];
	struct hitlist			hlist[30];
	register int			treemap;
	register int			i;
	int				anyop;
	extern QTREE			*makavar();
	int				usedmap;
	int				vars;

	/* compute bitmap of all possible vars in tree (can include xtra vars) */
	treemap = ((ROOT_NODE *) root)->lvarm | ((ROOT_NODE *) root)->rvarm;
	anyop = FALSE;

	/* scan the list of aggregates looking for one nested in root */
	for (al = alist; (agg = al->agpoint) && agg != root; al++)
	{
		if (agg->sym.type == AGHEAD && agg->left->sym.type == BYHEAD &&
				al->agfather == root)
		{

			/* this aggregate function is nested in root */

			/* make sure it has some vars of interest */
			if ((treemap & varfind(agg->left->left, (QTREE *) 0)) == 0)
				continue;

#			ifdef xDTR1
			if (AAtTf(6, 11))
				printree(agg, "nested agg");
#			endif

			/* form list of bydomains */
			lnodv[lnode(agg->left->left, lnodv, 0)] = 0;
			usedmap = 0;

			Hnext = &hlist[0];
			Hlimit = &hlist[30];

			/* find all possible replacements */
			vars = modtree(&root, lnodv, &usedmap);

			/*
			** All references to a variable must be replaced
			** in order to use this aggregates by-domains.
			*/
			if (usedmap && ((vars & usedmap) == 0))
			{
#				ifdef xDTR1
				AAtTfp(6, 7, "Committed\n");
#				endif
				/* success. Committ the tree changes */
				Hnext->trepr = (QTREE **) 0;

				for (hl = &hlist[0]; tpr = hl->trepr; hl++)
				{
					/* get bydomain number */
					i = hl->byno;

					/* get node being replaced */
					tree = *tpr;

					/* if it is already a var, just change it */
					if (tree->sym.type == VAR)
					{
						((VAR_NODE *) tree)->varno = al->agvarno;
						((VAR_NODE *) tree)->attno = i + 2;
					}
					else
						*tpr = makavar(lnodv[i], al->agvarno, i + 2);

					anyop = TRUE;
#					ifdef xDTR1
					if (AAtTf(6, 7))
						printree(*tpr, "modified tree");
#					endif
				}
			}
			/* vars is now a map of the variables in the root */
			treemap = vars;
		}
	}

	/* if any changes were made, get rid of the unnecessary links */
	if (anyop)
		chklink(root);
}




modtree(pnode, lnodv, replmap)
QTREE			**pnode;
QTREE			*lnodv[];
int			*replmap;
{
	register QTREE			*tree;
	register int			vars;
	register int			i;
	register QTREE			*afcn;

	/* point up current node */
	if (!(tree = *pnode))
		return (0);

	/* examine each by-list for match on this subtree */
	for (i = 0; afcn = lnodv[i]; i++)
	{
		if (sameafcn(tree, afcn->right))
		{
#			ifdef xDTR1
			if (AAtTf(6, 9))
				printree(tree, "potential Jackpot");
#			endif
			vars = varfind(tree, (QTREE *) 0);
			if (Hnext == Hlimit)
				return (vars);	/* no more room */

			/* record what needs to be done */
			Hnext->trepr = pnode;
			Hnext->byno = i;
			Hnext++;
			*replmap |= vars;
			return (0);
		}
	}
	if (tree->sym.type == VAR)
		return (01 << ((VAR_NODE *) tree)->varno);

	/* try the subtrees */
	vars = modtree(&(tree->left), lnodv, replmap);
	if ((vars & *replmap) == 0)
		vars |= modtree(&(tree->right), lnodv, replmap);

	return (vars);
}


chklink(root)
QTREE			*root;
{
	register QTREE			*r;
	register QTREE			*b;
	register QTREE			*last;

	last = root;

	for (r = last->right; r->sym.type != QLEND; r = r->right)
	{
		/* if this is an EQ node then check for an unnecessary compare */
		if ((b = r->left)->sym.type == BOP && ((OP_NODE *) b)->opno == (int) opEQ)
		{
			if (sameafcn(b->left, b->right))
			{
#				ifdef xDTR1
				if (AAtTf(6, 5))
					printree(b, "unnec clause");
#				endif
				last->right = r->right;
				continue;
			}
		}
		last = r;
	}
}



sameafcn(t1, t2)
register QTREE		*t1;
register QTREE		*t2;
{
	register int			len;
	register int			type;

	if (!(t1 && t2))
		return (!(t1 || t2));

	if (!AAbequal(&t1->sym, &t2->sym, TYP_LEN_SIZ))
		return (0);

	len = ctou(t1->sym.len);
	if ((type = t1->sym.type) == VAR)
		len = VAR_HDR_SIZ;
	else if (type == AND)
		len = 0;
	if (len && !AAbequal(t1->sym.value, t2->sym.value, len))
		return (0);
	return (sameafcn(t1->left, t2->left) && sameafcn(t1->right, t2->right));
}


varfind(root, aghead)
QTREE			*root;
QTREE			*aghead;

/*
**	varfind -- find all variables in the tree pointed to by "root".
**		Examine all parts of the tree except aggregates. For
**		aggregates, ignore simple aggregate and look only
**		at the by-lists of aggregate functions. If the aggregate
**		is "aghead" then ignore it. There is no reason to look
**		at yourself!!!!
**		This routine is called by byeval() to determine
**		whether to link the aggregate to the root tree.
**
**	Curiosity note: since the tree being examined has not been
**	processed by decomp yet, ckvar does not need to be called
**	since the var could not have been altered.
*/

{
	register QTREE			*tree;
	register int			type;


	if (!(tree = root))
		return (0);

	if ((type = tree->sym.type) == AGHEAD)
	{
		/* ignore if it matches aghead */
		if (tree == aghead)
			return (0);
		/* if this is an aggregate function, look at bylist */
		tree = tree->left;
		if ((type = tree->sym.type) != BYHEAD)
			return (0);
	}

	if (type == VAR)
		return (1 << ((VAR_NODE *) tree)->varno);

	return (varfind(tree->left, aghead) | varfind(tree->right, aghead));
}
