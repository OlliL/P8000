/************************************************************************/
/*									*/
/*		DBS  LOCK DEVICE SERVER					*/
/*		**************************				*/
/*		Release  6/3.3    20/03/89				*/
/*									*/
/************************************************************************/

# include	<stdio.h>
# include	<signal.h>
# include	"../conf/gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"
# include	"../h/lock.h"
# include	"../h/bs.h"

# ifndef major
# define	major(x)	((short) (((unsigned) (x) >> 8)))
# define	minor(x)	((short) ((x) & 0377))
# endif

# define	OBJS		(30 * LOCKS)
# define	REQS		(40 * LOCKS)
# define	KSIZE		12

# define	OK		0
# define	ERROR		1
# define	OBJ_OVFLO	2
# define	PROC_OVFLO	3
# define	REQ_OVFLO	4
# define	OPEN_ERROR	5
# define	RD_ERROR	6
# define	WR_ERROR	7
# define	FORK_ERROR	8

# define	CANT_LOCK	-1
# define	LOCK_NOT_SET	-2
# define	ILL_REQUEST	-3
# define	DRIVER_LOCKED	-4

# define	PROC_FREE	0
# define	REQ_FREE	((struct object *) 0)

# define	NO_PROC		((struct process *) 0)
# define	NO_REQ		((struct request *) 0)
# define	NO_OBJ		((struct object *) 0)

struct object
{
	long		o_db;		/* | KEY  | inode of data base	*/
	long		o_rel;		/* | FOR  | relation tid	*/
	long		o_page;		/* | LOCK | page address	*/
	char		o_reqs;		/* request reference count	*/
					/* free space if zero		*/
	char		o_locks;	/* lock reference count		*/
	char		o_tas;		/* TA reference count		*/
	char		o_mode;		/* mode of lock			*/
};

struct process
{
	struct request	*p_lock;	/* pointer to lock queue	*/
	struct request	*p_req;		/* pointer to request queue	*/
	struct process	*p_wait;	/* pointer to wait queue	*/
	short		p_pid;		/* requesting process id	*/
					/* free space if zero		*/
};

struct request
{
	struct request	*r_next;	/* next request in queue	*/
	struct object	*r_obj;		/* pointer to requested object	*/
					/* free space if zero		*/
	short		r_mode;		/* mode of lock			*/
};

LOCKREQ		AALOCKREQ		= { 0 } ;
int		AAlockdes		=  -1   ;
struct object	*Object			=   0   ;
struct process	*Process		=   0   ;
struct process	*Wait			=   0   ;
struct object	Obj[OBJS + 1]		= { 0 } ;
struct request	Req[REQS + 1]		= { 0 } ;
struct process	Proc[LOCKS]		= { 0 } ;
char		SERVER[]		= "DB.SERVER: ";


catch_signal(sig)
register int	sig;
{
	signal(sig, (int (*)()) 1);
	fflush(stdout);
	signal(sig, catch_signal);
}

