# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"
# include	"../h/aux.h"
# include	"../h/lock.h"
# include	"../h/bs.h"

/* access method buffers and					*/
/* other data areas for buffer maintenance			*/
static ACCBUF	AAcc_buf[NACCBUFS];	/* the buffers		*/
ACCBUF		*AAcc_head;		/* head of usage list	*/
ACCBUF		*AAcc_tail;		/* tail of usage list	*/
LOCKREQ		AALOCKREQ;

/* structs for AA_ADMIN file data				*/
struct admin	AAdmin;

/* global flag indicating if access methods			*/
/* have been initialized.					*/
int	AAcc_init =	FALSE;

/* dummy procedure for importing AAdmin via ranlib(1)		*/
AACCESS() {};

/* Flush the indicated page and reset all			*/
/* important information including the name			*/
AAresetacc(b)
register ACCBUF		*b;
{
	register int		i;

	if (!b)
		b = AAcc_head;

# ifdef xATR3
	if (AAtTfp(80, 0, "RESETACC: "))
		AAdumpbuf(b);
# endif

	i = AApageflush(b);	/* write the page if necessary */
	b->rel_tupid = -1;
	b->filedesc = -1;
	b->thispage = -1;
	b->bufstatus = 0;
	return (i);
}


/* initialize access method data areas	*/
AAam_start()
{
	register ACCBUF		*last;
	register ACCBUF		*b;
	struct stat		stbuf;
	extern char		*AAdbpath;
	extern int		errno;

# ifdef xATR3
	AAtTfp(81, 0, "ACC_INIT=%d\n", AAcc_init);
# endif

	if (AAcc_init)
		return;		/* already initialized */

	last = (ACCBUF *) 0;
	for (b = AAcc_buf; b < &AAcc_buf[NACCBUFS]; )
	{
		AAresetacc(b);
		b->modb = last;
		last = b;
		b++;
		last->modf = b;
	}
	last->modf = (ACCBUF *) 0;
	AAcc_head = AAcc_buf;
	AAcc_tail = last;

	/* get the AA_ADMIN file */
	AAreadadmin();

	/* Set up locking information. If the database has concurrency */
	/* control then the concurrency device will */
	/* be opened for writing. If there is no concurrency for the */
	/* data base or if the lock device isn't installed, then AAlockdes */
	/* = -1 and no locking will (or can) occure. */
	if ((AAdmin.adhdr.adflags & A_DBCONCUR) && AAlockdes < 0)
		AAlockdes = open(AA_DBS_LOCK, 1);
	errno = 0;	/* clear in case AA_DBS_LOCK isn't available */
	AAcclock = TRUE;
	stat(AAdbpath? AAdbpath: ".", &stbuf);
	AAbmove(&stbuf, &AALOCKREQ.l_db, sizeof (long));
	AALOCKREQ.l_pid = getpid();

	AAcc_init = TRUE;
}


/* place buffer at top of LRU list */
AAtop_acc(b)
register ACCBUF		*b;
{
	if (b == AAcc_head)
		return;

	if (b == AAcc_tail)
		AAcc_tail = b->modb;
	else
		b->modf->modb = b->modb;
	b->modb->modf = b->modf;
	AAcc_head->modb = b;
	b->modf = AAcc_head;
	AAcc_head = b;
	b->modb = (ACCBUF *) 0;

# ifdef xATR3
	if (AAtTfp(80, 0, "TOP_ACC: "))
		AAdumpbuf(b);
# endif
}


/* AAam_flush_rel -- flush all pages associated with the relation	*/
/* described by the descriptor. If resetflag is TRUE,		*/
/* then the buffers are reset so the pages will not be		*/
/* found on subsequent calls to AAfind_page().			*/
/* Returns "or'ed" result from calls to AApageflush.		*/
AAam_flush_rel(d, resetflag)
register DESC		*d;
int			resetflag;
{
	register ACCBUF			*b;
	register int			i;

	if (d->reldum.relock[0] & R_LOCKED)
		AAunlrl(d->reltid);
	for (i = 0, b = AAcc_head; b; b = b->modf)
		if (d->reltid == b->rel_tupid)
			i |= resetflag? AAresetacc(b): AApageflush(b);

	return (i);
}


/* AAchoose_buf -- Try to find an empty buffer for assignment.		*/
/* 	If there is no empty buffer, pick the last buffer		*/
/* 	in the LRU queue and make sure it is flushed.			*/
/* 	AAchoose_buf guarantees that the buffer will be reset		*/
/* 	if it was used previously for a different relation.		*/
/*									*/
/* AAchoose_buf -- choose a buffer for use with the given relation on	*/
/*	the given page. If a relation does not have a buffer, it	*/
/*	is given a free one (if any) or else the Least Recently Used.	*/
ACCBUF	*AAchoose_buf(d, pageid)
register DESC	*d;
long		pageid;
{
	register ACCBUF			*b;
	register ACCBUF			*free;
# ifdef xATR3
	register ACCBUF			*mine;

	mine = (ACCBUF *) 0;
# endif
	for (free = (ACCBUF *) 0, b = AAcc_head; b; b = b->modf)
	{
		if (b->rel_tupid == -1)
			free = b;
		else if (d->reltid == b->rel_tupid)
		{
			if (pageid == b->thispage)
			{
				if (d->relopn < 0)
					b->filedesc = d->relfp;
				return (b);
			}
# ifdef xATR3
			mine = b;
# endif
		}
	}

# ifdef xATR3
	/* "Free" and "Mine" now reflect the current state of the buffers. */
	/* There is no buffer with the currently requested page */
	AAtTfp(82, 9, "CHOOSEBUF free 0%o,mine 0%o\n", free, mine);
# endif

	/* no current buffer. Choose a free one or LRU */
	if (!free)	/* error if can't reset the LRU */
		free = AAresetacc(AAcc_tail)? (ACCBUF *) 0: AAcc_tail;
	if (free)
	{
		/* copy relevent material (in this order in case of rubout */
		free->filedesc = d->relfp;
		free->rel_tupid = d->reltid;
	}

# ifdef xATR1
	AAtTfp(82, 8, "CHOOSEBUF: rets 0%o\n", free);
# endif

	return (free);
}


/* AAam_exit -- flush any buffers left around				*/
/* 	and then close the files for relation & attribute.		*/
/* 	The relation and attribute relation are normally left open	*/
/* 	until the end of an DBS session but must be closed		*/
/* 	and re-opened in the dbu's whenever a new overlay is loaded.	*/
AAam_exit()
{
	register int	i;

	if (i = AApageflush((ACCBUF *) 0))
		AAsyserr(10000, i);
	close(AAdmin.adreld.relfp);
	close(AAdmin.adattd.relfp);
	AAdmin.adreld.relopn = AAdmin.adattd.relopn = 0;
	if (AAlockdes >= 0)
		close(AAlockdes);
	AAlockdes = -1;
	AAcc_init = FALSE;
	sync();
	return (0);
}


AAdumpbuf(buf)
register ACCBUF	*buf;
{
	printf("page# %ld, bufstatus 0%o, filedesc %u, rel_",
		buf->thispage, buf->bufstatus, buf->filedesc);
	AAdumptid(&buf->rel_tupid);
	printf("mainpg %ld, ovflpg %ld, nxtlino 0%o, free 0%o\n",
		buf->mainpg, buf->ovflopg, buf->nxtlino,
		buf->linetab[LINETAB(buf->nxtlino)]);
	printf("(HEAD 0%o) 0%o <- 0%o -> 0%o (0%o TAIL)\n",
		AAcc_head, buf->modb, buf, buf->modf, AAcc_tail);
}
