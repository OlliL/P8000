# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"
# include	"../h/batch.h"

struct batchbuf	Batchbuf;

extern	modify();
extern	create();
extern	destroy();
extern	resetrel();

int	(*Func[])() =
{
	modify,
	create,
	destroy,
	resetrel,
};
