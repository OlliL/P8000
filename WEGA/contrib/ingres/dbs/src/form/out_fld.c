# include	"form.h"

MMoutput_field(form, output_text)
register struct MM_form		*form;
register char			*output_text;
{
	register int			len;

	if (form->f_mode)
		return (0);

	form->f_mode = F_OUTPUT;
	AApmove(output_text, form->f_field, len = form->f_len, ' ');
	form->f_field[len] = 0;

	return (1);
}
