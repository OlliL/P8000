# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/pipes.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"
# include	"../h/bs.h"
# include	"parser.h"
# include	"../h/scanner.h"

static struct retcode	*Lastcnt;	/* ptr to last tuple count, or 0 */
static struct retcode	Lastcsp;	/* space for last tuple count */

/*
**  CONTROL.C -- -- collection of control functions for the parser
**
**	These routines administrate the operation of the parser for internal
**	sequencing.  There are 3 pairs of routines, one pair for each
**	QL statement, one for each go-block, and one to handle syncronization
**	with up and down processes.
**
**	Defines:
**		init_qlst	-- initialize for a QL statement
**		endqlst		-- clean up after a QL statement
**		syncup		-- sync the process above
**		syncdn		-- sync the process below
**		startgo		-- initialize for a go-block
**		endgo		-- clean up after a go-block
**
**
**
** INIT_QLST -- set vbles for default mode before each QL statement
*/
init_qlst()
{
	extern int	neederr();

#	ifdef	xPTR3
	AAtTfp(60, 0, "INIT_QLST:\n");
#	endif
	initp();				/* reset dbu buffer */
	initbuf(Qbuf, TREEMAX, TREEOFLO, neederr);	/* reset tree allocation */
	Dbserr = 0;				/* clear error condition */
	Patflag = 0;				/* reset pattern matching flag */
	Pars = 1;				/* set scanner into "parser" mode */
	Lcase = Dcase;				/* set case mapping to default */
	Agflag = 0;				/* reset aggregate flag */
	Opflag = 0;				/* reset qmode flag */
	Resrng = 0;				/* reset result relation ptr */
	Qlflag = 0;				/* reset qualification flag */
	freesym();				/* free symbol table space */
	rngreset();				/* reset used bits in range tbl */
	fflush(stdout);
}


/*
** ENDQLST -- finish command checking and processing for each QL statement
*/
endqlst(op1)
int	op1;
{
	register int			i;
	register int			op;
	char				ibuf[2];	/* two char buffer for index keys */

#	ifdef	xPTR3
	AAtTfp(60, 2, "ENDQLST:\n");
#	endif
	op = op1;

	/* check next token for GOVAL if the next token has been read */
	switch (op)
	{
	  case mdSAVE:
	  case mdCOPY:
	  case mdCREATE:

#	  ifdef	DISTRIB
	  case mdDCREATE:
#	  endif

	  case mdINDEX:
	  case mdRANGE:
		break;

	  default:
		/* has vble ending and therefore must detect valid end of command */
		if (Lastok.tokop != GOVAL)	/* next token not start of command */
			yyerror(NXTCMDERR, (char *) 0);
		break;
	}
	if (Agflag >= MAXAGG)	/* too many aggregates */
		yyerror(AGGXTRA, (char *) 0);

	/* command ok so far, finish up */
	switch (op)
	{
	  case mdRETR:
	  case mdRET_UNI:
	  case mdVIEW:
		if (Resrng)
		{
			/* need to do create, everything is ready */
			call_p(mdCREATE, EXEC_DBU);
			cleanrel(&Desc);
		}
		else if (!Eql)	/* need to print header */
			header();
		if (Dbserr)
		{
			/*
			** might be nice to back out the create already done
			** by this point so that the user doesn't need to
			*/
			endgo();	/* abort rest of go-block */
			reset();
		}
		/* fall through */

	  case mdAPP:
	  case mdDEL:
	  case mdREPL:
		if (op != (int) mdVIEW)
		{
			call_tree(op, EXEC_DECOMP);
			break;
		}

#	  ifdef DISTRIB
	  case mdDISTRIB:
		op = mdVIEW;
#	  endif
		/* else, do VIEW */
		initp();
		setp(Resrng->relnm);
		call_tree(mdDEFINE, EXEC_QRYMOD);
		call_p(op, EXEC_QRYMOD);
		break;

	  case mdINTEG:
	  case mdPROT:
		call_tree(mdDEFINE, EXEC_QRYMOD);
		call_p(op, EXEC_QRYMOD);
		break;

	  case mdCREATE:

#	  ifdef	DISTRIB
	  case mdDCREATE:
#	  endif

	  case mdDESTROY:
	  case mdMODIFY:
		call_p(op, EXEC_DBU);
		cleanrel(&Desc);
		break;

	  case mdCOPY:
	  case mdHELP:
	  case mdPRINT:
	  case mdSAVE:
	  case mdDISPLAY:
	  case mdREMQM:
		call_p(op, EXEC_DBU);
		break;

	  case mdRANGE:
		break;

	  case mdINDEX:
		call_p(op, EXEC_DBU);
		if (Dbserr)
		{
			endgo();	/* abort rest of go-block */
			reset();
		}
		if (Indexspec)
		{
			setp(Indexname);
			setp(Indexspec);
			setp("num");
			for (i = 1; i <= Rsdmno; i++)
			{
				ibuf[0] = ctou(i);
				ibuf[1] = '\0';
				setp(ibuf);
			}
			call_p(mdMODIFY, EXEC_DBU);
		}
		break;
	}

	/* refresh relstat bits if necessary */
	rngfresh(op);
	init_qlst();
}


/*
** SYNCUP -- sync with process above
**
**	send tuple count if present
*/
syncup()
{
	struct pipfrmt		p;
	register struct pipfrmt	*pp;

#	ifdef	xPTR3
	AAtTfp(60, 3, "SYNCUP:\n");
#	endif
	pp = &p;
	wrpipe(P_PRIME, pp, 0, (char *) 0, 0);
	if (Lastcnt)
	{
		wrpipe(P_NORM, pp, W_up, Lastcnt, sizeof(*Lastcnt));
	}
	wrpipe(P_END, pp, W_up);
}


/*
** SYNCDN -- wait for sync from below
**
**	save tuple count if present
*/
syncdn()
{
	struct retcode		up;
	struct pipfrmt		p;
	register struct pipfrmt	*pp;

#	ifdef	xPTR3
	AAtTfp(60, 4, "SYNCDN:\n");
#	endif
	pp = &p;
	rdpipe(P_PRIME, pp);
	if (rdpipe(P_NORM, pp, R_down, &up, sizeof(up)) != 0)
	{
		AAbmove(&up, &Lastcsp, sizeof(up));
		Lastcnt = &Lastcsp;
#		ifdef	xPTR3
		AAtTfp(60, 1, "Reading tuple count: %ld\n", Lastcnt->rc_tupcount);
#		endif
	}
}


/*
** STARTGO -- do whatever needs doing to set up a go-block
*/
startgo()
{
#	ifdef	xPTR3
	AAtTfp(60, 5, "STARTGO:\n");
#	endif
	/* initialize for go-block */
	getscr(1);	/* prime the scanner input */
	Lastcnt = (struct retcode *) 0;	/* reset ptr to last tuple count */
	init_qlst();	/* most other init's are done for each statement */
	yyline = 1;	/* reset line counter */
	yypflag = 1;	/* reset action stmnts */
}


/*
** ENDGO -- do whatever needs doing to clean up after a go block
*/
endgo()
{
#	ifdef	xPTR3
	AAtTfp(60, 6, "ENDGO:\n");
#	endif
	getscr(2);
	syncup();
}
