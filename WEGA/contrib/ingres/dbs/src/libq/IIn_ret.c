# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"
# include	"../h/pipes.h"
# include	"IIglobals.h"

/*
**  IIn_ret -- get next domain in a retrieve
**
**	Gets the next domain in a retrieve from the data
**	pipe. If an error occurred previously in the tuple,
**	will not load the c-var with the value of the domain.
**	Performs the conversion from the gotten type to
**	the expected type.
**
**	Signals any errors and calls IIerror() accordingly.
**
**	Expects the type and length of the next data item in
**	IIr_sym.
*/
IIn_ret(addr, type)
char	*addr;
int	type;
{
	register struct retsym	*ret;
	register int		length;
	char			*s;
	char			temp[MAXFIELD + 1];
	extern char		*IIitos();


	if (IIerrflag && IIerrflag != 1001)
		return;		/* error, no data will be coming, or the */
				/* rest of the query should be skipped   */

	if (length = ctou((ret = &IIr_sym)->len))
		if (IIrdpipe(P_NORM, &IIeinpipe, IIr_front, temp, length) != length)
			IIsyserr("N_RET: READ VALUE len=%d", length);

#	ifdef xETR1
	if (IIdebug)
		printf("N_RET: addr 0%o typ %d len %d ret.typ %d IIerrflag %d\n",
			addr, type, length, ret->type, IIerrflag);
#	endif

	IIdoms++;
	switch (type)
	{
	  case opINT:
		type = INT;
		length = sizeof (short);
		break;

	  case opLONG:
		type = INT;
		length = sizeof (long);
		break;

# ifndef NO_F4
	  case opFLT:
		type = FLOAT;
		length = sizeof (float);
		break;
# endif

	  case opDOUBLE:
		type = FLOAT;
		length = sizeof (double);
		break;

	  case opSTRING:
		type = CHAR;
		/* with the current implementation the length is not known */
		length = MAXFIELD;
		break;

	  default:
		IIsyserr("N_RET: Unbekannter Typ %d", type);
	}

	switch (ret->type)
	{
	  case INT:
	  case FLOAT:
		if (type == CHAR)
		{
			IIerrflag = 1000;
			goto error;
		}
		if (IIconvert(temp, IIerrflag? temp: addr,
		   ret->type, ctou(ret->len), type, length) < 0)
		{
			IIerrflag = 1001;
			goto error;
		}
		break;

	  case CHAR:
		if (type != CHAR)
		{
			IIerrflag = 1002;
error:
			s = IIitos(IIdoms);
			IIerror(IIerrflag, 1, &s);
			return;
		}
		if (!IIerrflag)
		{
			IIbmove(temp, addr, ctou(ret->len));

			/* null terminate string */
			addr[ctou(ret->len)] = '\0';
		}
		break;

	  default:
		IIsyserr("N_RET: READ TYP %d", ret->type);
	}

	if (IIrdpipe(P_NORM, &IIeinpipe, IIr_front, ret, TYP_LEN_SIZ) != TYP_LEN_SIZ)
		IIsyserr("N_RET: READ TYP_LEN");
}
