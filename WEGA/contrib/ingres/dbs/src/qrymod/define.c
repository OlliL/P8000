# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/catalog.h"
# include	"../h/symbol.h"
# include	"../h/lock.h"
# include	"../h/pipes.h"
# include	"../h/tree.h"
# include	"qrymod.h"

/*
**  DEFINE -- define various types of qrymod constraints
**
**	This module takes care of defining all the various types of
**	constraints that can be presented to qrymod.  It makes sense
**	out of them by looking at the funcid in the pipe.  They
**	correspond as follows:
**
**	mdDEFINE -- a tree.  This tree doesn't have any meaning yet; it is
**		just the raw tree.  It must be immediately followed
**		by a type 1, 2, or three definition, which will link
**		that tree into some other catalog, thereby giving
**		it some sort of semantic meaning.
**	mdVIEW -- a view.  This block has the view name in it.
**	mdPROT -- a protection constraint.
**	mdINTEG -- an integrity constraint.
**
**	Defines:
**		define -- the driver: just calls one of the others.
**		d_tree -- read definition tree.
**		puttree -- write tree into AA_TREE catalog.
**		relntrwr -- the physical write routine for puttree.
*/

QTREE			*Treeroot;
extern struct pipfrmt	Pipe;

/*
**  DEFINE -- define driver
**
**	This function does very little exciting; in fact, it just
**	calls one of the other four functions to do the specific
**	funcid functions.
**
**	Parameters:
**		func -- the function to perform:
**			mdDEFINE -- define tree (used by all).
**			mdVIEW -- define view.
**			mdPROT -- define protection constraint.
**			mdINTEG -- define integrity constraint.
*/
define(func)
register int	func;
{
	switch (func)
	{
	  case mdDEFINE:
		d_tree();
		break;

	  case mdVIEW:
		d_view();
		break;

	  case mdPROT:
		d_prot();
		break;

	  case mdINTEG:
		d_integ();
		break;

	  default:
		AAsyserr(19024, func);
	}
}


/*
**  D_TREE -- insert tree into system catalogs
**
**	This routine reads in and saves a tree for further use.
**	The root of this tree is saved in the global 'Treeroot'.
**	The tree will ultimately be written to the AA_TREE catalog
**	using the 'puttree' routine.
**
**	Side Effects:
**		The input pipe is read and the tree found there
**		is built.  A pointer to that tree is saved in
**		'Treeroot'.  Notice that 'Qbuf' and the range
**		table are clobbered.
*/
d_tree()
{
	extern			pipetrrd();
	extern QTREE		*readqry();

	Treeroot = readqry(pipetrrd, TRUE);
	rdpipe(P_SYNC, &Pipe, R_up);
#	ifdef xQTR1
	if (AAtTf(10, 0))
		treepr(Treeroot, "Treeroot");
#	endif
}


