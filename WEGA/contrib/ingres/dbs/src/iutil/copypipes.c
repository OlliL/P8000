# include	<stdio.h>
# include	"gen.h"
# include	"../h/pipes.h"

/*
**  COPY PIPES
**
**	Copys pipe 'fromd' to pipe 'tod' using pipe buffers 'fromb'
**	and 'tob'.
**
**	Neither pipe is primed or otherwise set up.  The end of pipe
**	is copied.
*/
copypipes(fromb, fromd, tob, tod)
register struct pipfrmt		*fromb;
register struct pipfrmt		*tob;
int				fromd;
int				tod;
{
	register int		i;
	char			buf[PBUFSIZ];

	while ((i = rdpipe(P_NORM, fromb, fromd, buf, sizeof buf)) > 0)
		wrpipe(P_NORM, tob, tod, buf, i);
	wrpipe(P_END, tob, tod);
}
