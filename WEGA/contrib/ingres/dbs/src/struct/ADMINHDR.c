# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/ing_db.h"
# include	"../h/access.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

main()
{
	register struct adminhdr	*d;
	register int			i;
	struct adminhdr			desc;
	char				buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct adminhdr [h/access.h]\n");
	d = &desc;
	off(d->adowner);
	out("adowner (char [2])");
	off(&d->adflags);
	out("adflags (short)");
	fflush(stdout);
}
