# include	"form.h"

static int	_del_ch(field, length)
register char			*field;
register int			length;
{
	register char			*p;

	for (p = field; length; --length, p++)
		*p = *(p + 1);
	*p = ' ';
	MM_pts(field);
	MM_repos();

	return (1);
}


static int	_ins_ch(field, length)
register char			*field;
register int			length;
{
	register char			*p;

	for (p = field + length; length; --length, --p)
		*p = *(p - 1);
	*field = ' ';
	MM_pts(field);
	MM_repos();

	return (1);
}


MM_edit(form)
register struct MM_form		*form;
{
	register int			ch;
	register char			*t;
	register int			p;
	register int			l;
	register int			line;
	register int			col;

	t = form->f_field;
	l = form->f_len - 1;
	p = 0;
	for ( ; ; )
	{
		switch (ch = MM_getchar())
		{
		  case C_HELP:
		  case C_MENUE:
			if (form->f_help)
			{
				line = MMline;
				MMline = 1;
				col = MMcol;
				MMcol = 1;
				MMclr_screen(1, 1);
				(*form->f_help)(form);
				MMclr_screen(2, 1);
				MMframe(form, line, col);
			}
			continue;

		  case C_NEXT:
		  case C_PREV:
			if (form->f_mode & F_SENDED)
			{
				MM_input(form);
				if (form->f_func && !(*form->f_func)(form))
					continue;
				form->f_mode &= ~F_SENDED;
			}
			/* fall through */

		  case C_QUIT:
			MM_backup(ch);
			MMline = form->f_line;
			MMcol = form->f_col;
			return (1);

		  case C_FIELD:
			ch = -p;
rel_pos:
			t += ch;
			MM_rel_pos(ch);
			p += ch;
			continue;

		  case C_LEFT:
			ch = p? -1: l;
			goto rel_pos;

		  case C_RIGHT:
right_pos:
			ch = (p == l)? -p: 1;
			goto rel_pos;

		  case C_DELETE:
			form->f_mode |= F_SENDED | F_FILLED;
			_del_ch(t, l - p);
			continue;

		  case C_INSERT:
			form->f_mode |= F_SENDED | F_FILLED;
			_ins_ch(t, l - p);
			continue;

		  default:
			if (ch >= ' ' && ch < 0177)
			{
				form->f_mode |= F_SENDED | F_FILLED;
				*t = ch;
				write(1, t, 1);
				goto right_pos;
			}
			MMerror(" UNGUELTIGES ZEICHEN ");
		}
	}
}
