# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/ing_db.h"
# include	"../h/access.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

main()
{
	register struct tup_hdr	*d;
	register int			i;
	struct tup_hdr			desc;
	char				buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct tup_hdr [h/access.h]\n");
	d = &desc;
	off(&d->tup_ptr);
	out("tup_ptr (char *)");
	off(&d->tup_end);
	out("tup_end (char *)");
	off(&d->tup_fd);
	out("tup_fd (int)");
	off(&d->tup_no);
	out("tup_no (short)");
	off(&d->tup_len);
	out("tup_len (short)");
	off(d->tup_buf);
	out("tup_buf (char [TUP_BUF_SIZ])");
	fflush(stdout);
}
