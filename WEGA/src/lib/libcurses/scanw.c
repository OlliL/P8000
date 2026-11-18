/*
 * scanw and friends
 *
 * 1/26/81 (Berkeley) @(#)scanw.c	1.1
 */

# include	"curses.ext"

/*
 *	This routine implements a scanf on the standard screen.
 */
scanw(fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)
char	*fmt, *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8, *a9, *a10; {

	return _sscans(stdscr, fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
}
/*
 *	This routine implements a scanf on the given window.
 */
wscanw(win, fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)
WINDOW	*win;
char	*fmt, *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8, *a9, *a10; {

	return _sscans(win, fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
}
/*
 *	This routine actually executes the scanf from the window.
 */
_sscans(win, fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)
WINDOW	*win;
char	*fmt, *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8, *a9, *a10; {

	char	buf[100];

	if (wgetstr(win, buf) == ERR)
		return ERR;
	sscanf(buf, fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
	return OK;
}
