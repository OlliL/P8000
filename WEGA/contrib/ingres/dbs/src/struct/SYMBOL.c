# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/ing_db.h"
# include	"../h/tree.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

main()
{
	register SYMBOL		*d;
	register int		i;
	SYMBOL			desc;
	char			buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct symbol   ==   SYMBOL [h/tree.h]\n");
	d = &desc;
	off(&d->type);
	out("type (char)");
	off(&d->len);
	out("len (char)");
# ifdef BIT_32
	off(&d->aligned);
	out("aligned (short)");
# endif
	off(d->value);
	out("value (short [1])");
	fflush(stdout);
}
