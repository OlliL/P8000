# include	"form.h"

MMfree(frame)
register struct MM_form		**frame;
{
	register struct MM_form		*form;
	register struct MM_form		*f;

	if (form = *frame)
	{
		do
		{
			f = form->f_next;
			free(form);
		} while ((form = f) != *frame);

		*frame = (struct MM_form *) 0;
	}

	return (1);
}
