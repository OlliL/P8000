# include	<stdio.h>
# include	"gen.h"
# include	"../h/pipes.h"

prpipe(buf)
register struct pipfrmt		*buf;
{
	register int		i;
	register int		c;

	printf("pipe struct=\t0%o\n", buf);
	printf("exec_id='%c'\tfunc_id='%c'\terr_id=%d\n",
		buf->exec_id, buf->func_id, buf->err_id);
	printf("hdrstat=0%o\tbuf_len=%d\tpbuf_pt=0%o\n",
		buf->hdrstat, buf->buf_len, buf->pbuf_pt);
	for (i = 0; i < buf->buf_len; i++)
	{
		c = buf->pbuf[i];
		printf("\t%3d", c);
		if (c > ' ')
			printf(" %c", c);
		if (i % 8 == 7)
			putchar('\n');
	}
	putchar('\n');
}
