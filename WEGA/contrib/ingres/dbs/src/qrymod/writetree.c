# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/pipes.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"qrymod.h"

/*
**  WRITETREE.C -- query tree output routines
**
**	These routines write out a query tree in internal format.
**
**	Defines:
**		writetree
**		writeqry
**		writesym
*/

extern struct pipfrmt	Pipe;
extern struct pipfrmt	Outpipe;

/*
**  WRITETREE -- write a query tree
**
**	A query tree is written to the down pipe.  The parameter is
**	the root of the tree to be written.
**
**	Parameters:
**		q1 -- the root of the tree to write.
**		wrfn -- the function to call to do physical
**			writes.
*/
writetree(q1, wrfn)
QTREE	*q1;
int	(*wrfn)();
{
	register QTREE	*q;
	register int	l;
	register char	t;

	q = q1;

	/* write the subtrees */
	if (q->left != NULL)
		writetree(q->left, wrfn);
	if (q->right != NULL)
		writetree(q->right, wrfn);

	/* write this node */
	t = q->sym.type;
	if (t == AND || t == ROOT || t == AGHEAD)
		q->sym.len = 0;
	(*wrfn)(&(q->sym), TYP_LEN_SIZ);
	if (l = ctou(q->sym.len))
		(*wrfn)(q->sym.value, l);
#	ifdef	xQTR1
	if (AAtTf(70, 5))
		nodepr(q, TRUE);
#	endif
}


/*
**  WRITEQRY -- write a whole query
**
**	An entire query is written, including range table, and so
**	forth.
**
**	Parameters:
**		root -- the root of the tree to write.
**		wrfn -- the function to do the physical write.
*/
writeqry(root, wrfn)
QTREE	*root;
int	(*wrfn)();
{
	register int		i;
	short			resvar;
	SRCID			sid;

	/* write the query mode */
	if (Qmode >= 0)
	{
		resvar = Qmode;
		writesym(QMODE, sizeof (short), &resvar, wrfn);
	}

	/* write the range table */
	for (i = 0; i < MAXVAR + 1; i++)
	{
		if (Rangev[i].rused)
		{
			((SYMBOL *) &sid)->type = SOURCEID;
			((SYMBOL *) &sid)->len = SRC_SIZ;
			AApmove(Rangev[i].relid, sid.srcname, MAXNAME, ' ');
			sid.srcvar = i;
			sid.srcstat = Rangev[i].rstat;
			AAbmove(Rangev[i].rowner, sid.srcown, 2);
			(*wrfn)(&sid, TYP_LEN_SIZ);
			(*wrfn)(&sid.srcvar, SRC_SIZ);
		}
	}

	/* write a possible result variable */
	if (Resultvar >= 0)
	{
		resvar = Resultvar;
		writesym(RESULTVAR, sizeof (short), &resvar, wrfn);
	}

	/* write the tree */
	writetree(root, wrfn);
}


/*
**  WRITESYM -- write a symbol block
**
**	A single symbol entry of the is written.
**	a 'value' of zero will not be written.
*/
writesym(typ, len, value, wrfn)
int	typ;
int	len;
char	*value;
int	(*wrfn)();
{
	SYMBOL		sym;
	register char	*v;
	register int	l;

	sym.type = ctou(typ);
	sym.len = l = ctou(len);
	(*wrfn)(&sym, TYP_LEN_SIZ);
	v = value;
	if (v != 0)
		(*wrfn)(v, l);
}
