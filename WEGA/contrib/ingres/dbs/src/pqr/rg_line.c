/*
** ---  RG_LINE.C ------- LINE EXCEPTIONS  ------------------------------------
**
**	Version: @(#)rg_line.c		4.0	02/05/89
**
*/

# include	"rg.h"

static char	Ovfline[MAX_LINE];
static char	*SYS_ovfl = Ovfline;

line_begin()
{
	register int	save;

# ifdef RG_TRACE
	if (TR_LINE)
		printf("#LINE\t%d\n", SYS_line);
# endif

	SYS_line++;	/* new line */
	SYS_col = Line;

	if (ONb_line)
	/*	==========---------------	*/
	/*	| HEADER | Text[R_cols] |	*/
	/*	==========---------------	*/
	{
		Col_begin = Line;
		Col_end = &Line[MAX_LINE - R_cols];
		save = Line_begin;
		Line_begin = 1;
		exception(&RUNline, ONb_line, FALSE);
		Line_begin = save;
	}

/*	---------================	*/
/*	| HEADER | Text[R_cols] |	*/
/*	---------================	*/

	Col_begin = SYS_col;
	Col_end = Col_begin + R_cols;
}


line_end()
{
	register int	save;
	extern char	*AApad();

# ifdef RG_TRACE
	if (TR_LINE)
		printf("LINE#\t%d\n", SYS_line);
# endif

	if (!ONe_line)
		return;

	if ((save = Col_end - SYS_col) > 0)
		SYS_col = AApad(SYS_col, save);

	Col_begin = Line;
	Col_end = &Line[MAX_LINE];
	save = Line_begin;
	Line_begin = 1;
	exception(&RUNline, ONe_line, FALSE);
	Line_begin = save;
}


set_line(line)
register int	line;
{
# ifdef RG_TRACE
	if (TR_LINE)
		printf("SET\tline %d to %d\n", SYS_line, line);
# endif

	if (RUNline)
	{
		RUN++;
		return;
	}

	/* set to line begin */
	if (SYS_col != Col_begin)
		new_line();

	/* check for valid line count */
	if (line <= 0 || line > R_lines)
		line = 0;
	else
		--line;

	while (line != SYS_line - Line_begin)
	{
		new_line();
		if (RUN)
			return;
	}
}


new_line()
{
	register char	*p;
	register int	c;
	register int	l;
	int		len;

# ifdef RG_TRACE
	if (TR_LINE)
		printf("NEW\tline %d\n", SYS_line);
# endif

	if (!SYS_line)
		page_header();

	line_end();

	if ((len = l = SYS_col - (p = Line)) > 0)
	{
		for ( ; l; --l)
		{
			if ((c = *p) < ' ')
				*p = ' ';
			else if (R_big && ((c >= 'a'
# ifdef EBCDIC
				   && c <= 'i') || (c >= 'j'
				   && c <= 'r') || (c >= 's'
# endif
				   && c <= 'z')))
					*p = c + 'A' - 'a';
# ifndef EBCDIC
			*p &= ASCII_MASK;
# endif
			p++;
		}
	}

	*p = '\n';
	rg_report(Line, ++len);

	line_begin();

	if (SYS_line >= Line_end)
		page_end();

	batch_line();
}


check_line()
{
	register char	*from;
	register char	*to;
	register int	len;
	register int	count;
	extern char	*AAbmove();

	while (SYS_col > (from = Col_end))
	{
		if (RUNline)
		{	/* abort exception */
			SYS_col = from;
			RUN++;
			return;
		}
		len = SYS_col - from;
		SYS_ovfl = AAbmove(SYS_col = from, to = SYS_ovfl, len);
		count = batch_count(0);
		new_line();
		batch_count(count);
		SYS_col = AAbmove(SYS_ovfl = to, SYS_col, len);
	}
}
