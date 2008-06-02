/* screen.h */

/*
 * screen.h
 *
 * this header file is part of the "screen-oriented" interface; see
 * the Design Objectives and Requirements for the "Screen-Oriented"
 * Interface 
 *
 */

#define	INFILE	0
#define	OUTFILE	1
#define	ERRFILE	2

#define	CTRL(c)		('c' & 0x1f)

#define	LEFT		'\b'
#define	RIGHT		'l'
#define	UP		'k'
#define	DOWN		'j'
#define	BACKWARD	'b'
#define	FORWARD		'f'
#define	WORD		'w'
#define	COLON		':'
#define	HELP		'?'
#define	PAGEFOR		CTRL(f)
#define	PAGEBACK	CTRL(b)

/*
 * pseudo functions for standard screen
 */
#define	mesg(s, d)			wmesg(stdscr, s, d)
#define	mvcursor(c, top, bottom)	wmvcursor(stdscr, c, top, bottom)
#define	forword(top, bottom)		wforword(stdscr, top, bottom)
#define	backword(top, bottom)		wbackword(stdscr, top, bottom)
#define	forspace(top, bottom)		wforspace(stdscr, top, bottom)
#define	backspace(top, bottom)		wbackspace(stdscr, top, bottom)		
#define	right(top, bottom)		wright(stdscr, top, bottom)
#define	left(top, bottom)		wleft(stdscr, top, bottom)
#define	highlight(flag)			whighlight(stdscr, flag)
#define	getword(str)			wgetword(stdscr, str)
#define	colon()				wcolon(stdscr)
#define	help(file)			whelp(stdscr, file)
#define	savescrn()			wsavescrn(stdscr)
#define	resscrn()			wresscrn(stdscr)
#define	scrolf()			wscrolf(stdscr)
#define	scrolb()			wscrolb(stdscr)
#define	pagefor(fp, top)		wpagefor(stdscr, fp, top)
#define	pageback()			wpageback(stdscr)

int	hilite;
int	scrnflg;

WINDOW	*scrn;
