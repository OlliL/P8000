# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/lock.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

main()
{
	register LOCKREQ	*d;
	register int		i;
	LOCKREQ			desc;
	char			buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct lockreq   ==   LOCKREQ [h/lock.h]\n");
	d = &desc;
	off(&d->l_db);
	out("l_db (long)");
	off(&d->l_rel);
	out("l_rel (long)");
	off(&d->l_page);
	out("l_page (long)");
	off(&d->l_act);
	out("l_act (char)");
	off(&d->l_mode);
	out("l_mode (char)");
	off(&d->l_pid);
	out("l_pid (short)");
	fflush(stdout);
}
