# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"
# include	"../h/pipes.h"
# include	"../h/access.h"
# include	"../h/batch.h"
# include	"ovqp.h"

/*
**	This file contains all the routines needed
**	for communicating with the EQL process.
**	They are called only if the flag Eql = TRUE.
*/

/* global EQL pipebuffer */
struct pipfrmt	Eoutpipe;

/*
**	eqlatt writes one symbol pointed to
**	by ss up the data pipe to the EQL process.
**
**	if a symbol is a character then *((SYMBOL *) ss)->value
**	contains a pointer to the character string.
**	otherwise the value is stored in successive
**	words starting in ((SYMBOL *) ss)->value.
*/
eqlatt(s)
register SYMBOL		*s;
{
#	ifdef xOTR1
	if (AAtTf(20, 0))
		prstack(s);
#	endif
	pwritesym(&Eoutpipe, W_front, s);
}


/*
**	eqleol is called at the end of the interpretation of
**	a tuple. Its purpose is to write an end-of-tuple
**	symbol to the EQL process and flush the pipe.
**
**	It is also called at the end of a query to write
**	an exit symbol to EQL.
*/
eqleol(code)
register int	code;
{
	register int	mode;
	SYMBOL		symb;

	mode = (code == EXIT)? P_END: P_FLUSH;
	symb.type = code;
	symb.len = 0;

#	ifdef xOTR1
	AAtTfp(20, 3, "EQLEOL: writing %d to EQL\n", code);
#	endif

	wrpipe(P_NORM, &Eoutpipe, W_front, &symb, TYP_LEN_SIZ);
	if (mode != P_FLUSH || Eql != 2)
		wrpipe(mode, &Eoutpipe, W_front);
}


/*
**	pwritesym write the stacksymbol
**	pointed to by "ss" to the pipe
**	indicated by filedesc.
**
**	The destination will either be EQL
**	or decomp
**
**	Since a CHAR isn't stored immediately following
**	the type and len of the symbol, A small bit
**	of manipulation must be done.
*/
pwritesym(pipedesc, filedesc, s)
struct pipfrmt		*pipedesc;
int			filedesc;
register SYMBOL		*s;
{
	register char			*p;
	register int			length;

	length = ctou(s->len);
	p = (char *) s->value;
	if (s->type == INT && length == sizeof (char))
		s->len = length = sizeof (short);

	wrpipe(P_NORM, pipedesc, filedesc, s, TYP_LEN_SIZ);

	if (s->type == CHAR)
		p = ((STRINGP *) p)->stringp;	/* p points to the string */

	if (length)
		wrpipe(P_NORM, pipedesc, filedesc, p, length);
}
