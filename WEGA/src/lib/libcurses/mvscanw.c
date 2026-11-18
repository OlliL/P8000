# include	"curses.ext"

/*
 * implement the mvscanw commands.  Due to the variable number of
 * arguments, they cannot be macros.  Another sigh....
 *
 * 5/17/83 (Berkeley) @(#)mvscanw.c	1.2
 */

mvscanw(y, x, fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)
reg int		y, x;
char		*fmt, *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8, *a9, *a10; {

	return move(y, x) == OK ? _sscans(stdscr, fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) : ERR;
}

mvwscanw(win, y, x, fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)
reg WINDOW	*win;
reg int		y, x;
char		*fmt, *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8, *a9, *a10; {

	return wmove(win, y, x) == OK ? _sscans(win, fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) : ERR;
}
