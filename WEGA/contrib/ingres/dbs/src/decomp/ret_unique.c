# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/access.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"
# include	"../h/pipes.h"
# include	"decomp.h"


mk_unique(root)
QTREE			*root;

/*
**	create a result relation for a ret_unique
*/

{
	register int			i;
	register int			domcnt;
	register QTREE			*r;

	r = root;

	/* verify that target list is within range */
	domcnt = r->left->sym.type != TREE ? ((RES_NODE *) r->left)->resno : 0;
	if (findwid(r) > AAmaxtup || domcnt > MAXDOM)
		derror(RETUTOBIG);
	i = MAXRANGE - 1;
	Rangev[i].relnum = mak_t_rel(r, "u", -1);
	Resultvar = i;

	/* don't count retrieve into portion as a user query */
	((ROOT_NODE *) r)->rootuser = 0;
}


pr_unique(root1, var1)
QTREE			*root1;
int			var1;

/*
**	Retrieve all domains of the variable "var".
**	This routine is used for ret_unique to retrieve
**	the result relation. First duplicates are removed
**	then the original tree is converted to be a
**	retrieve of all domains of "var", and then
**	ovqp is called to retrieve the relation.
*/

{
	register QTREE			*root;
	register QTREE			*r;
	register int			var;
	extern QTREE			*makavar();

	root = root1;
	var = var1;

	/* remove duplicates from the unopened relation */
	removedups(var);

	/* remove the qual from the tree */
	root->right = Qle;

	/* make all resdoms refer to the result relation */
	for (r = root->left; r->sym.type != TREE; r = r->left)
		r->right = makavar(r, var, ((RES_NODE *) r)->resno);

	/* count as a user query */
	((ROOT_NODE *) root)->rootuser = TRUE;

	/* run the retrieve */
	Sourcevar = var;
	Newq = Newr = TRUE;
	call_ovqp(root, mdRETR, NORESULT);
}
