# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/pipes.h"
# include	"../h/aux.h"
# include	"../h/bs.h"
# include	"../h/access.h"
# include	"../h/lock.h"

/*
**  DBU CONTROLLER -- Handles parameter passing & return for dbu's.
**
**	This is the common control routine for calling dbu routines
**	and switching overlays when necessary. The specific dbu routine
**	is indicated in the funcid according to a set of numbers
**	defined in ../symbol.h. The execid is EXEC_DBU.
**
**	The dbu controller calls init_proctab to read in the process table.
**	This defines the location of each dbu routine (whether they are in
**	dbu34_a, dbu34_c, dbu34_i or dbu34_m). If the requested dbu routine
**	is not in the current overlay, the correct overlay is called with
**	the execid of the correct overlay (a,c,i,m) and the correct funcid.
**
**	To call a dbu, the parameters are read into a buffer and a pc,pv
**	parameter structure is made. When the dbu returns, all system
**	relations are flushed and a sync is returned.
**	If the Dburetflag is set, then the controller returns the
**	Dburetcode structure back.
**
**	Positional Parameters:
**		standard plus
**			Xparams[0] - pathname for ksort
**			Xparams[1] - process table
**
**	Flags:
**		-Z -- trace flags. See individual routines for details
**		--X -- X & 077 is a file descriptor to read to get the
**			first pipe block (instead of R_up).
*/

# define	MAXARGS		18	/* max number of args to main() */
# define	STRBUFSIZ	1000	/* max bytes of arguments */

# ifdef P8000
ret_buf			AAjmp_buf;
# else
jmp_buf			AAjmp_buf;
# endif

extern int		Files[MAXFILES];
FILE			*Dbs_out = stdout;
int			Standalone;
static char		Cur_id;
char			Execid;
static char		Funcid;
int			Noupdt;
static struct pipfrmt	Pbuf;
int			Dburetflag;
struct retcode		Dburetcode;
char			SYSNAME[]	= "_SYS";
int			s_SYSNAME	= sizeof SYSNAME - 1;

