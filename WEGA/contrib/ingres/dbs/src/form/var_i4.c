# include	"form.h"

MMvari4(form, var, format)
register struct MM_form		*form;
register char			*var;
register char			*format;
{
	return (MM_var(form, var, format, F_I4));
}
