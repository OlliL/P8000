# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/ing_db.h"
# include	"../h/access.h"
# include	"../h/batch.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

main()
{
	register struct si_doms		*d;
	register int			i;
	struct si_doms			desc;
	char				buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct si_doms [h/batch.h]\n");
	d = &desc;
	off(&d->rel_off);
	out("rel_off (int)");
	off(&d->tupo_off);
	out("tupo_off (int)");
	off(&d->dom_size);
	out("dom_size (int)");
	fflush(stdout);
}
