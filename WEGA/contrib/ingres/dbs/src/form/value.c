# include	"form.h"

static struct MM_form	*_find(frame, var, no)
register struct MM_form		*frame;
register char			*var;
register int			no;
{
	register struct MM_form		*form;

	while (!(frame->f_mode & F_START))
		frame = frame->f_next;
	form = frame;
	do
	{
		if (form->f_var == var && no-- == 0)
		{
			MM_output(form);
			return (form);
		}
	}
	while ((form = form->f_next) != frame);

	return ((struct MM_form *) 0);
}


static int		_value(form)
register struct MM_form		*form;
{
	if (form->f_mode & F_VALUE)
	{
		MM_pos(form->f_line, form->f_col + 1);
		MM_pta(form->f_field, form->f_len);

		return (1);
	}

	return (0);
}


MMclr_value(frame)
register struct MM_form		*frame;
{
	register struct MM_form		*form;

	form = frame;
	do
	{
		if (form->f_mode & F_VALUE)
			MMpmove(" ", form->f_field, form->f_len, ' ');
	}
	while ((form = form->f_next) != frame);

	return (1);
}


MMset_field(frame, var, no)
register struct MM_form		*frame;
register char			*var;
register int			no;
{
	register struct MM_form		*form;

	if ((form = _find(frame, var, no)) && _value(form))
		return (MM_repos());

	return (0);
}


MMset_value(frame, var, no)
register struct MM_form		*frame;
register char			*var;
register int			no;
{
	if (frame = _find(frame, var, no))
		return (frame->f_func == (int (*)()) 0 || (*frame->f_func)(frame));

	return (0);
}


MMval_field(form, func)
register struct MM_form		*form;
int				(*func)();
{
	if (form->f_mode & F_FIELD)
		return (0);

	form->f_mode |= F_VALUE;
	form->f_func = func;
	MMpmove(" ", form->f_field, form->f_len, ' ');
	form->f_field[form->f_len] = 0;

	return (1);
}


MMvalue(frame)
register struct MM_form		*frame;
{
	register struct MM_form		*form;

	form = frame;
	do
	{
		_value(form);
	}
	while ((form = form->f_next) != frame);
	MM_repos();

	return (1);
}
