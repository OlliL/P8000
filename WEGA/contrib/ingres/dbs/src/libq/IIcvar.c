# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/symbol.h"
# include	"../h/pipes.h"
# include	"IIglobals.h"

/*
**	IIcvar -- write C variable values to parser
**
**		IIcvar is used to write the contents
**		of a C-variable to the QL parser.
**
**		Floats are converted to doubles first.
*/
IIcvar(addr, type, length)
register char	*addr;
register int	type;
register int	length;
{
	char		t;
	double		d;
# ifndef NO_F4
	struct _flt_
	{
		float	flt;
	};
# endif

	switch (type)
	{
# ifndef NO_F4
	  case opFLT:
		/* convert from f4 to f8 */
		d = ((struct _flt_ *) addr)->flt;
		addr = (char *) &d;
		length = sizeof (double);
		type = opDOUBLE;
		break;
# endif

	  case opSTRING:
		length = IIlength(addr) + 1;	/* length includes null byte at end */

	  case opINT:
	  case opLONG:
	  case opDOUBLE:
		break;

	  default:
		IIsyserr("CVAR: Unbekannter Typ %d", type);
	}

#	ifdef xETR1
	if (IIdebug)
		printf("CVAR: typ %d, len %d\n", type, length);
#	endif

	t = type;
	if (IIwrpipe(P_NORM, &IIoutpipe, IIw_down, &t, sizeof (char)) != sizeof (char))
		IIsyserr("CVAR: WRITE TYP");

	if (IIwrpipe(P_NORM, &IIoutpipe, IIw_down, addr, length) != length)
		IIsyserr("CVAR: WRITE VALUE len=%d", length);
}
