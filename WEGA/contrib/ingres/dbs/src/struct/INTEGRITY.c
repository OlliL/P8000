# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/ing_db.h"
# include	"../h/catalog.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

main()
{
	register struct integrity	*d;
	register int			i;
	struct integrity		desc;
	char				buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct integrity [h/catalog.h]\n");
	d = &desc;
	off(d->intrelid);
	out("intrelid (char [MAXNAME])");
	off(d->intrelowner);
	out("intrelowner (char [2])");
# ifdef BIT_32
	off(&d->inttree);
	out("inttree (short)");
	off(d->intdomset);
	out("intdomset (short [8])");
# else
	off(d->intdomset);
	out("intdomset (short [8])");
	off(&d->inttree);
	out("inttree (short)");
# endif
	off(&d->intresvar);
	out("intresvar (char)");
	fflush(stdout);
}
