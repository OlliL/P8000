# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"
# include	"../h/aux.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"../h/pipes.h"
# include	"../h/bs.h"
# include	"decomp.h"

/*
** Readagg_result -- read results from ovqp from a simple aggregate.
*/
readagg_result(result)
QTREE			*result[];
{
	register QTREE			**r;
	register QTREE			*aop;
	register int			vallen;
	int				i;

	r = result;
	while (aop = *r++)
	{
		vallen = ctou(aop->sym.len);
		if (rdpipe(P_NORM, &Inpipe, R_ovqp, &aop->sym, TYP_LEN_SIZ) != TYP_LEN_SIZ)
			AAsyserr(12003);
		if (vallen != ctou(aop->sym.len))
			AAsyserr(12004, vallen, aop->sym.len);
		if ((i = rdpipe(P_NORM, &Inpipe, R_ovqp, aop->sym.value, vallen)) != vallen)
			AAsyserr(12005, i, vallen);
#		ifdef xDTR1
		if (AAtTf(8, 3))
			writenod(aop);
#		endif
	}
}


/*
** Endovqp -- Inform ovqp that processing is complete. "Ack" indicates
**	whether to wait for an acknowledgement from ovqp. The overall
**	mode of the query is sent followed by an EXIT command.
**
**	Ovqp decides whether to use batch update or not. If ack == ACK
**	then endovqp will read a RETVAL symbol from ovqp and return
**	a token which specifies whether to call the update processor or not.
*/
endovqp(ack)
int	ack;
{
	register int		j;
	struct retcode		ret;
	extern int		Qry_mode;
	extern DESC		 Inddes;

	if (ack != RUBACK)
	{
		wrpipe(P_PRIME, &Outpipe, EXEC_OVQP, (char *) 0, 0);

		/* send mode of the query */
		pipesym(QMODE, sizeof (short), Qry_mode);

		/* send exit code */
		pipesym(EXIT, sizeof (short), ack);

		wrpipe(P_END, &Outpipe, W_ovqp);
	}

	j = NOUPDATE;
	if (ack == ACK)
	{
		/* read acknowledgement from ovqp */
		rdpipe(P_PRIME, &Inpipe);
		if (rdpipe(P_NORM, &Inpipe, R_ovqp, &ret, sizeof (ret)) != sizeof (ret))
			AAsyserr(12006);

		j = ret.rc_status;
		if (j != UPDATE && j != NOUPDATE)
			AAsyserr(12007, j);

		rdpipe(P_SYNC, &Inpipe, R_ovqp);
	}

	cleanrel(&Inddes);	/* could be closecatalog(FALSE) except for
				** text space limitations */
	return (j);
}


pipesym(token, length, val)
register int	token;
register int	length;
short		val;
{
	register SYMBOL		*s;
	char			temp[TYP_LEN_SIZ];

	s = (SYMBOL *) temp;
	s->type = token;
	s->len = length;
	wrpipe(P_NORM, &Outpipe, W_ovqp, s, TYP_LEN_SIZ);
	if (length)
		wrpipe(P_NORM, &Outpipe, W_ovqp, &val, length);
}


ovqpnod(node)
QTREE			*node;
{
	register int			typ;
	register QTREE			*q;
	register SYMBOL			*s;
	short				i;
	register char			*p;
	char				temp[SYM_HDR_SIZ + sizeof (short)];
	extern QTREE			*ckvar();

	q = node;
	s = (SYMBOL *) temp;

	s->type = typ = q->sym.type;

	switch (typ)
	{
	  case VAR:
		q = ckvar(q);

		/*  check to see if this variable has been substituted for.
		**  if so, pass the constant value to OVQP		      */

		if (p = ((VAR_NODE *) q)->valptr)
		{
			s->type = ((VAR_NODE *) q)->frmt;
			s->len = ((VAR_NODE *) q)->frml;
			if (s->type == INT && s->len == sizeof (char))
			{
				s->len = sizeof (short);
				i = ctoi(*p);
				p = (char *) &i;
			}
			wrpipe(P_NORM, &Outpipe, W_ovqp, s, TYP_LEN_SIZ);
			wrpipe(P_NORM, &Outpipe, W_ovqp, p, ctou(s->len));
			return;
		}
		else
		{
			/* double check that variable is sourcevar */
			if (((VAR_NODE *) q)->varno != Sourcevar)
				AAsyserr(12008, Sourcevar, ((VAR_NODE *) q)->varno);
			s->len = sizeof (short);
			s->value[0] = ctou(((VAR_NODE *) q)->attno);
			break;
		}

	  case TREE:
		return;

	  case AND:
	  case OR:
		s->len = 0;
		break;

	  case BOP:
	  case RESDOM:
	  case UOP:
		s->len = OP_SIZ;
		s->value[0] = *(q->sym.value);
		break;

	  default:
		wrpipe(P_NORM, &Outpipe, W_ovqp, &q->sym, TYP_LEN_SIZ);
		if (i = ctou(q->sym.len))
			wrpipe(P_NORM, &Outpipe, W_ovqp, q->sym.value, i);
		return;
	}

	wrpipe(P_NORM, &Outpipe, W_ovqp, s, TYP_LEN_SIZ);
	if (s->len)
		wrpipe(P_NORM, &Outpipe, W_ovqp, s->value, s->len);
}

/*
**	Use "AAinpclose()" for closing relations. See
**	desc_close in openrs.c for details.
*/
extern int	AAinpclose();
int		(*Des_closefunc)() =	AAinpclose;

init_decomp()
{
#	ifdef XTRA
	static ACCBUF		xtrabufs[XTRABUFS];

	AAam_addbuf(xtrabufs, XTRABUFS);
#	endif
}


startdecomp()
{
	/* called at the start of each user query */
	Retcode.rc_tupcount = 0;
	initrange();
	rnum_init();
}


/*
** Files_avail -- returns how many file descriptors are available
**	for decomposition. For decomp running by itself the fixed
**	overhead for files is:
**
**		4 -- pipes
**		1 -- relation relation
**		1 -- attribute relation
**		1 -- AA_INDEX relation
**		1 -- standard output
**		1 -- concurrency
*/
files_avail()
{
	return (MAXFILES - 9);
}
