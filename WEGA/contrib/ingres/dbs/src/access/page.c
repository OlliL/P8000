# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"
# include	"../h/aux.h"
# include	"../h/lock.h"

int	AAccerror;	/* error no for fatal errors		*/
long	AAccuread;	/*  read counter for    data pages	*/
long	AAccuwrite;	/* write counter for    data pages	*/
long	AAccusread;	/*  read counter for catalog pages	*/

AAgetpage(d, tid)
register DESC		*d;
struct tup_id		*tid;
{
	register int			i;
	register ACCBUF			*b;
	register ACCBUF			*b1;
	register int			lk;	/* lock condition*/
	long				pageid;
	extern long			lseek();
	extern ACCBUF			*AAchoose_buf();

#	ifdef xATR3
	if (AAtTfp(83, 0, "GET_PAGE: %.14s,", d->reldum.relid))
		AAdumptid(tid);
#	endif

	AApluck_page(tid, &pageid);
	if (!(b = AAchoose_buf(d, pageid)))
		return (-1);
	AAtop_acc(b);
	lk = AAcclock && (d->relopn < 0);
	if ((b->thispage != pageid) || (lk && !(b->bufstatus & BUF_LOCKED)))
	{
		if (i = AApageflush(b))
			return (i);
#		ifdef xATR1
		if (AAtTfp(83, 1, "GET_PAGE: read pg %ld rel", pageid))
			AAdumptid(&d->reltid);
#		endif
		b->thispage = pageid;
		if (lk)
		{
			for (b1 = AAcc_head; b1; b1 = b1->modf)
				if (b1->bufstatus & BUF_LOCKED)
					AApageflush(b1);
			if (AAsetpgl(b) < 0)
				AAsyserr(10018);
		}
#		ifdef xATM
		if (AAtTf(76, 1))
			timtrace(17, &pageid, &d->reltid);
#		endif
		if (lseek(d->relfp, pageid * AApgsize, 0) < 0L)
		{
			i = AMSEEK_ERR;
			goto error;
		}
		if (read(d->relfp, b, AApgsize) != AApgsize)
		{
			i = AMREAD_ERR;
error:
			AAresetacc(b);
			return (AAam_error(i));
		}
		AAccuread++;
		if (d->reldum.relstat & S_CATALOG)
			AAccusread++;
#		ifdef xATM
		if (AAtTf(76, 1))
			timtrace(18, &pageid, &d->reltid);
#		endif
	}

	return (0);
}


AApageflush(b)
register ACCBUF		*b;
{
	register ACCBUF		*allbufs;
	register int		err;
	extern long		lseek();

#	ifdef xATR3
	if (AAtTfp(84, 0, "PAGEFLUSH: buf 0%o rel", b))
		if (b)
			AAdumptid(&b->rel_tupid);
		else
			putchar('\n');
#	endif

	allbufs = b? (ACCBUF *) 0: (b = AAcc_head);
	err = 0;
	if (!b)
		return (err);

	do
	{
		if (b->bufstatus & BUF_DIRTY)
		{
#			ifdef xATR1
			if (AAtTfp(84, 1, "PAGEFLUSH: write pg %ld stat 0%o rel",
				b->thispage, b->bufstatus))
					AAdumptid(&b->rel_tupid);
#			endif

#			ifdef xATM
			if (AAtTf(76, 1))
				timtrace(19, &b->thispage, &b->rel_tupid);
#			endif

			/* temp debug step */
			if (b->mainpg > 32767)
			{
				/* "PAGEFLUSH: mainpg %ld" */
				AAsyserr(10019, b->mainpg, b->rel_tupid, b->thispage);
			}
			b->bufstatus &= ~BUF_DIRTY;
			if (lseek(b->filedesc, b->thispage * AApgsize, 0) < 0L)
			{
				err = AMSEEK_ERR;
				goto error;
			}
			if (write(b->filedesc, b, AApgsize) != AApgsize)
			{
				err = AMWRITE_ERR;
error:
				AAresetacc(b);
			}
			AAccuwrite++;

#			ifdef xATM
			if (AAtTf(76, 1))
				timtrace(20, &b->thispage, &b->rel_tupid);
#			endif
		}
		if (b->bufstatus & BUF_LOCKED)
			AAunlpg(b);

	} while (allbufs && (b = b->modf));

	return (AAam_error(err));
}


/*
**	ACC_ERR -- set global error indicator "AAccerror"
*/
AAam_error(errnum)
register int	errnum;
{
	AAccerror = errnum;
#	ifdef xATR1
	if (errnum)
		AAtTfp(85, 0, "ACC_ERR: %d\n", AAccerror);
#	endif
	return (errnum);
}
