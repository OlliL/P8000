#include	<curses.h>

wmesg(win, s, d)
WINDOW	*win;
char	*s;
char	*d;
{
	int	y, x;

	getyx(win, y, x);
	wmove(win, win->_maxy - 1, 0);
	wstandout(win);
	if (d == 0)
		wprintw(win, s);
	else
		wprintw(win, s, d);
	wstandend(win);
	wclrtoeol(win);
	wmove(win, y, x);
	wrefresh(win);
}
