# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"../h/pipes.h"
# include	"decomp.h"

/*
**	mklist
**
**	writes a list of query tree nodes in "OVQP order" --
**	that is, everything in postfix (endorder) except AND's and OR's
**	infixed (postorder) to OVQP.
**	called by call_ovqp().
*/



mklist(t)
register QTREE		*t;
{
	register int		typ;
	register int 		andor;

	if (!t || (typ = t->sym.type) == TREE || typ == QLEND)
		return;

	mklist(t->left);
	if (andor = (typ == AND || typ == OR))
		ovqpnod(t);
	mklist(t->right);
	if (!andor)
		ovqpnod(t);
}
