# include	"form.h"

int			MMline	= 1;
int			MMcol	= 1;
int			MM_cs_cm;

# ifndef SIGN_EXT
# include	"../gutil/i1toi2.c"
# endif

# define	AApmove		MMpmove
# include	"../gutil/pmove.c"

# define	AAsmove		MMsmove
# include	"../gutil/smove.c"


MMclr_line(line, column)
register int			line;
register int			column;
{
	MM_pos(line, column);
	MM_pts(MM_cs[CS_CE]);

	return (1);
}


MMclr_screen(line, column)
register int			line;
register int			column;
{
	MM_pos(line, column);
	MM_pts(MM_cs[CS_CD]);

	return (1);
}


MM_pos(line, column)
register int			line;
register int			column;
{
	register char		*p;
	char			buf[21];
	if (MM_cs_cm)
	{
		line += MM_cs_cm;
		column += MM_cs_cm;
	}
	sprintf(p = buf, MM_cs[CS_CM], line, column);
	MM_pts(p);

	return (1);
}


MM_abs_pos(line, column)
register int			line;
register int			column;
{
	MMline = line;
	MMcol = column;

	return (MM_repos());
}


MM_rel_pos(column)
register int			column;
{
	MMcol += column;

	return (MM_repos());
}


MM_repos()
{
	return (MM_pos(MMline, MMcol));
}
