# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"
# include	"../h/aux.h"
# include	"../h/catalog.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"../h/pipes.h"
# include	"../h/bs.h"
# include	"decomp.h"
# include	"../ovqp/ovqp.h"

int		Bopen;		/* TRUE if batch file is open		    */
static int	Qvpointer;	/* index into available Qvect space in ovqpnod() */
static SYMBOL	*Qvect[MAXNODES];
char		*Ovqpbuf;

/*
** CALL_OVQP -- Routines which interface to the One Variable Query Processor.
**
**	This file contains the routines associated with sending queries
**	and receiving results from OVQP. The interface to these routines is
**	still messy. Call_ovqp is given the query, mode, and result relation
**	as parameters and gets the source relation, and two flags
**	(Newq, Newr) as globals. The routines include:
**
**	Call_ovqp -- Sends a One-var query to ovqp and flushes the pipe.
**
**	Readresult -- Reads the result from a one-var query.
**
**	Endovqp    -- Informs ovqp that the query is over. Helps to synchronize
**			the batch file (if any).
**
*/


/*
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
	char				ovqpbuf[LBUFSIZE];
	extern int			Batchupd;
	extern DESC			Inddes;
	extern int			derror();
	extern DESC			*readopen();
	extern char			*rangename();
	extern DESC			*specopen();

#	ifdef xOTM
	if (AAtTf(76, 1))
		timtrace(7, 0);
#	endif

	tree = qtree;
#	ifdef xDTR1
	AAtTfp(8, 0, "CALL_OVQP:\n");
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
			printf(", userqry");
		putchar('\n');
	}
#	endif

	/* assign mode of this query */
	Ov_qmode = mode;

	if (Newr)
		Newr = FALSE;

	if (resultnum >= 0)
		Result = specopen(resultnum);
	else
		Result = (DESC *) 0;

	if (Sourcevar >= 0)
		Source = readopen(Sourcevar);
	else
		Source = (DESC *) 0;

	/* assume this will be direct update */
	Userqry = Buflag = FALSE;

	if (((ROOT_NODE *) tree)->rootuser)
	{
		Userqry = TRUE;
		/* handle batch file */
		if (Result && Ov_qmode != (int) mdRETR)
		{
			if (Batchupd || ctoi(Result->reldum.relindxd) == SECBASE)
			{
				if (Bopen == 0)
				{
					if (ctoi(Result->reldum.relindxd) == SECBASE)
						opencatalog(AA_INDEX, 0);
					if (i = openbatch(Result, &Inddes, Ov_qmode))
						AAsyserr(12009, i);
					Bopen = TRUE;
				}
				Buflag = TRUE;
			}
		}
	}

	/*  now write the query list itself  */
	if (Newq)
	{
		Ovqpbuf = ovqpbuf;
		initbuf(Ovqpbuf, LBUFSIZE, LISTFULL, derror);
		Qvpointer = 0;
		Alist = Bylist = Qlist = Tlist = (SYMBOL **) 0;
		Targvc = ((ROOT_NODE *) tree)->lvarc;
		Qualvc = bitcnt(((ROOT_NODE *) tree)->rvarm);
		Agcount = 0;

		if (tree->sym.type == AGHEAD)
		{
			Alist = &Qvect[0];
			if (tree->left->sym.type == BYHEAD)
			{
				mklist(tree->left->right);
				ovqpnod(tree->left);	/* BYHEAD node */
				Bylist = &Qvect[Qvpointer];
				mklist(tree->left->left);
			}
			else
				mklist(tree->left);
		}
		else
		{
			if (tree->left->sym.type != TREE)
			{
				Tlist = &Qvect[0];
				mklist(tree->left);
			}
		}

		/* now for the qualification */
		ovqpnod(tree);	/* ROOT node */

		if (tree->right->sym.type != QLEND)
		{
			Qlist = &Qvect[Qvpointer];
			mklist(tree->right);
		}
		ovqpnod(Qle);	/* QLEND node */
	}

	/* Now call ovqp */
	if (strategy())
	{

#		ifdef xOTM
		if (AAtTf(76, 2))
			timtrace(9, 0);
#		endif

		i = scan();	/* scan the relation */

#		ifdef xOTM
		if (AAtTf(76, 2))
			timtrace(10, 0);
#		endif

	}
	else
		i = EMPTY;