main(argc, argv)
int	argc;
char	*argv[];
{
	register int			i;
	register LOCKREQ		*lb;
	register struct process		*p;
	char				stdbuf[BUFSIZ];
	extern int			errno;
	extern int			(*signal())();

# ifdef xSTR1
	AAtTrace(&argc, argv, 'X');
# endif

	if (argc > 1)
		pr_startup();

	errno = 0;
	if ((i = fork()) < 0)
	{
		printf("\n%sFORK ERROR(%d)", SERVER, errno);
		term(FORK_ERROR);
	}
	else if (i)	/* PARENT */
		term(OK);

	/*********************/
	/***		   ***/
	/***  +---------+  ***/
	/***  |  CHILD  |  ***/
	/***  +---------+  ***/
	/***		   ***/
	/***    lock(1)    ***/
	/***		   ***/
	/*********************/

# ifdef SETBUF
	setbuf(stdout, stdbuf);
# endif

	/* user signal catch */
	for (i = SIGHUP; i <= SIGTERM; i++)
		signal(i, catch_signal);

	for (i = MAXFILES - 1; i; --i)
		if (i != 1)
			close(i);

	/* open AA_DBS_LOCK */
	errno = 0;
	if ((AAlockdes = open(AA_DBS_LOCK, 2)) < 0)
	{
		printf("\n%sOPEN %s: ERROR(%d)\n", SERVER, AA_DBS_LOCK, errno);
		term(OPEN_ERROR);
	}

	lb = &AALOCKREQ;
	for ( ; ; )
	{
# ifdef xSTR1
		if (AAtTf(10, 0))
			pr_locks();
		AAtTfp(12, 0, "\n%s", SERVER);
# endif

		if ((i = read(AAlockdes, lb, DBS_RSIZE)) != DBS_RSIZE)
		{
			if (i < 0 && errno == DRIVER_LOCKED)
				term(OK);
			printf("\n%sREAD ERROR(%d) got %d\n", SERVER, errno, i);
			term(RD_ERROR);
		}

		if ((i = lb->l_act) == A_INFO)
		{
			pr_locks();
			continue;
		}

		find_process();

# ifdef xSTR1
		if (AAtTfp(11, 0, " A%d M%02o ", i, lb->l_mode))
		{
			if (i >= A_RLSA)
				putchar('\n');
			else
				pr_object(&AALOCKREQ);
		}
# endif

		if (i == A_RLSA)
		{
			lb->l_db = lb->l_rel = lb->l_page = 0L;
			rm_all_locks();
		}
		else if (i == A_RLS1)
			response(rm_one_lock());
		else
			enter_lock();
		if (!(p = Process)->p_lock && !p->p_req)
			p->p_pid = PROC_FREE;
		relock();
	}
}


find_process()
{
	register struct process		*p;
	register short			pid;

	for (pid = AALOCKREQ.l_pid, p = &Proc[LOCKS - 1]; p >= Proc; --p)
		if (p->p_pid == pid)
			goto proc_found;
	for (p = &Proc[LOCKS - 1]; p >= Proc; --p)
		if (!p->p_pid)
		{
			p->p_lock = p->p_req = NO_REQ;
			p->p_wait = NO_PROC;
			p->p_pid = pid;
proc_found:
			Process = p;
# ifdef xSTR1
			AAtTfp(23, 0, "PID %5u", p->p_pid);
# endif
			return;
		}
	term(PROC_OVFLO);
}


rm_all_locks()
{
	register struct request		*r;
	register struct process		*p;

	for (r = (p = Process)->p_lock; r; r = r->r_next)
	{
		if (!(--(r->r_obj->o_locks)))
			r->r_obj->o_mode = 0;
		--(r->r_obj->o_reqs);
		r->r_obj = REQ_FREE;
	}
	for (r = p->p_req; r; r = r->r_next)
	{
		--(r->r_obj->o_reqs);
		if (r->r_mode & M_TA)
			--(r->r_obj->o_tas);
		r->r_obj = REQ_FREE;
	}
	p->p_pid = PROC_FREE;
}


rm_one_lock()
{
	register struct request		**rr;
	register struct request		*r;
	register LOCKREQ		*lb;

	lb = &AALOCKREQ;
	for (rr = &(Process->p_lock); r = *rr; rr = &r->r_next)
		if (AAbequal(lb, r->r_obj, KSIZE))
		{
			if (!(--(r->r_obj->o_locks)))
				r->r_obj->o_mode = 0;
			--(r->r_obj->o_reqs);
			r->r_obj = REQ_FREE;
			*rr = r->r_next;
			return (OK);
		}
	return (ERROR);
}


enter_lock()
{
	register struct process		*p;
	register LOCKREQ		*lb;
	register struct object		*o;
	extern struct object		*find_object();
	extern struct request		*enter_request();

	if ((lb = &AALOCKREQ)->l_mode & M_TA)
		enter_ta(1);
	else if (check_lock(find_object(), lb->l_mode))
	{
		p = Process;
		p->p_lock = enter_request(p->p_lock);
		if (!(((o = Object)->o_locks)++))
			o->o_mode = lb->l_mode & M_MODE;
		response(OK);
	}
	else if (lb->l_act == A_RTN)
		response(ERROR);
	else
	{
		lb->l_mode |= M_TE;
		enter_ta(0);
	}
}


