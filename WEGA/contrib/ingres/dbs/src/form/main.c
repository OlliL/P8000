#  include	<sgtty.h>
# include	"form.h"

# define	cbreak()	(_Tty.sg_flags |= CBREAK)
# define	no_cbreak()	(_Tty.sg_flags &= ~CBREAK)

# define	echo()		(_Tty.sg_flags |= ECHO)
# define	no_echo()	(_Tty.sg_flags &= ~ECHO)

# define	nl()		(_Tty.sg_flags |= CRMOD)
# define	no_nl()		(_Tty.sg_flags &= ~CRMOD)

# define	raw()		(_Tty.sg_flags |= RAW)
# define	no_raw()	(_Tty.sg_flags &= ~RAW)

# define	tabs()		(_Tty.sg_flags |= XTABS)
# define	no_tabs()	(_Tty.sg_flags &= ~XTABS)

# define	reset_tty()	(_Tty.sg_flags = _Tty_flags)
# define	save_tty()	(_Tty_flags = _Tty.sg_flags)

# define	get_tty()	(gtty(_Tty_fd, &_Tty))
# define	set_tty()	(stty(_Tty_fd, &_Tty))

static int		_Tty_fd;
static int		_Tty_flags;
static struct sgttyb	_Tty;


MMinit_form(clear)
register int			clear;
{
	register int			file;
	register char			*p;
	register int			code;
	char				buf[255];
	extern char			*getenv();

	if (!(p = getenv("TERM")))
		return (0);
	MMsmove(p, &buf[MMsmove("/usr/lib/form/", buf)]);
	if ((file = open(buf, 0)) < 0)
		return (0);

	for (_Tty_fd = 0; _Tty_fd < 20; _Tty_fd++)
		if (isatty(_Tty_fd))
			break;
	if ((get_tty() < 0) || (set_tty() < 0))
		return (0);

	save_tty();

	no_cbreak();
	no_echo();
	no_nl();
	raw();
	no_tabs();

	set_tty();
        ioctl(0, TIOCFLUSH, 0);
        ioctl(1, TIOCFLUSH, 1);

	for ( ; ; )
	{
		for (p = buf; ; p++)
		{
			if (read(file, p, 1) != 1)
			{
				close(file);
				if (clear)
					MMclr_screen(1, 1);
				return (1);
			}
			if (*p == '\n')
			{
				*p++ = 0;
				*p = 0;
				break;
			}
		}
		p = buf;
		switch (*p++)
		{
		  case 'C':
			code = C_CONTINUE;
			break;

		  case 'D':
			code = C_DELETE;
			break;

		  case 'F':
			code = C_FIELD;
			break;

		  case 'H':
			code = C_HELP;
			break;

		  case 'I':
			code = C_INSERT;
			break;

		  case 'L':
			code = C_LEFT;
			break;

		  case 'M':
			code = C_MENUE;
			break;

		  case 'N':
			code = C_NEXT;
			break;

		  case 'P':
			code = C_PREV;
			break;

		  case 'Q':
			code = C_QUIT;
			break;

		  case 'R':
			code = C_RIGHT;
			break;

		  case 'c':
			switch (*p++)
			{
			  case 'd':
				code = CS_CD;
				break;

			  case 'e':
				code = CS_CE;
				break;

			  case 'm':
				code = CS_CM;
				break;

			  default:
				continue;
			}
			break;

		  case 's':
			switch (*p++)
			{
			  case 'e':
				code = CS_SE;
				break;

			  case 'o':
				code = CS_SO;
				break;

			  default:
				continue;
			}
			break;

		  default:
			continue;
		}
		if (*p++ != '\t')
			continue;

		if (code < 0)
			MM_incode(p, code);
		else
			MM_outcode(p, code);
	}
}


MMexit_form(clear)
register int			clear;
{
	if (clear)
		MMclr_screen(1, 1);

	reset_tty();
	set_tty();
        ioctl(0, TIOCFLUSH, 0);
        ioctl(1, TIOCFLUSH, 1);

	return (1);
}
