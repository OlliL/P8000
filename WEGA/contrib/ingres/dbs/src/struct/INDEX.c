# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/ing_db.h"
# include	"../h/catalog.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

main()
{
	register struct index		*d;
	register int			i;
	struct index			desc;
	char				buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct index [h/catalog.h]\n");
	d = &desc;
	off(d->irelidp);
	out("irelidp (char [MAXNAME])");
	off(d->iownerp);
	out("iownerp (char [2])");
	off(d->irelidi);
	out("irelidi (char [MAXNAME])");
	off(&d->irelspeci);
	out("irelspeci (char)");
	off(d->idom);
	out("idom (char [MAXKEYS])");
	fflush(stdout);
}