relock()
{
	register struct process		*w;
	register struct request		*r;
	register int			i;
	register struct process		*lw;
	register struct request		*lr;

restart:
	for (lw = w = Wait; w; )
	{
		i = 0;
		for (r = w->p_req; r; r = r->r_next)
			if (!(i = check_lock(r->r_obj, r->r_mode)))
				break;
		if (i)
		{
			lr = w->p_lock;
			for (w->p_lock = r = w->p_req; ; r = r->r_next)
			{
				if (r->r_mode & M_TA)
				{
					r->r_mode &= ~M_TA;
					--(r->r_obj->o_tas);
				}
				if (!((r->r_obj->o_locks)++))
					r->r_obj->o_mode = r->r_mode & M_MODE;
				if (!r->r_next)
				{
					r->r_next = lr;
					break;
				}
			}
			w->p_req = NO_REQ;
			AALOCKREQ.l_pid = w->p_pid;
			response(OK);
			if (w == lw)
			{
				Wait = w->p_wait;
				w->p_wait = NO_PROC;
				goto restart;
			}
			else
			{
				lw->p_wait = w->p_wait;
				w->p_wait = NO_PROC;
				w = lw->p_wait;
			}
		}
		else
		{
			lw = w;
			w = w->p_wait;
		}
	}
}


check_lock(o, s)
register struct object	*o;
register int		s;
{
	return (!o || !o->o_locks || ((o->o_mode & M_SHARE) && (s & M_SHARE)));
}


enter_ta(find)
int	find;
{
	register struct request		**rr;
	register struct request		*r;
	register struct process		*w;
	long				rel_tid;
	extern struct object		*find_object();
	extern struct request		*enter_request();

	if (find)
		find_object();
	rel_tid = AALOCKREQ.l_rel;
	rr = &Process->p_req;
	while (r = *rr)
	{
		if (r->r_obj->o_rel > rel_tid)
			break;
		rr = &r->r_next;
	}
	*rr = enter_request(r);
	if (AALOCKREQ.l_mode & M_TA)
		(*rr)->r_obj->o_tas++;
	if (AALOCKREQ.l_mode & M_TE)
	{
		if (!(w = Wait))
			Wait = Process;
		else
		{
			while (w->p_wait)
				w = w->p_wait;
			w->p_wait = Process;
		}
	}
	else
		response(OK);
}


struct object	*find_object()
{
	register struct object		*o;
	register LOCKREQ		*lb;

	lb = &AALOCKREQ;
	for (o = &Obj[OBJS]; o >= Obj; --o)
		if (o->o_reqs && AAbequal(lb, o, KSIZE))
			return (Object = o);
	return (Object = NO_OBJ);
}


enter_object()
{
	register struct object		*o;

	if (o = Object)
	{
		(o->o_reqs)++;
		goto entered;
	}
	for (o = &Obj[OBJS]; o >= Obj; --o)
		if (!o->o_reqs)
		{
			AAbmove(&AALOCKREQ, o, KSIZE);
			o->o_reqs = 1;
			o->o_locks = o->o_tas = o->o_mode = 0;
			Object = o;
entered:
			return;
		}
	term(OBJ_OVFLO);
}


struct request	*enter_request(next)
register struct request		*next;
{
	register struct request		*r;

	enter_object();
	for (r = &Req[REQS]; r >= Req; --r)
		if (!r->r_obj)
		{
			r->r_next = next;
			r->r_obj = Object;
			r->r_mode = AALOCKREQ.l_mode;
			return (r);
		}
	term(REQ_OVFLO);
}


response(error)
register int			error;
{
	register int		i;
	register LOCKRES	*lr;
	LOCKRES			res_buf;
	extern int		errno;

	if (error < 0)
		return;
	(lr = &res_buf)->s_pid = AALOCKREQ.l_pid;
	lr->s_error = error;

# ifdef xSTR1
	AAtTfp(9, 0, "%s%s %5u\n", SERVER, error? "ERR": "RUN", lr->s_pid);
# endif

	if ((i = write(AAlockdes, lr, DBS_SSIZE)) != DBS_SSIZE)
	{
		printf("\n%sWRITE ERROR(%d) got %d\n", SERVER, errno, i);
		term(WR_ERROR);
	}
}


