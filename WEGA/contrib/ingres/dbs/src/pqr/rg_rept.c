/*
** ---  RG_REPT.C ------- REPEAT PROCESSOR  -----------------------------------
**
**	Version: @(#)rg_rept.c		4.0	02/05/89
**
*/

# include	"rg.h"

/* the repeat	table */
static REPEATYPE	R_repeat[MAX_REPEAT];	/* rr_code =  (char *) 0 */
						/* rr_count = 0          */


rg_repeat(rc)
register char	*rc;
{
	register REPEATYPE	*r;
	register int		i;

	for (r = R_repeat; r->rr_count; r++)
	{
		if (r == &R_repeat[MAX_REPEAT])
		{
			rg_error("REPEAT TABLE OVERFLOW");
			rg_exit(EXIT_R_CODE);
		}
		if (r->rr_code == rc)
			break;
	}

	if (!r->rr_code)
	{
		r->rr_code = rc;
		if ((i = R_val) < A_REPEAT)
			i = A_REPEAT;
		r->rr_count = i;
	}

	if (--(r->rr_count))
		return (TRUE);
	r->rr_code = 0;
	return (FALSE);
}
