# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/tree.h"
# include	"qrymod.h"

/*
**  QRYMOD -- main driver for query modification
**
**	Reads in the query tree, performs the modifications, writes
**	it out, and does process syncronization with below.  The
**	calling routine must sync with the process above.
**
**	Parameters:
**		root1 -- a pointer to the tree to modify.
**
**	Returns:
**		A pointer to the modified tree.
*/
QTREE	*qrymod(root1)
QTREE	*root1;
{
	register QTREE		*root;
	extern QTREE		*view();
	extern QTREE		*integrity();
	extern QTREE		*protect();

	root = root1;

#	ifdef xQTR1
	if (AAtTf(1, 0))
		treepr(root, "->QRYMOD");
#	endif

	/* view processing */
	root = view(root);

	/* integrity processing */
	root = integrity(root);

	/* protection processing */
	root = protect(root);

	return (root);
}
