# define	DBS_RSIZE	(sizeof (LOCKREQ))
# define	DBS_WSIZE	14
# define	DBS_SSIZE	(sizeof (LOCKRES))

# define	DB_LOCK		 0L
# define	REL_LOCK	-1L
# define	USE_LOCK	-2L
# define	CS_LOCK		-3L

# define	NO_REL		-1L

/* data structure for AALOCKREQs and AALOCKREQ table */
struct lockres
{
	short	s_pid;
	short	s_error;
};
typedef struct lockres		LOCKRES;

struct lockreq
{
	long	l_db;		/* KEY  | inode of data base		*/
	long	l_rel;		/* FOR  | relation tid			*/
	long	l_page;		/* LOCK | page address			*/
	char	l_act;		/* REQUESTED ACTION			*/
# define	   A_SLP	   0	/* request lock, sleep		*/
# define	   A_RTN	   1	/* request lock, err return	*/
# define	   A_RLS1	   2	/* release one lock for pid	*/
# define	   A_RLSA	   3	/* release all locks for pid	*/
# define	   A_ABT	   4	/* abort lock device driver	*/
# define	   A_INFO	   5	/* get info about lock server	*/
	char	l_mode;		/* MODE of LOCK				*/
# define	   M_DB		0000	/* data base lock		*/
# define	   M_REL	0001	/* relation lock		*/
# define	   M_TA		0002	/* begin of transaction		*/
# define	   M_TE		0004	/*   end of transaction		*/
# define	   M_MODE	0030
# define	   M_EXCL	0010	/* exclusive mode		*/
# define	   M_SHARE	0020	/* shared mode			*/
# define	   M_ON		0040	/* processes are waiting	*/
# define	   M_ERROR	0100
# define	   M_ABT	0200
	short	l_pid;		/* requesting process id		*/
};

typedef struct lockreq		LOCKREQ;


/* request or release relation lock for this pid			*/
# define	AAsetrll(a, r, m)	AAsyslock(a, M_REL | (m), r, REL_LOCK)
# define	AAunlrl(r)		AAsyslock(A_RLS1, M_REL, r, REL_LOCK)

/* request or release relation in use lock for this pid			*/
# define	AAsetusl(a, r, m)	AAsyslock(a, M_REL | (m), r, USE_LOCK)
# define	AAunlus(r)		AAsyslock(A_RLS1, M_REL, r, USE_LOCK)

/* request or release critical section lock for this pid		*/
# define	AAsetcsl(r)		AAsyslock(A_SLP, M_REL | M_EXCL, r, CS_LOCK)
# define	AAunlcs(r)		AAsyslock(A_RLS1, M_REL, r, CS_LOCK)

/* request or release data base lock for this pid			*/
# define	AAsetdbl(a, m)		AAsyslock(a, M_DB | (m), NO_REL, DB_LOCK)
# define	AAunldb()		AAsyslock(A_RLS1, M_DB, NO_REL, DB_LOCK)


extern int	AAcclock;	/* locks enabled flag			*/
extern int	AAlockdes;	/* file descriptor for lock dev 	*/
extern LOCKREQ	AALOCKREQ;
