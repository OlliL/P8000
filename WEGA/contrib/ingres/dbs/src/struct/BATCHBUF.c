# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/ing_db.h"
# include	"../h/access.h"
# include	"../h/batch.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

main()
{
	register struct batchbuf	*d;
	register int			i;
	struct batchbuf			desc;
	char				buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct batchbuf [h/batch.h]\n");
	d = &desc;
	off(d->file_id);
	out("file_id (char [IDSIZE])");
	off(d->bbuf);
	out("bbuf (char [BATCHSIZE])");
	fflush(stdout);
}
