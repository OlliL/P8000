# include	"gen.h"
# include	"need.h"

/*
**  FREEBUF.C -- more routines for LIFO dynamic buffer allocation [need.c]
**
**	These routines allow the deallocation of a need() type buffer,
**	and also using the same buffer for various SERIALIZED purposes
**	by marking the end of one, beginning of the next.
**
**	Defines:
**		freebuf()
**		markbuf()
**		seterr()
**
**	Requires:
**		that the buffer structure be IDENTICAL to that in [need.c]
**
**
**
**  MARKBUF -- Mark a place in the buffer to deallocate to
**
**	Parameters:
**		buf -- buffer
**
**	Returns:
**		int >= 0 marking place in buffer (should be used in calling
**			freebuf())
**
**	Side Effects:
**		none
*/
markbuf(buf)
register struct nodbuffer	*buf;
{
	return (buf->nleft);
}


/*
**  FREEBUF -- frees part of a buffer
**
**	Parameters:
**		buf -- buffer
**		bytes -- a previous return from markbuf().
*/
freebuf(buf, bytes)
register struct nodbuffer	*buf;
register int			bytes;
{
	register int			i;

	if ((i = bytes - buf->nleft) < 0)
		AAsyserr(14000, buf->nleft, bytes);
	buf->xfree -= i;
	buf->nleft += i;
}


/*
**  SETERR -- change the error info for a buffer
**
**	Parameters:
**		buf -- buffer
**		errnum -- new overflow error code
**		err_func -- new error handler
**
**	Side Effects:
**		adjusts buffer structure
*/
seterr(buf, errnum, err_func)
register struct nodbuffer	*buf;
register int			errnum;
int				(*err_func)();
{
	buf->err_num = errnum;
	buf->err_func = err_func;
}
