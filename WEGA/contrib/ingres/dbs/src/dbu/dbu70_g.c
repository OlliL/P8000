# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"
# include	"../h/batch.h"

extern int	create();
extern int	destroy();
extern int	rupdate();
extern int	print();
extern int	help();
extern int	resetrel();
extern int	copy();
extern int	save();
extern int	modify();
extern int	sec_index();
extern int	display();
extern int	dest_const();

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
	copy,
	save,
	modify,
	sec_index,
	display,
	dest_const,
};
