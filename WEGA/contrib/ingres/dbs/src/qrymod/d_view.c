# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/pipes.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"../h/catalog.h"
# include	"qrymod.h"

extern QTREE			*Treeroot;
extern DESC			Reldes;
extern struct pipfrmt		Pipe;

/*
**  D_VIEW -- define view
**
**	This procedure connects the tree in with the AA_REL catalog
**	and inserts the view tree into the AA_TREE catalog.
**
**	The information in the pipe is expected to come as follows:
**		create for view, with S_VIEW bit set so that a
**			physical relation is not created.
**		define tree, which will put the translation tree
**			into the AA_TREE catalog.
**		define view, which will connect the two together.
**			The first two absolutely must be done before
**			this step can be called.
*/
d_view()
{
	char		viewid[MAXNAME + 1];

	/*
	**  Read parameters.
	**	Only parameter is view name (which must match the
	**	name of the Resultvar).
	*/

	if (rdpipe(P_NORM, &Pipe, R_up, viewid, 0) > MAXNAME + 1)
		AAsyserr(19019);
	AApad(viewid, MAXNAME);

	/* done with pipe... */
	rdpipe(P_SYNC, &Pipe, R_up);

#	ifdef xQTR3
	/* do some extra validation */
	if (!Treeroot)
		AAsyserr(19020);
	if (Qmode != mdDEFINE)
		AAsyserr(19021, Qmode);
	if (Resultvar < 0)
		AAsyserr(19022, Resultvar);
	if (!Rangev[Resultvar].rused || !AAbequal(Rangev[Resultvar].relid, viewid, MAXNAME))
		AAsyserr(19023, Rangev[Resultvar].rused, Rangev[Resultvar].relid);
#	endif

	Rangev[Resultvar].rused = FALSE;
	Resultvar = -1;
	Qmode = -1;

	/* output tree to AA_TREE catalog */
	puttree(Treeroot, viewid, AAusercode, mdVIEW);

	/* clear Treeroot to detect some errors */
	Treeroot = (QTREE *) 0;
}
