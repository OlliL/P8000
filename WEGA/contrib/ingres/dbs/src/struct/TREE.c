# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/ing_db.h"
# include	"../h/catalog.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

main()
{
	register struct tree	*d;
	register int		i;
	struct tree		desc;
	char			buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct tree [h/tree.h]\n");
	d = &desc;
	off(d->treerelid);
	out("treerelid (char [MAXNAME])");
	off(d->treeowner);
	out("treeowner (char [2])");
	off(&d->treeid);
	out("treeid (short)");
	off(&d->treeseq);
	out("treeseq (short)");
	off(&d->treetype);
	out("treetype (char)");
	off(&d->treexxxx);
	out("treexxxx (char)");
	off(d->treetree);
	out("treetree (char [100])");
	fflush(stdout);
}
