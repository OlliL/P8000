# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"

/*
**	MAPVAR -- construct varable maps for ROOT, AND, and AGHEAD nodes.
**	tl is a flag  which indicates if the target list should
**	be included in the mapping.  If tl = 0, it should; else it should not.
*/

mapvar(tree, tl)
QTREE			*tree;
int			 tl;
{
	register QTREE			*t;
	register int			rmap;
	register int			lmap;
	extern QTREE			*ckvar();

	t = tree;
	if (t == 0)
		return (0);

	switch (t->sym.type)
	{

	  case ROOT:
	  case AND:
	  case AGHEAD:
		/* map the right side */
		((ROOT_NODE *) t)->rvarm = rmap = mapvar(t->right, tl);

		/* map the left side or else use existing values */
		if (tl == 0)
		{
			((ROOT_NODE *) t)->lvarm = lmap = mapvar(t->left, tl);
			((ROOT_NODE *) t)->lvarc = bitcnt(lmap);
		}
		else
			lmap = ((ROOT_NODE *) t)->lvarm;

		/* form map of both sides */
		rmap |= lmap;

		/* compute total var count */
		((ROOT_NODE *) t)->tvarc = bitcnt(rmap);

		return (rmap);

	  case VAR:
		if (((VAR_NODE *) (t = ckvar(t)))->valptr)
			return (0);	/* var is a constant */
		return (01 << ((VAR_NODE *) t)->varno);
	}

	/* node is not a VAR, AND, ROOT, or AGHEAD */
	return (mapvar(t->left, tl) | mapvar(t->right, tl));
}
