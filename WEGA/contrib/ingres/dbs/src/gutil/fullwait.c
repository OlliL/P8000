# include	"gen.h"

/*
**  FULLWAIT -- performs a wait(II) primitive for a given child.
**
**	The wait primitive is executed as many times as needed to get
**	termination status for a given child.  The case of an interrupt
**	during the wait is handled.  No other children may die during
**	the wait.  Also, the child must die "normally", that is, as the
**	result of an exit() system call, or from an interrupt.
**
**	Parameters:
**		"child" -- the pid of the child process to wait for,
**			returned from the fork() system call.
**		"name" -- a character string representing the name of
**			the calling routine; printed on AAsyserr's.
**	Return value:
**		The exit parameter of the child process.
*/
fullwait(child, name)
register int	child;
register char	*name;
{
	register int	i;
	int		stat;
	extern int	errno;

	/* wait for a child to die */
	while ((i = wait(&stat)) != child)
	{
		/* it is not the child we want; chew on it a little more */
		if (i != -1)
			AAsyserr(14001, name, i, stat, child);

		/* check for interrupted system call */
		if (errno != 4)	/* wait with no child */
			AAsyserr(14004, name, child);
		errno = 0;

		/* dropped out from signal: reexecute the wait */
	}

	/* check termination status */
	if ((i = ctou(stat)) > 2)	/* child collapsed */
		AAsyserr(14005, name, child, i & 0177,
			(i & 0200)? "-- core dumped" : (char *) 0);

	/* return exit status */
	return (stat >> 8);
}
