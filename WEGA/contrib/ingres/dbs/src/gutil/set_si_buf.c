# include	<stdio.h>
# include	"gen.h"

/*
**  SET_SI_BUF -- set standard input buffer unconditionally
**
**	This routine sets the standard input buffer to give
**	buffered input.  The buffer is contained internally.
*/
set_si_buf()
{
# ifdef SETBUF
	static char	buffer[BUFSIZ];

	setbuf(stdin, buffer);
# endif
}
