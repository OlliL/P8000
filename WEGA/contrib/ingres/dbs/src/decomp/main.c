# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"
# include	"../h/pipes.h"
# include	"../h/bs.h"
# include	"decomp.h"

# ifdef P8000
ret_buf		AAjmp_buf;
# else
jmp_buf		AAjmp_buf;
# endif

# define	AAtTFLAG	'D'

int		Newr;		/* force OVQP to reopen result relation */
int		Qmode;		/* flag set to indicate mode of tuple disposition */
int		Resultvar;	/* if >= 0 result variable */
int		Sourcevar;	/* likewise for source variable */
int		R_ovqp;		/* pipe for reading from ovqp */
int		W_ovqp;		/* pipe for writing to ovqp */
int		R_dbu;		/* ditto dbu */
int		W_dbu;		/* ditto dbu */
int		Batchupd;
struct pipfrmt 	Inpipe;
struct pipfrmt 	Outpipe;

main(argc, argv)
int	argc;
char	*argv[];
{
	register int		exec_id;
	register int		func_id;

#	ifdef xDTR1
	AAtTrace(&argc, argv, AAtTFLAG);
#	endif

	initproc("DECOMP", argv);

	Batchupd = setflag(argv, 'b', 1);

	AAam_start();
	set_so_buf();

	R_ovqp = R_up;
	W_ovqp = W_up;
	R_dbu = R_down;
	W_dbu = W_down;

	/*
	** Do the necessary decomp initialization. This includes
	** giving access methods more pages (if i/d system).
	** init_decomp is defined in either call_ovqp34 or call_ovqp70.
	*/
	init_decomp();

	/* resync to startup */
	wrpipe(P_PRIME, &Outpipe, 0, (char *) 0, 0);
	wrpipe(P_INT, &Outpipe, W_up);
	setexit();

	for ( ; ; )
	{
		rdpipe(P_PRIME, &Inpipe);
		exec_id = rdpipe(P_EXECID, &Inpipe, R_up);
		func_id = rdpipe(P_FUNCID, &Inpipe, R_up);
		if (exec_id == EXEC_DECOMP)
		{
			qryproc();
			continue;
		}

		/* copy pipes on down */
		wrpipe(P_PRIME, &Outpipe, exec_id, (char *) 0, func_id);
		copypipes(&Inpipe, R_up, &Outpipe, W_down);

		/* read response and send eop back up to parser */
		writeback(-1);
	}
}


/*
**  RUBPROC -- process a rubout signal
**
**	Called from the principle rubout catching routine
**	when a rubout is to be processed. Notice that rubproc
**	must return to its caller and not call reset itself.
**
**	Side Effects:
**		reinitializes the state of the world.
*/
rubproc()
{
	struct pipfrmt	pipebuf;
	extern int	Eql;

	/*
	** Sync with EQL if we have the EQL pipe.
	**	This can happen only if ovqp and decomp
	**	are combined.
	*/
	if (W_front >= 0 && Eql)
		wrpipe(P_INT, &pipebuf, W_front);

	flush();
	resyncpipes();
	endovqp(RUBACK);
	reinit();
	return;
}
