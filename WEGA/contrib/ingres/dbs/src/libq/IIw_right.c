# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/pipes.h"
# include	"IIglobals.h"

/*
**  IIW_RIGHT -- Write down to the QL parser a string
**	for a target list of anything but a tupret.
**
**	Parameters:
**		string -- a string which contains the target list
**			of a QL statement, where values from C variables
**			to be plugged in are flagged by the construct
**			'%<dbs_type>" a la printf(). String is left
**			unchanged.
**			To escape the '%' mechanism use '%%'.
**
**		argv -- a vector of pointers to
**			variables from which the values flagged by the '%'
**			mechanism are taken.
**
**	Usage:
**		argv[0] = &double_raise;
**		IIw_right("dom1 = i.ifield * (1+%f8)", argv);
*/
IIw_right(string, argv)
char	*string;
char	**argv;
{
	register char	*b_st;
	register char	*e_st;
	register char	**av;
	register int	i;
	extern char	*IIitos();

#	ifdef xETR1
	if (IIdebug)
		printf("RIGHT: string \"%s\"\n", string);
#	endif
	av = argv;
	for (b_st = e_st = string; *e_st; )
	{
		if (*e_st != '%')
		{
			e_st++;
			continue;
		}

		/* provide '%%' escape mechanism */
		if (e_st[1] == '%')
		{
			e_st[1] = '\0';
			IIwrite(b_st);
			e_st[1] = '%';
			b_st = e_st = &e_st[2];
			continue;
		}
		*e_st = '\0';
		IIwrite(b_st);
		*e_st++ = '%';

		switch (*e_st)
		{
		  case 'f':
			i = *++e_st - '0';
			switch (i)
			{
			  case sizeof (double):
				IIcvar(*av, opDOUBLE, sizeof (double));
				break;

# ifndef NO_F4
			  case sizeof (float):
				IIcvar(*av, opFLT, sizeof (float));
				break;
# endif

			  default:
				goto error_label;
			}
			av++;
			break;

		  case 'i':
			i = *++e_st - '0';
			switch (i)
			{
			  case sizeof (long):
				IIcvar(*av, opLONG, sizeof (long));
				break;

			  case sizeof (short):
				IIcvar(*av, opINT, sizeof (short));
				break;

			  case sizeof (char):
				IIcvar(*av, opCHAR, sizeof (char));

			  default:
				goto error_label;
			}
			av++;
			break;

		  case 'c':
			IIcvar(*av++, opSTRING, 0);
			break;
		}
		b_st = ++e_st;
	}
	IIwrite(b_st);
	return;

error_label:
	IIerror(IIerrflag = 1004, 1, &string);
	/* make sure that part already written down will     */
	/* cause an error, and don't print it.               */
	/*	The old IIprint_err is restored in IIerror() */
	IIwrite(",");
	IIo_print = IIprint_err;
	IIprint_err = IIno_err;
}
