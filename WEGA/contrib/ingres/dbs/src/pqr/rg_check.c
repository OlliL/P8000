/*
** ---  RG_CHECK.C ------ SEVERAL SYNTAX CHECKS  ------------------------------
**
**	Version: @(#)rg_check.c		4.0	02/05/89
**
*/

# include	"rg_int.h"


char	*rc_word(rc)
register char	*rc;
{
	R_word = RC_BYTE << BYTE_LOG2;
	R_word += RC_BYTE;
	return (rc);
}


var_to_int()
/* if VAR is a variable, VAR_TO_INT() returns its */
/* contents as integer, else value of VAR         */
{
	return ((R_word & VAR_MASK)? Var[ck_var()].f8type: R_word);
}


ck_var()
/* returns the var#, if it's a legal no, */
/* else A_VAR (standard variable)        */
{
	register int	var;

	if ((var = R_word) & VAR_MASK)
	{
		if ((var & VAR_VAR_MASK) == VAR_VAR_MASK)
			var = Var[var & VAR_VALUE].f8type;
		else
			var &= VAR_VALUE;
		if (var >= 0 && var < MAX_VAR)
			return (var);
	}
	rg_error( "ILLEGAL VAR `%d'\n\tassume VAR %d", var, A_VAR);
	return (A_VAR);
}


ck_bvar()
/* returns the switch#, if it's a legal no, */
/* else A_BVAR (standard switch)            */
{
	register int	bvar;

	if ((bvar = R_word) & VAR_MASK)
		bvar = Var[ck_var(bvar)].f8type;
	if (bvar >= 0 || bvar < MAX_BVAR)
		return (bvar);
	rg_error("ILLEGAL SWITCH `%d'\n\tassume SWITCH %d", bvar, A_BVAR);
	return (A_BVAR);
}


ck_len(len, from, to)
register int	*len;
int		from;
int		to;
/* returns the start position of substring in string, */
/* the length LEN is modified too                     */
{
	register int	f;
	register int	t;

	f = from;
	if (--f	< A_FROM)
		f = A_FROM;

	if (!(t = to))
		t = A_TO;

	if (t > *len)
		t = *len;

	if (f >= t)
	{
		rg_error("ILLEGAL SUBSTRING [`%d'..`%d']\n\tassume [1..%d]",
			from, to, *len);
		return (0);
	}

	*len = t - f;
	return (f);
}


ck_frm(s, frmt, frml)
register SYMBOL	*s;
register int	frmt;
register int	frml;
/* returns  0 (FALSE)  for ok */
/*    else  1 (TRUE)          */
{
	extern char		*print_type();

	if (s->sy_frmt	== frmt	&& ctou(s->sy_frml) == frml)
		return (FALSE);

	rg_error("WRONG FORMAT %s%d, expected %s%d",
		print_type(s->sy_frmt), ctou(s->sy_frml), print_type(frmt), frml);
	return (TRUE);
}


ck_2_frm(s1, s2, frmt, frml, msg)
register SYMBOL	*s1;
SYMBOL		*s2;
register int	frmt;
register int	frml;
char		*msg;
/* returns  0 (FALSE)  for ok */
/*    else  1 (TRUE)          */
{
	if (ck_frm(s1, frmt, frml) || ck_frm(s2, frmt, frml))
	{
		rg_error("%s: expression aborted", msg);
		return (TRUE);
	}
	return (FALSE);
}


ck_val1(new_val)
register int	new_val;
/* check if R_val in RANGE[1 .. MAX_FIELD] */
/* returns NEW_VAL if R_val out of range   */
/*    else R_val                           */
{
	register int	v;

	if ((v = R_val)	< 1 || v > MAX_FIELD)
		R_val =	v = new_val;
	return (v);
}


DESC_T	*ck_dom(dom)
register int	dom;
{
	register DESC_T		*d;
	register int		frmt;
	extern char		*print_type();

	if (dom	<= 0 || dom > Doms)
	{
		rg_error("ILLEGAL DOMAIN [1..`%d'..%d]\n\tassume DOMAIN %d",
			dom, Doms, A_DOM + 1);
		dom = A_DOM;
	}
	else
		--dom;

	dom = ctou((d = &Desc[dom])->domfrml);
	switch (frmt = d->domfrmt)
	{
	  case INT:
		switch (dom)
		{
		  case sizeof (char):
		  case sizeof (short):
		  case sizeof (long):
			break;

		  default:
			goto ill_type;
		}
		break;

	  case FLOAT:
		switch (dom)
		{
# ifndef NO_F4
		  case sizeof (float):
# endif
		  case sizeof (double):
			break;

		  default:
			goto ill_type;
		}
		break;

	  case CHAR:
		if (dom)
			break;
		/* fall through */

	  default:
ill_type:
		rg_error("ILLEGAL TYPE %s%d", print_type(frmt), dom);
		rg_exit(EXIT_DESC);
	}

	return (d);
}
