# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"
# include	"../h/aux.h"
# include	"qrymod.h"

/*
**  TREEPR -- print tree for debugging
**
**	This module prints a tree for debugging purposes.  There are
**	two interfaces: treepr prints an entire treeree, and nodepr
**	prints a single node.
**
**	Defines:
**		treepr -- tree print interface.
**		rcsvtrpr -- the recursive part to traverse the tree.
**		nodepr -- node print interface.
*/

extern char	*Qmdname[];	/* defined in util.c */

/*
**  TREEPR -- print tree for debugging
**
**	This routine prints a tree for debugging.
**
**	Parameters:
**		t -- root of tree to be printed
**		label -- label to print for identifying the tree.  NULL
**			is OK and produces a default label.
*/
treepr(tree, label)
register QTREE	*tree;
register char	*label;
{
	register int	i;

	/* print label */
	putchar('\n');
	if (label != (char *) 0)
		printf("%s ", label);
	printf("Querytree @ 0%o:\n", tree);

	/* print range table */
	for (i = 0; i < MAXVAR + 1; i++)
	{
		if (!Rangev[i].rused)
			continue;
		printf("range of %d is %.12s [O=%.2s, S=0%o]\n",
		    i, Rangev[i].relid, Rangev[i].rowner, Rangev[i].rstat);
	}

	/* print query type */
	if (Qmode >= 0)
		printf("%s ", Qmdname[Qmode]);

	/* print result relation if realistic */
	if (Resultvar >= 0)
		printf("Resultvar %d ", Resultvar);
	putchar('\n');

	/* print tree */
	rcsvtrpr(tree);

	/* print exciting final stuff */
	putchar('\n');
}


/*
**  RCSVTRPR -- traverse and print tree
**
**	This function does the real stuff for treepr.  It recursively
**	traverses the tree in postfix order, printing each node.
**
**	Parameters:
**		tree -- the root of the tree to print.
*/
rcsvtrpr(tree)
register QTREE	*tree;
{
	while (tree != (QTREE *) 0)
	{
		rcsvtrpr(tree->left);
		nodepr(tree, FALSE);
		tree = tree->right;
	}
}


/*
**  NODEPR -- print node for debugging
**
**	Parameters:
**		tree -- the node to print.
*/
nodepr(tree)
register QTREE	*tree;
{
	register int	ty;
	register int	l;
	register char	*cp;

	ty = tree->sym.type;
	l = ctou(tree->sym.len);

	printf("0%o: 0%o, 0%o/ ", tree, tree->left, tree->right);
	printf("%d, %d: ", ty, l);

	switch (ty)
	{
	  case VAR:
		printf("%d.%d [%d/%d]",
		((VAR_NODE *) tree)->varno, ((VAR_NODE *) tree)->attno,
		((VAR_NODE *) tree)->frmt, ctou(((VAR_NODE *) tree)->frml));
		break;

	  case RESDOM:
		printf("%d [%d/%d]", ((RES_NODE *) tree)->resno,
		((VAR_NODE *) tree)->frmt, ((VAR_NODE *) tree)->frml);
		break;

	  case AOP:
		printf("%d [%d/%d] [%d/%d]", ((RES_NODE *) tree)->resno,
		((VAR_NODE *) tree)->frmt, ((VAR_NODE *) tree)->frml,
		((AOP_NODE *) tree)->agfrmt, ((AOP_NODE *) tree)->agfrml);
		break;

	  case UOP:
	  case BOP:
	  case COP:
	  case INT:
		switch (tree->sym.len)
		{
		  case sizeof (char):
		  case sizeof (short):
			printf("%d", tree->sym.value[0]);
			break;

		  case sizeof (long):
			printf("%ld", ((I4TYPE *) tree->sym.value)->i4type);
			break;
		}
		break;

	  case FLOAT:
		printf("%.10f", ((I4TYPE *) tree->sym.value)->i4type);
		break;

	  case CHAR:
		cp = (char *) tree->sym.value;
		while (l--)
		{
			putchar(*cp);
			cp++;
		}
		break;

	  case TREE:
	  case AND:
	  case OR:
	  case QLEND:
	  case BYHEAD:
	  case ROOT:
	  case AGHEAD:
		break;

	  default:
		AAsyserr(19053, ty);
	}
	printf("/\n");
}
