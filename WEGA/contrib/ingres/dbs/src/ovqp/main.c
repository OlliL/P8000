# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/tree.h"
# include	"../h/access.h"
# include	"../h/batch.h"
# include	"../h/pipes.h"
# include	"../h/bs.h"
# include	"../h/lock.h"
# include	"ovqp.h"

# ifdef P8000
ret_buf		AAjmp_buf;
# else
jmp_buf		AAjmp_buf;
# endif

int		Bopen;		/* TRUE if batch file is open		    */
int		Batchupd;
int		R_decomp;	/* file for reading info from decomp	    */
int		W_decomp;	/* file for writing to decomp		    */
struct pipfrmt	Inpipe;
struct pipfrmt	Outpipe;

main(argc, argv)
int	argc;
char	**argv;
{
	register int		execid;
	register int		funcid;
	extern int		proc_error();

#	ifdef xOTR1
	AAtTrace(&argc, argv, 'O');
#	endif
	initproc("OVQP", argv);
	R_decomp = R_down;
	W_decomp = W_down;
	Batchupd = setflag(argv, 'b', 1);
	AAam_start();	/* init access methods */
	set_so_buf();	/* buffer standard output if appropriate */

	/* resync to startup */
	wrpipe(P_PRIME, &Outpipe, 0, (char *) 0, 0);
	wrpipe(P_INT, &Outpipe, W_up);
	setexit();

	for ( ; ; )
	{

		/* copy the message from parser to decomp */
		rdpipe(P_PRIME, &Inpipe);
		execid = rdpipe(P_EXECID, &Inpipe, R_up);
		funcid = rdpipe(P_FUNCID, &Inpipe, R_up);
		wrpipe(P_PRIME, &Outpipe, execid, (char *) 0, funcid);
		copypipes(&Inpipe, R_up, &Outpipe, W_down);

		/* do decomp to ovqp processing */
		if (execid == EXEC_DECOMP)
		{
			rdpipe(P_PRIME, &Inpipe);
			execid = rdpipe(P_EXECID, &Inpipe, R_down);
			funcid = rdpipe(P_FUNCID, &Inpipe, R_down);
			if (execid == EXEC_OVQP)
			{
				ovqp();
				flush();
			}
		}

		/* finish ovqp msg -or- do dbu return stuff */
		copyreturn();
	}
}



rubproc()
{
	extern int	Eql;
	struct pipfrmt	pipebuf;

	/* all relations must be closed before decomp
	** potentially destroys any temporaries
	*/
	if (Bopen)
	{
		rmbatch();	/* remove the unfinished batch file */
		Bopen = FALSE;
	}
	closeall();
	AAunlall();	/* remove any outstanding locks */

	/* if EQL then clear EQL data pipe */
	if (Eql)
		wrpipe(P_INT, &pipebuf, W_front);
	resyncpipes();

	flush();
}
