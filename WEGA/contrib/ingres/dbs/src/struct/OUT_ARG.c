# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/ing_db.h"
# include	"../h/aux.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

main()
{
	register struct out_arg	*d;
	register int			i;
	struct out_arg			desc;
	char				buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct out_arg [h/aux.h]\n");
	d = &desc;
	off(&d->c0width);
	out("c0width (short)");
	off(&d->i1width);
	out("i1width (short)");
	off(&d->i2width);
	out("i2width (short)");
	off(&d->i4width);
	out("i4width (short)");
# ifndef NO_F4
	off(&d->f4width);
	out("f4width (short)");
# endif
	off(&d->f8width);
	out("f8width (short)");
# ifndef NO_F4
	off(&d->f4prec);
	out("f4prec (short)");
# endif
	off(&d->f8prec);
	out("f8prec (short)");
# ifndef NO_F4
	off(&d->f4style);
	out("f4style (char)");
# endif
	off(&d->f8style);
	out("f8style (char)");
	off(&d->linesperpage);
	out("linesperpage (short)");
	off(&d->coldelim);
	out("coldelim (char)");
	fflush(stdout);
}
