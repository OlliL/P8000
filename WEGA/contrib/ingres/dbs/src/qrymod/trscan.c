# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"
# include	"qrymod.h"

/*
**  TRSCAN -- scan trees for various conditions
**
**	These are basically utility routines which do tree scans for
**	one thing or another.  Really, there is nothing exciting at
**	all in this module.
**
**	Defines:
**		aggcheck -- scan tree for aggregates
**		vfind -- find definition for VAR in a view tree
**
**
**
**  AGGCHECK -- check for any aggregate in subtree.
**
**	This routine checks to insure that the view algorithm can
**	proceed safely by checking for aggregates in the view tree.
**
**	Parameters:
**		t -- the root of the tree to check.
**
**	Returns:
**		TRUE -- an aggregate found.
**		FALSE -- no aggregates in tree.
*/
aggcheck(t)
register QTREE	*t;
{
	/* check for no pointer */
	while (t != (QTREE *) 0)
	{
		/* check for this node an AGHEAD */
		if (t->sym.type == AGHEAD)
			return (TRUE);

		/* check left subtree recursively */
		if (aggcheck(t->left))
			return (TRUE);

		/* check right subtree iteratively */
		t = t->right;
	}

	return (FALSE);
}


/*
**  VFIND -- find definition for attribute in view tree
**
**	The view tree is scanned for a specified RESDOM; a pointer
**	to the value is returned.
**
**	Parameters:
**		vn -- the variable number to dig out.
**		vtree -- a pointer to the target list of the
**			view definition tree.
**
**	Returns:
**		a pointer to the substitution value for the specified
**			'vn'.
**		(QTREE *) 0 -- if not found.
*/
QTREE	*vfind(vn, vtree)
int	vn;
QTREE	*vtree;
{
	register int	n;
	register QTREE	*v;

	n = vn;

	for (v = vtree; v->sym.type == RESDOM; v = v->left)
	{
		if (((RES_NODE *) v)->resno != n)
			continue;

		/* found the correct replacement */
		return (v->right);
	}

	if (v->sym.type != TREE)
		AAsyserr(19054, v->sym.type);
	return ((QTREE *) 0);
}


/*
**  QSCAN -- find specified VAR node in subtree
**
**	Intended for finding a variable in a qualification, this
**	routine just scans a tree recursively looking for a node
**	with the specified VAR (varno.attno) node.
**
**	Parameters:
**		root -- the root of the tree to scan.
**		vn -- the varno to scan for.
**		an -- the attno to scan for.
**
**	Returns:
**		A pointer to the first found VAR node which matches.
**			Scan is prefix.
**		(QTREE *) 0 if not found at all.
*/
QTREE	*qscan(root, vn, an)
QTREE	*root;
int	vn;
int	an;
{
	register QTREE	*t;
	register QTREE	*u;

	t = root;

	/* check for null node */
	if (t == (QTREE *) 0)
		return (t);

	/* check to see if this node qualifies */
	if (t->sym.type == VAR && ((VAR_NODE *) t)->varno == vn && ((VAR_NODE *) t)->attno == an)
		return (t);

	/* check other nodes */
	if ((u = qscan(t->left, vn, an)) != (QTREE *) 0)
		return (u);
	return (qscan(t->right, vn, an));
}


/*
**  VARSET -- scan tree and set a bit vector of variables
**
**	The return value is a bit vector representing the set of
**	variables used in that subtree.
**
**	Parameters:
**		root -- the root of the tree to check.
**
**	Returns:
**		A bit vector, such that bit zero (on the low order,
**			right-hand end) means var zero.
*/
varset(root)
QTREE	*root;
{
	register QTREE		*t;
	register int		s;

	t = root;

	if (t == (QTREE *) 0)
		return (0);

	/* scan left and right branches */
	s = varset(t->left);
	s |= varset(t->right);

	/* check out this node */
	if (t->sym.type == VAR)
	{
		/* or in bit corresponding to this varno */
		s |= 1 << ((VAR_NODE *) t)->varno;
	}

	return (s);
}


