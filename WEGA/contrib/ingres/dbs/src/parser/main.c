# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/pipes.h"
# include	"../h/tree.h"
# include	"parser.h"
# include	"../h/access.h"
# include	"../h/bs.h"

# ifdef P8000
ret_buf		AAjmp_buf;
# else
jmp_buf		AAjmp_buf;
# endif

DESC		Desc;		/* descriptor for attribute relation */
char		*Relspec;	/* ptr to storage structure of result relation */
char		*Indexspec;	/* ptr to stor strctr of index */
int		Dbserr;		/* set if a query returns an error from processes below */
int		Noupdt;		/* DBS user override of no update restriction */
int		Dcase;		/* default case mapping */
int		Lcase;		/* UPPER->lower conversion flag	*/
char		*Qbuf;		/* buffer which holds tree allocated on stack in main.c to get data space */
int		Qrymod;		/* qrymod on in database flag */

main(argc, argv1)
int		argc;
char		*argv1[];
{
	register char		**argv;
	struct pipfrmt		pbuf;
	extern char		*valflag;
	extern int		yydebug;
	/*
	** This is a ROYAL kludge used to obtain data space for the
	** parser.  The treebuffer should really be allocated in
	** ../parser.h but unfortunately.
	*/
	char			treebuffer[TREEMAX];

	/* set up parser */
	set_so_buf();
	argv = argv1;
	Qbuf = treebuffer;	/* see kludge notice above */

#	ifdef	xPTR1
	AAtTrace(&argc, argv, 'P');
#	endif
#	ifdef	xPTR2
	if (AAtTf(17, 0))
		AAprargs(argc, argv);
#	endif

	initproc("PARSER", argv);
	Noupdt = !setflag(argv, 'U', 0);
	Dcase = setflag(argv, 'L', 1);
	Relspec = setflag(argv, 'r', 1)? "cheapsort": valflag;
	Indexspec = setflag(argv, 'n', 1)? "isam": valflag;
	if (AAsequal(Relspec, "heap"))
		Relspec = (char *) 0;
	if (AAsequal(Indexspec, "heap"))
		Indexspec = (char *) 0;

	rnginit();
	AArelopen(&Desc, 0, AA_ATT);
	/*
	** The 'AArelopen' must be done before this test so that the 'AAdmin'
	**	structure is initialized.  The 'Qrymod' flag is set when
	**	the database has query modification turned on.
	*/
	Qrymod = ((AAdmin.adhdr.adflags & A_QRYMOD) == A_QRYMOD);

	/* resync to startup */
	wrpipe(P_PRIME, &pbuf, 0, (char *) 0, 0);
	wrpipe(P_INT, &pbuf,  W_up);
	setexit();

	/* EXECUTE */
	for ( ; ; )
	{
		startgo();	/* initializations for start of go-block */

		yyparse();	/* will not return until end of go-block or error */

		endgo();	/* do cleanup (resync, etc) for a go-block */
	}
}


/*
** RUBPROC
*/
rubproc()
{
	resyncpipes();
}


/*
**  PROC_ERROR ROUTINE
**
**	This routine handles the processing of errors for the parser
**	process.  It sets the variable 'Dbserr' if an error is passes
**	up from one of the processes below.
*/
proc_error(s, fd)
register struct pipfrmt	*s;
register int		fd;
{
	struct pipfrmt		t;

	if (fd != R_down)
		AAsyserr(18011);

	wrpipe(P_PRIME, &t, s->exec_id, (char *) 0, s->func_id);
	t.err_id = s->err_id;

	copypipes(s, fd, &t, W_up);
	rdpipe(P_PRIME, s);
	Dbserr = 1;
}
