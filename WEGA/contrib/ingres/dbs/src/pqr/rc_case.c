/*
** ---  RC_CASE.C ------- UPDATE CASE JUMP TABLE  -----------------------------
**
**	Version: @(#)rc_case.c		4.0	02/05/89
**
*/

# include	"rc.h"


rc_case(rc)
register char	*rc;
{
	register int	i;
	register char	*rc_save;

	/* write CASE # and addr(CASE jump table) */
	rc_save = R_rc;
	R_rc = rc;
	RC_WORD(Case_no++);
	R_rc = rc_save;
	RC_UPDATE(rc + sizeof (short));

	/* update CASE jump table, relativ to table and write */
	rc = R_rc;
	for (i = 0; i < Case_no; i++)
		RC_WORD(Case[i] - rc);

	/* initialize new CASE */
	Case[0] = 0;
}
