# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/pipes.h"
# include	"IIglobals.h"

/*
**	IIW_LEFT -- writes down a "tupret's" target list.
**
**	Parameters:
**		string -- a char * to a string containing everything
**			inside the equivalent "retrieve" statement,
**			but instead of result domain names, the string
**			should have '%<dbs_type>', where <dbs_type>
**			is the QL type of the resulting C variable.
**			To escape a '%' use 2 ("%%").
**			'String' is left unchanged after the call.
**		argv -- a vector of pointers to the
**			corresponding C variables.
**
**	Usage:
**		argv[0] = &double_var;
**		argv[1] = &int_var;
**		IIw_left("%f8 = i.double, %i2=i.ifield", argv);
*/
IIw_left(string, argv)
char	*string;
char	**argv;
{
	register char	*b_st;
	register char	*e_st;
	register char	**av;
	register int	type;
	extern char	*IIitos();

#	ifdef xETR1
	if (IIdebug)
		printf("LEFT: string \"%s\"\n", string);
#	endif
	av = argv;
	for (b_st = e_st = string; *e_st; )
	{
		if (*e_st != '%')
		{
			e_st++;
			continue;
		}

		/* provide escape method */
		if (e_st[1] == '%')
		{
			e_st[1] = '\0';
			IIwrite(b_st);
			/* leave string intact */
			e_st[1] = '%';
			b_st = e_st = &e_st[2];
			continue;
		}
		*e_st = '\0';
		IIwrite(b_st);
		*e_st++ = '%';
		IIwrite(" RET_VAR ");

		switch (*e_st)
		{
		  case 'f':
			type = *++e_st - '0';
			switch (type)
			{
# ifndef NO_F4
			  case sizeof (float):
				type = opFLT;
				break;
# endif

			  case sizeof (double):
				type = opDOUBLE;
				break;

			  default:
				goto error_label;
			}
			break;

		  case 'i':
			type = *++e_st - '0';
			switch (type)
			{
			  case sizeof (long):
				type = opLONG;
				break;

			  case sizeof (short):
				type = opINT;
				break;

			  default:
error_label:
				IIerror(IIerrflag = 1003, 1, &string);
				/* make sure that part already written down */
				/* will cause an error, and ignore that error */
				IIwrite(",");
				IIo_print = IIprint_err;
				IIprint_err = IIno_err;
			}
			break;

		  case 'c':
			type = opSTRING;
			break;
		}
		IIretrieve(*av++, type);
		b_st = ++e_st;
	}
	IIwrite(b_st);
}
