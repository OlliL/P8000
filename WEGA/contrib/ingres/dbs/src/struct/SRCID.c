# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/ing_db.h"
# include	"../h/tree.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

main()
{
	register SRCID		*d;
	register int		i;
	SRCID			desc;
	char			buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct srcid   ==   SRCID [h/tree.h]\n");
	d = &desc;
	off(d->sym_hdr);
	out("sym_hdr (char [SYM_HDR_SIZ])");
	off(&d->srcvar);
	out("srcvar (short)");
	off(d->srcname);
	out("srcname (char [MAXNAME])");
	off(d->srcown);
	out("srcown (char [2])");
	off(&d->srcstat);
	out("srcstat (short)");
	fflush(stdout);
}
