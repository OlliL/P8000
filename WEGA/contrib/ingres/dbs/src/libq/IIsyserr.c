# include	<stdio.h>
# include	"gen.h"
# include	"../h/pipes.h"
# include	"IIglobals.h"

/*
**  SYSERR -- SYStem ERRor message print and abort
**
**	Syserr acts like a printf with up to five arguments.
**
**	If the first argument to syserr is not zero,
**	the message "SYSERR:" is prepended.
**
**	If the extern variable `IIproc_name' is assigned to a
**	string, that string is prepended to the message.
**
**	All arguments must be null-terminated.
**
**	The function pointed to by `AAexitfn' is then called.
**	It is initialized to be `exit'.
*/
# ifdef	P8000
IIsyserr(p0, p1, p2, p3)
register char	*p0;
register int	p1;
register int	p2;
register int	p3;
{
# else
IIsyserr(pv)
char	*pv;
{
	register char	**p;
# endif
	extern int	errno;

# ifndef P8000
	p = &pv;
# endif

	putchar('\n');
	if (IIproc_name)
		printf("%s ", IIproc_name);
	printf("SYSERR: ");

# ifdef P8000
	printf(p0, p1, p2, p3);
# else
	printf(p[0], p[1], p[2], p[3]);
# endif

	printf("\n");
	if (errno)
		printf("\tBS error %d\n", errno);
	fflush(stdout);
	exit(-1);
}
