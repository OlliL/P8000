# include	"form.h"

MMvalue_field(form, func)
register struct MM_form		*form;
int				(*func)();
{
	register int			len;

	if (form->f_mode)
		return (0);

	form->f_mode = F_VALUE;
	form->f_func = func;
	form->f_field[len = form->f_len] = 0;
	while (len)
		form->f_field[len] = ' ';

	return (1);
}
