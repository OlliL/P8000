# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/pipes.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

main()
{
	register struct pipfrmt		*d;
	register int			i;
	struct pipfrmt			desc;
	char				buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct pipfrmt [h/pipes.h]\n");
	d = &desc;
	off(&d->exec_id);
	out("exec_id (char)");
	off(&d->func_id);
	out("func_id (char)");
	off(&d->buf_len);
	out("buf_len (short)");
	off(&d->err_id);
	out("err_id (short)");
	off(&d->hdrstat);
	out("hdrstat (char)");
	off(&d->param_id);
	out("param_id (char)");
	off(d->pbuf);
	out("pbuf (char [PBUFSIZ])");
	off(&d->pbuf_pt);
	out("pbuf_pt (int)");
	fflush(stdout);
}
