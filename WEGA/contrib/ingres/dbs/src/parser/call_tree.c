# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/pipes.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"
# include	"parser.h"

struct pipfrmt	Pipebuf;	/* define pipe buffer structure
				also referenced in rngsend and treepr */

/*
** CALL_TREE -- send a tree to decomp (or qrymod)
*/
call_tree(qmode1, execid)
short	qmode1;
int	execid;
{
	register int	qmode;
	register int	retr;

	fflush(stdout);
	qmode = qmode1;

#	ifdef	xPTR2
	AAtTfp(26, 0, "CALL_TREE: qm=%d\tex=%d\n", qmode, execid);
#	endif

	wrpipe(P_PRIME, &Pipebuf, execid, (char *) 0, qmode);
	writesym(QMODE, sizeof (short), &qmode1);
	if (Resrng)
	{
#		ifdef	xPTR2
		AAtTfp(26, 1, "resvarno:%d\n", Resrng->rentno);
#		endif
		writesym(RESULTVAR, sizeof (short), &Resrng->rentno);
	}
	rngsend();
	treepr(Lastree);
	wrpipe(P_END, &Pipebuf, W_down);

	/* optomize test for these two conditions */
	retr = (qmode == (int) mdRETR || qmode == (int) mdRET_UNI);

	/* output sync block to EQL if necessary */
	if (Eql && retr && Resrng == 0)
		syncup();
	syncdn();
	if (retr)
	{
		if (Resrng)
		{
			if (Relspec)
			{
				setp(Resrng->relnm);
				setp(Relspec);
				call_p(mdMODIFY, EXEC_DBU);
			}
		}
		else if (!Eql)
			AAendhdr();
	}
}


/*
** WRITESYM -- does the physical write stuff, etc.
**	for a normal tree symbol.  (i.e. other
**	than a SOURCEID--range table entry
*/
writesym(typ, len, value)
register int	typ;
register int	len;
register char	*value;
{
	SYMBOL		sym;

	sym.type = typ;
	sym.len = len = ctou(len);
	wrpipe(P_NORM, &Pipebuf, W_down, &sym, TYP_LEN_SIZ);
	if (value)
		wrpipe(P_NORM, &Pipebuf, W_down, value, len);
}


/*
**  RNGWRITE -- does the physical write operation, etc.
**	for SOURCEID symbol (i.e. a range table entry).
*/
rngwrite(rptr)
struct rngtab	*rptr;
{
	register SRCID		*sp;
	SRCID			s;

	sp = &s;
	((SYMBOL *) sp)->type = SOURCEID;
	((SYMBOL *) sp)->len = SRC_SIZ;
	sp->srcvar = rptr->rentno;
	AApmove(rptr->relnm, sp->srcname, MAXNAME, ' ');
	AAbmove(rptr->relnowner, sp->srcown, 2);
	sp->srcstat = rptr->rstat;
	wrpipe(P_NORM, &Pipebuf, W_down, sp, TYP_LEN_SIZ);
	wrpipe(P_NORM, &Pipebuf, W_down, &sp->srcvar, SRC_SIZ);
#	ifdef	xPTR3
	AAtTfp(26, 2, "srcvar:%d\tsrcname:%.12s\tsrcown:%.2s\tsrcstat:0%o\n",
		sp->srcvar, sp->srcname, sp->srcown, sp->srcstat);
#	endif
}
