# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/ing_db.h"
# include	"../h/access.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(x)		printf("%4d %04o #%04x:\t%s\n", i, i, i, x)

main()
{
	register struct pgtuple		*d;
	register int			i;
	struct pgtuple			desc;
	char				buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct pgtuple\n");
	d = &desc;
# ifdef REV_BYTES
# ifdef REV_WORDS
	off(&d->childtid.line_id);
	out("childtid.line_id");
	off(&d->childtid.pg2);
	out("childtid.pg2");
	off(&d->childtid.pg1);
	out("childtid.pg1");
	off(&d->childtid.pg0);
	out("childtid.pg0");
# else
	off(&d->childtid.pg1);
	out("childtid.pg1");
	off(&d->childtid.pg0);
	out("childtid.pg0");
	off(&d->childtid.line_id);
	out("childtid.line_id");
	off(&d->childtid.pg2);
	out("childtid.pg2");
# endif
# else
# ifdef REV_WORDS
	off(&d->childtid.pg2);
	out("childtid.pg2");
	off(&d->childtid.line_id);
	out("childtid.line_id");
	off(&d->childtid.pg0);
	out("childtid.pg0");
	off(&d->childtid.pg1);
	out("childtid.pg1");
# else
	off(&d->childtid.pg0);
	out("childtid.pg0");
	off(&d->childtid.pg1);
	out("childtid.pg1");
	off(&d->childtid.pg2);
	out("childtid.pg2");
	off(&d->childtid.line_id);
	out("childtid.line_id");
# endif
# endif
	off(d->childtup);
	out("childtup");
	fflush(stdout);
}
