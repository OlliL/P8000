# include	"form.h"

MMform(frame)
register struct MM_form		*frame;
{
	register struct MM_form		*form;
	register struct MM_form		*f;
	register int			ch;
	register int			input;
	register int			line;
	register int			col;

	MMframe(form = frame, 1, 1);
	for ( ; ; )
	{
		input = form->f_mode & F_INPUT;
		if (!(form->f_mode & (F_INPUT | F_MENUE)))
		{
			form = form->f_next;
			continue;
		}
		MM_repos();
		MM_pts(MM_cs[CS_SE]);
		MM_abs_pos(form->f_line, form->f_col);
		MM_pts(MM_cs[CS_SO]);
		if (input)
			MM_abs_pos(form->f_line, form->f_col + 1);
		else
			MM_repos();
		switch (ch = MM_getchar())
		{
		  case C_QUIT:
			MMclr_screen(2, 1);
			return (1);

		  case C_MENUE:
			if (input)
				continue;

			if (f = MM_need(frame))
			{
				form = f;
				continue;
			}

			f = form;
			do
			{
				f->f_mode &= ~F_FILLED;
			}
			while ((f = f->f_next) != form);

			if (form->f_func)
				(*form->f_func)(frame);
			continue;

		  case C_FIELD:
			if (input)
				goto edit;
			while (form = form->f_next)
				if (form->f_mode & F_START)
					break;
			continue;

		  case C_NEXT:
			MMline = form->f_line;
			MMcol = form->f_col;
			form = form->f_next;
			continue;

		  case C_PREV:
			MMline = form->f_line;
			MMcol = form->f_col;
			while (form = form->f_prev)
				if (form->f_mode & (F_INPUT | F_MENUE))
					break;
			continue;

		  default:
			if (input)
			{
edit:
				MM_backup(ch);
				MM_edit(form);
			}
			else
				MMerror(" UNGUELTIGES ZEICHEN ");
		}
	}
}
