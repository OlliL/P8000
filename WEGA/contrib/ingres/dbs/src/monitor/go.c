# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/pipes.h"
# include	"monitor.h"

/*
**  PROCESS QUERY
**
**	The appropriate messages are printed, and the query is scanned.
**	Tokens are passed to the parser.  A parser response is then
**	expected.
*/
go()
{
	register char			*p;
	register FILE			*iop;
	register struct pipfrmt		*pip;
	char				c;
	struct pipfrmt			pip_buf;
	struct retcode			rc;
# ifdef SETBUF
	char				iop_buf[BUFSIZ];
# endif
	extern char			*AAcustomer();
# ifdef MACRO
	extern char			*mcall();
	extern char			*macro();
	extern int			fgetc();
# endif

	clrline(TRUE);
	fflush(Qryiop);
	if (!(iop = fopen(Qbname, "r")))
		AAsyserr(16012);
# ifdef SETBUF
	setbuf(iop, iop_buf);
# endif

	if (Nodayfile >= 0)
		printf(EXECUTING, AAcustomer());

	fflush(stdout);

#	ifdef xMTM
	if (AAtTf(76, 1))
		timtrace(3, 0);
#	endif

	if (!Nautoclear)
		Autoclear = 1;

	wrpipe(P_PRIME, pip = &pip_buf, 'M', (char *) 0, 0);

# ifdef MACRO
	macinit(fgetc, iop, TRUE);
	while ((c = macgetch()) > 0)
# else
	while ((c = getc(iop)) > 0)
# endif
		wrpipe(P_NORM, pip, W_down, &c, sizeof (char));

	wrpipe(P_END, pip, W_down);

	Error_id = 0;

	rdpipe(P_PRIME, pip);

# ifdef MACRO
	/* read number of tuples return */
	rc.rc_status = (rdpipe(P_NORM, pip, R_down, &rc, sizeof rc) == sizeof rc)? RC_OK: RC_BAD;

	if (p = macro(QUERYTRAP))
	{
		if (!Error_id)
			trapquery(&rc, p);
	}
	else if (Trapfile)
	{
		fclose(Trapfile);
		Trapfile = (FILE *) 0;
		Trap = 0;
	}
	if (rc.rc_status == RC_OK)
		macdefine(TUPLECOUNT, AA_locv(rc.rc_tupcount), TRUE);
# endif

	/* clear out the rest of the pipe */
	rdpipe(P_SYNC, pip, R_down);

# ifdef MACRO
	mcall(CONTINUETRAP);
# endif

#	ifdef xMTM
	if (AAtTf(76, 1))
		timtrace(4, 0);
#	endif

	putchar('\n');
	prompt(FULL);
	fclose(iop);
}
