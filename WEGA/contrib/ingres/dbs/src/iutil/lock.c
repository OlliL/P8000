# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"
# include	"../h/lock.h"

int		AAcclock;	/* locks enabled flag			*/
LOCKREQ		AALOCKREQ = {-1L, -1L, -1L};

AAsyslock(act, mode, rel, page)
register int	act;
register int	mode;
long		rel;
long		page;
{
	extern int		errno;

# ifdef xATR1
	if (AAtTf(98, 0))
		printf("##%4d->A_%d M_%03o rl %8lx pg %8lx\n",
			AALOCKREQ.l_pid, act, mode, rel, page);
# endif

	if (AAlockdes < 0)
		act = 1;
	else
	{
		AALOCKREQ.l_rel = rel;
		AALOCKREQ.l_page = page;
		AALOCKREQ.l_act = act;
		AALOCKREQ.l_mode = mode;
		errno = 0;
		act = write(AAlockdes, &AALOCKREQ, DBS_WSIZE);
	}

# ifdef xATR1
	AAtTfp(98, 0, "##%4d<-%2d|%2d\n", AALOCKREQ.l_pid, act, errno);
# endif

	return (act);
}


/* release all locks for this pid */
AAunlall()
{
	AAcclock = TRUE;		/* reset page lock flag */
	AApageflush((ACCBUF *) 0);
	return (AAsyslock(A_RLSA, M_DB, 0L, 0L));
}


/* request page lock for this pid */
AAsetpgl(b)
register ACCBUF		*b;
{
	register int		i;

	i = AAsyslock(A_SLP, M_REL | M_EXCL, b->rel_tupid, b->thispage);
	b->bufstatus |= BUF_LOCKED;
	return (i);
}


/* release page lock for this pid */
AAunlpg(b)
register ACCBUF		*b;
{
	register int		i;

	i = AAsyslock(A_RLS1, M_REL, b->rel_tupid, b->thispage);
	b->bufstatus &= ~BUF_LOCKED;
	return (i);
}
