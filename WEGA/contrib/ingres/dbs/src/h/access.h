/* ACCESS.H -- definitions relating to the access methods. */

# ifndef PGSIZE

# define	BLKSIZ		512	/* never change               */
# define	PGHDR		12	/* size of long    mainpg     */
					/*	 + long    ovflopg    */
					/*	 + short   nxtlino    */
					/*	 + short   linetab[0] */
# define	PGSIZE	(PGBLKS * BLKSIZ)	/* size of page       */

# if PGBLKS == 1
# define	LINETAB(x)	-x
# define	AApgsize	PGSIZE
# define	AAmaxtup	MINTUP
# else
# define	VPGSIZE			/* variable PGSIZE            */
# define	LINETAB(x)	-(AAltsize + x)
extern int	AApgsize;		/* actual page size           */
extern int	AAmaxtup;		/* actual max. tuple width    */
extern int	AAltsize;		/* actual linetab size        */
# endif

/*define	PGPTRSIZ	4	** size of a tid              */
# define	MAXTID		253	/* max. tuples per pages      */
# define	MODTID		254	/* impossible TID for modify  */
# define	SCANTID		255	/* TID for start of scan      */

/* storage structure flags; < 0 means compressed */
# define	M_HEAP		5	/* paged heap */
# define	M_ISAM		11	/* indexed sequential */
# define	M_HASH		21	/* random hash */
/*define	M_BTREE		31	** BTREES */
# define	M_TRUNC		99	/* internal pseudo-mode: truncated */

# define	NACCBUFS	3	/* number of access method buffers */

/* error flags */
# define	AMREAD_ERR	-1
# define	AMWRITE_ERR	-2
# define	AMNOFILE_ERR	-3	/* can't open file for a relation */
/* define	AMREL_ERR	-4	** can't open relation relation */
/* define	AMATTR_ERR	-5	** can't open attribute relation */
# define	AMNOATTS_ERR	-6	/* attribute missing or xtra in att-rel */
# define	AMCLOSE_ERR	-7	/* can't close relation */
# define	AMFIND_ERR	-8	/* unidentifiable storage structure in find */
# define	AMINVL_ERR	-9	/* invalid TID */
# define	AMOPNVIEW_ERR	-10	/* attempt to open a view for rd or wr */
# define	AMSEEK_ERR	-11
# define	AMTA_ERR	-12

/* the following is the access methods buffer */
struct accbuf
{
	/* this stuff is actually stored in the relation */
	long		mainpg;		/* next main page (0 - eof) */
	long		ovflopg;	/* next ovflo page (0 - none) */
	short		nxtlino;	/* next avail line no for this page */
	char		firstup[PGSIZE - PGHDR];	/* tuple space */
	short		linetab[1];	/* line table at end of buffer - grows down */
					/* linetab[lineno] is offset into
					** the buffer for that line; linetab[nxtlino]
					** is free space pointer */

	/* this stuff is not stored in the relation */
	long		rel_tupid;	/* unique relation id */
	long		thispage;	/* page number of the current page */
	int		filedesc;	/* file descriptor for this reln */
	struct accbuf	*modf;		/* use time link list forward pointer */
	struct accbuf	*modb;		/* back pointer */
	int		bufstatus;	/* various bits defined below */
};
typedef struct accbuf	ACCBUF;

/* The following assignments are status bits for accbuf.bufstatus */
# define	BUF_LOCKED	003	/* page has a page lock on it	   */
/*define	P_SHR		001		--> shared lock		   */
/*define	P_EXCL		002		--> exclusive lock	   */
# define	BUF_DIRECT	004	/* this is a page from isam direct */
# define	BUF_DIRTY	010	/* page has been changed	   */

/* access method buffer typed differently for various internal operations */
struct _acc_buf_
{
	char	acc_buf[PGSIZE];
};
typedef struct _acc_buf_	ACC_BUF;

/* pointers to maintain the buffer */
extern struct accbuf	*AAcc_head;	/* head of the LRU list */
extern struct accbuf	*AAcc_tail;	/* tail of the LRU list */


/*
**  ADMIN file struct
**
**	The ADMIN struct describes the initial part of the ADMIN file
**	which exists in each database.  This file is used to initially
**	create the database, to maintain some information about the
**	database, and to access the RELATION and ATTRIBUTE relations
**	on OPENR calls.
*/

struct adminhdr
{
	char	adowner[2];	/* user code of data base owner */
	short	adflags;	/* database flags */
};

struct admin
{
	struct adminhdr		adhdr;
	struct descriptor	adreld;
	struct descriptor	adattd;
};

/*
**  AAdmin status bits
**
**	These bits define the status of the database.  They are
**	contained in the adflags field of the admin struct.
*/

# define	A_DBCONCUR	0000001		/* set database concurrency */
# define	A_QRYMOD	0000002		/* database uses query modification */
# define	A_NEWFMT	0000004		/* database is post-6.3 */


/* following is buffer space for data from AA_ADMIN file */
extern struct admin		AAdmin;

/*
** global counters for the number of BS read and write
**  requests issued by the access methods.
*/

extern long	AAccuread;
extern long	AAccuwrite;

/*
**	Global values used by everything
*/

extern char	*AAcctuple;		/* pointer to canonical tuple */
extern int	AAccerror;		/* error no for fatal errors */

# ifdef ESER_PSU
# define	MERGESIZE	6
# define	COREBUFSIZE	( 31 * 1024)
# else
# define	MERGESIZE	16
# ifdef MEM_E
# define	COREBUFSIZE	(100 * 1024)
# else
# define	COREBUFSIZE	( 60 * 1024)
# endif
# endif
# define	TUP_BUF_SIZ	(PGSIZE + MAXTUP + sizeof (long))

struct tup_hdr
{
	char	*tup_ptr;
	char	*tup_end;
	int	tup_fd;
	int	tup_no;
	int	tup_len;
	char	tup_buf[TUP_BUF_SIZ];
};

extern char	Tup_buf[];
extern char	*Tup_ptr;
extern short	Tup_size;
extern int	Tup_fd;

# endif
