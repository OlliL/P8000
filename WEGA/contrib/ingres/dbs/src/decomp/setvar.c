# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"../h/pipes.h"
# include	"decomp.h"

/*
** SETVAR -- Routines to convert a VAR to a constant and back to a VAR
**
**	This file contains the routines for tuple substitution.
**
**	Setvar -- Make a VAR node reference its position in the tuple.
**
**	Clrvar -- Make the VAR node refer back to a VAR
*/

setvar(tree1, var, intid, tuple)
QTREE		 	*tree1;
int			var;
long			*intid;
char			*tuple;

/*
** Setvar -- Var's are changed to reference their values in a tuple.
**	ROOT and AND nodes are changed to update the variable maps.
*/

{
	register QTREE			*tree;
	register int			mask;
	register int			nvc;
	register DESC			*d;
	extern DESC			*readopen();
	extern QTREE			*ckvar();

	tree = tree1;
	if (!tree)
		return;
	switch (tree->sym.type)
	{
	  case VAR:
		if (((VAR_NODE *) (tree = ckvar(tree)))->varno == var)
		{
#			ifdef xDTR1
			if (AAtTfp(12, 0, "SETVAR: %d)tree:", var))
				writenod(tree);
#			endif
			if (((VAR_NODE *) tree)->attno)
			{
				d = readopen(var);
				nvc = ctou(((VAR_NODE *) tree)->attno);
				((VAR_NODE *) tree)->valptr = tuple + d->reloff[nvc];
			}
			else
				((VAR_NODE *) tree)->valptr = (char *) intid;
		}
		return;

	  case ROOT:
	  case AND:
		mask = 01 << var;
		nvc = ((ROOT_NODE *) tree)->tvarc;
		if (((ROOT_NODE *) tree)->lvarm & mask)
		{
			setvar(tree->left, var, intid, tuple);
			((ROOT_NODE *) tree)->lvarm &=  ~mask;
			--((ROOT_NODE *) tree)->lvarc;
			nvc = ((ROOT_NODE *) tree)->tvarc - 1;
		}
		if (((ROOT_NODE *) tree)->rvarm & mask)
		{
			setvar(tree->right, var, intid, tuple);
			((ROOT_NODE *) tree)->rvarm &=  ~mask;
			nvc = ((ROOT_NODE *) tree)->tvarc - 1;
		}
		((ROOT_NODE *) tree)->tvarc = nvc;
		return;

	  default:
		setvar(tree->left, var, intid, tuple);
		setvar(tree->right, var, intid, tuple);
		return;
	}
}


clearvar(tree1, var1)
QTREE			*tree1;
int			var1;

/*
**	Clearvar is the opposite of setvar. For
**	each occurence of var1 in the tree, clear
**	the valptr.
*/

{
	register QTREE			*tree;
	int				var;
	extern QTREE			*ckvar();

	tree = tree1;
	if (!tree)
		return;

	var = var1;
	if (tree->sym.type == VAR)
	{
		if (((VAR_NODE *) (tree = ckvar(tree)))->varno == var)
			((VAR_NODE *) tree)->valptr = (char *) 0;
		return;
	}
	clearvar(tree->left, var);
	clearvar(tree->right, var);
}
