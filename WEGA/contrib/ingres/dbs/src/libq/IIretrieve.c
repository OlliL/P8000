# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/symbol.h"
# include	"../h/pipes.h"
# include	"IIglobals.h"

/*
**	IIretrieve is called once for each element
**	in the target list of a retrieve.
**
**	The purpose is to set up the IIretsym structure
**	for IIgettup.
*/
IIretrieve(addr, type)
char	*addr;
int	type;
{
	register struct retsym	*sym;
	register int		t;
	register int		l;

	sym = &IIretsym[IIndoms++];
	switch (type)
	{
	  case opINT:
		t = INT;
		l = sizeof (short);
		break;

	  case opLONG:
		t = INT;
		l = sizeof (long);
		break;

# ifndef NO_F4
	  case opFLT:
		t = FLOAT;
		l = sizeof (float);
		break;
# endif

	  case opDOUBLE:
		t = FLOAT;
		l = sizeof (double);
		break;

	  case opSTRING:
		t = CHAR;
		/* with the current implementation the length is not known */
		l = MAXFIELD;
		break;

	  default:
		IIsyserr("RETRIEVE: Unbekannter Typ %d", type);
	}
	sym->type = t;
	sym->len = l;
	sym->addr = addr;

#	ifdef xETR1
	if (IIdebug)
		printf("RETRIEVE: IIndoms %d typ %d len %d\n", IIndoms, t, l);
#	endif
}
