# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"../h/pipes.h"
# include	"decomp.h"

/*
** DECOMP -- Process a query given a query tree and range table.
**
**	Decomp processes any arbitrary query by converting it into
**	a sequence of "one variable queries"; eg. queries involving
**	at most one source relation. This file and decision.c contain
**	the principle decision making routines.
**
**	Decomp() is called with a pointer to a query tree, the mode
**	of the query (retrieve, append, etc.), and the internal name
**	of the result relation (if any). The routines included are:
**
**	Decomp  -- Opens the source relations and decides whether the
**			query is multi-variable or single/zero variable.
**
**	Decompx -- Takes a multivariable query, removes and executes any
**			one-var restrictions and passes the remaining query
**			to decompz (if one/zero variable) or decision().
**
**	Decompy -- Performs "tuple substitution" on a multi-var query,
**			does any new one-var restrictions and passes the
**			remaining query to decompz (if one/zero variable)
**			or decision().
**
**	Decompz -- Executes a one/zero variable query by calling call_ovqp().
*/

decomp(q, qmode, result_num)
QTREE			*q;
int			qmode;
int			result_num;

/*
**	Process query by calling either decompx for multivar
**	or decompz for 0 or 1 var query. Decomp() must guarantee
**	that the range table is the same upon exiting as it was
**	when entered. Newquery() and endquery() perform that function.
*/

{
	register QTREE		 	*root;
	register int 			vc;
	register int			i;
	int				locrange[MAXRANGE];

	root = q;
	vc = ((ROOT_NODE *) root)->tvarc;
#	ifdef xDTR1
	if (AAtTf(9, 0))
		printf("DECOMP: %d-var query, result_num=%d\n", vc, result_num);
	if (AAtTf(9, 1))
		printree(root, "DECOMP");
#	endif

	openrs(root);

	if (vc > 1)
	{
		newquery(locrange);
		i = decompx(root, qmode, result_num);
		endquery(locrange, FALSE);	/* don't reopen previous range */
	}
	else
	{
		Newq = 1;
		Sourcevar = -1;
		i = decompz(root, qmode, result_num);
	}
	return (i);
}


decompx(root, qmode, result_num)
QTREE		 *root;
int		 qmode;
int		result_num;

/*
** Decompx -- Initialize for multi-variable query.
**	All one-variable subqueries are run.
**	If the remaining query is still multi-var
**	then decision() is called; else decompz()
**	is called. The range table is restored
**	after the query is complete.
**	The tempvars from the exec_sq() are left on the
**	tree since it is presumed that the tree will be discarded
**	anyway.
*/

{
	register int 		i;
	register int		vc;
	int			disj;
	char  			sqbuf[SQSIZ];
	QTREE		 	*sqlist[MAXRANGE];
	int			locrang[MAXRANGE];
	int			sqrange[MAXRANGE];
	extern int		derror();

	vc = ((ROOT_NODE *) root)->tvarc;
	initbuf(sqbuf, SQSIZ, SQBUFFULL, derror);
	pull_sq(root, sqlist, locrang, sqrange, sqbuf);
	if ((i = exec_sq(sqlist, sqrange, &disj)) != -1)
	{
		undo_sq(sqlist, locrang, sqrange, i, i, FALSE);
		return (FALSE);
	}
	vc -= disj;
	tempvar(root, sqlist, sqbuf);
	if (pull_const(root, sqbuf) == 0)
		return (FALSE);
	if (vc <= 1)
	{
		Sourcevar = -1;
		Newq = 1;
		return (decompz(root, qmode, result_num));
	}
	i = decision(root, qmode, result_num, sqbuf);
	undo_sq(sqlist, locrang, sqrange, MAXRANGE, MAXRANGE, FALSE);
	return (i);
}


decompy(q, qmode, result_num, sqbuf)
QTREE		 	*q;
int			qmode;
int			result_num;
char			*sqbuf;

/*
** Decompy -- decompose a multi-variable query by tuple substitution.
**	First a variable is selected
**	for substitution. Then for each tuple in the
**	selected variable, all one variable restrictions
**	are done (exec_sq) and the remaining query is
**	solved by calling either decompz() or recursively
**	decision().
**
**	The original tree and range table are guaranteed to
**	be the same on entry and exit (modulo the effects of
**	reformat()).
*/

