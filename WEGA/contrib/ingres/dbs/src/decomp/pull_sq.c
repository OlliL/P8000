# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"../h/pipes.h"
# include	"decomp.h"

/*
** DECOMP1.C
**
**	contains routines associated with setting up
**	detachable 1-variable sub-queries.
**	ptrs to these sq's are kept in the
**	array 'sqlist' declared in the main decomp routine.
**
*/


pull_sq(tree1, sqlist, locrang, sqrange, buf)
QTREE			*tree1;
QTREE			*sqlist[];
int			locrang[];
int			sqrange[];
char			*buf;
{
	register QTREE		 	*q;
	register QTREE			*tree;
	register QTREE			*r;
	register QTREE	 		*s;
	register int			anysq;
	register int			j;
	register int			badvar;
	extern QTREE			*makroot();

	tree = tree1;

#	ifdef xDTR1
	AAtTfp(10, 0, "PULL_SQ: tree=0%o\n", tree);
#	endif

	anysq = 0;
	for (j = 0; j < MAXRANGE; j++)
		sqlist[j] = 0;

	if (((ROOT_NODE *) tree)->tvarc == 1)
		return;

	/* detach all one variable clauses except:
	** if the target list is one variable and
	** that variable is disjoint from the other
	** variables, then don't pull it.
	**
	** It will be more efficient to process it
	** all at once in decompy
	*/

	badvar = 0;
	if (((ROOT_NODE *) tree)->lvarc == 1)
	{
		badvar = ((ROOT_NODE *) tree)->lvarm;	/* get bit position of var */

		/* look for a two variable clause involving badvar */
		for (r = tree->right; r->sym.type != QLEND; r = r->right)
		{
			if (((ROOT_NODE *) r)->lvarc > 1 && (((ROOT_NODE *) r)->lvarm & badvar))
			{
				badvar = 0;
				break;
			}
		}
	}
#	ifdef xDTR1
	AAtTfp(10, 2, "Detachable clauses: (badvar=0%o)\n", badvar);
#	endif
	for (r=tree; r->right->sym.type!=QLEND; )
	{
#		ifdef xDTR1
		if (AAtTf(10, 3))
			writenod(r);
#		endif
		q = r;
		r = r->right;
		if (((ROOT_NODE *) r)->lvarc == 1)
		{
			j = bitpos(((ROOT_NODE *) r)->lvarm);
#			ifdef xDTR1
			if (AAtTfp(10, 4, "var=%d,", j))
				printree(r->left, "clause");
#			endif
			if (((ROOT_NODE *) r)->lvarm == badvar)
			{
#				ifdef xDTR1
				AAtTfp(10, 5, "not detaching \n");
#				endif
				continue;
			}
			anysq++;

			if (!sqlist[j])		/* MAKE ROOT NODE FOR SUBQUERY */
				sqlist[j] = makroot(buf);
			s = sqlist[j];

			/* MODIFY MAIN QUERY */

			q->right = r->right;

			/* MODIFY `AND` NODE OF DETACHED CLAUSE */

			r->right = s->right;
			((ROOT_NODE *) r)->rvarm = ((ROOT_NODE *) s)->rvarm;
			((ROOT_NODE *) r)->tvarc = 1;

			/* ADD CLAUSE TO SUB-QUERY */

			s->right = r;
			((ROOT_NODE *) s)->rvarm = ((ROOT_NODE *) r)->lvarm;
			((ROOT_NODE *) s)->tvarc = 1;

#			ifdef xDTR1
			if (AAtTf(10, 6))
				printree(s, "SQ");
#			endif

			r = q;
		}
	}

	/* NOW SET UP TARGET LIST FOR EACH SUBQUERY IN SQLIST */

#	ifdef xDTR1
	AAtTfp(10, 7, "# sq clauses=%d\n", anysq);
#	endif
	if (anysq)
	{
#		ifdef xDTR1
		AAtTfp(10, 8, "Dfind\n");
#		endif
		dfind(tree, buf, sqlist);
		mapvar(tree, 1);

		/* create the result relations */
		for (j = 0; j < MAXRANGE; j++)
		{
			if (q = sqlist[j])
			{
				if (q->left->sym.type != TREE)
				{
					savrang(locrang, j);
					sqrange[j] = mak_t_rel(q, "d", -1);
				}
				else
					sqrange[j] = NORESULT;
			}
		}
	}
}


dfind(tree1, buf, sqlist)
QTREE		 *tree1;
char		 *buf;
QTREE		 *sqlist[];
{
	register int			varno;
	register QTREE			*tree;
	register QTREE			*sq;
	extern QTREE			*ckvar();

	tree = tree1;
	if (!tree)
		return;
#	ifdef xDTR1
	if (AAtTf(10, 9))
		writenod(tree);
#	endif
	if (tree->sym.type == VAR)
	{
		tree = ckvar(tree);
		varno = ((VAR_NODE *) tree)->varno;
		if (sq = sqlist[varno])
			maktl(tree, buf, sq, varno);
		return;
	}

	/* IF CURRENT NODE NOT A `VAR` WITH SQ, RECURSE THRU REST OF TREE */

	dfind(tree->left, buf, sqlist);
	dfind(tree->right, buf, sqlist);
	return;
}



maktl(node, buf, sq1, varno)
QTREE			*node;
char			*buf;
QTREE			*sq1;
int			varno;
{
	register QTREE		 	*resdom;
	register QTREE			*tree;
	register QTREE			*sq;
	register int			domno;
	register int			map;
	extern QTREE			*makresdom();
	extern QTREE			*copytree();

	sq = sq1;
	domno = ctou(((VAR_NODE *) node)->attno);

#	ifdef xDTR1
	AAtTfp(10, 12, "\tVar=%d,Dom=%d ", varno, domno);
#	endif
	/* CHECK IF NODE ALREADY CREATED FOR THIS DOMAIN */

	for (tree = sq->left; tree->sym.type != TREE; tree = tree->left)
		if (ctou(((VAR_NODE *) tree->right)->attno) == domno)
		{
#			ifdef xDTR1
			AAtTfp(10, 13, "Domain found\n");
#			endif
			return;
		}

	/* create a new resdom for domain */

	resdom = makresdom(buf, node);
	*resdom->sym.value = sq->left->sym.type == TREE? 1:
					*sq->left->sym.value + 1;
	/* resdom->right is a copy of the var node in order to
	** protect against tempvar() changing the var node.
	*/
	resdom->left = sq->left;
	resdom->right = copytree(node, buf);


	/* update ROOT node if necessary */

	sq->left = resdom;
	map = 1 << varno;
	if (!(((ROOT_NODE *) sq)->lvarm & map))
	{
		/* var not currently in tl */
		((ROOT_NODE *) sq)->lvarm |= map;
		((ROOT_NODE *) sq)->lvarc++;

		/* if var is not in qualification then update total count */
		if (!(((ROOT_NODE *) sq)->rvarm & map))
			((ROOT_NODE *) sq)->tvarc++;
#		ifdef xDTR1
		if (AAtTfp(10, 15, "new root "))
			writenod(sq);
#		endif
	}

#	ifdef xDTR1
	if (AAtTfp(10, 14, "new dom "))
		writenod(resdom);
#	endif
	return;
}
