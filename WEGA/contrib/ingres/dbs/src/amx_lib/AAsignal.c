# include	<signal.h>
# include	"AA_liba.h"

/* C library functions */
# ifdef MSDOS
# define	NO_KILL
# else
extern int	(*signal())();
# endif
extern long	time();

/* user signal catch */
static int	(*AAsigint)();
# ifndef MSDOS
static int	(*AAsigquit)();
static int	(*AAsigterm)();
# endif

# ifndef NO_KILL
static int	AAcntint;
static int	AAcntquit;
static int	AAcntterm;
# endif

/* the update daemon for long transactions */
# define	AADAEMON	30	/* seconds */

static char	AAINTERRUPT[] = "database in progress (%s)\n";


/* AMX interrupt signal catch */
AA_int()
{
	signal(SIGINT, (int (*)()) 1);
# ifndef NO_KILL
	AAcntint++;
# endif
	printf(AAINTERRUPT, "INT");
}

# ifndef MSDOS
/* AMX quit signal catch */
AA_quit()
{
	signal(SIGQUIT, (int (*)()) 1);
# ifndef NO_KILL
	AAcntquit++;
# endif
	printf(AAINTERRUPT, "QUIT");
}

/* AMX software termination signal catch */
AA_term()
{
	signal(SIGTERM, (int (*)()) 1);
# ifndef NO_KILL
	AAcntterm++;
# endif
	printf(AAINTERRUPT, "TERM");
}
# endif

AA_on()
{
	AAsigint = signal(SIGINT, AA_int);
# ifndef MSDOS
	AAsigquit = signal(SIGQUIT, AA_quit);
	AAsigterm = signal(SIGTERM, AA_term);
# endif

	if (!AAdaemon)
		time(&AAdaemon);

# ifdef AMX_TRACE
	AAtTfp(16, 0, "daemon on\t%ld\n", AAdaemon);
# endif
}

AA_off()
{
	register DESCXX 	*d;
	register int		i;

	i = time((char *) 0) - AAdaemon;

# ifdef AMX_TRACE
	AAtTfp(17, 0, "daemon off\t%d\n", i);
# endif

	if (i > AADAEMON)
	{
# ifdef AMX_TRACE
		AAtTfp(17, 1, "flush bufs\n");
# endif
		/* flush all pages */
		AApageflush((ACCBUF *) 0);

		d = AA_DESC;

		/* flush catalog informations */
		while (d = d->xx_next)
		{
			if (d->xx_desc.reladds)
				AAnoclose(d);
		}

		sync();
		AAdaemon = 0L;
	}

	signal(SIGINT, AAsigint);
# ifndef NO_KILL
	if (AAcntint)
	{
		AAcntint = 0;
		kill(SIGINT, 0);
	}
# endif

# ifndef MSDOS
	signal(SIGQUIT, AAsigquit);
# ifndef NO_KILL
	if (AAcntquit)
	{
		AAcntquit = 0;
		kill(SIGQUIT, 0);
	}
# endif

	signal(SIGTERM, AAsigterm);
# ifndef NO_KILL
	if (AAcntterm)
	{
		AAcntterm = 0;
		kill(SIGTERM, 0);
	}
# endif
# endif
}
