# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"monitor.h"

/*
**  WAIT FOR PROCESS TO DIE
**
**	Used in edit() and shell() to terminate the subprocess.
**	Waits on pid "Xwaitpid".  If this is zero, xwait() returns
**	immediately.
**
**	This is called by "catchsig()" so as to properly terminate
**	on a rubout while in one of the subsystems.
*/
xwait()
{
	register int		i;
	int			status;
# ifdef MACRO
	extern char		*mcall();
# endif

#	ifdef xMTR2
	AAtTfp(15, 0, "XWAIT: [%u]\n", Xwaitpid);
#	endif

	if (!Xwaitpid)
	{
		cgprompt();
		return;
	}

	while ((i = wait(&status)) != -1)
	{
#		ifdef xMTR2
		AAtTfp(15, 1, "pid %u stat 0%o\n", i, status);
#		endif
		if (i == Xwaitpid)
			break;
	}

	if (i < 0)
	{
		AAunlall();
		fflush(stdout);
		exit(status >> 8);
	}
	Xwaitpid = 0;

# ifdef MACRO
	/* call execptionhandler EXECTRAP */
	if (i = status >> 8)
	{
		macdefine(ERRORCOUNT, AA_iocv(i), TRUE);
		mcall(EXECTRAP);
	}
# endif

	print(FALSE);

	/* reopen query buffer */
	if (!(Qryiop = fopen(Qbname, "a")))
		AAsyserr(16026);
# ifdef SETBUF
	setbuf(Qryiop, Qryxxx);
# endif
}