/*
**  PUTTREE -- put tree into AA_TREE catalog
**
**	The named tree is inserted into the AA_TREE catalog.
**
**	The algorithm is to lock up the entire catalog and try to
**	find the smallest unique id possible for the named relation.
**
**	Parameters:
**		root -- the root of the tree to insert.
**		treerelid -- the relid of the relation for which
**			this tree applies.
**		treeowner -- the owner of the above relation.
**		treetype -- the type of this tree; uses the mdXXX
**			type (as mdPROT, mdINTEG, mdDISTR, etc.).
**
**	Returns:
**		The treeid that was assigned to this tree.
**
**	Side Effects:
**		The AA_TREE catalog gets locked, and information is
**		inserted.
*/
puttree(root, treerelid, treeowner, treetype)
QTREE	*root;
char	*treerelid;
char	*treeowner;
char	treetype;
{
	struct tree	treekey;
	struct tree	treetup;
	struct tup_id	treetid;
	register int	i;
	short		treeid;

	opencatalog(AA_TREE, 2);
	AAam_clearkeys(&Treedes);

	/*
	**  Find a unique tree identifier.
	**	Lock the AA_TREE catalog, and scan until we find a
	**	tuple which does not match.
	*/

	if (AAsetrll(A_SLP, Treedes.reltid, M_EXCL) < 0)
		AAsyserr(5000);

	AAam_setkey(&Treedes, &treekey, treerelid, TREERELID);
	AAam_setkey(&Treedes, &treekey, treeowner, TREEOWNER);
	AAam_setkey(&Treedes, &treekey, &treetype, TREETYPE);
	for (treeid = 0; ; treeid++)
	{
		AAam_setkey(&Treedes, &treekey, &treeid, TREEID);
		i = AAgetequal(&Treedes, &treekey, &treetup, &treetid);
#	ifdef xQTR1
		AAtTfp(10, 1, "PUTTREE: treeid %d AAgetequal %d\n", treeid, i);
#	endif
		if (i < 0)
			AAsyserr(19025);
		else if (i > 0)
			break;
	}

	/*
	**  We have a unique tree id.
	**	Insert the new tuple and the tree into the
	**	AA_TREE catalog.
	*/

	relntrwr((char *) 0, 0, treerelid, treeowner, treetype, treeid);
	writeqry(root, relntrwr);
	relntrwr((char *) 0, 1);

	/* all inserted -- flush pages and unlock */
	if (AAnoclose(&Treedes))
		AAsyserr(19026);
	AAunlrl(Treedes.reltid);

	return (treeid);
}


/*
**  RELNTRWR -- physical tree write to relation
**
**	This is the routine called from writeqry to write trees
**	to the 'tree' relation (rather than the W_down pipe).
**
**	It is assumed that the (treerelid, treeowner, treetype,
**	treeid) combination is unique in the AA_TREE catalog, and that
**	the AA_TREE catalog is locked.
**
**	Parameters:
**		ptr -- a pointer to the data.  If NULL, this is
**			a control call.
**		len -- the length of the data.  If ptr == NULL, this
**			field is a control code:  zero means
**			initialize (thus taking the next two param-
**			eters); one means flush.
**		treerelid -- the name of the relation for which this
**			tree applies (init only).
**		treeowner -- the owner of this relation (init only).
**		treetype -- on initialization, this tells what the
**			tree is used for.
**		treeid -- on initialization, this is the tree id we
**			want to use.
**
**	Returns:
**		The number of bytes written ('len').
**
**	Side Effects:
**		Well, yes.  Activity occurs in the AA_TREE catalog.
*/
relntrwr(ptr, len, treerelid, treeowner, treetype, treeid)
char	*ptr;
int	len;
char	*treerelid;
char	*treeowner;
int	treetype;
int	treeid;
{
	static struct tree	treetup;
	struct tup_id		treetid;
	register char		*p;
	register int		l;
	static char		*tptr;

	p = ptr;
	l = len;

	/* check for special function */
	if (!p)
	{
		switch (l)
		{
		  case 0:
			clr_tuple(&Treedes, &treetup);
			AAbmove(treerelid, treetup.treerelid, MAXNAME);
			AAbmove(treeowner, treetup.treeowner, 2);
			treetup.treetype = treetype;
			treetup.treeid = treeid;
			tptr = treetup.treetree;
			break;

		  case 1:
			if (tptr != treetup.treetree)
			{
				if (AAam_insert(&Treedes, &treetid, &treetup, FALSE) < 0)
					AAsyserr(19027);
			}
			break;

		  default:
			AAsyserr(19028, l);
		}
		return;
	}

	/* output bytes */
	while (l-- > 0)
	{
		*tptr++ = *p++;

		/* check for buffer overflow */
		if (tptr < &treetup.treetree[sizeof treetup.treetree])
			continue;

		/* yep, flush buffer to relation */
		if (AAam_insert(&Treedes, &treetid, &treetup, FALSE) < 0)
			AAsyserr(19029);
		treetup.treeseq++;
		tptr = treetup.treetree;

		/* clear out the rest of the tuple for aesthetic reasons */
		*tptr = ' ';
		AAbmove(tptr, tptr + 1, sizeof treetup.treetree - 1);
	}

	return (len);
}
