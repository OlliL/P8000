# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"
# include	"../h/pipes.h"
# include	"qrymod.h"

extern struct pipfrmt	Outpipe;

/*
**  ISSUE -- issue query to rest of system
**
**	This function issues a query to the rest of the DBS.
**	It can deal with true queries (that is, a tree), or with a
**	DBU request (which must already be in the pipe).
**
**	The sync from below is read, but not passed up; a pointer
**	to it (or NULL if none) is returned.
**
**	Parameters:
**		execid -- the execid to call.
**		funcid -- the function to call.
**		tree -- NULL -- just sync from below.
**			else -- pointer to tree to issue.
**
**	Returns:
**		The resultcode of the query.
**
**	Side Effects:
**		A query is executed.
*/
struct retcode	*issue(execid, funcid, tree)
char	execid;
char	funcid;
QTREE	*tree;
{
	register QTREE		*t;
	extern			pipetrwr();
	static struct retcode	rc;
	register int		i;
	register struct retcode	*r;

	t = tree;
	r = &rc;

#	ifdef xQTR2
	AAtTfp(13, 1, "ISSUE:\n");
#	endif

	/* write query tree if given */
	if (t)
	{
		wrpipe(P_PRIME, &Outpipe, execid, (char *) 0, funcid);
		writeqry(t, pipetrwr);
		wrpipe(P_END, &Outpipe, W_down);
		fflush(stdout);
	}

	/* sync with below */
#	ifdef xQTR2
	AAtTfp(13, 2, "ISSUE: response:\n");
#	endif
	rdpipe(P_PRIME, &Outpipe);
	i = rdpipe(P_NORM, &Outpipe, R_down, r, sizeof *r);
	rdpipe(P_SYNC, &Outpipe, R_down);
	if (i == 0)
		return ((struct retcode *) 0);
	else
		return (r);
}


/*
**  ISSUEINVERT -- issue a query, but invert the qualification
**
**	This routine is similar to 'issue', except that it issues
**	a query with the qualification inverted.  The inversion
**	(and subsequent tree normalization) is done on a duplicate
**	of the tree.
**
**	Parameters:
**		root -- the root of the tree to issue.
**
**	Returns:
**		pointer to retcode struct.
*/
struct retcode	*issueinvert(root)
QTREE	*root;
{
	register QTREE		*t;
	extern QTREE		*tree();
	extern QTREE		*treedup();
	extern QTREE		*trimqlend();
	extern QTREE		*norml();
	register struct retcode	*r;
	extern struct retcode	*issue();

	/* make duplicate of tree */
	t = treedup(root);

	/* prepend NOT node to qualification */
	t->right = tree((QTREE *) 0, t->right, UOP, OP_SIZ, opNOT);

	/* normalize and issue */
	t->right = norml(trimqlend(t->right));
	r = issue(EXEC_DECOMP, 0, t);

#	ifdef xQTR3
	/* check for valid return */
	if (!r)
		AAsyserr(19040);
#	endif

	return (r);
}


/*
**  PIPETRWR -- write tree to W_down pipe
**
**	Acts like a 'wrpipe' call, with some arguments included
**	implicitly.
**
**	Parameters:
**		val -- the value to write.
**		len -- the length.
**
**	Returns:
**		same as wrpipe.
*/
pipetrwr(val, len)
register char	*val;
register int	len;
{
	return (wrpipe(P_NORM, &Outpipe, W_down, val, len));
}


/*
**  NULLSYNC -- send null query to sync EQL program
**
**	On a RETRIEVE to terminal, EQL is left reading the data
**	pipe by the time we get to this point; on an error (which
**	will prevent the running of the original query) we must
**	somehow unhang it.  The solution is to send a completely
**	null query, as so:
**
**			ROOT
**		       /    \
**		    TREE   QLEND
**
**	This will cause OVQP to send a sync back to the EQL program,
**	incuring an incredible amount of overhead in the process
**	(but it was his own fault!).
**
**	Side Effects:
**		A query is sent.
*/
struct retcode	*nullsync()
{
	register QTREE		*t;
	extern struct retcode	*issue();
	register struct retcode	*rc;
	extern QTREE		*tree();

	t = tree(tree((QTREE *) 0, (QTREE *) 0, TREE, 0), tree((QTREE *) 0, (QTREE *) 0, QLEND, 0), ROOT, 0);
	rc = issue(EXEC_DECOMP, '0', t);
	return (rc);
}
