# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/ing_db.h"
# include	"../h/catalog.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

main()
{
	register struct tup_id		*d;
	register int			i;
	struct tup_id			desc;
	char				buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct tup_id [h/ing_db.h]\n");
	d = &desc;
# ifdef REV_BYTES
# ifdef REV_WORDS
	off(&d->line_id);
	out("line_id (char)");
	off(&d->pg2);
	out("pg2 (char)");
	off(&d->pg1);
	out("pg1 (char)");
	off(&d->pg0);
	out("pg0 (char)");
# else
	off(&d->pg1);
	out("pg1 (char)");
	off(&d->pg0);
	out("pg0 (char)");
	off(&d->line_id);
	out("line_id (char)");
	off(&d->pg2);
	out("pg2 (char)");
# endif
# else
# ifdef REV_WORDS
	off(&d->pg2);
	out("pg2 (char)");
	off(&d->line_id);
	out("line_id (char)");
	off(&d->pg0);
	out("pg0 (char)");
	off(&d->pg1);
	out("pg1 (char)");
# else
	off(&d->pg0);
	out("pg0 (char)");
	off(&d->pg1);
	out("pg1 (char)");
	off(&d->pg2);
	out("pg2 (char)");
	off(&d->line_id);
	out("line_id (char)");
# endif
# endif
	fflush(stdout);
}
