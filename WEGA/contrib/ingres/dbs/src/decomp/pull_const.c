# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"
# include	"../h/pipes.h"
# include	"decomp.h"

/*
** PULL_CONST - Detach and execute all constant clauses in the qualification.
**
**	Pull_const examines the root tree for any constant clauses.
**	If none are present then it returns TRUE. If there are any
**	constant clauses, then they are removed, executed and if
**	TRUE then pull_const returns TRUE and other wise it returns
**	FALSE.
**
**	This routine is not necessary to decomposition but rather
**	can be called as an optimization when constant clauses are
**	expected. Note that without this routine, constant clauses
**	would only be examined at the bottom level of decomposition.
**	Thus a multivar query which was true except for a constant clause
**	would look at the required cross-product space before returning.
*/

pull_const(root, buf)
QTREE			*root;
char			*buf;
{
	register QTREE			*r;
	register QTREE			*q;
	register QTREE			*s;
	extern QTREE			*makroot();

	s = 0;

	for (r = root; r->right->sym.type != QLEND; )
	{
		q = r;
		r = r->right;	/* r is now the AND node */

		if (((ROOT_NODE *) r)->lvarc == 0)
		{
			/* we have a constant clause */
			if (s == 0)
				s = makroot(buf);

			/* remove AND from root tree */
			q->right = r->right;

			/* put node into constant tree */
			r->right = s->right;
			s->right = r;

			/* fix up var counts (just for good form!) */
			((ROOT_NODE *) r)->rvarm = ((ROOT_NODE *) r)->tvarc = 0;

			r = q;
		}
	}

	if (s)
	{
		/* run the constant query */
		return (execsq1(s, -1, NORESULT));
	}

	return (TRUE);
}
