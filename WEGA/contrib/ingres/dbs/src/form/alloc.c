# include	"form.h"

struct MM_form	*MMalloc(frame, line, col, len, var)
register struct MM_form		**frame;
int				line;
int				col;
int				len;
char				*var;
{
	register struct MM_form		*form;
	register struct MM_form		*f;
	register int			i;
	extern char			*calloc();

	i = sizeof (struct MM_form);
	if (!var)
		i += len + 1;
	if (!(f = (struct MM_form *) calloc(1, i)))
		return ((struct MM_form *) 0);

	if (form = *frame)
	{
		f->f_next = form;
		f->f_prev = form->f_prev;
		form->f_prev->f_next = f;
		form->f_prev = f;
	}
	else
	{
		*frame = f;
		f->f_next = f;
		f->f_prev = f;
		f->f_mode = F_START;
	}

	f->f_line = line;
	f->f_col = col;
	f->f_len = len;
	f->f_field = var? var: ((char *) f) + sizeof (struct MM_form);

	return (f);
}
