# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/pipes.h"
# include	"monitor.h"

/* list of fatal signals */
static char	*Siglist[] =
{
	"Signal 0",
	"hangup",
	"interrupt",
	"quit",
	"illegal instruction",
	"trace trap",
	"IOT instruction",
	"EMT instruction",
	"floating point exception",
	"kill",
	"bus error",
	"segmentation violation",
	"bad argument to system call",
	"write on broken pipe or link with no one to read it",
	"alarm clock",
	"software termination signal"
};

/*
**  QUIT DBS
**
**	This routine starts the death of the other processes.  It
**	then prints out the logout message, and then waits for the
**	rest of the system to die.  Note, however, that no relations
**	are removed; this must be done using the PURGE command.
*/
quit()
{
	register int	ndx;
	register int	pidptr;
	register int	err;
	int		status;
	int		pidlist[50];
	char		buf[100];
	char		indexx[0400];
	extern int	(*AAexitfn)();
	extern		exit();
	extern int	sys_nerr;
	extern char	*sys_errlist[];
	extern char	*AAcustomer();
	extern long	time();
	extern char	*ctime();

#	ifdef xMTR1
	AAtTfp(1, -1, "entered QUIT\n");
#	endif

	/* INTERCEPT ALL FURTHER INTERRUPTS */
	signal(1, (int (*)()) 1);
	signal(2, (int (*)()) 1);
	signal(3, (int (*)()) 1);
	AAexitfn = exit;

	close(W_down);

#	ifdef xMTR3
	AAtTfp(1, 2, "unlinking %s\n", Qbname);
#	endif

	/* REMOVE THE QUERY-BUFFER FILE */
	fclose(Qryiop);
	unlink(Qbname);
# ifdef MACRO
	if (Trapfile)
		fclose(Trapfile);
# endif
	pidptr = 0;
	err = 0;

	/* clear out the system error index table */
	for (ndx = 0; ndx < 0400; ndx++)
		indexx[ndx] = 0;

	/* wait for all process to terminate */
	while ((ndx = wait(&status)) != -1)
	{
#		ifdef xMTR2
		AAtTfp(1, 5, "QUIT: pid %u: 0%o/0%o\n", ndx, status >> 8, status & 0177);
#		endif
		pidlist[pidptr++] = ndx;
		if (status & 0177)
		{
			printf("%u: ", ndx);
			ndx = status & 0177;
			if (ndx > sizeof Siglist / sizeof Siglist[0])
				printf("Abnormal Termination %u", ndx);
			else
				printf("%s", Siglist[ndx]);
			if (status & 0200)
				printf(" -- Core Dumped");
			putchar('\n');
			err++;
			indexx[0377 - ndx]++;
		}
		else
			indexx[(status >> 8) & 0377]++;
	}
	if (err)
	{
		printf("pid list:");
		for (ndx = 0; ndx < pidptr; ndx++)
			printf(" %u", pidlist[ndx]);
		putchar('\n');
	}

	/* print index of system errors */
	err = 0;
	for (ndx = 1; ndx <= 0377; ndx++)
	{
		if (!indexx[ndx])
			continue;
		if (ndx <= sys_nerr)
		{
			if (!err)
				printf("\nBS error dictionary:\n");
			printf("%3d: %s\n", ndx, sys_errlist[ndx]);
		}
		if (!err)
			err = ndx;
	}
	if (err > 0 && err <= sys_nerr)
		putchar('\n');

	/* PRINT LOG CUE ? */
	if (Nodayfile >= 0)
	{
		printf(LOG_1, AAversion);
		time(buf);
# ifdef ESER_VMX
		printf("%s\n%s", AAcustomer(), ctime(*((long *) buf)));
# else
		printf("%s\n%s", AAcustomer(), ctime(buf));
# endif
		if (!getuser(AAusercode, buf))
		{
			for (ndx = 0; buf[ndx]; ndx++)
				if (buf[ndx] == ':')
					break;
			buf[ndx] = 0;
			printf(LOG_21, buf);
		}
		else
			printf(LOG_22);
		printf(LOG_3);
	}
#	ifdef xMTR1
	AAtTfp(1, 3, "QUIT: exit(%d)\n", err);
#	endif
	AAunlall();
	fflush(stdout);
	exit(err);
}
