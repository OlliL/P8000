# define	IDSIZE		6	/* size of file id */

					/* available buffer space */
# define	BATCHSIZE	(PGSIZE - IDSIZE)

struct batchbuf
{
	char	file_id[IDSIZE];	/* unique file name identifier */
	char	bbuf[BATCHSIZE];	/* buffer for batch storage */
};


struct si_doms
{
	int	rel_off;	/* offset in primary tuple */
	int	tupo_off;	/* offset in saved tuple-old */
	int	dom_size;	/* width of the domain */
				/* if zero then domain not used */
};
struct batchhd
{
	char	db_name[MAXNAME + 1];	/* data base name */
	char	rel_name[MAXNAME + 1];	/* relation name */
	char	userid[2];		/* DBS user code */
	long	num_updts;	/* actual number of tuples to be updated */
	short	mode_up;		/* type of update */
	short	tido_size;		/* width of old_tuple_id field */
	short	tupo_size;		/* width of old tuple */
	short	tupn_size;		/* width of new tuple */
	short	tidn_size;		/* width of new_tuple_id field */
	short	si_dcount;	/* number of sec. index domains affected */
	struct si_doms	si[MAXDOM + 1];	/* entry for each domain with sec. index */
};



extern int	Batch_fp;	/* file descriptor for batch file */
extern int	Batch_cnt;	/* no of bytes taken from the current buffer */
extern int	Batch_dirty;	/* used during update to mark a dirty page */
extern int	Batch_lread;	/* number of bytes last read in readbatch() */
extern int	Batch_recovery;	/* TRUE is this is recovery, else FALSE */
extern struct batchbuf	Batchbuf;
extern struct batchhd	Batchhd;

extern char	*Fileset;	/* unique id of batch maker */
