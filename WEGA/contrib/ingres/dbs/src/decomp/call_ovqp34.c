# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"
# include	"../h/aux.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"../h/pipes.h"
# include	"../h/bs.h"
# include	"decomp.h"

int		Newq;		/* flags new user query to OVQP		*/

/*
** CALL_OVQP -- Routines which interface to the One Variable Query Processor.
**
**	This file contains the routines associated with sending queries
**	and receiving results from OVQP. The interface to these routines is
**	still messy. Call_ovqp is given the query, mode, and result relation
**	as parameters and gets the source relation, and two flags
**	(Newq, Newr) as globals. The routines include:
**
**	Call_ovqp -- Sends a One-var query to ovqp, flushes the pipe,
**			and reads result from ovqp.
**
**	Endovqp    -- Informs ovqp that the query is over. Helps to synchronize
**			the batch file (if any).
**
**	Pipesym    -- Writes a token symbol to ovqp.
**
**
** Call_ovqp -- send query down pipe to ovqp and flush pipe.
**	Inputs are:
**		mode		retrieve, append, etc.
**		resultnum	result relation id
**		qtree		the query
**		Sourcevar	(global) if >= 0 then source var
**		Newq		send NEWQ symbol
**		Newr		send NEWR symbol
*/
call_ovqp(qtree, mode, resultnum)
QTREE		 	*qtree;
int			mode;
int			resultnum;
{
	register QTREE			*tree;
	register int			i;
	char				*rangename();
	struct retcode			ret;
	SYMBOL				s;
	int				j;

	tree = qtree;
#	ifdef xDTR1
	AAtTfp(8, 0, "CALL_OVQP\n");
	if (AAtTf(8, 1))
	{
		if (Newq)
			printree(tree, "new query to ovqp");
		else
			printf("query same as previous\n");
	}
	if (AAtTfp(8, 2, "Sourcevar=%d\t", Sourcevar))
	{
		if (Sourcevar >= 0)
			printf("relid=%s\t", rangename(Sourcevar));
		if (resultnum >= 0)
			printf("Resultname=%s", rnum_convert(resultnum));
		if (((ROOT_NODE *) tree)->rootuser)
			printf(", userquery");
		putchar('\n');
	}
#	endif



	/*  header information needed by OVQP for each query  */
	wrpipe(P_PRIME, &Outpipe, EXEC_OVQP, (char *) 0, 0);
	pipesym(QMODE, sizeof (short), mode);

	if (Newq)
		pipesym(CHANGESTRAT, 0);

	if (Newr)
	{
		pipesym(REOPENRES, 0);
		Newr = FALSE;
	}

	if (Sourcevar >= 0)
	{
		s.type = SOURCEID;
		s.len = MAXNAME;
		wrpipe(P_NORM, &Outpipe, W_ovqp, &s, TYP_LEN_SIZ);
		wrpipe(P_NORM, &Outpipe, W_ovqp, rangename(Sourcevar), MAXNAME);
	}

	if (resultnum >= 0)
	{
		s.type = RESULTID;
		s.len = MAXNAME;
		wrpipe(P_NORM, &Outpipe, W_ovqp, &s, TYP_LEN_SIZ);
		wrpipe(P_NORM, &Outpipe, W_ovqp, rnum_convert(resultnum), MAXNAME);
	}

	/*	this symbol, USERQRY, should be sent last because OVQP
	 *	requires the query mode and result relation when it
	 *	encounters USERQRY.
	 */

	if (((ROOT_NODE *) tree)->rootuser)
	{
		pipesym(USERQRY, 0);
	}

	/*  now write the query list itself  */

	pipesym(TREE, 0);

	if (tree->sym.type == AGHEAD)
	{
		pipesym(AGHEAD, 0);
		if (tree->left->sym.type == BYHEAD)
		{
			mklist(tree->left->right);
			pipesym(BYHEAD, 0);
			mklist(tree->left->left);
		}
		else
			mklist(tree->left);
	}
	else
		mklist(tree->left);

	pipesym(ROOT, 0);
	mklist(tree->right);
	pipesym(QLEND, 0);

	/* now flush the query to ovqp */
	wrpipe(P_FLUSH, &Outpipe, W_ovqp);

	/*
	** Read the result of the query from ovqp. A Retcode
	** struct is expected.
	** The possble values of Retcode.rt_status are:
	**	EMPTY     -- no tuples satisfied
	**	NONEMPTY  -- at least one tuple satisfied
	**	error num -- ovqp user error
	*/


	rdpipe(P_PRIME, &Inpipe);
	if (rdpipe(P_NORM, &Inpipe, R_ovqp, &ret, sizeof (ret)) != sizeof (ret))
		AAsyserr(12002);
	j = ret.rc_status;
	switch (j)
	{
	  case EMPTY:
		i = FALSE;
		break;

	  case NONEMPTY:
		i = TRUE;
		if (((ROOT_NODE *) tree)->rootuser)
			Retcode.rc_tupcount = ret.rc_tupcount;
		break;

	  default:
		derror(j);

	}
	return (i);
}
