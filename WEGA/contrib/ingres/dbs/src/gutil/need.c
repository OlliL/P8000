# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"need.h"

/*
**  NEED.C -- general buffer allocation routines
**
**	allow buffers with LIFO de-allocation
**
**	Defines:
**		need()
**		initbuf()
**
**
**
**  NEED -- allocate space from a buffer
**
**	On buffer overflow, calls err_func from that field
**	in the buffer with the error code err_code from that field
**	in the buffer, then returns 0.
**	need() guarantees an even adress on return.
**
**	Parameters:
**		bf -- buffer
**		nbytes -- number of bytes desired
**
**	Returns:
**		pointer to allocated area
**		on  buffer overflow returns 0.
**
**	Side Effects:
**		adjusts buffer structure to reflect allocation.
*/
char	*need(buf, nbytes)
register struct nodbuffer 	*buf;
register int			nbytes;
{
	register char			*x;

	nbytes = ALIGNMENT(nbytes);
	if (nbytes > buf->nleft)
	{
		(*buf->err_func)(buf->err_num);
		return (0);
	}
	x = buf->xfree;
	buf->xfree += nbytes;
	buf->nleft -= nbytes;
	return (x);
}


/*
**  INITBUF -- initialize a buffer
**
**	Must be called before the first need() call on the buffer.
**
**	Parameters:
**		bf -- buffer
**		size -- size fo buffer area
**		err_num -- error code for overflow
**		err_func -- function to call with err_code on error
**
**	Returns:
**		none
**
**	Side Effects:
**		initializes buffer structure
*/
initbuf(buf, size, err_num, err_func)
register struct nodbuffer	*buf;
register int			size;
register int			err_num;
int				(*err_func)();
{
	if (((int) buf) & 01)
		AAsyserr(14008, buf);
	buf->nleft = size - sizeof *buf;
	buf->xfree = buf->buffer;
	buf->err_num = err_num;
	buf->err_func = err_func;
}
