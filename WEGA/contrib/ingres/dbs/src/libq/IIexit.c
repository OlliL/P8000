# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/symbol.h"
# include	"../h/pipes.h"
# include	"IIglobals.h"

/*
**	Exit DBS --
**		waits for the parser to return,
**		catching all children's death till then
**		or till an error return.
**		In case wait(II) is interrupted while waiting,
**		as evidenced by errno == 4, waiting will resume.
*/
IIexit()
{
	register int	ndx;
	register int	pidptr;
	register int	err;
	int		status;
	int		pidlist[10];
	extern		errno;		/* perror(III) */

#	ifdef xETR1
	if (IIdebug)
		printf("EXIT\n");
#	endif

	close(IIw_down);
	close(IIr_down);
	close(IIr_front);
	pidptr = 0;
	err = 0;
	errno = 0;
	while ((ndx = wait(&status)) != IIqlpid && (ndx != -1 || errno == 4))
	{
		if (ndx == -1)
		{
			errno = 0;
			continue;
		}
#		ifdef xETR1
		if (IIdebug)
			printf("\tpid %u stat 0%o sig %d\n", ndx, status >> 8, status & 0177);
#		endif

		pidlist[pidptr++] = ndx;
		if (status & 0177)
		{
			printf("PID %u Status 0%o -- ", ndx, status & 0177);
			if (status & 0200)
				printf("Core Dumped");
			else
				printf("Abnormal beendet");
			err++;
		}
	}
	if (err)
	{
		printf("\nPID Liste:");
		for (ndx = 0; ndx < pidptr; ndx++)
			printf(" %u", pidlist[ndx]);
		putchar('\n');
	}
	sync();
	IIqlpid = 0;
}
