# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/scanner.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

struct optab	Optab[1];
struct optab	Keyword[1];

main()
{
	register struct lastok		*d;
	register int			i;
	struct lastok			desc;
	char				buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct lastok [h/scanner.h]\n");
	d = &desc;
	off(&d->toktyp);
	out("toktyp (short)");
	off(&d->tok);
	out("tok (char *)");
	off(&d->tokop);
	out("tokop (short)");
	fflush(stdout);
}
