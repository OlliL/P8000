# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/ing_db.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

main()
{
	register struct relation	*d;
	register int			i;
	struct relation			desc;
	char				buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct relation [h/ing_db.h]\n");
	d = &desc;
	off(d->relid);
	out("relid (char [MAXNAME])");
	off(d->relowner);
	out("relowner (char [2])");
	off(&d->relspec);
	out("relspec (char)");
	off(&d->relindxd);
	out("relindxd (char)");
# ifndef BIT_32
	off(&d->relstat);
	out("relstat (short)");
# endif
	off(&d->relsave);
	out("relsave (long)");
	off(&d->reltups);
	out("reltups (long)");
	off(&d->relatts);
	out("relatts (short)");
	off(&d->relwid);
	out("relwid (short)");
	off(&d->relprim);
	out("relprim (long)");
	off(&d->relspare);
	out("relspare (long)");
# ifdef BIT_32
	off(&d->relstat);
	out("relstat (short)");
# endif
	fflush(stdout);
}
