# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/bs.h"
# include	"../h/catalog.h"
# include	"../h/pipes.h"
# include	"qrymod.h"

extern struct pipfrmt		Pipe;

/*
**  FILLPROTUP -- fill the protection tuple from pipe.
**
**	This routine is broken off from d_prot.c so that we
**	don't get symbol table overflows.
**
**	Parameters:
**		protup -- pointer to the protection tuple to fill.
**
**	Returns:
**		A bool telling whether we have PERMIT ALL TO ALL
**		permission.
**
**	Side Effects:
**		The R_up pipe is flushed.
*/
fillprotup(protup)
struct protect	*protup;
{
	register struct protect	*pt;
	register int		i;
	register char		*p;
	char			buf[30];
	int			all_pro;
	short			ix;
	char			ubuf[MAXLINE + 1];

	/*
	**  Fill in the protection tuple with the information
	**	from the parser, validating as we go.
	**
	**	Also, determine if we have a PERMIT xx to ALL
	**	with no further qualification case.  The variable
	**	'all_pro' is set to reflect this.
	*/

	all_pro = TRUE;
	pt = protup;

	/* read operation set */
	if (rdpipe(P_NORM, &Pipe, R_up, buf, 0) <= 0)
		AAsyserr(19030);
	if (AA_atoi(buf, &ix))
		AAsyserr(19031, buf);
	pt->proopset = ix;
	if ((ctoi(pt->proopset) & PRO_RETR) != 0)
		pt->proopset |= PRO_TEST | PRO_AGGR;

	/* read relation name */
	if (rdpipe(P_NORM, &Pipe, R_up, buf, 0) > MAXNAME + 1)
		AAsyserr(19032);
	AApmove(buf, pt->prorelid, MAXNAME, ' ');

	/* read relation owner */
	if (rdpipe(P_NORM, &Pipe, R_up, buf, 0) != 3)
		AAsyserr(19033);
	AAbmove(buf, pt->prorelown, 2);

	/* read user name */
	rdpipe(P_NORM, &Pipe, R_up, buf, 0);
	if (AAsequal(buf, "all"))
		AAbmove("  ", pt->prouser, 2);
	else
	{
		/* look up user in AA_USERS */
		if (getnuser(buf, ubuf))
			far_error(3591, -1, Resultvar, buf, (char *) 0);
		for (p = ubuf; *p != ':' && *p != 0; p++)
			continue;
		AAbmove(++p, pt->prouser, 2);
		if (p[0] == ':' || p[1] == ':' || p[2] != ':')
			AAsyserr(19034, ubuf);
		all_pro = FALSE;
	}

	/* read terminal id */
	rdpipe(P_NORM, &Pipe, R_up, buf, 0);
	if (AAsequal(buf, "all"))
	{
		AAbmove("    ", pt->proterm, PROTERM);
	}
	else
	{
		if (AAbequal(buf, "tty", 3))
			AApmove(&buf[3], pt->proterm, PROTERM, ' ');
		else
			AApmove(buf, pt->proterm, PROTERM, ' ');
		all_pro = FALSE;
	}

	/* read starting time of day */
	rdpipe(P_NORM, &Pipe, R_up, buf, 0);
	if (AA_atoi(buf, &ix))
		AAsyserr(19035, buf);
	pt->protodbgn = ix;
	if (ix > 0)
		all_pro = FALSE;

	/* read ending time of day */
	rdpipe(P_NORM, &Pipe, R_up, buf, 0);
	if (AA_atoi(buf, &ix))
		AAsyserr(19036, buf);
	pt->protodend = ix;
	if (ix < 24 * 60 - 1)
		all_pro = FALSE;

	/* read beginning day of week */
	rdpipe(P_NORM, &Pipe, R_up, buf, 0);
	i = cvt_dow(buf);
	if (i < 0)
		far_error(3594, -1, Resultvar, buf, (char *) 0);	/* bad dow */
	pt->prodowbgn = i;
	if (i > 0)
		all_pro = FALSE;

	/* read ending day of week */
	rdpipe(P_NORM, &Pipe, R_up, buf, 0);
	i = cvt_dow(buf);
	if (i < 0)
		far_error(3594, -1, Resultvar, buf, (char *) 0);	/* bad dow */
	pt->prodowend = i;
	if (i < 6)
		all_pro = FALSE;

	/* finished with pipe... */
	rdpipe(P_SYNC, &Pipe, R_up);

	return (all_pro);
}
