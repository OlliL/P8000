# include	"form.h"

MMframe(frame, line, col)
register struct MM_form		*frame;
int				line;
int				col;
{
	register struct MM_form		*form;
	register int			invers;

	MMclr_screen(2, 1);
	form = frame;
	do
	{
		if (form->f_len)
		{
			if (invers = form->f_mode & (F_INPUT | F_MENUE))
			{
				MM_pos(form->f_line, form->f_col);
				MM_pts(MM_cs[(form == frame)? CS_SO: CS_SE]);
			}
			MM_pos(form->f_line, form->f_col + 1);
			MM_pta(form->f_field, form->f_len);
			if (invers)
				MM_pts(MM_cs[CS_SE]);
		}
	}
	while ((form = form->f_next) != frame);
	MM_abs_pos(line, col);

	return (1);
}
