# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"
# include	"../h/pipes.h"
# include	"decomp.h"

/*
** READQUERY
**
** 	Reads in query symbols from input pipe into core
**	locations and sets up information needed for later
**	processing.
**
**	Returns:
**		pointer to start of tree
**
**	Side Effects:
**
**		Sets Qmode to mode of query
**		Resultvar gets result variable number or -1
**		Range table gets initialized.
*/

QTREE	*readqry()
{
	register SYMBOL	 		*s;
	register QTREE			*q;
	register int			mark;
	register int			i;
	extern QTREE			*readnod();

#	ifdef xDTR1
	AAtTfp(1, -1, "READQUERY:\n");
#	endif

	Resultvar = -1;
	Qmode = -1;
	mark = markbuf(Qbuf);
	for ( ; ; )
	{
		freebuf(Qbuf, mark);
		q = readnod(Qbuf);
		s = &(q->sym);
#		ifdef xDTR1
		if (AAtTfp(1, 1, "%d, %d, %d/", s->type, s->len, ctou(s->value[0])))
		{
			if (s->type == SOURCEID)
				printf("%.12s",((SRCID *) s)->srcname);
			putchar('\n');
		}
#		endif
		switch (s->type)
		{
		  case QMODE:
			Qmode = s->value[0];
			break;

		  case RESULTVAR:
			Resultvar = s->value[0];
			break;

		  case SOURCEID:
			i =((SRCID *) s)->srcvar;
			Rangev[i].relnum = rnum_assign(((SRCID *) s)->srcname);
			break;

		  case TREE:
			readtree(Qbuf);
			return (q);

		  default:
			AAsyserr(12018, s->type);
		}
	}
}

QTREE		 *readnod(buf)
char	*buf;
{
	register int			len;
	register int			user;
	register QTREE			*q;
	extern QTREE			*need();

	q = need(buf, QT_HDR_SIZ);	/* space for left,right ptrs + type & len */

	/* read in type and length */
	if (rdpipe(P_NORM, &Inpipe, R_up, &(q->sym), TYP_LEN_SIZ) < TYP_LEN_SIZ)
		goto err3;
	len = ctou(q->sym.len);

	if (len)
	{
		need(buf, len);
		if (rdpipe(P_NORM, &Inpipe, R_up, q->sym.value, len) < len)
			goto err3;
		return (q);
	}

	/* if type is and, root, or aghead, allocate 6 or 8 byte for var maps */
	user = FALSE;
	switch (q->sym.type)
	{

	  case ROOT:
		user = TRUE;
	  case AGHEAD:
		len = ROOT_SIZ;
		need(buf, len);
		((ROOT_NODE *) q)->rootuser = user;
		break;

	  case AND:
		len = AND_SIZ;
		need(buf, len);
	}
	q->sym.len = len;
	return (q);
err3:
	AAsyserr(12019);
}


/*
 * readtree
 *
 * 	reads in tree symbols into a buffer up to an end root symbol
 *
 */
readtree(buf)
char	*buf;
{
	register QTREE		 	*nod;

	do
	{
		nod = readnod(buf);
#		ifdef xDTR1
		if (AAtTfp(1, 2, "\t"))
			writenod(nod);
#		endif
	} while (nod->sym.type != ROOT);
	clearpipe();	/* This is needed in case query was multiple of 120 bytes */
}
