# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"

/* DECOMP3 -- This file contains routines associated with redefining
**	attribute numbers. This is needed when one variable sub queries
**	or reduction change the positions of attributes in a relation.
**	This file includes:
**
**	Tempvar -- Change the attribute numbers to new ones.
**
**	Origvar -- Restore attribute numbers back to their previous values.
**
**	Ckvar   -- Return the currently active VAR node
*/



tempvar(node, sqlist, buf)
QTREE			*node;
QTREE			*sqlist[];
char			*buf;

/*
** Tempvar -- Replace a VAR attribute number with its new number.
**
**	Tempvar is given a list of subqueries which will potentially
**	alter the attribute numbers of VARs they reference. An attno
**	is changed by making the current VAR point to a new VAR node
**	which has the updated attno.
**
**	The new attno is determined from the target list of the subquery
**	for that VAR. The RESDOM number is the new attno and the VAR it
**	points to is the old attno. For example:
**		RESDOM/2 -> right = VAR 1/3
**	The right subtree of result domain 2 is domain 3 of variable 1.
**	Thus domain 3 should be renumbered to be domain 2.
*/

{
	register QTREE			*v;
	register QTREE			*sq;
	register QTREE			*nod;
	register int			i;
	extern QTREE			*ckvar();
	extern char			*need();

	if (!(nod = node))
		return;

	if (nod->sym.type == VAR)
	{
		nod = ckvar(nod);
		i = ((VAR_NODE *) nod)->varno;
		if (sq = sqlist[i])
		{
			/* This var has a subquery on it */

			/* allocate a new VAR node */
			if (buf)
			{
				((VAR_NODE *) nod)->valptr = need(buf, QT_HDR_SIZ + VAR_SIZ);
				v = (QTREE *) ((VAR_NODE *) nod)->valptr;
				v->left = (QTREE *) 0;
				v->right = (QTREE *) 0;
				AAbmove(&nod->sym, &v->sym, SYM_HDR_SIZ + VAR_SIZ);
				((VAR_NODE *) nod)->varno = -1;
				((VAR_NODE *) v)->valptr = (char *) 0;
			}
			else
				v = nod;

			/* search for the new attno */
			for (sq = sq->left; sq->sym.type != TREE; sq = sq->left)
			{
				if (ctou(((VAR_NODE *) ckvar(sq->right))->attno) == ctou(((VAR_NODE *) nod)->attno))
				{

					((VAR_NODE *) v)->attno = ((RES_NODE *) sq)->resno;
#					ifdef xDTR1
					if (AAtTfp(12, 3, "TEMPVAR:"))
						writenod(nod);
#					endif

					return;
				}
			}
			AAsyserr(12027, ctou(((VAR_NODE *) nod)->attno),
				rangename(((VAR_NODE *) nod)->varno));
		}
		return;
	}

	tempvar(nod->left, sqlist, buf);
	tempvar(nod->right, sqlist, buf);
}




origvar(node, sqlist)
QTREE			*node;
QTREE			*sqlist[];

/*
** Origvar -- Restore VAR node to previous state.
**
**	Origvar undoes the effect of tempvar. All vars listed
**	in the sqlist will have their most recent tempvar removed.
*/

{
	register QTREE			*t;
	register int			v;

	t = node;
	if (!t)
		return;
	if (t->sym.type == VAR && ctoi(((VAR_NODE *) t)->varno) < 0)
	{
		for ( ; ctoi(((VAR_NODE *) ((VAR_NODE *) t)->valptr)->varno) < 0; t = (QTREE *) ((VAR_NODE *) t)->valptr)
			continue;
		v = ((VAR_NODE *) ((VAR_NODE *) t)->valptr)->varno;
		if (sqlist[v])
		{
			((VAR_NODE *) t)->varno = v;
			((VAR_NODE *) t)->valptr = (char *) 0;
		}
		return;
	}
	origvar(t->left, sqlist);
	origvar(t->right, sqlist);
}



QTREE		 *ckvar(node)
QTREE		 *node;

/*
** Ckvar -- Return pointer to currently "active" VAR.
**
**	This routine guarantees that "t" will point to
**	the most current definition of the VAR.
*/

{
	register QTREE			*t;

	t = node;
	if (t->sym.type != VAR)
		AAsyserr(12028, t->sym.type);
	while (ctoi(((VAR_NODE *) t)->varno) < 0)
		t = (QTREE *) ((VAR_NODE *) t)->valptr;
	return (t);
}
