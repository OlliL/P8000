# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"
# include	"../h/pipes.h"
# include	"ovqp.h"


char		Outtup[MAXTUP];
char		Intup[MAXTUP];
char		*Origtup;
long		Intid;
long		Uptid;
int		Ov_qmode;	/* flag set to indicate mode of tuple	    */
long		*Counter;	/* cnts "gets" done in OVQP		    */
char		*Tend;		/* pts to end of data in Outtup		    */
int		Buflag;		/* flags a batch update situation	    */
int		Targvc;		/* var count in Target list		    */
int		Qualvc;		/* var count in Qual list		    */
int		Userqry;	/* flags a query on the result rel	    */
int		Retrieve;	/* true is a retrieve, else false	    */
int		Diffrel;	/* true if Source and Result are different  */
int		Agcount;	/* count # of aggregates in the query	    */
long		Tupsfound;	/* counts # tuples which satified the query */
SYMBOL		**Tlist;
SYMBOL		**Alist;
SYMBOL		**Qlist;
SYMBOL		**Bylist;
int		Batch_fp;	/* file descriptor for batch file            */
int		Batch_cnt;	/* no of bytes taken from the current buffer */
int		Batch_dirty;	/* used during update to mark a dirty page   */

/*
**	SCAN
**
**	performs tuple by tuple scan of source reln or index reln
**	within limits found by strategy routine.
**	When the source reln tuple is obtained the interpreter is invoked
**	to continue further processing
**
*/


scan()
{
	register int		j;
	register int		mode;
	register int		domno;
	char			*rlist;	/* "result" list of query */
	long			count;
	long			tid;
	long			temptid;
	char			agtup[MAXTUP];
	char			outtup1[MAXTUP];
	int			qualfound;
	int			ok;
	extern SYMBOL		*interpret();

#	ifdef xOTR1
	AAtTfp(30, -1, "SCAN\tScanr=%.12s\n", Scanr? Scanr->reldum.relid: "(none)");
	AAtTfp(30, 4, " Alist=0%o, Bylist=0%o, Tlist=0%o, Qlist=0%o\n",
				Alist, Bylist, Tlist, Qlist);
#	endif

	if (Result || Alist)
	{
		if (Result)
			clr_tuple(Result, Outtup);
		else
		{
			j = MAXTUP;
			while (j--)
				Outtup[j] = 0;
		}
	}

	count = 0;
	qualfound = EMPTY;
	mode = Ov_qmode;

	/*
	** Check for identical source and result relations.
	** For modes mdREPL and mdDEL, Origtup must point
	** to the original (unmodified result tuple).
	**
	** If there is no Source or Result relations then
	** the code has no effect.
	*/
	if (!AAbequal(Source->reldum.relid, Result->reldum.relid, MAXNAME))
	{
		Diffrel = TRUE;
		Origtup = outtup1;
	}
	else
	{
		Diffrel = FALSE;
		Origtup = Intup;
	}

	/*  determine type of result list */
	/* the only valid combinations are:
	**
	** Tlist=no	Alist=no	Bylist=no
	** Tlist=yes	Alist=no	Bylist=no
	** Tlist=no	Alist=yes	Bylist=no
	** Tlist=no	Alist=yes	Bylist=yes
	*/
	rlist = (char *) (Tlist? Tlist: Alist);
	if (Bylist)
		rlist = 0;

	Counter= &count;
	if (Bylist)
	{
		/*
		** For aggregate functions the result relation
		** is in the format:
		** domain 1 = I4 (used as a counter)
		** domain 2 through relatts - Agcount (by-domains)
		** remaining domains (the actual aggregate values)
		*/

		/* set up keys for the AAgetequal */
		/* domno must end with the domain number of the first aggregate */
		for (domno = 2; domno <= Result->reldum.relatts - Agcount; domno++)
			Result->relgiven[domno] = 1;

		Counter = (long *) Outtup;	/* first four bytes of Outtup is counter for Bylist */
	}

	/*
	** check for constant qualification.
	** If the constant qual is true then remove
	** the qual to save reprocessing it.
	** If it is false then block further processing.
	*/

	ok = TRUE;
	if (Qlist && !Qualvc)
		if (*interpret(Qlist)->value)
			Qlist = 0;	/* qual always true */
		else
			ok = FALSE;	/* qual always false */


	/* if no source relation, interpret target list */
	if (!Scanr && ok)
	{
		/* there is no source relation and the qual is true */
		qualfound = NONEMPTY;
		Tend = Outtup;
		/* if there is a rlist then process it. (There should always be one) */
		if (rlist)
		{
			(*Counter)++;
			interpret(rlist);
		}
		if (Tlist)
			dispose(mode);
		else
			if (Userqry)
				Tupsfound++;
	}

	if (Scanr && ok)
	{
		/* There is a source relation. Iterate through each tuple */
		while (!(j = AAam_get(Scanr, &Lotid, &Hitid, Intup, NXTTUP)))
		{
#			ifdef xOTR1
			if (AAtTf(30, 5))
			{
				if (Scanr != Source)
					printf("Sec Index:");
				else
					printf("Intup:");
				AAprtup(Scanr, Intup);
			}
#			endif
			Intid = Lotid;
			if (Scanr != Source)
			{
				/* make sure index tuple is part of the solution */
				if (!indexcheck())
					/* index keys don't match what we want */
					continue;
				AAbmove(Intup + Scanr->reldum.relwid - TIDLEN, &tid, TIDLEN);
				if (j = AAam_get(Source, &tid, &temptid, Intup, CURTUP))
					AAsyserr(17013, j, Scanr->reldum.relid);
#				ifdef xOTR1
				if (AAtTf(30, 6))
				{
					printf("Intup:");
					AAprtup(Source, Intup);
				}
#				endif
				Intid = tid;
			}

			if ( !Qlist || *interpret(Qlist)->value)
			{
				qualfound = NONEMPTY;
				Tend = Outtup;
				if (rlist)
				{
					(*Counter)++;
					interpret(rlist);
				}
				if (Tlist)
					dispose(mode);
				else
					if (Userqry)
						Tupsfound++;

				if (!Targvc)	/* constant Target list */
					break;

				/* process Bylist if any */
				if (Bylist)
				{
					interpret(Bylist);
					if ((j = AAgetequal(Result, Outtup, agtup, &Uptid)) < 0)
						AAsyserr(17014, j, Result->reldum.relid);

					if (!j)
					{
						/* match on bylist */
						AAbmove(agtup, Outtup, Result->reldum.relwid);
						mode = (int) mdREPL;
						(*Counter)++;
					}
					else
					{
						/* first of this bylist */
						mode = (int) mdAPP;
						*Counter = 1;
					}

					Tend = Outtup + Result->reloff[domno];
					interpret(Alist);
					dispose(mode);
				}
			}
		}

		if (j < 0)
			AAsyserr(17015, j, Source->reldum.relid);
	}
	if (Result)
	{
		if (j = AAnoclose(Result))
			AAsyserr(17016, j, Result->reldum.relid);
	}
	return (qualfound);
}

