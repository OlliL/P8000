# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"
# include	"../h/pipes.h"
# include	"IIglobals.h"

/*
**	IIgettup is called to retrieve one instance
**	of the target list into the c-variables.
**
**	Integers and Floating point numbers can be converted
**	to other numbers.
**
**	Characters fields must match other character fields.
*/
IIgettup(file_name, line_no)
char	*file_name;
int	line_no;
{
	register int		length;
	register int		domain;
	register struct retsym	*ret;
	char			*s;
	struct retsym		sym;
	char			temp[MAXFIELD + 1];
	extern char		*IIitos();



	if (IIproc_name = file_name)
		IIline_no = line_no;

	if (IIerrflag)
		return (0);	/* error. no data will be coming */

	ret = IIretsym;
	domain = 0;

	for ( ; ; )
	{
		if (IIrdpipe(P_NORM, &IIeinpipe, IIr_front, &sym, TYP_LEN_SIZ) != TYP_LEN_SIZ)
			IIsyserr("GETTUP: READ TYP_LEN");
		if (length = ctou(sym.len))
			if (IIrdpipe(P_NORM, &IIeinpipe, IIr_front, temp, length) != length)
				IIsyserr("GETTUP: READ VALUE len=%d", length);
#		ifdef xETR1
		if (IIdebug)
			printf("GETTUP: typ %d len %d\n", sym.type, length);
#		endif
		domain++;
		switch (sym.type)
		{

		  case INT:
		  case FLOAT:
			if (ret->type == CHAR)
			{
				s = IIitos(domain);
				IIerror(1000, 1, &s);
				return (0);
			}
			if (IIconvert(temp, ret->addr, sym.type, length, ret->type, ctou(ret->len)) < 0)
			{
					s = IIitos(domain);
					IIerror(1001, 1, &s);
			}
			break;

		  case CHAR:
			if (ret->type != CHAR)
			{
				s = IIitos(domain);
				IIerror(1002, 1, &s);
				return (0);
			}
			IIbmove(temp, ret->addr, length);
			ret->addr[length] = '\0';	/* null terminate string */
			break;

		  case EOTUP:
			return (1);

		  case EXIT:
			return (0);
		}
		ret++;
	}
}
