# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/ing_db.h"
# include	"../h/catalog.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

main()
{
	register struct refs		*d;
	register int			i;
	struct refs			desc;
	char				buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct refs [h/catalog.h]\n");
	d = &desc;
	off(d->refrelid);
	out("refrelid (char [MAXNAME])");
	off(d->refowner);
	out("refowner (char [2])");
	off(d->refbrelid);
	out("refbrelid (char [MAXNAME])");
	off(d->refbowner);
	out("refbowner (char [2])");
	off(&d->reftype);
	out("reftype (char)");
	fflush(stdout);
}