dispose(mode)
{
	register int	i;

	i = 0;

	if (!Result)
	{
		if (Eql)
			eqleol(EOTUP);
		else
			AApreol();
	}
	else
	{
#		ifdef xOTR1
		AAtTfp(30, 1, "mode=%d, ", mode);
		if (AAtTf(30, 2) && (mode == mdREPL || mode == mdDEL))
			printf("Uptid:%ld, ", Uptid);
		if (AAtTf(30, 3))
			if (mode == mdDEL)
				AAprtup(Source, Intup);
			else
				AAprtup(Result, Outtup);
#		endif

		/* SPOOL UPDATES OF EXISTING USER RELNS TO BATCH PROCESSOR */
		if (Buflag)
		{
			addbatch(&Uptid, Outtup, Origtup);
			return;
		}

		/* PERFORM ALL OTHER OPERATIONS DIRECTLY */
		switch (mode)
		{
		  case mdRETR:
		  case mdAPP:
			if ((i = AAam_insert(Result, &Uptid, Outtup, NODUPS)) < 0)
				AAsyserr(17017, i, Result->reldum.relid);
			break;

		  case mdREPL:
			if ((i = AAam_replace(Result, &Uptid, Outtup, NODUPS)) < 0)
				AAsyserr(17018, i, Result->reldum.relid);
			break;

		  case mdDEL:
			if ((i = AAam_delete(Result, &Uptid)) < 0)
				AAsyserr(17019, i, Result->reldum.relid);
			break;

		  default:
			AAsyserr(17020, mode);
		}
	}

	if (Userqry && !i)
		Tupsfound++;
}
