# include	<stdio.h>
# include	"gen.h"
# include	"../h/pipes.h"

/*
**  RESYNCHRONIZE PIPES AFTER AN INTERRUPT
**
**	The pipes are all cleared out.  This routines must be called
**	by all processes in the system simultaneously.  It should be
**	called from the interrupt catching routine.
*/
resyncpipes()
{
	register struct pipfrmt	*pbuf;
	register int		fd;
	struct pipfrmt		buf;

	wrpipe(P_PRIME, pbuf = &buf, 0, (char *) 0, 0);

	/* synchronize the downward pipes */
	if ((fd = R_up) >= 0)
		rdpipe(P_INT, pbuf, fd);
	if ((fd = W_down) >= 0)
		wrpipe(P_INT, pbuf, fd);

	/* now the upward pipes */
	if ((fd = R_down) >= 0)
		rdpipe(P_INT, pbuf, fd);
	if ((fd = W_up) >= 0)
		wrpipe(P_INT, pbuf, fd);
}
