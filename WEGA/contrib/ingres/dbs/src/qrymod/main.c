# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/catalog.h"
# include	"../h/pipes.h"
# include	"../h/tree.h"
# include	"../h/bs.h"
# include	"qrymod.h"

# ifdef P8000
ret_buf		AAjmp_buf;
# else
jmp_buf		AAjmp_buf;
# endif

struct pipfrmt		Pipe;
struct pipfrmt		Outpipe;
char			Terminal[PROTERM + 1];	/* terminal id */
DESC			Prodes;		/* AA_PROTECT catalog descriptor */
DESC			Reldes;		/* AA_REL catalog descriptor */
DESC			Treedes;	/* AA_TREE catalog descriptor */
DESC			Intdes;		/* AA_INTEG catalog descriptor */
int			Nullsync;	/* send null qry on error */
extern int		Eql;		/* EQL flag */
char			*Qbuf;		/* the query buffer */

/*
**  QRYMOD -- query modification process
**
**	This process modifies queries to implement views, protection,
**	and integrity.
**
**	Defines:
**		Pipe -- the input pipe block.
**		Outpipe -- the output pipe block.
**		Terminal -- the current terminal id.
**		Reldes -- the relation relation descriptor.
**		Treedes -- the tree relation descriptor.
**		Prodes -- the AA_PROTECT descriptor.
**		Nullsync -- set if we should send a null query on an
**			error (to free up an EQL program).
**		main()
**		rubproc()
*/
main(argc, argv)
int	argc;
char	**argv;
{
	register int			execid;
	register int			funcid;
	register QTREE			*root;
	register struct retcode		*rc;
	register char			*tty;
	struct pipfrmt			pbuf;
	char				qbufbuf[QBUFSIZ];
	extern char			*ttyname();
	extern struct retcode		*issue();
	extern QTREE			*readqry();
	extern QTREE			*qrymod();
	extern int			pipetrrd();
	extern int			relntrrd();

#	ifdef xQTR1
	AAtTrace(&argc, argv, 'Q');
#	endif

	set_so_buf();
	initproc("QRYMOD", argv);
	AAam_start();
	Qbuf = qbufbuf;

	/* determine user's terminal for protection algorithm */
	tty = ttyname(1);
	if (AAbequal(tty, "tty", 3))
		tty = &tty[3];
	else if (AAbequal(tty, "/dev/tty", 8))
		tty = &tty[8];
	AApmove(tty, Terminal, PROTERM, ' ');
	Terminal[PROTERM] = '\0';

	/* resync to startup */
	wrpipe(P_PRIME, &pbuf, 0, (char *) 0, 0);
	wrpipe(P_INT, &pbuf,  W_up);
	setexit();
#	ifdef xQTR3
	AAtTfp(0, 1, "setexit->\n");
#	endif

	/*
	**  MAIN LOOP
	**	Executed once for each query, this loop reads the
	**	exec & funcid, determines if the query is
	**	"interesting", modifies it if appropriate, and
	**	if there is anything left worth running, passes
	**	it down the pipe.
	*/
	for ( ; ; )
	{
		Nullsync = FALSE;

		/* read the new function */
		rdpipe(P_PRIME, &Pipe);
		execid = rdpipe(P_EXECID, &Pipe, R_up);
		funcid = rdpipe(P_FUNCID, &Pipe, R_up);

		/* decide on the action to perform */
		switch (execid)
		{
		  case EXEC_DECOMP:
			/* read in query and return tree */
			root = readqry(pipetrrd, TRUE);
			rdpipe(P_SYNC, &Pipe, R_up);

			/* test for sync of EQL on errors */
			if (Resultvar == -1 && Eql)
				Nullsync = TRUE;

			/* do tree modification and issue query */
			root = qrymod(root);
			rc = issue(execid, funcid, root);
			break;

		  case EXEC_QRYMOD:	/* define, view, protect, or integrity */
			define(funcid);
			rc = (struct retcode *) 0;
			break;

		  default:		/* DBU function */
			wrpipe(P_PRIME, &Outpipe, execid, (char *) 0, funcid);
			copypipes(&Pipe, R_up, &Outpipe, W_down);
			rc = issue(execid, funcid, (QTREE *) 0);
			break;
		}

		/* signal done to above */
		wrpipe(P_PRIME, &Pipe, execid, (char *) 0, funcid);
		if (rc)
			wrpipe(P_NORM, &Pipe, W_up, rc, sizeof *rc);
		wrpipe(P_END, &Pipe, W_up);
		fflush(stdout);
	}
}


rubproc()
{
	resyncpipes();
}
