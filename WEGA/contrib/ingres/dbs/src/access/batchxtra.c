# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/symbol.h"
# include	"../h/access.h"
# include	"../h/batch.h"

char	SYSBATCH[]	= "_SYSbatch";
int	s_SYSBATCH	= sizeof SYSBATCH - 1;

rmbatch()
{
	register char	*p;
	extern char	*batchname();

	p = batchname();

	if (close(Batch_fp))
		AAsyserr(10007, p);

	if (unlink(p))
		AAsyserr(10008, p);

	Batchhd.mode_up = 0;
	sync();
}


char	*batchname()
{
	extern char	*Fileset;
	extern char	*AAztack();

	return (AAztack(SYSBATCH, Fileset));
}
