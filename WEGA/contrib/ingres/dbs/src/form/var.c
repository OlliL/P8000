# include	"form.h"

MM_var(form, var, format, type)
register struct MM_form		*form;
register char			*var;
register char			*format;
register int			type;
{
	if (!(form->f_mode & (F_INPUT | F_VALUE)) || (form->f_mode & F_TYPE))
		return (0);

	form->f_mode |= type;
	form->f_var = var;
	form->f_format = format;

	return (1);
}
