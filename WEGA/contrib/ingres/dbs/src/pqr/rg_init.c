/*
** ---  RG_INIT.C ------- INIT PROCEDURES  ------------------------------------
**
**	Version: @(#)rg_init.c		4.0	02/05/89
**
*/

# include	"rg.h"


struct init_t
{
	int		ip_low;
	int		ip_high;
	int		ip_default;
	char		*ip_error;
};

static struct init_t	Initialize[] =
{
/*	-----------------------------------------------
**	   low	   high	      default	 error
**	-----------------------------------------------	*/
	     1,	    512,       D_COLS,	 "COLUMNS",
	     1,	    512,      D_LINES,	 "LINES",
	     1,	    512,  D_ALL_LINES,	 "ALL LINES",
	MIN_I2,	 MAX_I2,    D_FOR_ALL,	 "FOR ALL END",
  D_FIRST_PAGE,	 MAX_I2, D_FIRST_PAGE,	 "FIRST	PAGE",
  D_FIRST_PAGE,	 MAX_I2,  D_LAST_PAGE,	 "LAST PAGE",
  D_FROM_TUPLE,	 MAX_I2, D_FROM_TUPLE,	 "FROM TUPLE",
  D_FROM_TUPLE,	 MAX_I2,   D_TO_TUPLE,	 "TO TUPLE",
/*	-----------------------------------------------	*/
	     0,	      0,	    0,	 (char *) 0,
};


rg_init()
{
	register struct	init_t	*i;
	register int		*ip;	/* param to check */
	register char		*msg;	/* error message  */

	/* go through R-CODE header, beginning with R_cols */
	ip = &R_cols;

	/* initialize parameters */
	for (i = Initialize; i->ip_error; i++)
	{
		msg = (char *) 0;
		if (*ip	< i->ip_low)
			msg = "SMALL";
		else if (*ip > i->ip_high)
			msg = "HIGH";
		if (msg)
		{
			rg_error("parameter %s = `%d' is too %s\n\tSET to `%d'",
				i->ip_error, *ip, msg, i->ip_default);
			*ip = i->ip_default;
		}
		ip++;
	}

	if (R_first_page > R_last_page)
		/* nothing to print */
		rg_exit(EXIT_END);

	/* adjust R_all_lines */
	if (*(ip = &R_all_lines) < R_lines)
		*ip *= R_lines / *ip + 1;

	/* rest	of initialization, see rg.c */
	R_rc = &R_code[1];
	Line_end = R_lines;
	Col_end	= &Line[R_cols];
}


ck_arg(a, i, c)
register char	*a;	/* the ASCII string	    */
/*register*/ int	*i;	/* holds the integer result */
int		*c;	/* error count		    */
{
	register int	err;
	short		ix;
	extern int	AA_atoi();

	err = AA_atoi(a, &ix);
	*i = ix;
	if (err)
	{
		if (err	< 0)
			rg_error("%d: VALUE `%s' not numeric", *c, a);
		else
			rg_error("%d: VALUE `%s' out of range", *c, a);
		(*c)++;
	}
}
