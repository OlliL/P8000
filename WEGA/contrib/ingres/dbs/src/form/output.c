# include	"form.h"

MM_output(form)
register struct MM_form		*form;
{
	register char			*p;

	p = form->f_var;
	switch (form->f_mode & F_TYPE)
	{
	  case F_I1:
		sprintf(form->f_field, form->f_format, ctoi(p));
		break;

	  case F_I2:
		sprintf(form->f_field, form->f_format, *((short *) p));
		break;

	  case F_I4:
		sprintf(form->f_field, form->f_format, *((long *) p));
		break;

# ifndef NO_F4
	  case F_F4:
		sprintf(form->f_field, form->f_format, *((float *) p));
		break;
# endif

	  case F_F8:
		sprintf(form->f_field, form->f_format, *((double *) p));
		break;

	  default:
		if (form->f_format)
			sprintf(form->f_field, form->f_format, p);
		else
			MMpmove(p, form->f_field, form->f_len, ' ');
	}

	return (1);
}


MMout_field(form, output_text)
register struct MM_form		*form;
register char			*output_text;
{
	register int			len;

	if (form->f_mode & F_FIELD)
		return (0);

	form->f_mode |= F_OUTPUT;
	MMpmove(output_text, form->f_field, len = form->f_len, ' ');
	form->f_field[len] = 0;

	return (1);
}
