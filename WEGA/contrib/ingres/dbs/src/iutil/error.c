# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/pipes.h"

/*
**  DBS ERROR MESSAGE GENERATOR
**
**	Error message `num' is sent up towards process 1 with param-
**	eters `msg'.  This routine may have any number of parameters,
**	but the last one must be zero.
**
**	In process one, the appropriate error file is scanned for the
**	actual message.  The parameters are then substituted into that
**	message.  If the error message doesn't exist, then the first
**	parameter is printed as is.
*/
# ifdef P8000
error(num, msg1, msg2, msg3, msg4, msg5, msg6)
# else
error(num, msg1)
# endif
int	num;
char	*msg1;
# ifdef P8000
char	*msg2;
char	*msg3;
char	*msg4;
char	*msg5;
char	*msg6;
# endif
{
	register struct pipfrmt	*pip;
	register char		**msg;
	struct pipfrmt		pip_buf;
	extern int		W_err;
# ifdef P8000
	char			*msg_buf[6];

	msg = msg_buf;
	*msg++ = msg1;
	*msg++ = msg2;
	*msg++ = msg3;
	*msg++ = msg4;
	*msg++ = msg5;
	*msg = msg6;
	msg = msg_buf;
# else
	msg = &msg1;
# endif
	pip = &pip_buf;
	wrpipe(P_PRIME, pip, EXEC_ERROR, (char *) 0, 0);
	pip->err_id = num;

	if (W_err >= 0)
	{
		while (*msg)
			wrpipe(P_NORM, pip, W_err, *msg++, 0);
		wrpipe(P_END, pip, W_err);
	}

	return (num);
}
