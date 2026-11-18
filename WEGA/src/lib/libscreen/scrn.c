#include	<curses.h>
#include	<screen.h>

char	*crmsg = "Type <CR> to continue";
char	*pgmsg = "Type ^f for next page";
char	*qmsg = "Type ?<CR> for help";

wsavescrn(win)
WINDOW	*win;
{
	int	i, j;

	if (scrnflg)
		delwin(scrn);
	if ((scrn = newwin(win->_maxy, win->_maxx, win->_begy, win->_begx)) == NULL)
		return(FALSE);
	scrnflg = TRUE;
	scrn->_cury = win->_cury;
	scrn->_curx = win->_curx;
	scrn->_flags = win->_flags;
	scrn->_clear = win->_clear;
	scrn->_leave = win->_leave;
	scrn->_scroll = win->_scroll;
	for (i = 0; i < win->_maxy; i++) {
		for (j = 0; j < win->_maxx; j++)
			scrn->_y[i][j] = win->_y[i][j];
		scrn->_firstch[i] = win->_firstch[i];
		scrn->_lastch[i] = win->_lastch[i];
	}
	return(TRUE);
}

wresscrn(win)
WINDOW	*win;
{
	if (scrnflg && win->_maxy >= scrn->_maxy && win->_maxx >= scrn->_maxx) {
		overwrite(scrn, win);
		wrefresh(win);
		return(TRUE);
	}
	return(FALSE);
}

wscrolf(win)
WINDOW	*win;
{
}

wscrolb(win)
WINDOW	*win;
{
}

wpagefor(win, fp, top)
WINDOW	*win;
FILE	*fp;
int	top;
{
	int	i;
	char	c;
	char	buf[256];

	werase(win);
	wmove(win, top, 0);
	i = 0;
	while (fgets(buf, 256, fp) != NULL) {
		if (i++ >= win->_maxy - 1) {
			wmove(win, win->_maxy - 1, strlen(pgmsg));
			wmesg(win, pgmsg, (char *)0);
			if ((c = getkey()) != PAGEFOR)
				return;
			wrefresh(win);
			wclear(win);
			wmove(win, top, 0);
			i = 0;
		} else
			waddstr(win, buf);
	}
	for (i = i; i < win->_maxy - 1; i++) {
		wmove(win, i, 0);
		wclrtoeol(win);
	}
	wrefresh(win);
}

wpageback(win)
WINDOW	*win;
{
}

wcolon(win)
WINDOW	*win;
{
	int	y, x;
	char	c, c2;

	getyx(win, y, x);
	if (hilite)
		whighlight(win, FALSE);
	wmove(win, win->_maxy - 1, 0);
	wclrtoeol(win);
	waddch(win, COLON);
	wrefresh(win);
	do {
		if ((c = getkey()) == '\b') {
			bs(win);
			goback(win, y, x);
			return(0);
		}
		waddch(win, c);
		wrefresh(win);
		if ((c2 = getkey()) == '\b')
			bs(win);
	} while (c2 == '\b');
	goback(win, y, x);
	if (c2 != '\r')
		return(0);
	return(c);
}

whelp(win, file)
WINDOW	*win;
char	*file;
{
	int	y, x;
	char	c;
	FILE	*fp;

	getyx(win, y, x);
	if (hilite)
		whighlight(win, FALSE);
	wmove(win, win->_maxy - 1, 0);
	wclrtoeol(win);
	waddch(win, HELP);
	wrefresh(win);
	if ((c = getkey()) == '\b')
		bs(win);
	else if (c == '\r') {
		if ((fp = fopen(file, "r")) == NULL)
			goto bad;
		wsavescrn(win);
		wpagefor(win, fp, 0);
		fclose(fp);
		wmove(win, win->_maxy - 1, strlen(crmsg));
		wmesg(win, crmsg, (char *)0);
		while (getkey() != '\r')
			;
		if (!wresscrn(win))
			return(0);
	}
	goback(win, y, x);
	return(1);
bad:
	wmesg(win, "Cannot open %s", file);
	wmove(win, y, x);
	wrefresh(win);
	return(0);
}

goback(win, y, x)
WINDOW	*win;
int	y, x;
{
	wmove(win, win->_maxy - 1, 0);
	wclrtoeol(win);
	wmove(win, y, x);
	if (hilite)
		whighlight(win, TRUE);
	wrefresh(win);
}

bs(win)
WINDOW	*win;
{
	mvwaddch(win, win->_cury, win->_curx - 1, ' ');
	wmove(win, win->_cury, win->_curx - 1);
	wrefresh(win);
}
