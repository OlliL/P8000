# include	"form.h"

MM_input(form)
register struct MM_form		*form;
{
	register char			*p;

	p = form->f_var;
	switch (form->f_mode & F_TYPE)
	{
	  case F_I2:
		*((short *) p) = 0;
		break;

	  case F_I4:
		*((long *) p) = 0;
		break;

# ifndef NO_F4
	  case F_F4:
		*((float *) p) = 0;
		break;
# endif

	  case F_F8:
		*((double *) p) = 0;
		break;

	  default:
		*p = 0;
	}
	form->f_field[form->f_len] = 0;
	if (form->f_format)
		sscanf(form->f_field, form->f_format, p);
	else
		MMsmove(form->f_field, form->f_var);

	return (1);
}


struct MM_form	*MM_need(frame)
register struct MM_form		*frame;
{
	register struct MM_form		*form;

	form = frame;
	do
	{
		if ((form->f_mode & F_NEED) && !(form->f_mode & F_FILLED))
		{
			MMerror(" BITTE Wert eingegeben ");
			return (form);
		}
	}
	while ((form = form->f_next) != frame);

	return ((struct MM_form *) 0);
}


MMin_field(form, func, help, need)
register struct MM_form		*form;
int				(*func)();
int				(*help)();
register int			need;
{
	register int			len;

	if (form->f_mode & F_FIELD)
		return (0);

	form->f_mode |= F_INPUT;
	if (need)
		form->f_mode |= F_NEED;
	form->f_func = func;
	form->f_help = help;
	form->f_field[len = form->f_len] = 0;
	while (len--)
		form->f_field[len] = ' ';

	return (1);
}
