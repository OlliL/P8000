# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"
# include	"../h/pipes.h"
# include	"IIglobals.h"

/*
**  IIN_GET -- Routine to check if there is a next
**	tuple coming up the data pipe in a retrieve.
**	Loads IIr_sym with the length and type of
**	the next domain.
**
**	Returns:
**		0 -- on error or end of tuple
**		1 -- otherwise
*/
IIn_get(file_name, line_no)
register char		*file_name;
register int		line_no;
{
	register struct retsym	*ret;

	if (IIproc_name = file_name)
		IIline_no = line_no;

#	ifdef xETR1
	if (IIdebug)
		printf("N_GET: IIdoms %d IIr_sym (typ %d len %d) IIerrflag %d\n",
		IIdoms, IIr_sym.type, IIr_sym.len, IIerrflag);
#	endif

	IIdoms = 0;
	/* in case of overflow error, a tuple has been skipped */
	if (IIerrflag == 1001)
		IIerrflag = 0;
	if (IIerrflag)
		return (0);

	ret = &IIr_sym;
	if (ret->type && ret->type != EOTUP)
		IIsyserr("N_GET: Unbekannter Typ %d", ret->type);

	/* read next type, length pair */
	if (IIrdpipe(P_NORM, &IIeinpipe, IIr_front, ret, TYP_LEN_SIZ) != TYP_LEN_SIZ)
		IIsyserr("N_GET: READ TYP_LEN");

	if (ret->type == EXIT)
		return (0);

	return (1);
}


/*
**  IIERRTEST -- Test if an error occurred within a retrieve.
**
**	Returns:
**		1 -- if so
**		0 -- if not
*/
IIerrtest()
{
	return (IIerrflag? 1: 0);
}
