# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/pipes.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"
# include	"parser.h"

# define	PARGSIZE	(MAXDOM * 2 + 10)	/* max no of params to dbu */

char		**Pv;			/* ptr to list of dbu params */
int		Pc;			/* number of dbu commands */

static char	Pbuffer[PARBUFSIZ];	/* buffer for dbu commands */

/*
** CALL_P -- send the buffered command
**	the execid and funcid are given.
*/
call_p(qmode, execid)
int	qmode;
int	execid;
{
	register struct pipfrmt	*pp;
	register int		i;
	struct pipfrmt		p;

	fflush(stdout);
	pp = &p;
	wrpipe(P_PRIME, pp, execid, (char *) 0, qmode);
	for (i = 0; i < Pc; i++)
	{
#		ifdef	xPTR2
		AAtTfp(6, 0, "Pv[%d]=%s\n", i, Pv[i]);
#		endif
		wrpipe(P_NORM, pp, W_down, Pv[i], 0);
	}
	wrpipe(P_END, pp, W_down);
	initp();
	syncdn();
}


setp(msg)
register char	*msg;
{
	register char	*p;
	extern char	*need();

	p = need(Pbuffer, AAlength(msg) + 1);
	AAsmove(msg, p);

	Pv[Pc++] = p;

	if (Pc >= PARGSIZE)
		AAsyserr(18001, Pc);
}


initp()
{
	extern int	neederr();
	extern char	*need();

	initbuf(Pbuffer, PARBUFSIZ, PBUFOFLO, neederr);
	Pv = (char **) need(Pbuffer, PARGSIZE * (sizeof(*Pv)));
	Pc = 0;
}
