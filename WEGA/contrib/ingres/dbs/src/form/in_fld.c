# include	"form.h"

MMinput_field(form, func, help, need)
register struct MM_form		*form;
int				(*func)();
int				(*help)();
register int			need;
{
	register int			len;

	if (form->f_mode)
		return (0);

	form->f_mode = F_INPUT;
	if (need)
		form->f_mode |= F_NEED;
	form->f_func = func;
	form->f_help = help;
	form->f_field[len = form->f_len] = 0;
	while (len)
		form->f_field[len] = '.';

	return (1);
}
