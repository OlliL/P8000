# include	<stdio.h>
# include	"gen.h"
# include	"../h/pipes.h"

/*
**  DEFAULT PROC_ERROR ROUTINE
**
**	This routine handles the processing of errors for a typical
**	process.  Please feel free to redefine it if you need other
**	features.
*/

proc_error(s, fd)
register struct pipfrmt		*s;
register int			fd;
{
	struct pipfrmt		t;

	if (fd != R_down)
		AAsyserr(15027);
	wrpipe(P_PRIME, &t, s->exec_id, (char *) 0, s->func_id);
	t.err_id = s->err_id;

	copypipes(s, fd, &t, W_up);
	rdpipe(P_PRIME, s);
}
