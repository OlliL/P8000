# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/scanner.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

struct optab	Optab[1];
struct optab	Keyword[1];

main()
{
	register struct special		*d;
	register int			i;
	struct special			desc;
	char				buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct special [h/scanner.h]\n");
	d = &desc;
	off(&d->sconst);
	out("sconst (short)");
	off(&d->bgncmnt);
	out("bgncmnt (short)");
	off(&d->endcmnt);
	out("endcmnt (short)");
	off(&d->i2const);
	out("i2const (short)");
	off(&d->i4const);
	out("i4const (short)");
# ifndef NO_F4
	off(&d->f4const);
	out("f4const (short)");
# endif
	off(&d->f8const);
	out("f8const (short)");
	off(&d->name);
	out("name (short)");
	fflush(stdout);
}