{
	register QTREE			*root;
	register int			j;
	register int			vc;
	register DESC			*descript;
	register QTREE			*newroot;
	int				constl;
	int				sqcnt;
	int				var;
	int				srcvar;
	int				maxsqcnt;
	int				disj;
	register int			tc;
	register int			qtrue;
	long				tid;
	long				hitid;
	register char			*tuple;
	QTREE				*sqlist[MAXRANGE];
	int				sqmark;
	int				sqmark1;
	int				locrang[MAXRANGE];
	int				sqrange[MAXRANGE];
	extern QTREE			*copy_ands();
	extern char			*need();
	extern DESC			*readopen();

	root = q;
	vc = ((ROOT_NODE *) root)->tvarc;

#	ifdef xDTR1
	if (AAtTf(9, -1))
		printf("DECOMPY: 0%o, vc=%d\n", root, vc);
#	endif

	sqmark = markbuf(sqbuf);
	constl = !((ROOT_NODE *) root)->lvarc;
	qtrue = FALSE;

	if ((var = selectv(root)) < 0)
		return (qtrue);
	descript = readopen(var);	/* gets full descriptor for setvar & get */
	tuple = need(sqbuf, descript->reldum.relwid);
	setvar(root, var, &tid, tuple);
	pull_sq(root, sqlist, locrang, sqrange, sqbuf);
	tempvar(root, sqlist, sqbuf);
	reformat(var, sqlist, locrang, sqbuf, root);
	vc--;

	/* HERE FOR MULTI-VAR SUBSTITUTION */
	sqmark1 = markbuf(sqbuf);
	Newq = 1;
	tc = 0;
	sqcnt = maxsqcnt = 0;
	srcvar = -1;
	Sourcevar = -1;
	AAam_find(readopen(var), NOKEY, &tid, &hitid);
	while (!(j = AAam_get(readopen(var), &tid, &hitid, tuple, NXTTUP)))
	{
#		ifdef xDTR1
		if (AAtTf(9, 2))
		{
			printf("Subst:");
			AAprtup(readopen(var), tuple);
		}
#		endif
		tc++;
		if (vc > 1)
		{
			reset_sq(sqlist, locrang, sqcnt);
			if ((sqcnt = exec_sq(sqlist, sqrange, &disj)) != -1)
				continue;

			maxsqcnt = sqcnt;
			vc -= disj;
			if (vc <= 1)
			{
				Sourcevar = srcvar;
				qtrue |= decompz(root, qmode, result_num);
				srcvar = Sourcevar;
			}
			else
			{
				freebuf(sqbuf, sqmark1);
				newroot = copy_ands(root, sqbuf);
				qtrue |= decision(newroot, qmode, result_num, sqbuf);
			}
			vc += disj;
		}
		else
			qtrue |= decompz(root, qmode, result_num);

		/* check for early termination on constant Target list */
		if (constl && qtrue)
			break;
	}
	if (j < 0)
		AAsyserr(12030, j, readopen(var)->reldum.relid);

	/* undo the effect of pulling the sub queries */
	origvar(root, sqlist);
	undo_sq(sqlist, locrang, sqrange, sqcnt, maxsqcnt, TRUE);

	/* undo the setvar on the main tree and all subtrees */
	clearvar(root, var);
	for (j = 0; j < MAXRANGE; j++)
		clearvar(sqlist[j], var);

	/* return any used buffer space */
	freebuf(sqbuf, sqmark);

#	ifdef xDTR1
	if (AAtTf(9, 2))
		printf("tc[%.12s]=%d,qtrue=%d\n", rangename(var), tc, qtrue);
#	endif

	return (qtrue);
}



decompz(q, qmode, result_num)
QTREE		 	*q;
int			qmode;
int			result_num;

/*
**	Decompz processes a one variable query
**	by calling call_ovqp().
*/

{
	register QTREE			*root;
	register int			qualfound;

	root = q;
	if (((ROOT_NODE *) root)->tvarc)
	{
		if (Sourcevar < 0)
		{
			if ((Sourcevar = selectv(root)) < 0)
				return (FALSE);
		}
	}
	else
	{
		Sourcevar = -1;
	}

	qualfound = call_ovqp(root, qmode, result_num);
	Newq = 0;
	return (qualfound);
}
