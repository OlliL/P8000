# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/tree.h"
# include	"../h/lock.h"
# include	"../h/pipes.h"
# include	"decomp.h"

/*
**	lockit - sets relation locks for integrity locking
**
**	arguements:
**		r - the root of a query tree;
**		resvar - index of variable to be updated.
*/
lockit(r, resvar)
register QTREE		*r;
int			resvar;
{
	register int			i;
	register int			j;
	register int			bmap;
	register DESC			*d;
	register int			k;
	long				restid;
	long				vlist[MAXRANGE];
	extern DESC			*openr1();

	bmap = ((ROOT_NODE *) r)->lvarm | ((ROOT_NODE *) r)->rvarm;
	if (resvar >= 0)
		bmap |= 01 << resvar;
	else
		restid = NO_REL;
	/* put relids of relations to be locked into vlist
	   check for and remove duplicates */
	for (i = 0, j = 0; j < MAXRANGE; j++)
		if (bmap & (01 << j))
		{
			d = openr1(j);
			if (j == resvar)
				restid = d->reltid;
			for (k = 0; k < i; k++)
				if (vlist[k] == d->reltid)
					break;
			if (k == i)
				vlist[i++] = d->reltid;
		}
	/* SET LOCKS AS TRANSACTION */
	for ( ; --i >= 0; )
	{
		j = M_TA | ((restid == vlist[i])? M_EXCL: M_SHARE);
		if (!i)
			j |= M_TE;
		if (AAsetrll(A_RTN, vlist[i], j) < 0)
		{
			AAunlall();	/* release all locks */
			/* a lock request failed */
			derror(LOCKERR);
		}
	}
}
