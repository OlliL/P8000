# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"
# include	"../h/aux.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"../h/pipes.h"
# include	"decomp.h"

static int	Synconly;
static int	Error_flag;

call_dbu(code, errflag)
int	code;
int	errflag;

/*
**	Call the appropriate dbu with the arguments
**	given in the globals Pc and Pv. Code is a
**	number identifing which dbu to call. Errflag
**	indicates whether an error return from the dbu
**	is possibly expected.
**
**	If errflag < 0 then call_dbu will not wait for sync.
**	If errflag = 0 then call_dbu will AAsyserr on an error
**	If errflag > 0 then call_dbu will return error value
*/

{
	struct pipfrmt 	pipe;
	register int 	i;
	register int 	j;
	register int	(*func)();
	extern char	*Pv[];
	extern int	Pc;
	extern int	create();
	extern int	destroy();
	extern int	resetrel();

#	ifdef xDTR1
	AAtTfp(3, 0, "Calling DBU %d\n", code);
	if (AAtTf(3, 1))
		AAprargs(Pc, Pv);
#	endif

	switch (code)
	{

	  case mdCREATE:
		func = create;
		break;

	  case mdDESTROY:
		func = destroy;
		break;

	  case mdRESETREL:
		func = resetrel;
		break;

	  default:
		func = (int (*) ()) 0;
	}

	if (func)
	{
		Pv[Pc] = (char *) -1;
		Error_flag = (*func)(Pc, Pv);
		closecatalog(FALSE);
	}
	else
	{
		wrpipe(P_PRIME, &pipe, EXEC_DBU, (char *) 0, code);
		i = 0;
		j = Pc;
		while (j--)
			wrpipe(P_NORM, &pipe, W_dbu, Pv[i++], 0);
		wrpipe(P_END, &pipe, W_dbu);

		Error_flag = 0;
		if (errflag >= 0)
		{
			Synconly++;
			rdpipe(P_PRIME, &pipe);
			rdpipe(P_SYNC, &pipe, R_dbu);
			Synconly = 0;
		}
	}


	if (Error_flag && !errflag)
	{
		AAprargs(Pc, Pv);
		AAsyserr(12000, code, Error_flag);
	}
	return (Error_flag);

}


proc_error(s1, rpipnum)
struct pipfrmt	*s1;
int		rpipnum;

/*
**	Proc_error is called from rdpipe if an error
**	block is encountered. If Synconly is true then
**	the dbu request was initiated by decomp.
**	Otherwise the error block(s) are passed on up.
*/

{
	register struct pipfrmt	*s;
	register int		fd;
	struct pipfrmt		t;
	extern int		Synconly;
	extern int		Error_flag;

	fd = rpipnum;
	s = s1;

	if (Synconly)
	{
		Synconly = 0;
		Error_flag = s->err_id;
		rdpipe(P_SYNC, s, fd);		/*  throw away the error message  */
	}
	else
	{
		wrpipe(P_PRIME, &t, s->exec_id, (char *) 0, s->func_id);
		t.err_id = s->err_id;

		copypipes(s, fd, &t, W_up);
		rdpipe(P_PRIME, s);
	}
}

/*
**	SYSTEM RELATION DESCRIPTOR CACHE DEFINITION
**
**	This is defined here to prevent the ovqp definition (if any)
**	from overriding these definitions. This file is a direct
**	steal from ".../source/utility/catalog_desc.c"
*/

DESC			Reldes;
DESC			Attdes;
DESC			Inddes;


struct desxx	Desxx[] =
{
	AA_REL,	&Reldes,	&AAdmin.adreld,
	AA_ATT,	&Attdes,	&AAdmin.adattd,
	AA_INDEX,	&Inddes,	0,
	0
};


/*
**	override definition of userdestroy() for destroy() function.
**	Since destroy is only used by decomp to destroy SYSNAME relations,
**	userdestroy is not needed.
*/
userdestroy()
{
	return;
}
