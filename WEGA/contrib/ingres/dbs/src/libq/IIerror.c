# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/symbol.h"
# include	"../h/pipes.h"
# include	"IIglobals.h"

char		*IIproc_name;		/* file name			    */
int		IIline_no;		/* line no			    */
int		IIdebug = 0;		/* debug flag			    */

int		IIerrflag;		/* error flag.  Set in IIerror,	    */
					/* cleared in IIsync 		    */
extern int	IIret_err();
int		(*IIprint_err)() =	IIret_err;
int		(*IIo_print)();		/* a one value stack for 	    */
					/* temporarily turned off printing  */
					/* of errors. Done in [IIw_left.c   */
					/* and IIw_right.c].		    */

/*
**	IIerror -- std. EQL error reporting routine.
**
**		IIerror is called either directly by a
**		routine detecting an error or by
**		IIrdpipe where DBS returns an error.
**
**		In the case of an appropriate error, IIerrflag is set to
**		prevent the reading of the ovqp data pipe. This happens
**		on errors that happen during retrieves to C vars.
**
**		Errno is the error number: 1??? is an EQL error
**		others are DBS errors.
**
**		Argc and argv are structured as in BS main program calls.
**
**		The error is printed if the call (*IIprint_err)()
**		returns > 0. Otherwise no error message is printed.
**		The error message that is printed is the one corresponding
**		to the number returned by (*IIprint_err)().
*/
IIerror(errno, argc, argv)
int	errno;
int	argc;
char	*argv[];
{
	register int		i;
	extern char		*IIp_err();

#	ifdef xETR1
	if (IIdebug > 1)
		printf("ERROR: errno %d argc %d\n", errno, argc);
#	endif

	IIerrflag = errno;
	if (!(errno = (*IIprint_err)(errno)))
	{
#		ifdef xETR2
		if (IIdebug > 1)
			printf("ERROR: IIprint_err returned 0\n");
#		endif
		/* if must restore printing turned off in IIw_left()
		 * or IIw_right(), then do so.
		 */
		if (IIo_print)
		{
			IIprint_err = IIo_print;
			IIo_print = 0;
		}
		return;
	}

	if (errno > 2000)
		printf("DBS ERROR: ");
	else
		printf("EQL ERROR: ");

	if (!IIp_err(errno, argc, argv))
	{
		/* couldn't process error in IIp_err() */
		printf("%d mit den Argumenten:", errno);
		for (i = 0; i < argc; )
			printf(" %s", argv[i++]);
		putchar('\n');
	}
	if (IIproc_name)
		printf("Zeile %4d file %s\n\n", IIline_no, IIproc_name);
}


/*
**  IIret_err -- returns its single argument for IIerror.
*/
IIret_err(err)
register int	err;
{
	return (err);
}


/*
**  IIno_err -- returns 0. Called from IIerror
**		(through (*IIprint_err)())
**		to supress error message printing.
*/
IIno_err()
{
	return (0);
}
