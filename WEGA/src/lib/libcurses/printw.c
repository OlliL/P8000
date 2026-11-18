/*
 * printw and friends
 *
 * 1/26/81 (Berkeley) @(#)printw.c	1.1
 */

# include	"curses.ext"

/*
 *	This routine implements a printf on the standard screen.
 */
printw(fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)
char	*fmt, *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8, *a9, *a10; {

	return _sprintw(stdscr, fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
}

/*
 *	This routine implements a printf on the given window.
 */
wprintw(win, fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)
WINDOW	*win;
char	*fmt, *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8, *a9, *a10; {

	return _sprintw(win, fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
}
/*
 *	This routine actually executes the printf and adds it to the window
 */
_sprintw(win, fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)
WINDOW	*win;
char	*fmt, *a1, a2, a3, a4, a5, a6, a7, a8, a9, a10; {

	char	buf[256];

	sprintf(buf, fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
	return waddstr(win, buf);
}
