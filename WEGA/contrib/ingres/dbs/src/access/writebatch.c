# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/symbol.h"
# include	"../h/access.h"
# include	"../h/batch.h"

wrbatch(c, cnt)
register char	*c;
register int	cnt;
{
	register int	size;

#	ifdef xATR1
	AAtTfp(89, 8, "WRBATCH: %d (%d)\n", cnt, Batch_cnt);
#	endif

	while (cnt)
	{
		Batch_dirty = TRUE;	/* mark this buffer as dirty */
		if (cnt + Batch_cnt > BATCHSIZE)
			size = BATCHSIZE - Batch_cnt;
		else
			size = cnt;
		AAbmove(c, &Batchbuf.bbuf[Batch_cnt], size);
		c += size;
		Batch_cnt += size;
		cnt -= size;
		if (Batch_cnt == BATCHSIZE)
			flushbatch();
	}
}


flushbatch()
{
	register int	i;

	if (Batch_cnt)
	{
#		ifdef xATR1
		AAtTfp(89, 9, "FLUSHBATCH: %d\n", Batch_cnt + IDSIZE);
#		endif
		if ((i = write(Batch_fp, &Batchbuf, Batch_cnt + IDSIZE)) != Batch_cnt + IDSIZE)
			AAsyserr(10027, i, Batch_cnt + IDSIZE);
		Batch_cnt = 0;
	}
}
