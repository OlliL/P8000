# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/ing_db.h"
# include	"../h/aux.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

main()
{
	register struct retcode		*d;
	register int			i;
	struct retcode			desc;
	char				buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct retcode [h/aux.h]\n");
	d = &desc;
	off(&d->rc_tupcount);
	out("rc_tupcount (long)");
	off(&d->rc_status);
	out("rc_status (short)");
	off(d->rc_siteid);
	out("rc_siteid (char [2])");
	fflush(stdout);
}
