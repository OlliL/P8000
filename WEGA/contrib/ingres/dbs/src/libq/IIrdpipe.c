# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/symbol.h"
# include	"../h/pipes.h"
# include	"IIglobals.h"

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
**		on the next normal call to IIrdpipe().  It also resets
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
**		themselves.  It must ALWAYS be followed by a IIrdpipe of
**		mode P_FUNCID!!
**
**	P_FUNCID -- The second through last bytes of the pipe are read
**		into the buffer, and the function code ("func_id") is
**		returned.  This is the second half of a P_EXECID call.
**
**	P_INT -- In the event of an interrupt, a wrpipe() call should
**		be made to all writable pipes of mode P_INT, and then
**		IIrdpipe() calls should be made on all readable pipes
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
IIrdpipe(mode, buf, des, msg, n)
int			mode;
char			*msg;
int			n;
int			des;		/* file descriptor for pipe */
register struct pipfrmt	*buf;
{
	register int		knt;
	register int		syncflg;
	register char		*ap;
	register int		i;
	register int		argc;
	register int		err;
	char			*argv[10];
	char			args[512];

#	ifdef xETR1
	if (IIdebug > 1)
		printf("RDPIPE(%d): mode %d buf 0%o\n", des, mode, buf);
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
		if (read(des, &buf->exec_id, 1) < 1)
			goto eoj;
#		ifdef xETR3
		if (IIdebug > 1)
			printf("RDPIPE: exec_id %c\n",  buf->exec_id);
#		endif
		return (buf->exec_id);

	  case P_FUNCID:
		i = read(des, &buf->func_id, HDRSIZ + PBUFSIZ-1) + 1;
		buf->pbuf_pt = -1;
		goto chkerr;

	  case P_INT:
		syncflg--;
		buf->hdrstat = NORM_STAT;
		break;

	  default:
		IIsyserr("RDPIPE(%d): Unbekannter Mode %d", des, mode);
	}
	knt = 0;
	if (buf->pbuf_pt < 0)
		buf->pbuf_pt = 0;

	do
	{
		/* check for buffer empty */
		while (buf->pbuf_pt >= buf->buf_len || syncflg)
		{
			/* read a new buffer full */
			if (buf->hdrstat == LAST_STAT && syncflg >= 0)
				goto out;
#			ifdef xETR3
			if (IIdebug > 1)
				printf("RDPIPE(%d): BUF len %d addr 0%o err %d syncflag %d\n",
					des, buf->buf_len, buf->pbuf_pt, buf->err_id, syncflg);
#			endif
			i = read(des, buf, HDRSIZ + PBUFSIZ);
			if (!i)
			{
eoj:
#				ifdef xETR3
				if (IIdebug > 1)
					printf("RDPIPE: exit\n");
#				endif
				if (syncflg < 0)
					IIsyserr("RDPIPE(%d): EOF", des);
				/* exit(end_job(buf, des)); */
				fflush(stdout);
				exit(-1);
			}
			buf->pbuf_pt = 0;
chkerr:
#			ifdef xETR2
			if (IIdebug > 1)
				IIprpipe(buf);
#			endif
			if (i < HDRSIZ + PBUFSIZ)
				IIsyserr("RDPIPE(%d): READ ERROR", des, buf);
			if (buf->hdrstat == SYNC_STAT)
			{
				if (syncflg < 0)
					return (1);
				else
					IIsyserr("RDPIPE(%d): Unerwarteter SYNC", des);
			}
			if (buf->err_id && syncflg >= 0)
			{
#				ifdef xETR2
				if (IIdebug > 1)
					printf("RDPIPE: err_id %d\n", buf->err_id);
#				endif
				/* proc_error(buf, des); */
				/* The error parameters are read here
				** and put into an argv, argc structure
				** so that the user doesn't have to use
				** DBS utilities to write an intelligent
				** error handler.
				*/
				ap = args;
				argc = 0;
				err = buf->err_id;
				while (i = IIrdpipe(P_NORM, buf, des, ap, 0))
				{
					argv[argc++] = ap;
					ap += i;
					if (argc > 10 || (args - ap) >= 500)
						IIsyserr("RDPIPE(%d): Zu viele Parameter", des);
				}
				IIerror(err, argc, argv);
#				ifdef xETR3
				if (IIdebug > 1)
					printf("RDPIPE: return from ERROR\n");
#				endif
				buf->pbuf_pt = buf->buf_len;
				buf->hdrstat = NORM_STAT;
			}
			if (buf->pbuf_pt < 0)
			{
				/* P_FUNCID mode */
#				ifdef xETR1
				if (IIdebug > 1)
					printf("RDPIPE: func_id %c\n", buf->func_id);
#				endif
				return (buf->func_id);
			}
		}
		/* get a byte of information */
		msg[knt++] = buf->pbuf[buf->pbuf_pt++];
	} while (!n? (msg[knt - 1]): (knt < n));

out:
#	ifdef xETR1
	if (IIdebug > 1)
	{
		printf("RDPIPE:  return(%d", knt);
		if (!n && !syncflg)
			printf(", str `%s'", msg);
		printf(")\n");
	}
#	endif
	return (knt);
}
