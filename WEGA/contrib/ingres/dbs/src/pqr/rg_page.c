/*
** ---  RG_PAGE.C ------- PAGE EXCEPTIONS  ------------------------------------
**
**	Version: @(#)rg_page.c		4.0	02/05/89
**
*/

# include	"rg.h"


page_header()
{
	register int	len;
	char		save[MAX_LINE];
	extern char	*AAbmove();

# ifdef RG_TRACE
	if (TR_PAGE)
		printf("HEADER\tpage=%d\n", SYS_page);
# endif

	len = SYS_col - Line;

	/* save line */
	AAbmove(Line, save, len);

	line_begin();
	page_begin();

	/* restore line */
	SYS_col = AAbmove(save, SYS_col, len);
}


page_begin()
{
	register int	page;

# ifdef RG_TRACE
	if (TR_PAGE)
		printf("#PAGE\t%d\n", SYS_page);
# endif

	page = ++SYS_page;	/* new page */
	SYS_line = 1;
	RUNoutput = page < R_first_page || page > R_last_page;

	if (ONb_page)
	{
		Line_begin = 1;
		Line_end = MAX_I2;
		exception(&RUNpage, ONb_page, TRUE);
	}

	Line_begin = SYS_line;
	Line_end = Line_begin +  R_lines;
}


page_end()
{
	register int	i;

# ifdef RG_TRACE
	if (TR_PAGE)
		printf("PAGE#\t%d\n", SYS_page);
# endif

	if (ONe_page)
	{
		Line_begin = 1;
		Line_end = MAX_I2;
		exception(&RUNpage, ONe_page, TRUE);
	}

	/* check for too long page */
	i = R_all_lines - SYS_line + 1;
	while (i < 0)
	{
		i += R_all_lines;
		if (!i)
			i = R_all_lines;
	}
	for ( ; i; --i)
		rg_report("\n", 1);	/* empty lines */

	SYS_line = 0;
	SYS_col = Col_begin = Line;
	Col_end = Col_begin + R_cols;
}


new_page()
{
	register int	i;

# ifdef RG_TRACE
	if (TR_PAGE)
		printf("NEW\tpage%d\n", SYS_page);
# endif

	if (SYS_col != Col_begin)
		new_line();

	if (!SYS_line)
		return;

	for (i = Line_end - SYS_line; i > 0; --i)
		new_line();
}
