# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/ing_db.h"
# include	"../h/aux.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

main()
{
	register struct accessparam	*d;
	register int			i;
	struct accessparam		desc;
	char				buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct accessparam [h/aux.h]\n");
	d = &desc;
	off(&d->mode);
	out("mode (short)");
	off(&d->sec_index);
	out("sec_index (short)");
	off(d->keydno);
	out("keydno (char [MAXDOM + 1])");
	fflush(stdout);
}