/*
**  SUBSVARS -- scan query tree and replace VAR nodes
**
**	Scans a tree and finds all VAR nodes for this variable.
**	These nodes are looked up in the translation tree and
**	replaced by the value found there.  If this is for a
**	view, the corresponding node must exist in the translation
**	tree, otherwise, a 'zero' node (of a type appropriate based
**	on the context) is created and inserted.
**
**	This routine is one half of the guts of the whole view
**	algorithm.
**
**	VAR nodes are detached and replaced with the replacement
**	as defined by the view.  Note that there can never be any
**	problems here, since VAR nodes are only used in retrieve
**	contexts.
**
**	It does some extra processing with RESDOM nodes with
**	resno = 0.  These nodes specify a 'tid' domain, and are
**	included by the parser on REPLACE and DELETE commands
**	(for some reason decomp wants them).  Subsvars will allow
**	this construct iff the right hand pointer is a VAR node
**	with attno = 0.  In this case it just changes the varno
**	of the VAR node to be the Resultvar number.  This is be-
**	cause the Resultvar is the variable number of the one and
**	only underlying base relation of the view on an update
**	(which is presumably the only case where this can come
**	up).  Vrscan has already insured that there can only be
**	a single base relation in this case.
**
**	This whole messy thing is only done with view substitutions.
**
**	Parameters:
**		proot -- a pointer to the pointer to the root of the
**			tree to be updated.
**		vn -- the varno of the view variable.  This is the
**			varno which will be scanned for.
**		transtree -- a pointer to the left branch (target list)
**			of the translation tree.
**		vmode -- mdVIEW if called from view processor, mdAPP
**			if called from the integrity processor with
**			an APPEND command, else something else.
**			Mostly, changes the handling of TID type
**			nodes, and forces an error on a view if the
**			VAR node in the scanned tree does not exist
**			in the vtree.
**
**	Side Effects:
**		The tree pointed to by *proot is updated in possibly
**			very exciting ways.
*/
subsvars(proot, vn, transtree, vmode)
QTREE	**proot;
int	vn;
QTREE	*transtree;
int	vmode;
{
	register QTREE		*t;
	register QTREE		*v;
	extern QTREE		*vfind();
	extern int		Newresvar;	/* defined in view.c */
	extern QTREE		*makezero();
	extern QTREE		*treedup();

	t = *proot;
	v = transtree;

#	ifdef xQTR3
	AAtTfp(32, 0, "SUBSVARS: vn %d root 0%o transtree 0%o\n", vn, t, v);
#	endif

	if (t == (QTREE *) 0)
		return;

	/* check left branch of the tree */
	subsvars(&t->left, vn, v, vmode);

	/* check for special 'tid' RESDOM (used by DEL and REPL) */
	if (t->sym.type == RESDOM && ((RES_NODE *) t)->resno == 0)
	{
		/* test for not Resultvar, in which case we ignore leaf */
		if (vn != Resultvar)
			return;

		/* t->right better be VAR node, attno 0 */
		t = t->right;
		if (t->sym.type != VAR || ((VAR_NODE *) t)->attno != 0 || ((VAR_NODE *) t)->varno != vn)
			AAsyserr(19055, vn, ((VAR_NODE *) t)->attno, t->sym.type);

		/* change varno to new Newresvar (set by vrscan) */
#		ifdef xQTR3
		AAtTfp(32, 1, "RESDOM 0: Newresvar %d\n", Newresvar);
#		endif
		((VAR_NODE *) t)->varno = Newresvar;
		return;
	}

	/* scan right branch */
	subsvars(&t->right, vn, v, vmode);

	/* check for interesting node */
	if (t->sym.type != VAR || ((VAR_NODE *) t)->varno != vn)
		return;

	/* test for special 'tid' attribute case */
	if (((VAR_NODE *) t)->attno == 0 && vmode == (int) mdVIEW)
	{
		far_error(3340, Qmode, vn, (char *) 0);	/* views do not have tids */
	}

	/* find var in vtree */
	v = vfind(((VAR_NODE *) t)->attno, v);
	if (v == (QTREE *) 0)
	{
		if (vmode == (int) mdVIEW)
			AAsyserr(19056, ((VAR_NODE *) t)->attno);
		else if (vmode == (int) mdAPP)
			v = makezero();
	}
	else
		v = treedup(v);

	/* replace VAR node */
	if (v != (QTREE *) 0)
		*proot = v;
}