#	ifdef xOTM
	if (AAtTf(76, 1))
		timtrace(8, 0);
#	endif


	/* return result of query */
	return (i == NONEMPTY);	/* TRUE if tuple satisfied */
}


/*
** Endovqp -- Inform ovqp that processing is complete. "Ack" indicates
**	whether to wait for an acknowledgement from ovqp. The overall
**	mode of the query is sent followed by an EXIT command.
**
**	Ovqp decides whether to use batch update or not. If ack == ACK
**	then endovqp will read a RETVAL symbol from ovqp and return
**	a token which specifies whether to call the update processor or not.
*/
endovqp(ack)
int	ack;
{
	register int		i;
	extern int		Qry_mode;

	if (ack != RUBACK)
	{
		if (Eql && Qry_mode == (int) mdRETTERM)
			eqleol(EXIT);	/* signal end of retrieve to EQL process */
	}

	i = NOUPDATE;

	if (ack == ACK)
	{
		if (Bopen)
		{
			closebatch();
			Bopen = FALSE;
			i = UPDATE;
		}
	}
	else
	{
		if (Bopen)
		{
			rmbatch();
			Bopen = FALSE;
		}
	}

	Retcode.rc_tupcount = Tupsfound;

	closecatalog(FALSE);

	return (i);
}



/*
**	Add node n to ovqp's list
*/
ovqpnod(n)
QTREE			*n;
{
	register SYMBOL			*s;
	register QTREE			*q;
	register int			i;
	extern QTREE			*ckvar();
	extern SYMBOL			*need();

	q = n;
	s = &q->sym;

	/* VAR nodes must be specially processed */
	if (s->type == VAR)
	{
		/* locate currently active VAR */
		q = ckvar(q);

		/* Allocate an ovqp var node for the VAR */
		s = need(Ovqpbuf, SYM_HDR_SIZ + VAR_SIZ);
		s->len = VAR_SIZ;
		((SVAR_NODE *) s)->vattno = ctou(((VAR_NODE *) q)->attno);
		((SVAR_NODE *) s)->vtype = ((VAR_NODE *) q)->frmt;
		((SVAR_NODE *) s)->vlen = ((VAR_NODE *) q)->frml;

		/* If VAR has been substituted for, get value */
		if (((VAR_NODE *) q)->valptr)
		{
			/* This is a substituted variable */
			if (((VAR_NODE *) q)->varno == Sourcevar)
				AAsyserr(12010, ((VAR_NODE *) q)->varno, Sourcevar);

			s->type = S_VAR;
			((SVAR_NODE *) s)->vpoint = (short *) ((VAR_NODE *) q)->valptr;
		}
		else
		{
			/* Var for one variable query */
			if (((VAR_NODE *) q)->varno != Sourcevar)
				AAsyserr(12011, ((VAR_NODE *) q)->varno, Sourcevar);
			s->type = VAR;
			if (((VAR_NODE *) q)->attno)
			{
				i = ctou(((VAR_NODE *) q)->attno);
				((SVAR_NODE *) s)->vpoint = (short *) (Intup + Source->reloff[i]);
			}
			else
				((SVAR_NODE *) s)->vpoint = (short *) &Intid;
		}
	}
	if (s->type == AOP)
		Agcount++;

	/* add symbol to list */
	if (Qvpointer > MAXNODES - 1)
		ov_err(NODOVFLOW);
	Qvect[Qvpointer++] = s;
}
