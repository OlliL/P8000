# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"

extern int	AAtTany;

/*
**  SET_SO_BUF -- set standard output buffer conditionally
**
**	This routine sets the standard output buffer conditionally,
**	based on whether trace flags are set or not.
*/
set_so_buf()
{
# ifdef SETBUF
	extern int	errno;
	static char	buffer[BUFSIZ];

#	ifdef xATR1
	if (AAtTany)
		return;
#	endif

	errno = 0;
	setbuf(stdout, buffer);
# endif
}
