# include	"form.h"

MMmenue_field(form, func, help, menue_text)
register struct MM_form		*form;
int				(*func)();
int				(*help)();
register char			*menue_text;
{
	register int			len;

	if (form->f_mode)
		return (0);

	form->f_mode = F_MENUE;
	form->f_func = func;
	form->f_help = help;
	AApmove(menue_text, form->f_field, len = form->f_len, ' ');
	form->f_field[len] = 0;

	return (1);
}
