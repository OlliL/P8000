# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/tree.h"
# include	"../h/pipes.h"
# include	"decomp.h"

struct retcode	Retcode;

writeback(flag)
int	flag;
{
	struct pipfrmt		buf;

	flush();
	wrpipe(P_PRIME, &buf, EXEC_PARSER, (char *) 0, 0);
	if (flag > 0)
	{
		/* send back tuple count info */

#		ifdef xDTR1
		AAtTfp(8, 10, "%ld tups found\n", Retcode.rc_tupcount);
#		endif

		wrpipe(P_NORM, &buf, W_up, &Retcode, sizeof (Retcode));
	}

	if (flag < 0)
	{
		rdpipe(P_PRIME, &Inpipe);
		copypipes(&Inpipe, R_down, &buf, W_up);
	}
	if (flag >= 0)
		wrpipe(P_END, &buf, W_up);
}
