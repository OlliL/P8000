# include	"curses.ext"

/*
 * implement the mvprintw commands.  Due to the variable number of
 * arguments, they cannot be macros.  Sigh....
 *
 * 1/26/81 (Berkeley) @(#)mvprintw.c	1.1
 */

mvprintw(y, x, fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)
reg int		y, x;
char		*fmt, *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8, *a9, *a10; {

	return move(y, x) == OK ? _sprintw(stdscr, fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) : ERR;
}

mvwprintw(win, y, x, fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)
reg WINDOW	*win;
reg int		y, x;
char		*fmt, *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8, *a9, *a10; {

	return wmove(win, y, x) == OK ? _sprintw(win, fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) : ERR;
}
