# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"
# include	"../h/bs.h"

/*
**	AAlast_page -- computes a tid for the last page in the relation.
*/
AAlast_page(d, tid, buf)
register DESC			*d;
register struct tup_id		*tid;
register ACCBUF			*buf;
{
	long				lpage;
	extern long			lseek();

	if (buf && (abs(ctoi(d->reldum.relspec)) == M_HEAP) && !buf->mainpg && !buf->ovflopg)
		lpage = buf->thispage;
	else
	{
		if ((lpage = lseek(d->relfp, 0L, 2)) < 0)
			AAsyserr(10013, d->reldum.relid);
		lpage = lpage / AApgsize - 1;
#		ifdef xATR2
		AAtTfp(86, 2, "LAST_PAGE: %.12s %ld\n", d->reldum.relid, lpage);
#		endif
	}
	AAstuff_page(tid, &lpage);
	tid->line_id = 0;
	return (0);
}
