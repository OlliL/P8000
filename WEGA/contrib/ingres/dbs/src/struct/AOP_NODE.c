# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/ing_db.h"
# include	"../h/tree.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

main()
{
	register AOP_NODE		*d;
	register int			i;
	AOP_NODE			desc;
	char				buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct aop_node   ==   AOP_NODE [h/tree.h]\n");
	d = &desc;
	off(d->qt_hdr);
	out("qt_hdr (char [QT_HDR_SIZ])");
	off(d->var_hdr);
	out("var_hdr (char [VAR_HDR_SIZ])");
	off(&d->agfrmt);
	out("agfrmt (char)");
	off(&d->agfrml);
	out("agfrml (char)");
	fflush(stdout);
}
