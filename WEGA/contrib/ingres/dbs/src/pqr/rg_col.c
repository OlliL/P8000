/*
** ---  RG_COL.C -------- COLUMN PROCESSOR  -----------------------------------
**
**	Version: @(#)rg_col.c		4.0	02/05/89
**
*/

# include	"rg.h"


set_col(col)
register int	col;
{
	register int	len;
	extern char	*AApad();

	if (--col < 0)
		return;

	if ((len = Col_begin + col - SYS_col) < 0)
	{
		if (RUNline)
			goto ign_col;
		new_line();
		if (RUN)
		{
ign_col:
			RUN++;
			return;
		}
		len = Col_begin	- Line + col;
	}
	if (len)
		SYS_col = AApad(SYS_col, len);
}
