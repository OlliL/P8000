# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/ing_db.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

main()
{
	register struct attribute	*d;
	register int			i;
	struct attribute		desc;
	char				buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct attribute [h/ing_db.h]\n");
	d = &desc;
	off(d->attrelid);
	out("attrelid (char [MAXNAME])");
	off(d->attowner);
	out("attowner (char [2])");
	off(d->attname);
	out("attname (char [MAXNAME])");
	off(&d->attid);
	out("attid (short)");
	off(&d->attoff);
	out("attoff (short)");
	off(&d->attfrmt);
	out("attfrmt (char)");
	off(&d->attfrml);
	out("attfrml (char)");
	off(&d->attxtra);
	out("attxtra (char)");
	fflush(stdout);
}
