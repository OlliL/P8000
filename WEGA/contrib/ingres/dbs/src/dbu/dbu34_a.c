# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"
# include	"../h/batch.h"

extern	create();
extern	destroy();
extern	rupdate();
extern	print();
extern	help();
extern	resetrel();

int		Batch_recovery;	/* TRUE is this is recovery, else FALSE */
struct batchhd	Batchhd;
struct batchbuf	Batchbuf;

int	(*Func[])() =
{
	create,
	destroy,
	rupdate,
	print,
	help,
	resetrel,
};
