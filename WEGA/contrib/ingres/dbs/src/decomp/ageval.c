# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"../h/pipes.h"
# include	"decomp.h"

/*
**	AGEVAL -- evaluate simple aggregate.
**
**	Uses trace flag 5
**
**	Ageval is passed the tree of a simple aggregate,
**	and an array of space to store the results. The
**	amount of space actually allocated is stored in
**	(*result)->sym.len
**
**	If the aggregate is unique (eg. countu, sumu, avgu)
**	or if the aggregate is multi-variable, special
**	processing is done. A temporary relation is formed
**	with a result domain for each single agg in the tree.
**	Decomp is called to retrieve
**	the values to be aggregated into that relation.
**
**	If the aggregate is unique, then duplicates are
**	removed from the temporary relation.
**
**	Next the aggregate is run on either the original relation
**	or on the temporary relation.
**
**	Finally the result is read from OVQP and if a
**	temporary relation was used, it is destroyed.
**
*/


QTREE	*ageval(tree, result)
QTREE	*tree;		/* root of aggregate */
QTREE	*result[];	/* space for results */
{
	register QTREE			*aghead;
	register QTREE			*resdom;
	register QTREE			*aop;
	register QTREE			*newtree;
	register int			temp_relnum;
	register int			i;
	QTREE				*lnodv[MAXDOM + 2];
	char				agbuf[AGBUFSIZ];
	extern int			derror();
	extern QTREE			*makavar();
	extern QTREE			*makroot();
	extern QTREE			*makresdom();

	aghead = tree;
	aop = aghead->left;
	temp_relnum = NORESULT;

	/* if PRIME or multi-var, form aggregate domain in temp relation */
	if (prime(aop) || ((ROOT_NODE *) aghead)->tvarc > 1)
	{
		initbuf(agbuf, AGBUFSIZ, AGBUFFULL, derror);

		lnodv[lnode(aop, lnodv, 0)] = 0;

		/* create new tree for retrieve and give it the qualification */
		newtree = makroot(agbuf);
		newtree->right = aghead->right;
		aghead->right = Qle;

		/* put a resdom on new tree for each aop in orig tree */
		/* make each aop in orig tree reference new relation */
		for (i = 0; aop = lnodv[i]; )
		{
			/* create resdom for new tree */
			resdom = makresdom(agbuf, aop);
			((RES_NODE *) resdom)->resno = ++i;
			resdom->right = aop->right;

			/* connect it to newtree */
			resdom->left = newtree->left;
			newtree->left = resdom;

			/* make orig aop reference new relation */
			aop->right = makavar(resdom, FREEVAR, i);
		}

		/* make result relation */
		temp_relnum = mak_t_rel(newtree, "a", -1);

		/* prepare for query */
		mapvar(newtree, 0);
		decomp(newtree, mdRETR, temp_relnum);
		Rangev[FREEVAR].relnum = temp_relnum;
		Sourcevar = FREEVAR;

		/* if prime, remove dups */
		if (prime(aghead->left))
		{
			/* modify to heapsort */
			removedups(FREEVAR);
		}

	}

	Newq = 1;
	Newr = TRUE;

	call_ovqp(aghead, mdRETR, NORESULT);	/* call ovqp with no result relation */
	Newq = 0;

	/* pick up results */
	readagg_result(result);

	/* if temp relation was created, destroy it */
	if (temp_relnum != NORESULT)
		dstr_rel(temp_relnum);

}

prime(aop)
QTREE			*aop;

/*
**	Determine if an aggregate contains any
**	prime aggregates. Note that there might
**	be more than one aggregate.
*/

{
	register QTREE			*a;

	a = aop;
	do
	{
		switch (a->sym.value[0])
		{
		  case opCNTU:
		  case opSUMU:
		  case opAVGU:
			return (TRUE);
		}
	} while (a = a->left);
	return (FALSE);
}

removedups(var)
register int	var;
/*
**	Remove dups from an unopened relation
**	by calling heapsort
*/
{
	register char	*p;
	extern char	*rangename();

	closer1(var);	/* guarantee that relation has been closed */
	initp();
	p = rangename(var);	/* get name of relation */
#	ifdef xDTR1
	AAtTfp(5, 1, "removing dups from %s\n", p);
#	endif
	setp(p);
	setp("heapsort");
	setp("num");
	call_dbu(mdMODIFY, FALSE);
}
