# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"../h/pipes.h"
# include	"decomp.h"


/*
**	Make a copy of a tree.
*/
QTREE		 *copytree(root, buf)
QTREE			*root;
char			*buf;
{
	register QTREE			*r;
	register QTREE			*q;
	register int			length;
	extern QTREE			*need();

	if (!(r = root))
		return (0);

	length = ctou(r->sym.len);
	q = need(buf, length + QT_HDR_SIZ);
	AAbmove(&r->sym, &q->sym, length + SYM_HDR_SIZ);

	q->left = copytree(r->left, buf);
	q->right = copytree(r->right, buf);

	return (q);
}


/*
**	Make a new copy of the root by making
**	new AND nodes and connecting them to the
**	same branches.
*/
QTREE		 *copy_ands(root, buf)
QTREE			*root;
char		 	*buf;
{
	register QTREE			*q;
	register QTREE			*x;
	register QTREE			*y;
	register QTREE			*newroot;
	extern QTREE			*need();

#	ifdef xDTR1
	AAtTfp(10, -1, "COPY_ANDS");
#	endif
	newroot = need(buf, 0);
	y = 0;

	for (q = root; q->sym.type != QLEND; q = q->right)
	{
		x = need(buf, q->sym.len + QT_HDR_SIZ);
		x->left = q->left;
		AAbmove(&q->sym, &x->sym, q->sym.len + SYM_HDR_SIZ);
		if (y)
			y->right = x;
		y = x;
	}
	y->right = q;

#	ifdef xDTR1
	if (AAtTf(10, 0))
		printree(newroot, "Newtree");
#	endif
	return (newroot);
}


QTREE		 *makroot(buf)
char 	*buf;
{
	register QTREE		 *s;
	extern QTREE		*need();

	s = need(buf, QT_HDR_SIZ + ROOT_SIZ);
	s->right = Qle;
	s->left = Tr;
	((ROOT_NODE *) s)->rootuser = 0;
	((ROOT_NODE *) s)->lvarm = 0;
	((ROOT_NODE *) s)->rvarm = 0;
	((ROOT_NODE *) s)->tvarc = 0;
	((ROOT_NODE *) s)->lvarc = 0;
	s->sym.type = ROOT;
	s->sym.len = ROOT_SIZ;
	return (s);
}


QTREE		 *makresdom(buf, node)
char			*buf;
QTREE			*node;
{
	register QTREE			*res;
	register QTREE			*n;
	extern QTREE			*need();

	n = node;
	res = need(buf, QT_HDR_SIZ + RES_SIZ);
	res->sym.type = RESDOM;
	res->sym.len = RES_SIZ;
	if (n->sym.type == AOP)
	{
		((VAR_NODE *) res)->frmt = ((AOP_NODE *) n)->agfrmt;
		((VAR_NODE *) res)->frml = ((AOP_NODE *) n)->agfrml;
	}
	else
	{
		((VAR_NODE *) res)->frmt = ((VAR_NODE *) n)->frmt;
		((VAR_NODE *) res)->frml = ((VAR_NODE *) n)->frml;
	}
	return (res);
}


QTREE		 *makavar(node, varnum, attnum)
QTREE			*node;
int			varnum;
int			attnum;
{
	register QTREE		 *avar;
	register QTREE		 *n;
	extern QTREE		*need();

	n = node;

	avar = need(Qbuf, QT_HDR_SIZ + VAR_SIZ);
	avar->left = avar->right = 0;
	((VAR_NODE *) avar)->valptr = (char *) 0;
	avar->sym.type = VAR;
	avar->sym.len = VAR_SIZ;
	((VAR_NODE *) avar)->frmt = ((VAR_NODE *) n)->frmt;
	((VAR_NODE *) avar)->frml = ((VAR_NODE *) n)->frml;
	((VAR_NODE *) avar)->varno = varnum;
	((VAR_NODE *) avar)->attno = attnum;
#	ifdef xDTR1
	if (AAtTfp(14, 3, "MAKAVAR: node=0%o  ", n))
		writenod(avar);
#	endif
	return (avar);
}
