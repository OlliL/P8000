# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"
# include	"qrymod.h"

/*
**  RANGE.C -- range table manipulation
**
**	Defines:
**		declare -- declare variable
**		clrrange -- clear range table
**		mapvars -- map varno's in a tree to unique numbers
**		Remap[] -- a mapping from specified (preferred) varno's
**			to actual varno's.  -1 indicates no mapping.
**			Presumably this cannot be set when reading an
**			original query tree, but can happen easily when
**			reading a tree from AA_TREE catalog.
**		Rangev[] -- the range table.
*/

int		Remap[MAXVAR + 1];
struct rngtab	Rangev[MAXVAR + 1];

/*
**  CLRRANGE -- clear range table(s)
**
**	The range table (Rangev) and range table map (Remap) are
**	initialized in one of two ways.
**
**	Parameters:
**		prim -- if TRUE, the primary range table (Rangev)
**			is cleared and Remap is set to be FULL (for
**			reading in an initial query).  If FALSE,
**			Rangev is untouched, but Remap is cleared.
**
**	Side Effects:
**		Rangev[i].rused set to FALSE for all entries.
**		Remap[i] set to -1 or MAXVAR + 1 for all entries.
*/
clrrange(prim)
register int	prim;
{
	register int	i;

	for (i = 0; i < MAXVAR + 1; i++)
	{
		if (prim)
		{
			Rangev[i].rused = FALSE;
			Remap[i] = MAXVAR + 1;
		}
		else
			Remap[i] = -1;
	}
}


/*
**  DECLARE -- declare a range variable
**
**	A range variable is declared.  If possible, the preferred varno
**	stated is used (this is the one already in the tree).  This
**	should always be possible when reading the original tree (and
**	should probably stay this way to make debugging easier).  When
**	not possible, a new varno is chosen, and Remap[oldvarno] is
**	set to newvarno, so that the tree can later be patched up by
**	'mapvars' (below).
**
**	Parameters:
**		varno -- the preferred varno.
**		name -- the relation name.
**		stat -- the 'relstat' of this relation.
**
**	Side Effects:
**		Rangev and possible Remap are updated.  No Rangev
**			entry is ever touched if the 'rused' field
**			is set.
*/
declare(varno, name, owner, stat)
int	varno;
char	*name;
char	owner[2];
int	stat;
{
	register int	i;
	register int	vn;
	register int	s;

	vn = varno;
	s = stat;

	/* check for preferred slot in range table available */
	if (Rangev[vn].rused)
	{
		/* used: check if really redeclared */
		if (Remap[vn] >= 0)
			AAsyserr(19047, vn);

		/* try to find another slot */
		for (i = 0; i < MAXVAR + 1; i++)
			if (!Rangev[i].rused)
				break;

		if (i >= MAXVAR + 1)
		{
			far_error(3100, Qmode, -1, name, (char *) 0);	/* too many variables */
		}

		Remap[vn] = i;
		vn = i;
	}

	/* declare variable in the guaranteed empty slot */
	AAbmove(name, Rangev[vn].relid, MAXNAME);
	AAbmove(owner, Rangev[vn].rowner, 2);
	Rangev[vn].rstat = s;
	Rangev[vn].rused = TRUE;

#	ifdef xQTR2
	AAtTfp(11, 0, "DECLARE: %d, %.12s, %.2s, 0%o into slot %d\n",
		    varno, name, owner, s, vn);
#	endif
#	ifdef xQTR3
	AAtTfp(11, 1, "DECLARE: %.12s%.2s 0%o %d\n", Rangev[vn].relid,
		    Rangev[vn].rowner, Rangev[vn].rstat, Rangev[vn].rused);
#	endif
}


/*
**  MAPVARS -- remap varno's to be unique in AA_TREE tree
**
**	A tree is scanned for VAR nodes; when found, the
**	mapping defined in Remap[] is applied.  This is done so that
**	varno's as defined in trees in the AA_TREE catalog will be
**	unique with respect to varno's in the user's query tree.  For
**	example, if the view definition uses variable 1 and the user's
**	query also uses variable 1, the routine 'declare' will (when
**	called to define the view definition varno 1) allocate a new
**	varno (e.g. 3) in a free slot in the range table, and put
**	the index of the new slot into the corresponding word of Remap;
**	in this example, Remap[1] == 3.  This routine does the actual
**	mapping in the tree.
**
**	Parameters:
**		tree -- pointer to tree to be remapped.
**
**	Side Effects:
**		the tree pointed to by 'tree' is modified according
**		to Remap[].
*/
mapvars(tree)
QTREE	*tree;
{
	register QTREE	*t;
	register int	i;

	t = tree;
#	ifdef xQTR3
	if (AAtTf(12, 0) && t != NULL && t->sym.type == ROOT)
	{
		treepr(t, "mapvars:");
		for (i = 0; i < MAXVAR + 1; i++)
			if (Rangev[i].rused && Remap[i] >= 0)
				printf("\t%d => %d\n", i, Remap[i]);
	}
#	endif

	while (t != NULL)
	{
		/* map right subtree */
		mapvars(t->right);

		/* check this node */
		if (t->sym.type == VAR)
		{
			i = ((VAR_NODE *) t)->varno;
			if ((i = Remap[i]) >= 0)
				((VAR_NODE *) t)->varno = i;
		}

		/* map left subtree (iteratively) */
		t = t->left;
	}
}
