# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/ing_db.h"
# include	"../h/tree.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

main()
{
	register SVAR_NODE		*d;
	register int			i;
	SVAR_NODE			desc;
	char				buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct svar_node   ==   SVAR_NODE [h/tree.h]\n");
	d = &desc;
	off(d->sym_hdr);
	out("sym_hdr (char [SYM_HDR_SIZ])");
	off(&d->vattno);
	out("vattno (short)");
	off(&d->vtype);
	out("vtype (char)");
	off(&d->vlen);
	out("vlen (char)");
	off(&d->vpoint);
	out("vpoint (short *)");
	fflush(stdout);
}
