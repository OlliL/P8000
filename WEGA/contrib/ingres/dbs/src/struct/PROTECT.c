# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/ing_db.h"
# include	"../h/catalog.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

main()
{
	register struct protect		*d;
	register int			i;
	struct protect			desc;
	char				buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct protect [h/catalog.h]\n");
	d = &desc;
	off(d->prorelid);
	out("prorelid (char [MAXNAME])");
	off(d->prorelown);
	out("prorelown (char [2])");
	off(&d->propermid);
	out("propermid (short)");
	off(d->prouser);
	out("prouser (char [2])");
	off(d->proterm);
	out("proterm (char [PROTERM])");
	off(&d->proresvar);
	out("proresvar (char)");
	off(&d->proopset);
	out("proopset (char)");
	off(&d->protodbgn);
	out("protodbgn (short)");
	off(&d->protodend);
	out("protodend (short)");
	off(&d->prodowbgn);
	out("prodowbgn (char)");
	off(&d->prodowend);
	out("prodowend (char)");
	off(d->prodomset);
	out("prodomset (short [8])");
	off(&d->protree);
	out("protree (short)");
	fflush(stdout);
}
