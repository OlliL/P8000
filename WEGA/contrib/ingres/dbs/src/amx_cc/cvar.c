# include	"amx.h"

init_ctbl()
{
	register struct cvartbl	*v;
	register int		i;

	for (i = 0, v = Cvartbl; i < MAXCVAR + 1; i++, v++)
		*v->varname = 0;
	Cvarindex = 0;
}


/*
**	integer function to look up variable in c variable table
**	return value	=  value in table if variable appears
**			=  - 1 if variable not declared
*/
c_lookup(vble)
register char	*vble;
{
	register struct cvartbl	*v;
	register int		i;

	for (i = 0, v = Cvartbl; i < MAXCVAR + 1 && *v->varname; i++, v++)
		if (AAsequal(vble, v->varname))
			return (i);

	return (-1);
}


decl(vble, scan_var)
register char	*vble;
int		scan_var;
{
	register struct cvartbl	*v;
	register AMX_FRM	*f;
	register int		i;

	f = scan_var? &Source: &Destination;

	if ((i = c_lookup(vble)) < 0)
	{
		f->type = NOTYPE;
		amxerror(70, vble);
		return;
	}

	v = &Cvartbl[i];
	f->off = C_VAR;
	f->type = v->vartype;
	switch (v->vartype)
	{
	  case opINT:
		f->len = sizeof (short);
		break;

	  case opLONG:
		f->len = sizeof (long);
		break;

# ifndef NO_F4
	  case opFLT:
		f->len = sizeof (float);
		break;
# endif

	  case opDOUBLE:
		f->len = sizeof (double);
		break;

	  case opSTRING:
		f->len = MAXFIELD;
		break;

	  default:
		f->type = NOTYPE;
		yyexit(2, v->vartype, v->varname);
	}
	f->c_var = v->varname;
}


/*
**	insert variable in c variable table
*/
ctbl(vble, vtype)
register char	*vble;
int		vtype;
{
	register struct cvartbl	*v;

	if (Cvarindex == MAXCVAR + 1)
		amxerror(71, vble);
	else if (c_lookup(vble) < 0)
	{
		v = &Cvartbl[Cvarindex++];
		AAsmove(vble, v->varname);
		v->vartype = vtype;
	}
	else
		amxerror(73, vble);
}
