# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/catalog.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"../h/pipes.h"
# include	"../h/bs.h"

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
**		relntrrd
**		gettree
**		declare
**		clrrange
*/

extern struct pipfrmt	Pipe;
extern struct pipfrmt	Outpipe;

/*
** if DBU's other than DISPLAY choose to use readtree.c,
** QBUFFULL should be passed to gettree() as a parameter
*/

# define	QBUFFULL	5410
# define	STACKFULL	5411

/* this structure should be identical to the one in pr_prot.c */
struct rngtab
{
	char	relid[MAXNAME];
	char	rowner[2];
	char	rused;
};

extern struct rngtab	Rangev[];
extern int		Resultvar;

/* check out match for these two in decomp.h */
# define	QBUFSIZ		2000		/* query buffer */
char		Qbuf[QBUFSIZ];		/* tree buffer space */
int		Qmode;			/* type of query */

extern DESC			Treedes;

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
QTREE	*readqry(rdfn)
int	(*rdfn)();	/* tree read function */
{
	register SYMBOL	 	*s;
	struct srcid		locbuf;
	register QTREE		*rtval;
	register char		*p;
	int			berror();
	extern char		*readtree();
	extern QTREE		*trbuild();

	/* initialize for new query block */
	initbuf(Qbuf, QBUFSIZ, QBUFFULL, berror);
	clrrange();
	Resultvar = -1;
	Qmode = -1;

	s = (SYMBOL *) &locbuf;

	/* read symbols from input */
	for ( ; ; )
	{
		readsymbol(s, rdfn);
#		ifdef xZTR1
		AAtTfp(12, 0, "READQRY: symbol type %d\n", s->type);
#		endif
		switch (s->type)
		{
		  case QMODE:
			if (Qmode != -1)
				AAsyserr(11127);
			Qmode = s->value[0];
			break;

		  case RESULTVAR:
			if (Resultvar != -1)
				AAsyserr(11128);
			Resultvar = s->value[0];
			break;

		  case SOURCEID:
			declare(((SRCID *) s)->srcvar, ((SRCID *) s)->srcname, ((SRCID *) s)->srcown);
			break;

		  case TREE:	/* beginning of tree, no more other stuff */
			p = readtree(s, rdfn);
			if (!(rtval = trbuild(p)))
				berror(STACKFULL);
			return (rtval);

		  default:
			AAsyserr(11129, s->type);
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
		{
err3:
			AAsyserr(11130);
		}
	}
}


/*
** READTREE
**
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
	/* insert type and len of TREE node */
	AAbmove(tresym, &(((QTREE *) rtval)->sym), TYP_LEN_SIZ);
	for ( ; ; )
	{
		/* space for left & right pointers */
		nod = (QTREE *) need(Qbuf, QT_PTR_SIZ);
		readsymbol((char *) 0, rdfn);
		if (nod->sym.type == ROOT)
			return (rtval);
	}
}


/*
**  RELNTRRD -- read tree from AA_TREE relation
**
**	This looks exactly like the 'pipetrrd' call, except that info
**	comes from the AA_TREE catalog instead of from the pipe.  It
**	must be initialized by calling it with a (char *) 0 pointer and
**	the segment name wanted as 'treeid'.
**
**	Parameters:
**		ptr -- (char *) 0 -- "initialize".
**			else -- pointer to read area.
**		cnt -- count of number of bytes to read.
**		treeid -- if ptr == (char *) 0, this is the tree id,
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
				AAsyserr(11131, i);
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
**	that tree from the AA_TREE catalog.
**
**	Parameters:
**		treeid -- internal id of tree to fetch and build.
**		init -- passed to 'readqry' to tell whether or not
**			to initialize the query buffer.
**
**	Returns:
**		Pointer to root of tree.
*/
QTREE	*gettree(treerelid, treeowner, treetype, treeid)
char	*treerelid;
char	*treeowner;
char	treetype;
int	treeid;
{
	register QTREE	*t;
	extern int	relntrrd();

	/* initialize relntrrd() for this treeid */
	relntrrd((char *) 0, 0, treerelid, treeowner, treetype, treeid);

	/* read and build query tree */
	t = readqry(relntrrd);

	return (t);
}


/*
** BERROR -- Buffer overflow routine.
**
**	Called on Tree buffer overflow; returns via reset()
**	to DBU controller after sending back an error to process 1.
*/
berror(err)
int	err;
{
#	ifdef xZTR1
	AAtTfp(12, 1, "BERROR: %d\n", err);
#	endif
	error(err, (char *) 0);
	reset();
}


/*
** declare -- declare a range table entry
*/
declare(var, name, owner)
int		var;
char		*name;
char		*owner;
{
	register struct rngtab	*r;
	register int		v;

	v = var;
	if (v > MAXNAME)
		AAsyserr(11132, v);

#	ifdef xZTR1
	AAtTfp(12, 2, "DECLARE: var=%d, name=\"%s\", owner=\"%s\"\n",
		var, name, owner);
#	endif

	r = &Rangev[v];
	r->rused++;
	AAbmove(name, r->relid, sizeof r->relid);
	AAbmove(owner, r->rowner, sizeof r->rowner);
}


/*
** CLRRANGE -- clear range table entries
*/
clrrange()
{
	register int		i;
	register struct rngtab	*r;

#	ifdef xZTR1
	AAtTfp(12, 3, "CLRRANGE\n");
#	endif

	r = Rangev;
	for (i = 0; i <= MAXVAR; )
		r[i++].rused = 0;
}
