# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/ing_db.h"
# include	"../h/tree.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

main()
{
	register QTREE		*d;
	register int		i;
	QTREE			desc;
	char			buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct querytree   ==   QREE [h/tree.h]\n");
	d = &desc;
	off(&d->left);
	out("left (struct querytree *)");
	off(&d->right);
	out("right (struct querytree *)");
	off(&d->sym);
	out("sym (struct symbol)");
	off(&d->sym.type);
	out("sym.type (char)");
	off(&d->sym.len);
	out("sym.len (char)");
# ifdef BIT_32
	off(&d->sym.aligned);
	out("sym.aligned (short)");
# endif
	off(d->sym.value);
	out("sym.value (short [1])");
	fflush(stdout);
}
