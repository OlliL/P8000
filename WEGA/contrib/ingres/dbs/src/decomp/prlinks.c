# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"../h/pipes.h"
# include	"decomp.h"


prlinks(label, linkmap)
char	*label;
char	linkmap[];
{

	register char	*lm;
	register int	i;

	printf("%s: ", label);
	lm = linkmap;
	for (i = 0; i < MAXDOM; i++)
		if (*lm++)
			printf("dom %d,", i);
	putchar('\n');
}
