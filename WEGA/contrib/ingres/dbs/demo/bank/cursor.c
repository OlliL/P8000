# include	<stdio.h>
# include	"cursor.h"

static int	blinking_on;
static int	revers_on;
static int	underline_on;
static int	bold_on;
static int	cursor_on;


on_blinking()
{
	if (blinking_on++)
		return;
	printf(ON_BLINKING);
}

off_blinking()
{
	if (--blinking_on)
		return;
	printf(OFF_BLINKING);
}

on_revers()
{
	if (revers_on++)
		return;
	printf(ON_REVERS);
}

off_revers()
{
	if (--revers_on)
		return;
	printf(OFF_REVERS);
}

on_underline()
{
	if (underline_on++)
		return;
	printf(ON_UNDERLINE);
}

off_underline()
{
	if (--underline_on)
		return;
	printf(OFF_UNDERLINE);
}

on_bold()
{
	if (bold_on++)
		return;
	printf(ON_BOLD);
}

off_bold()
{
	if (--bold_on)
		return;
	printf(OFF_BOLD);
}

cursor(line, col)
register int	line;
register int	col;
{
	printf(CURSOR, line, col);
}

on_cursor()
{
	if (cursor_on++)
		return;
	printf(ON_CURSOR);
}

off_cursor()
{
	if (--cursor_on)
		return;
	printf(OFF_CURSOR);
}

cls(line1, col1, line2, col2)
register int	line1;
register int	col1;
register int	line2;
register int	col2;
{
	printf(CLS, line1, col1, line2, col2);
}

box(line1, col1, line2, col2)
int		line1;
int		col1;
int		line2;
int		col2;
{
	register int	l;
	register int	c;

	cls(line1, col1, line2, col2);
	off_cursor();
	on_revers();
	on_blinking();

	for (l = line1; l <= line2; l++)
	{
		cursor(l, col1);
		if (l == line1 || l == line2)
			for (c = col1; c <= col2; c++)
				putchar(' ');
		else
		{
			putchar(' ');
			cursor(l, col2);
			putchar(' ');
		}
	}

	off_blinking();
	off_revers();
	on_cursor();
}
