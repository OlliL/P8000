# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"../h/aux.h"

/*
**  TRBUILD -- Rebuild a tree in memory
**
**	Trbuild is called with a pointer to the TREE node of
**	a query already in memory. It rebuilds the pointer
**	structure assuming the querytree is in postfix order.
**
**	Parameters:
**		p - a pointer to the TREE node
**
**	Returns:
**		(QTREE *) 0 - Internal stack overflow (STACKSIZ)
**		pointer to the ROOT node
**
**	Side Effects:
**		All left & right pointers are rebuilt.
*/
QTREE	 *trbuild(p)
register char	*p;	/* really ptr to QTREE		 */
{
	register QTREE		 	**stackptr;
	register SYMBOL	 		*s;
	QTREE				*treestack[STACKSIZ];

	stackptr = treestack;

	for ( ; ; p += QT_HDR_SIZ + ctou(ALIGNMENT(ctou(s->len))))
	{
		s = &((QTREE *) p)->sym;
		((QTREE *) p)->left = ((QTREE *) p)->right = (QTREE *) 0;

		/* reunite p with left and right children on stack, if any*/
		if (!leaf(p))		/* this node has children */
		{
			if (s->type != UOP)
				if (stackptr <= treestack)
				{
err:
					AAsyserr(15038);
				}
				else
					((QTREE *) p)->right = *(--stackptr);
			if (s->type != AOP)
				if (stackptr <= treestack)
					goto err;
				else
					((QTREE *) p)->left = *(--stackptr);
		}

		/*
		** If this is a ROOT node then the tree is complete.
		** verify that there are no extra nodes in the
		** treestack.
		*/
		if (s->type == ROOT)	 	/* root node */
		{
			if (stackptr != treestack)
				AAsyserr(15039);
			return ((QTREE *) p);
		}

		/* stack p */
		if (stackptr-treestack >= STACKSIZ)
			return ((QTREE *) 0);	/* error:stack full */
		*(stackptr++) = (QTREE *) p;

	}
}


leaf(p)
register QTREE	 *p;
{
	switch (p->sym.type)
	{
	  case VAR:
	  case TREE:
	  case QLEND:
	  case INT:
	  case FLOAT:
	  case CHAR:
	  case COP:
		return (1);

	  default:
		return (0);
	}
}