main(argc, argv)
int	argc;
char	**argv;
{
	extern char	*AAproc_name;
	register char	*sp;
	char		*parmv[MAXPARMS];	/* ptrs to string parameters */
	char		stringbuf[STRBUFSIZ];	/* hold parameters for DBU functions */
	char		*vect[MAXARGS];
	int		exind;
	register int	i;
	int		j;
	extern char	**Xparams;
	extern		(*Func[])();		/* function pointers */
	register char	*funcflag;
	char		**vp;
	extern long	AAccuread;
	extern long	AAccusread;
	extern long	AAccuwrite;
	int		pipevect[2];

	set_so_buf();
	initproc("DBU_x", argv);
	Noupdt = !setflag(argv, 'U', 0);
	exind = AAlength(argv[0]) - 1;
	Cur_id = argv[0][exind];
	AAproc_name[4] = Cur_id;
	AAbmove(argv, vect, argc * sizeof argv[0]);
	vect[argc] = 0;
	funcflag = (char *) 0;
	i = argc;
#	ifdef xZTR1
	AAtTrace(&argc, argv, 'Z');
	if (AAtTf(1, 2))
		AAprargs(i, vect);
#	endif

	/*
	**  Scan argument vector for '--' flag; if found, save location
	**  of 'X' part for use when we exec another overlay, and read
	**  the block from the last overlay.  If not found, create a
	**  '--' flag for later use.
	*/

	for (vp = vect; *vp; vp++)
	{
		sp = *vp;
		if (sp[0] == '-' && sp[1] == '-')
		{
			/* deferred EXECID/FUNCID */
			funcflag = &sp[2];
			i = *funcflag & 077;
			Execid = rdpipe(P_EXECID, &Pbuf, i);
			Funcid = rdpipe(P_FUNCID, &Pbuf, i);
			close(i);
		}
	}
	if (!funcflag)
	{
		/* resync to startup */
		wrpipe(P_PRIME, &Pbuf, 0, (char *) 0, 0);
		wrpipe(P_INT, &Pbuf,  W_up);

		/* put in a -- flag */
		*vp++ = sp = "---";
		funcflag = &sp[2];
		*vp = (char *) 0;
		Execid = 0;		/* read initial exec/func id */
	}

	/*
	**  Create list of open files.
	**	This allows us to close all extraneous files when an
	**	overlay completes (thus checking consistency) or on
	**	an interrupt, without really knowing what was going
	**	on.  Notice that we call AAam_start so that any files
	**	the access methods need will be open for this test.
	*/

	AAam_start();

#	ifdef xZTR3
	AAtTfp(1, 8, "Open files: ");
#	endif
	for (i = 0; i < MAXFILES; i++)
		if ((Files[i] = dup(i)) >= 0)
		{
			close(Files[i]);
#			ifdef xZTR3
			AAtTfp(1, 8, " %d", i);
#			endif
		}
#	ifdef xZTR3
	AAtTfp(1, 8, "\n");
#	endif

	/* initialize the process table */
	init_proctab(Xparams[1], Cur_id);

	setexit();

	/*  *** MAIN LOOP ***  */
	for ( ; ; )
	{
		/* get exec and func id's */
		if (!Execid)
		{
			rdpipe(P_PRIME, &Pbuf);
			Execid = rdpipe(P_EXECID, &Pbuf, R_up);
			Funcid = rdpipe(P_FUNCID, &Pbuf, R_up);
		}

		if (Execid == EXEC_DBU)
			get_proctab(Funcid, &Execid, &Funcid);

		/* see if in this overlay */
		if (Execid != Cur_id)
			break;
		i = 0;

		/* read the parameters */
		/*** this should check for overflow of stringbuf ***/
		for (sp = stringbuf; j = rdpipe(P_NORM, &Pbuf, R_up, sp, 0); sp += j)
			parmv[i++] = sp;
		parmv[i] = (char *) -1;
#		ifdef xZTR1
		if (AAtTfp(1, 4, "DBU: %c%c: ", Execid, Funcid))
			AAprargs(i, parmv);
#		endif
		if (i >= MAXPARMS)
			AAsyserr(11056, i, Cur_id, Funcid);

		/* call the specified function (or it's alias) */
#		ifdef xZTR3
		AAtTfp(1, 7, "calling %c%c\n", Execid, Funcid);
#		endif
#		ifdef xZTR1
		if (AAtTf(50, 0))
			AAccuread = AAccuwrite = AAccusread = 0;
#		endif
/*?EBCDIC?*/	j = Funcid - '0';
/*?EBCDIC?*/	if (Funcid >= 'A')
/*?EBCDIC?*/		j -= 'A' - '9' - 1;
		Dburetflag = FALSE;
		i = (*Func[j])(i, parmv);
#		ifdef xZTR1
		AAtTfp(1, 5, "returning %d\n", i);
		AAtTfp(50, 0, "DBU read %ld pages,%ld catalog pages,wrote %ld pages\n",
			AAccuread, AAccusread, AAccuwrite);
#		endif

		/* do termination processing */
		finish();

		if (Dburetflag)
			wrpipe(P_NORM, &Pbuf, W_up, &Dburetcode, sizeof (Dburetcode));
		wrpipe(P_END, &Pbuf, W_up);
	}

	/*
	**  Transfer to another overlay.
	**	We only get to this point if we are in the wrong
	**	overlay to execute the desired command.
	**
	**	We close system catalog caches and access method files
	**	so that no extra open files will be laying around.
	**	Then we adjust the pathname of this overlay to point
	**	to the next overlay.  We then perform a little magic:
	**	we create a pipe, write the current pipe block into
	**	the write end of that pipe, and then pass the read
	**	end on to the next overlay (via the --X flag) -- thus
	**	using the pipe mechanism as a temporary buffer.
	*/

	/* close the system catalog caches */
	closecatalog(TRUE);

	/* close any files left open for access methods */
	if (AAam_exit())
		AAsyserr(11057);

	/* setup the execid/Funcid for next overlay */
	vect[0][exind] = Execid;

	/* create and fill the communication pipe */
	if (pipexx(pipevect) < 0)
		AAsyserr(11058);
	*funcflag = pipevect[0] | 0100;
	wrpipe(P_WRITE, &Pbuf, pipevect[1], (char *) 0, 0);
	close(pipevect[1]);

#	ifdef xZTR1
	AAtTfp(1, 1, "calling %s\n", vect[0]);
#	endif

	fflush(stdout);
	execv(vect[0], vect);
	AAsyserr(11059, vect[0]);
}


finish()
{
	struct stat	fstat_buf;
#	ifdef xZTR1
	register int	i;
	register int	j;

	if (AAtTf(1, 15))
	{
		for (i = 0; i < MAXFILES; i++)
		{
			j = dup(i);
			if (j < 0)
			{
				if (Files[i] >= 0)
					printf("file %d closed by %c%c\n",
						i, Cur_id, Funcid);
			}
			else
			{
				close(j);
				if (Files[i] < 0)
				{
					fstat(i, &fstat_buf);
					printf("file %d (i# %u) opened by %c%c\n",
						i, fstat_buf.st_ino, Cur_id, Funcid);
					close(i);
				}
			}
		}
	}
#	endif

	/* flush the buffers for the system catalogs */
	closecatalog(FALSE);
	fflush(stdout);

	wrpipe(P_PRIME, &Pbuf, Execid, (char *) 0, Funcid);
	Execid = Funcid = 0;
}


rubproc()
{
	register int	i;

	resyncpipes();
	finish();

	/* close any extraneous files */
	AAunlall();
	for (i = 0; i < MAXFILES; i++)
		if (Files[i] < 0)
			close(i);
}


/*
**  PIPEXX -- dummy call to pipe
**
**	This is exactly like pipe (in fact, it calls pipe),
**	but is included because some versions of the C interface
**	to BS clobber a register varaible.  Yeuch, shit, and
**	all that.
*/
pipexx(pv)
int	pv[2];
{
	return (pipe(pv));
}
