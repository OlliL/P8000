# include	"amx.h"
# ifdef MSDOS
# include	<fcntl.h>
# endif

# define	AMX_MODE	06755
static char	DUMMY[] =	"-DAMX";
char		*AMX =		"AMX";
static char	*CC =		"cc";
static char	*LIBA =		"/lib/libamx.a";
static char	*IX =		"/sys/etc/amx_ixc6.3";
char		AMXF =		'c';

# ifdef F77
# define	F77_FLAG	'f'
# define	LIB_F77		"-lF77"
# define	LIB_I77		"-lI77"
# define	LIB_U77		"-lU77"
# endif

# ifdef MEM_S
# define	SEG_FLAG	's'
# ifdef P8000
static char	*SAMX =		"SAMX";
static char	*SCC =		"scc";
static char	*SLIBA =	"/lib/slibamx.a";
# endif
# endif

# ifdef MEM_O
# define	OVL_FLAG	'o'
static char	*OAMX =		"OAMX";
static char	*OCC =		"cc";
# endif

char		*A_out;
int		Argc;
char		**Argv;
int		Xargc;
char		**Xargv;
int		Xerrors;
extern int	(*AAexitfn)();

# ifdef STACK
# ifdef INP_STACK
static char	meminp[BUFSIZ];
# endif
# ifdef AMX_STACK
static char	memamx[BUFSIZ];
# endif
# ifdef SCN_STACK
static char	memscn[BUFSIZ];
# endif
# ifdef TMP_STACK
static char	memtmp[BUFSIZ];
# endif
static char	memreltab[MAX_RELS * (MAXNAME + 1)];
static AMX_REL	memrel[3];
# endif

main(argc, argv, env)
int	argc;
char	**argv;
char	**env;
{
# ifdef F77
	register int		pid;
# endif
# ifndef STACK
# ifdef INP_STACK
	char			meminp[BUFSIZ];
# endif
# ifdef AMX_STACK
	char			memamx[BUFSIZ];
# endif
# ifdef SCN_STACK
	char			memscn[BUFSIZ];
# endif
# ifdef TMP_STACK
	char			memtmp[BUFSIZ];
# endif
	char			memreltab[MAX_RELS * (MAXNAME + 1)];
	AMX_REL			memrel[3];
# endif
	extern char		**talloc();
	extern char		*AAztack();
	extern int		end_amx();
# ifdef MSDOS
	extern char		*Fileset;
	extern char		*getenv();
	extern char		*mkfileset();
# endif

# ifdef INP_STACK
	BUF_inp = meminp;
# endif
# ifdef AMX_STACK
	BUF_amx = memamx;
# endif
# ifdef SCN_STACK
	BUF_scn = memscn;
# endif
# ifdef TMP_STACK
	BUF_tmp = memtmp;
# endif
	Reltab = memreltab;
	Temp_rel = memrel;
	Temp_rel[0].rel_uniq = MAX_RELS;
	Temp_rel[1].rel_uniq = MAX_RELS;
	Temp_rel[2].rel_uniq = MAX_RELS;
	AAtTrace(&argc, argv, 'A');
	AAexitfn = end_amx;
# ifdef MSDOS
	/* get pathname entry into 'AApath' variable */
	if (!(AApath = getenv("INDP")))
		AAsyserr("shell-Variable INDP nicht gesetzt");
	/* create msg file */
	if ((FILE_msg = open(AAztack(AApath, "/files/amx_msc6.3"), 0)) <= 0)
		AAsyserr("cannot open file amx_msc6.3\n");
	setmode(FILE_msg, O_BINARY);
	Fileset = mkfileset();
# else
	/* get pathname entry into 'AApath' variable */
	getpath();

# ifdef F77
	pid = AAlength(*argv) - 1;
	if ((*argv)[pid] == F77_FLAG)
		IX[15] = AMXF = F77_FLAG;
# endif

	/* open msg file */
	if ((FILE_msg = open(AAztack(AApath, IX), 0)) <= 0)
	{
		printf("cannot open file %s\n", IX);
		exit(1);
	}
# endif

	if (argc < 2)
	{
		amxerror(182);
		return;
	}

	Argv = argv;
	Argc = 0;
	if (!(Xargv = talloc((argc + 1) * sizeof (char *))))
		yyexit(194, 1);
	Xargc = 0;

	/*
	** setup temp file names:
	**	MSDOS '###pid##'
	**	else  '/tmp/#.#pid#'
	*/
	AA_itoa(getpid(), &Filename[Filepos + 2]);
	main1(argc, argv, env);
}

static int	main1(argc, argv, env)
int	argc;
char	**argv;
char	**env;
{
	register AMX_REL 	*r;
	register int		pid;
	extern char		**talloc();
	extern char		*AAztack();

# ifdef MEM_S
	if (**argv == SEG_FLAG)
	{
		AMX = SAMX;
		CC = SCC;
		LIBA = SLIBA;
	}
# endif

# ifdef MEM_O
	if (**argv == OVL_FLAG)
	{
		AMX = OAMX;
		CC = OCC;
	}
# endif

	while (get_next_file())
	{
		AAusercode = (char *) 0;
		Line = 1;
		Newline = 1;
		Xlocks = DB_SHARE;
		No_locks = 0;
		Relcount = 0;
		Xbufs = 0;
		Cvarindex = 0;
		AP_stmt = 0;
# ifdef AMX_SC
		ExpImp = 0;
# endif

		pid = parse();
		Line = 0;
		if (pid)
		{
			Xerrors++;
			amxerror(186);
			unlink(Argv[Argc]);
			Argv[Argc] = DUMMY;
		}
		free_sym();
		init_ctbl();
	}

	if (Xerrors)
		yyexit(192);

# ifndef MSDOS
	if (A_out
# ifdef MEM_O
		&& **argv != OVL_FLAG
# endif
	)
	{
		if (!(Argv = talloc((argc +
# ifdef F77
			((AMXF == F77_FLAG)? 5: 2)
# else
			2
# endif
			) * sizeof (char *))))
			yyexit(194, 2);
		AAbmove(argv, Argv, argc * sizeof (char *));
		Argv[argc++] = AAztack(AApath, LIBA);
# ifdef F77
		if (AMXF == F77_FLAG)
		{
			Argv[argc++] = LIB_U77;
			Argv[argc++] = LIB_F77;
			Argv[argc++] = LIB_I77;
		}
# endif
		Argv[argc] = (char *) 0;
	}

	fflush(stdout);
	if ((pid = fork()) < 0)
		yyexit(135, CC);
	if (!pid)
	{
		Argv[0] = CC;
		execvp(CC, Argv);
		yyexit(103, CC);
	}
	my_wait(pid);
}

static int	my_wait(pid)
register int	pid;
{
	int			stat;

	signal(2, (int (*)()) 1);
	while (wait(&stat) != pid)
		continue;

	if (stat)
		yyexit(136, CC);

	if (A_out && chmod(A_out, AMX_MODE))
		yyexit(191, A_out, AMX_MODE);
# endif

	end_amx(0);
}
