# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/catalog.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"../h/pipes.h"
# include	"qrymod.h"

/*
**  READTREE.C -- query tree input procedures
**
**	This file contains routines to read trees from pipes and
**	catalogs.
**
**	Defines:
**		readqry
**		readsymbol
**		readtree
**		pipetrrd
**		relntrrd
**		gettree
**		Qmode
**		Resultvar
*/

extern struct pipfrmt	Pipe;
extern struct pipfrmt	Outpipe;

int			Qmode;		/* Query mode */
int			Resultvar;	/* Result variable number */

/*
** READQRY
**
** 	Reads in query symbols from input pipe into core
**	locations and sets up information needed for later
**	processing.
**
**	Returns ptr to root of querytree
**
**	Locbuf is a 'struct srcid' since that is the largest node of
**	a QMODE, SOURCEID, or RESULTVAR node.
*/
QTREE	*readqry(rdfn, initialize)
int	(*rdfn)();	/* tree read function */
int	initialize;	/* if set, initialize Qbuf */
{
	register SYMBOL	 	*s;
	SRCID			locbuf;
	register QTREE		*rtval;
	register char		*p;
	extern			xerror();
	extern char		*readtree();
	extern QTREE		*trbuild();

#	ifdef xQTR1
	AAtTfp(70, 0, "READQRY:\n");
#	endif

	if (initialize)
	{
		/* initialize for new query block */
		initbuf(Qbuf, QBUFSIZ, QBUFFULL, xerror);
		clrrange(TRUE);
		Resultvar = -1;
		Qmode = -1;
	}
	else
		clrrange(FALSE);

	s = (SYMBOL *) &locbuf;

	/* read symbols from input */
	for ( ; ; )
	{
		readsymbol(s, rdfn);
#		ifdef xQTR1
		if (AAtTfp(70, 1, "%d, %d, %d/",
			s->type, s->len, ctou(s->value[0])))
		{
			if (s->type == SOURCEID)
				printf("%.12s", ((SRCID *) s)->srcname);
			putchar('\n');
		}
#		endif
		switch (s->type)
		{
		  case QMODE:
			if (Qmode != -1)
				AAsyserr(19048);
			Qmode = s->value[0];
			break;

		  case RESULTVAR:
			if (Resultvar != -1)
				AAsyserr(19049);
			Resultvar = s->value[0];
			break;

		  case SOURCEID:
			declare(((SRCID *) s)->srcvar, ((SRCID *) s)->srcname, ((SRCID *) s)->srcown, ((SRCID *) s)->srcstat);
			break;

		  case TREE:	/* beginning of tree, no more other stuff */
			p = readtree(s, rdfn);
			rtval = trbuild(p);
			if (!rtval)
				far_error(STACKFULL, -1, -1, (char *) 0);
			return (rtval);

		  default:
			AAsyserr(19050, s->type);
		}
	}
}


/*
** READSYMBOL
**	reads in one symbol from pipe into symbol struct.
*/
readsymbol(dest, rdfn)
char	*dest;		/* if non-zero, pts to allocated space */
int	(*rdfn)();	/* tree read function */
{
	register int		len;
	register SYMBOL		*p;
	extern char		*need();

	/* check if enough space for type and len of sym */
	p = (SYMBOL *) (dest? dest: need(Qbuf, SYM_HDR_SIZ));
	if ((*rdfn)(p, TYP_LEN_SIZ) < TYP_LEN_SIZ)
		goto err3;
	len = ctou(p->len);
	if (len)
	{
		if (!dest)
		{
			/* this will be contiguous with above need call */
			need(Qbuf, len);
		}
		if ((*rdfn)(p->value, len) < len)
			goto err3;
	}

	return;

err3:
	AAsyserr(19051);
}


/*
** READTREE
** 	reads in tree symbols into a buffer up to a root (end) symbol
*/
char	*readtree(tresym, rdfn)
SYMBOL		*tresym;
int		(*rdfn)();
{
	register QTREE	*nod;
	register char	*rtval;
	extern char	*need();

	rtval = need(Qbuf, QT_HDR_SIZ);
	AAbmove(tresym, &(((QTREE *) rtval)->sym), TYP_LEN_SIZ);	/* insert type and len of TREE node */
	for ( ; ; )
	{
		/* space for left & right pointers */
		nod = (QTREE *) need(Qbuf, QT_PTR_SIZ);
		readsymbol((char *) 0, rdfn);
#		ifdef xQTR1
		if (AAtTf(70, 2))
			nodepr(nod, TRUE);
#		endif
		if (nod->sym.type == ROOT)
			return (rtval);
	}
}


