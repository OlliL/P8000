# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/pipes.h"

/*
**  Buffered Read From Pipe
**
**	Reads from the pipe with the file descriptor `des' into
**	the buffer `buf'.  `Mode' tells what is to be done.  `Msg' is
**	the address of the input buffer, and `n' is the length of the
**	info.  If `n' is zero, the data is assumed to be a null-
**	terminated string.
**
**	`Buf' is defined in "../pipes.h" together with the modes.
**	The pipe should be written by wrpipe() to insure that the
**	pipe formats match.
**
**	Modes:
**
**	P_PRIME -- Primes the buffer.  This involves cleaning out all
**		the header stuff so that a physical read is forced
**		on the next normal call to rdpipe().  It also resets
**		the error flag, etc.
**
**	P_NORM -- This is the normal mode.  If the buffer is empty,
**		it is filled from the pipe.  `N' bytes are moved into
**		`msg'.  If `n' is zero, bytes are moved into `msg' up
**		to and including the first null byte.  The pipe is
**		read whenever necessary, so the size of the data
**		buffer (controlled by PBUFSIZ) has absolutely no
**		effect on operation.
**
**	P_SYNC -- This mode reads the buffer, filling the pipe as
**		necessary, until an End Of Pipe (EOP) is encountered.
**		An EOP is defined as an empty buffer with a mode
**		("hdrstat") of "END_STAT".  Any data left in the pipe
**		is thrown away, but error messages are not.  Anything
**		not already read by the user is read by P_SYNC, so if
**		you are not certain that you have already read the
**		previous EOP.
**
**	P_EXECID -- The first (exec_id) field of the pipe buffer header
**		is read and returned.  This is used (for instance) by
**		the DBU routines, which must get the exec_id, but must
**		not read the rest of the pipe in case they must overlay
**		themselves.  It must ALWAYS be followed by a RDPIPE of
**		mode P_FUNCID!!
**
**	P_FUNCID -- The second through last bytes of the pipe are read
**		into the buffer, and the function code ("func_id") is
**		returned.  This is the second half of a P_EXECID call.
**
**	P_INT -- In the event of an interrupt, a wrpipe() call should
**		be made to all writable pipes of mode P_INT, and then
**		rdpipe() calls should be made on all readable pipes
**		of this mode.  This clears out the pipe up until a
**		special type of pipe header ("SYNC_STAT") is read.  It
**		is absolutely critical that this is called in conjunc-
**		tion with all other processes.  This mode is used in
**		resyncpipes(), which performs all the correct calls
**		in the correct order.
**
**	In all cases except P_INT mode, if an error is read from the
**	pipe it is automatically passed to proc_error().  This routine
**	is responsible for passing the error message to the next higher
**	process.
**
**	If an end of file is read from the pipe, the end_job() routine
**	is called.  This routine should do any necessary end of job
**	processing needed (closing relations, etc.) and return with
**	a zero value.
**
**	Default proc_error() and end_job() routines exist in the
**	library, so if you don't want any extra processing, you can
**	just not bother to define them.
**
**	In general, the number of bytes actually read is returned.  If
**	you read an EOP, zero is returned.
*/

extern int	(*AAexitfn)();	/* defined in AAsyserr */
extern int	read();		/* standard read routine */
int		(*Pi_rd_fn)() =	read;		/* fn to read from pipe */

rdpipe(mode, buf, des, msg, n)
int				mode;
char				*msg;
int				n;
int				des;	/* file descriptor for pipe */
register struct pipfrmt		*buf;
{
	register int		knt;
	register int		syncflg;
	int			i;
	extern char		*AAproc_name;

#	ifdef xATR1
	AAtTfp(99, 0, "%s ent RDPIPE md %d buf 0%o des %d\n",
		AAproc_name, mode, buf, des);
#	endif

	syncflg = 0;
	switch (mode)
	{

	  case P_PRIME:
		buf->pbuf_pt = buf->buf_len = buf->err_id = 0;
		buf->hdrstat = NORM_STAT;
		return (0);

	  case P_NORM:
		break;

	  case P_SYNC:
		syncflg++;
		break;

	  case P_EXECID:
#		ifdef xATR3
		AAtTfp(99, 1, "%s RDPIPE ret %c\n", AAproc_name,  buf->exec_id);
#		endif
		syncflg++;
		break;

	  case P_FUNCID:
		return (buf->func_id);

	  case P_INT:
		syncflg--;
		buf->hdrstat = NORM_STAT;
		break;

	  case P_PARAM:
		/* If there is currently some data, return param_id.
		** else read next block are return its param_id
		*/
		if (buf->pbuf_pt < buf->buf_len || buf->hdrstat == LAST_STAT)
			return (buf->param_id);
		break;

	  default:
		AAsyserr(15028, mode);
	}
	knt = 0;

	do
	{
		/* check for buffer empty */
		while (buf->pbuf_pt >= buf->buf_len || syncflg)
		{
			/* read a new buffer full */
			if (buf->hdrstat == LAST_STAT && syncflg >= 0)
				goto out;
#			ifdef xATR3
			AAtTfp(99, 2, "%s RDPIPE %d w len %d pt 0%o err %d sf %d\n",
				AAproc_name, des, buf->buf_len, buf->pbuf_pt, buf->err_id, syncflg);
#			endif
			if (!(i = (*Pi_rd_fn)(des, buf, HDRSIZ + PBUFSIZ)))
			{
#				ifdef xATR3
				AAtTfp(99, 1, "%s RDPIPE exit\n", AAproc_name);
#				endif
				if (syncflg < 0)
					AAsyserr(15029, des);
				close(W_down);
				if (des != R_up)
				{
#					ifdef xATR1
					if (AAtTf(99, -1))
						AAsyserr(15030, buf, des);
#					endif
					(*AAexitfn)(-1);
				}
				fflush(stdout);
				exit(end_job());
			}
			buf->pbuf_pt = 0;
#			ifdef xATR2
			if (AAtTf(99, 2))
				prpipe(buf);
#			endif
			if (i < HDRSIZ + PBUFSIZ)
				AAsyserr(15033, buf, des);
			if (buf->hdrstat == SYNC_STAT)
			{
				if (syncflg < 0)
					return (1);
				else
					AAsyserr(15036);
			}
			if (buf->err_id && syncflg >= 0)
			{
#				ifdef xATR2
				AAtTfp(99, 3, "%s RDPIPE err %d\n", AAproc_name, buf->err_id);
#				endif
				proc_error(buf, des);
#				ifdef xATR3
				AAtTfp(99, 3, "%s RDPIPE ret\n", AAproc_name);
#				endif
				buf->pbuf_pt = buf->buf_len;
				buf->hdrstat = NORM_STAT;
				continue;
			}
			if (mode == P_PARAM)
				return (buf->param_id);
			if (mode == P_EXECID)
				return (buf->exec_id);
		}
		/* get a byte of information */
		msg[knt++] = buf->pbuf[buf->pbuf_pt++];
	} while (!n? (msg[knt - 1]): (knt < n));

out:
#	ifdef xATR1
	if (AAtTfp(99, 1, "%s RDPIPE ret %d", AAproc_name, knt))
	{
		if (!n && !syncflg)
			printf(", str `%s'", msg);
		putchar('\n');
	}
#	endif
	return (knt);
}
