# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/ing_db.h"
# include	"../h/aux.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

main()
{
	register struct desxx		*d;
	register int			i;
	struct desxx			desc;
	char				buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct desxx [h/aux.h]\n");
	d = &desc;
	off(&d->cach_relname);
	out("cach_relname (char *)");
	off(&d->cach_desc);
	out("cach_desc (DESC *)");
	off(&d->cach_alias);
	out("cach_alias (DESC *)");
	fflush(stdout);
}
