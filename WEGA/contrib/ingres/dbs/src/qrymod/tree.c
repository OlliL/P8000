# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"qrymod.h"

/*
**  TREE -- create new tree node.
**
**	This is a stripped down version of the same thing in the
**	parser.
**
**	It only knows about lengths of zero and two.
**
**	Parameters:
**		lptr -- the left pointer.
**		rptr -- the right pointer.
**		typ -- the node type.
**		len -- the node length.
**		value -- the node value.
**
**	Returns:
**		A pointer to the created node.
*/
QTREE	*tree(lptr, rptr, typ, len, value)
QTREE	*lptr;
QTREE	*rptr;
char	typ;
int	len;
int	value;
{
	register QTREE	*tptr;
	extern char	*need();
	register int	l;

	l = len;

	tptr = (QTREE *) need(Qbuf, l + QT_HDR_SIZ);
	tptr->left = lptr;
	tptr->right = rptr;
	tptr->sym.type = typ;
	tptr->sym.len = l;

	if (l > 0)
		tptr->sym.value[0] = value;
	return (tptr);
}