term(code)
register int	code;
{
	close(AAlockdes);
	fflush(stdout);
	exit(code);
}

pr_locks()
{
	register struct object		*o;
	register LOCKREQ		*lb;

	pr_process();
	lb = &AALOCKREQ;
	for (o = &Obj[OBJS]; o >= Obj; --o)
		if (o->o_reqs && (!lb->l_db || o->o_db == lb->l_db))
		{
			printf("L%5u   | %2dRQ %2dLK %2dTA ",
				o - Obj, o->o_reqs, o->o_locks, o->o_tas);
			pr_mode(o->o_mode);
			pr_object(o);
		}
	fflush(stdout);
}

pr_process()
{
	register struct process		*p;
	register struct request		*r;
	register LOCKREQ		*lb;

	lb = &AALOCKREQ;
	if (p = Wait)
	{
		printf("%sWAITQUEUE", SERVER);
		for ( ; p; p = p->p_wait)
			printf("-->%5u", p->p_pid);
		putchar('\n');
	}
	for (p = &Proc[LOCKS - 1]; p >= Proc; --p)
	{
		if (p->p_pid)
		{
			printf("P%5u:", p->p_pid);
			for (r = p->p_lock; r; r = r->r_next)
			{
				if (lb->l_db && r->r_obj->o_db != lb->l_db)
					continue;
				printf("\tL%4u", r->r_obj - Obj);
			}
			for (r = p->p_req; r; r = r->r_next)
			{
				if (lb->l_db && r->r_obj->o_db != lb->l_db)
					continue;
				printf("\tR%4u", r->r_obj - Obj);
			}
			putchar('\n');
		}
	}
	fflush(stdout);
}

pr_object(o)
register struct object	*o;
{
	register struct stat		*s;

	s = (struct stat *) &o->o_db;
	printf(" |%5d", major(s->st_dev));
	printf("  |%5d", minor(s->st_dev));
	printf("  |%6d | ", s->st_ino);
	pr_tid(&o->o_rel);
	printf(" |");
	if (o->o_page == REL_LOCK)
		printf("   REL");
	else if (o->o_page == USE_LOCK)
		printf("   USE");
	else if (o->o_page == CS_LOCK)
		printf("    CS");
	else
		printf("%6ld", o->o_page);
	putchar('\n');
	fflush(stdout);
}

pr_tid(tid)
register struct tup_id	*tid;
{
	register int			lineid;
	long				pageid;

	AApluck_page(tid, &pageid);
	if (pageid == 0xffffff)
		pageid = -1L;
	if ((lineid = ctou(tid->line_id)) == SCANTID)
		lineid = -1;
	printf("%3ld/%3d", pageid, lineid);
}

AApluck_page(tid, var)
register struct tup_id	*tid;
register struct lpage	*var;
{
	var->lpg0 = tid->pg0;
	var->lpg1 = tid->pg1;
	var->lpg2 = tid->pg2;
	var->lpgx = 0;
}

pr_mode(mode)
register int	mode;
{
	if (mode & M_REL)
		putchar('R');
	else if (mode & M_TA)
		putchar('A');
	else if (mode & M_TE)
		putchar('E');
	else
		putchar(' ');

	if (mode & M_EXCL)
		putchar('E');
	else if (mode & M_SHARE)
		putchar('S');
	else
		putchar(' ');
}

pr_startup()
{
	printf("\n%sWAITQUEUE-->pid-->pid...\n", SERVER);
	printf("Ppid: {Llock} {Rlock}\n");
	printf("Llock | nRQ nLK nTA mode | major | minor | inode | rel_tid | page\n");
	printf("\npid\tProzessnummer\n");
	printf("mode\tR-relation A-TA E-TE E-exclusive S-shared\n");
	printf("Llock\tbelegter Lock\n");
	printf("Rlock\tangeforderter Lock\n");
	printf("nRQ\tAnzahl der Verweise auf diesen Lock\n");
	printf("nLK\tAnzahl der Prozesse, die diesen Lock belegen\n");
	printf("nTA\tAnzahl der Transaktionen, die diesen Lock anfordern\n\n");
	fflush(stdout);
}
