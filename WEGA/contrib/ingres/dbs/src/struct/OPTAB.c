# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/scanner.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

struct optab	Optab[1];
struct optab	Keyword[1];

main()
{
	register struct optab		*d;
	register int			i;
	struct optab			desc;
	char				buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct optab [h/scanner.h]\n");
	d = &desc;
	off(&d->term);
	out("term (char *)");
	off(&d->token);
	out("token (short)");
	off(&d->opcode);
	out("opcode (short)");
	fflush(stdout);
}
