# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"
# include	"../h/pipes.h"
# include	"../h/bs.h"
# include	"ovqp.h"

extern DESC			Reldesc;
extern DESC			Srcdesc;

static DESC			Indesc;

/*
**	Endqry is called at the end of a query, either
**	in response to getqry receiving an EXIT or as a
**	result of a user error
*/
endqry(code)
int	code;
{
	register int	i;

	i = code;

	if (Eql && Retrieve)
		eqleol(EXIT);	/* signal end of query to EQL process */
	if (i == ACK)
	{
		if (Bopen)
			i = UPDATE;
		else
			i = NOUPDATE;

		closeall();
		retdecomp(i);
	}
	else
	{
		if (i == NOACK)
		{
			if (Bopen)
			{
				/* remove the batch file */
				rmbatch();
				Bopen = FALSE;
			}
		}
		else
			AAsyserr(17025, code);
		closeall();
	}
}




ov_err(code)
int	code;
/*
**	Usererr is called when a user generated
**	error is detected. The error number is
**	sent to DECOMP for processing. The read pipe
**	is flushed. A non-local goto (reset) returns
**	to main.c (see setexit()).
*/
{
	retdecomp(code);	/* send error message to decomp */
	endqry(NOACK);		/* end the query with no ack to decomp */
				/* and close all relations */
	rdpipe(P_SYNC, &Inpipe, R_decomp);	/* flush the pipe */

	/* copy the error message from decomp to the parser */
	copyreturn();
	reset();	/* return to main.c */
	AAsyserr(17026);
}



closeall()
{
	closerel(&Srcdesc);
	closerel(&Reldesc);
	closecatalog(FALSE);
	closerel(&Indesc);
	if (Bopen)
	{
		closebatch();
		Bopen = FALSE;
	}
}

retdecomp(code)
int	code;
/*
**	writes a RETVAL symbol with value "code".
**	if code = EMPTY then retdecomp will wait
**	for an EOP acknowledgement from DECOM before
**	returning.
**	If the query was a simple
**	aggregate, then the aggregate values are also written.
*/

{
	struct retcode	ret;
	register int	i;
	register int	t;
	register int	l;
	register char	*p;
	struct stacksym	ressym;
	SYMBOL		**alist;
	struct _aop_
	{
		char	type;
		char	len;
		int	aoper;
		char	atype;
		char	alen;
	};

#	ifdef xOTR1
	AAtTfp(33, 1, "RETDECOMP: returning %d\n", code);
#	endif
	ret.rc_status = code;
	ret.rc_tupcount = Tupsfound;
	wrpipe(P_NORM, &Outpipe, W_decomp, &ret, sizeof (ret));

	if (Alist && !Bylist)
	{
		alist = Alist;
		i = Agcount;
		Tend = Outtup;

		while (i--)
		{

			/* find next AOP */
			while ((t = ((SYMBOL *) *++alist)->type) != AOP)
				if (t == ROOT)
					AAsyserr(17027, i);

			t = ((SYMBOL *) &ressym)->type = ((struct _aop_ *) *alist)->atype;
			l = ((SYMBOL *) &ressym)->len = ((struct _aop_ *) *alist)->alen;
			l = ctou(l);
			p = (char *) ((SYMBOL *) &ressym)->value;
			if (t == CHAR)
			{
				((STRINGP *) p)->stringp = Tend;
				/* make sure it is blank padded */
				AApad(Tend, l);
			}
			else
			{
				AAbmove(Tend, p, sizeof (double));
				if (t == INT && l == sizeof (char))
					((I1TYPE *) p)->i1type = ((I2TYPE *) p)->i2type;
# ifndef NO_F4
				if (t == FLOAT && l == sizeof (float))
					((F4TYPE *) p)->f4type = ((F8TYPE *) p)->f8type;
# endif
			}
			Tend += l;

#			ifdef xOTR1
			if (AAtTfp(33, 2, "returning aggregate:"))
				prstack(&ressym);
#			endif
			/* write top of stack to decomp */
			pwritesym(&Outpipe, W_decomp, &ressym);
		}
	}

	wrpipe(P_END, &Outpipe, W_decomp);
}
openrel(desc, relname, mode)
DESC			*desc;
char			*relname;
int			mode;

/*
**	openrel checks to see if the descriptor already
**	holds the relation to be opened. If so it returns.
**	Otherwise it closes the relation and opens the new one
*/

{
	register DESC			*d;
	register char			*name;
	register int			i;

	d = desc;
	name = relname;
#	ifdef xOTR1
	AAtTfp(33, 4, "OPENREL: open=%d current=%.12s, new=%.12s\n",
		d->relopn, d->reldum.relid, name);
#	endif

	if (d->relopn)
	{
		if (AAbequal(d->reldum.relid, name, MAXNAME))
			return;
#		ifdef xOTR1
		AAtTfp(33, 5, "OPENREL: closing the old\n");
#		endif
		closerel(d);
	}
	if (i = AArelopen(d, mode, name))
		AAsyserr(17028, name, i);
}


closerel(desc)
DESC			*desc;

/*
**	close the relation
*/

{
	register DESC			*d;
	register int			i;

	d = desc;
#	ifdef xOTR1
	AAtTfp(33, 6, "CLOSEREL: open=%d rel=%.12s\n", d->relopn, d->reldum.relid);
#	endif
	if (d->relopn)
		if (i = AArelclose(d))
			AAsyserr(17029, d->reldum.relid, i);
}
DESC		 *openindex(relid)
char		*relid;
{
	openrel(&Indesc, relid, 0);
	return (&Indesc);
}
