# include	"form.h"

MMvarf4(form, var, format)
register struct MM_form		*form;
register char			*var;
register char			*format;
{
# ifdef NO_F4
	return (MM_var(form, var, format, F_F8));
# else
	return (MM_var(form, var, format, F_F4));
# endif
}
