# include	"form.h"

static int		_Error;

MMclr_error()
{
	if (!_Error)
		return (0);

	MMclr_line(1, 1);
	MM_repos();
	_Error = 0;

	return (1);
}


MMerror(error_text)
register char	*error_text;
{
	MMclr_line(1, 1);
	MM_ptis(error_text);
	MM_repos();
	_Error = 1;

	return (1);
}