/*
**  PIPETRRD -- read tree from R_up pipe
**
**	This routine looks like a rdpipe with only the last two
**	parameters.
**
**	Parameters:
**		ptr -- pointer to data area
**		cnt -- byte count
**
**	Returns:
**		actual number of bytes read
*/
pipetrrd(ptr, cnt)
register char	*ptr;
register int	cnt;
{
	return (rdpipe(P_NORM, &Pipe, R_up, ptr, cnt));
}


/*
**  RELNTRRD -- read tree from AA_TREE relation
**
**	This looks exactly like the 'pipetrrd' call, except that info
**	comes from the AA_TREE catalog instead of from the pipe.  It
**	must be initialized by calling it with a NULL pointer and
**	the segment name wanted as 'treeid'.
**
**	Parameters:
**		ptr -- NULL -- "initialize".
**			else -- pointer to read area.
**		cnt -- count of number of bytes to read.
**		treeid -- if ptr == NULL, this is the tree id,
**			otherwise this parameter is not supplied.
**
**	Returns:
**		count of actual number of bytes read.
**
**	Side Effects:
**		static variables are adjusted correctly.  Note that
**			this routine can be used on only one tree
**			at one time.
*/
relntrrd(ptr, cnt, treerelid, treeowner, treetype, treeid)
char	*ptr;
int	cnt;
char	*treerelid;
char	*treeowner;
char	treetype;
int	treeid;
{
	static struct tree	trseg;
	static char		*trp;
	static short		seqno;
	register char		*p;
	register int		n;
	register int		i;
	struct tree		trkey;
	struct tup_id		tid;

	p = ptr;
	n = cnt;

	if (!p)
	{
		/* initialize -- make buffer appear empty */
		trp = &trseg.treetree[sizeof trseg.treetree];
		AAbmove(treerelid, trseg.treerelid, MAXNAME);
		AAbmove(treeowner, trseg.treeowner, 2);
		trseg.treetype = treetype;
		trseg.treeid = treeid;
		seqno = 0;
		opencatalog(AA_TREE, 0);

#		ifdef xQTR2
		AAtTfp(70, 6, "RELNTRRD: n=%.12s o=%.2s t=%d i=%d\n",
			    treerelid, treeowner, treetype, treeid);
#		endif

		return (0);
	}

	/* fetch characters */
	while (n-- > 0)
	{
		/* check for segment empty */
		if (trp >= &trseg.treetree[sizeof trseg.treetree])
		{
			/* then read new segment */
			AAam_clearkeys(&Treedes);
			AAam_setkey(&Treedes, &trkey, trseg.treerelid, TREERELID);
			AAam_setkey(&Treedes, &trkey, trseg.treeowner, TREEOWNER);
			AAam_setkey(&Treedes, &trkey, &trseg.treetype, TREETYPE);
			AAam_setkey(&Treedes, &trkey, &trseg.treeid, TREEID);
			AAam_setkey(&Treedes, &trkey, &seqno, TREESEQ);
			seqno++;
			if (i = AAgetequal(&Treedes, &trkey, &trseg, &tid))
				AAsyserr(19052, i);
			trp = &trseg.treetree[0];
		}

		/* do actual character fetch */
		*p++ = *trp++;
	}

	return (cnt);
}


/*
**  GETTREE -- get tree from AA_TREE catalog
**
**	This function, given an internal treeid, fetches and builds
**	that tree from the AA_TREE catalog.  There is nothing exciting
**	except the mapping of variables, done by mapvars().
**
**	Parameters:
**		treeid -- internal id of tree to fetch and build.
**		init -- passed to 'readqry' to tell whether or not
**			to initialize the query buffer.
**
**	Returns:
**		Pointer to root of tree.
*/
QTREE	*gettree(treerelid, treeowner, treetype, treeid, init)
char	*treerelid;
char	*treeowner;
char	treetype;
int	treeid;
int	init;
{
	register QTREE	*t;
	extern int	relntrrd();

	/* initialize relntrrd() for this treeid */
	relntrrd(NULL, 0, treerelid, treeowner, treetype, treeid);

	/* read and build query tree */
	t = readqry(relntrrd, init);

	/* remap varno's to be unique */
	if (!init)
		mapvars(t);

	return (t);
}
