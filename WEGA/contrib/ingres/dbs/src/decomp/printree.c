# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/tree.h"
# include	"../h/pipes.h"
# include	"decomp.h"

static int		tr_level;

printree(p, string)
register QTREE		*p;
register char		*string;
{

	tr_level = 0;
	printf("%s: 0%o\n", string, p);
	prtree1(p);
}

prtree1(p)
register QTREE		 *p;
{
	register int		i;

	if (!p)
		return;

	tr_level++;
	prtree1(p->left);
	prtree1(p->right);
	printf("...%d\t", tr_level);
	writenod(p);
	--tr_level;
}
