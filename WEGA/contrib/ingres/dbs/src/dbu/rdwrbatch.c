# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/symbol.h"
# include	"../h/access.h"
# include	"../h/batch.h"

int		Batch_fp;	/* file descriptor for batch file            */
int		Batch_cnt;	/* no of bytes taken from the current buffer */
int		Batch_dirty;	/* used during update to mark a dirty page   */
static int	Batch_lread;	/* number of bytes last read in readbatch()  */

getbatch(loc, count)
char	*loc;
int	count;
{
	register char	*c;
	register int	cnt;
	register int	size;
	int		i;

	cnt = count;
#	ifdef xZTR1
	AAtTfp(15, 15, "GETBATCH: %d (%d)\n", cnt, Batch_cnt);
#	endif
	c = loc;

	while (cnt)
	{
		/* see if there is anything in the buffer */
		if (Batch_cnt == BATCHSIZE)
			if ((i = readbatch()) < cnt)
				AAsyserr(11121, i, cnt);
		if (cnt <= BATCHSIZE - Batch_cnt)
			size = cnt;
		else
			size = BATCHSIZE - Batch_cnt;
		AAbmove(&Batchbuf.bbuf[Batch_cnt], c, size);
		Batch_cnt += size;
		cnt -= size;
		c += size;
		/* flush the buffer if full */
		if (Batch_cnt == BATCHSIZE)
			batchflush();	/* re-write buffer if necessary */
	}
	return (0);
}
putbatch(cp, count)
char	*cp;
int	count;
{
	register char	*c;
	register int	size;
	register int	cnt;
	int		i;

	cnt = count;
	c = cp;
#	ifdef xZTR1
	AAtTfp(15, 2, "PUTBATCH: %d\n", cnt);
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
		{
			batchflush();
			/* is there is more to write, must read ahead first */
			if (cnt)
				if ((i = readbatch()) < cnt)
					AAsyserr(11122, i);
		}
	}
}


readbatch()
{

	if ((Batch_lread = read(Batch_fp, &Batchbuf, BATCHSIZE + IDSIZE)) < 0)
		AAsyserr(11123, Batch_lread, Batch_fp);
	Batch_cnt = 0;
#	ifdef xZTR1
	AAtTfp(15, 10, "read %d bytes from batch\n", Batch_lread);
#	endif
	/* check file-id */
	if (!AAsequal(Fileset, Batchbuf.file_id))
		AAsyserr(11124, Fileset, Batchbuf.file_id, Batch_lread);
	return (Batch_lread);
}


batchflush()
{
	register int	i;
	long		ltemp;
	extern long	lseek();

	if (Batch_cnt && Batch_dirty)
	{
#		ifdef xZTR1
		AAtTfp(15, 5, "BATCHFLUSH: backing up %d\n", Batch_lread);
#		endif
		ltemp = -Batch_lread;
		if (lseek(Batch_fp, ltemp, 1) < 0L)
			AAsyserr(11125, Batch_lread);
#		ifdef xZTR1
		AAtTfp(15, 4, "BATCHFLUSH: %d\n", Batch_cnt + IDSIZE);
#		endif
		if ((i = write(Batch_fp, &Batchbuf, Batch_cnt + IDSIZE)) != Batch_cnt + IDSIZE)
			AAsyserr(11126, i);
		Batch_dirty = FALSE;
	}
}
