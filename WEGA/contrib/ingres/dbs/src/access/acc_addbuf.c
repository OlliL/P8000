# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"

AAam_addbuf(b, cnt)
register ACCBUF	*b;
register int	cnt;
{
	register ACCBUF		*end;

	AAam_start();

	for (end = &b[cnt - 1]; b <= end; b++)
	{
		b->bufstatus = 0;
		AAresetacc(b);
		AAcc_tail->modf = b;
		b->modb = AAcc_tail;
		b->modf = (ACCBUF *) 0;
		AAcc_tail = b;
	}
}
