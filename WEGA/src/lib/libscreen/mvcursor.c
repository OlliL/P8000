#include	<curses.h>
#include	<screen.h>

wmvcursor(win, c, top, bottom)
WINDOW	*win;
char	c;
int	top, bottom;
{
	if (hilite)
		whighlight(win, FALSE);
	switch (c) {
	case DOWN:
		if (win->_cury == bottom) {
			wmove(win, top, win->_curx);
			wforspace(win, top, bottom);
			do {
				if (win->_curx == win->_maxx - 1)
					wmove(win, top, 0);
				else
					wmove(win, win->_cury, win->_curx + 1);
			} while (win->_y[win->_cury][win->_curx] == ' ');
		} else {
			wmove(win, win->_cury + 1, win->_curx);
			while (win->_y[win->_cury][win->_curx] == ' ')
				if (win->_cury == bottom)
					wmove(win, top, 0);
				else
					wmove(win, win->_cury + 1, win->_curx);
		}
		break;
	case UP:
		if (win->_cury == top) {
			if (win->_curx == 0) {
				wmove(win, win->_cury, win->_maxx - 1);
				wbackword(win, top, bottom);
				while (win->_y[win->_cury][win->_curx] != ' ')
					wmove(win, win->_cury + 1, win->_curx);
				wmove(win, win->_cury - 1, win->_curx);
			} else {
				wmove(win, bottom, win->_curx);
				wbackword(win, top, bottom);
			}
		} else
			wmove(win, win->_cury - 1, win->_curx);
		break;
	case BACKWARD:
		wbackword(win, top, bottom);
		break;
	case FORWARD:
	case WORD:
		wforword(win, top, bottom);
		break;
	default:
		if (hilite)
			whighlight(win, TRUE);
		wrefresh(win);
		return(FALSE);
	}
	if (hilite)
		whighlight(win, TRUE);
	wrefresh(win);
	return(TRUE);
}

wforword(win, top, bottom)
WINDOW	*win;
int	top, bottom;
{
	wforspace(win, top, bottom);
	while (win->_y[win->_cury][win->_curx] == ' ')
		if (win->_cury == bottom)
			if (win->_curx == win->_maxx - 1)
				wmove(win, top, 0);
			else
				wmove(win, win->_cury, win->_curx + 1);
		else if (win->_curx == win->_maxx - 1)
			wmove(win, win->_cury + 1, 0);
		else
			wmove(win, win->_cury, win->_curx + 1);
}

wbackword(win, top, bottom)
WINDOW	*win;
int	top, bottom;
{
	wbackspace(win, top, bottom);
	while (win->_y[win->_cury][win->_curx] == ' ')
		wleft(win, top, bottom);
	wbackspace(win, top, bottom);
	wforword(win, top, bottom);
}

wforspace(win, top, bottom)
WINDOW	*win;
int	top, bottom;
{
	while (win->_y[win->_cury][win->_curx] != ' ')
		wright(win, top, bottom);
}

wbackspace(win, top, bottom)
WINDOW	*win;
int	top, bottom;
{
	while (win->_y[win->_cury][win->_curx] != ' ')
		wleft(win, top, bottom);
}

wright(win, top, bottom)
WINDOW	*win;
int	top, bottom;
{
	if (win->_curx == win->_maxx - 1)
		if (win->_cury == bottom)
			wmove(win, top, 0);
		else
			wmove(win, win->_cury + 1, 0);
	else
		wmove(win, win->_cury, win->_curx + 1);
}

wleft(win, top, bottom)
WINDOW	*win;
int	top, bottom;
{
	if (win->_curx == 0)
		if (win->_cury == top)
			wmove(win, bottom, win->_maxx - 1);
		else
			wmove(win, win->_cury - 1, win->_maxx - 1);
	else
		wmove(win, win->_cury, win->_curx - 1);
}

whighlight(win, flag)
WINDOW	*win;
int	flag;
{
	int	x;
	char	*p;

	x = win->_curx;
	p = &win->_y[win->_cury][win->_curx + 1];
	wmove(win, win->_cury, win->_curx + 1);
	if (flag)
		wstandout(win);
	else
		wstandend(win);
	while (*p != ' ') {
		if (flag)
			waddch(win, *p);
		else
			waddch(win, *p & ~0200);
		p++;
	}
	wstandend(win);
	wmove(win, win->_cury, x);
	wrefresh(win);
}
