# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"monitor.h"

/*
**  OUTPUT PROMPT CHARACTER
**
**	The prompt is output to the standard output.  It will not be
**	output if -ss mode is set or if we are not at a newline.
**
**	The parameter is printed out if non-zero.
*/
prompt(msg)
register char	*msg;
{
	if (!Prompt || Peekch < 0)
		return;
	if (Nodayfile >= 0)
	{
		if (msg)
			printf("\07%s\n", msg);
		printf(PROMPT);
	}
	fflush(stdout);
}


/*
**  PROMPT WITH CONTINUE OR GO
*/
cgprompt()
{
	prompt(Notnull? FULL: EMPTY);
}
