#include	<curses.h>
#include	<screen.h>

goraw()
{
	gtty(OUTFILE, &_tty);
	_tty.sg_flags |= CBREAK;
	_tty.sg_flags &= ~ECHO;
	_rawmode = TRUE;
	stty(OUTFILE, &_tty);
}

gonormal()
{
	_tty.sg_flags &= ~CBREAK;
	_tty.sg_flags |= ECHO;
	_tty.sg_flags |= CRMOD;
	_rawmode = FALSE;
	stty(OUTFILE, &_tty);
}

getkey()
{
	char	c;

	_tty.sg_flags &= ~CRMOD;
	stty(OUTFILE, &_tty);
	c = getchar() & 0177;
	_tty.sg_flags |= CRMOD;
	stty(OUTFILE, &_tty);
	if (c == '\r')
		return('\r');
	switch (c) {
	case '\b':
	case 'h':
		return(LEFT);
	case '\n':
	case 'j':
		return(DOWN);
	case CTRL(k):
	case 'k':
		return(UP);
	case CTRL(l):
	case ' ':
	case 'l':
		return(RIGHT);
	default:
		return(c);
	}
}

wgetword(win, str)
WINDOW	*win;
char	*str;
{
	char	*p;

	p = &win->_y[win->_cury][win->_curx];
	while ((*str = *p++ & ~0200) != ' ')
		str++;
	*str = '\0';
}
