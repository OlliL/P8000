# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/symbol.h"
# include	"../h/access.h"

char		*AAcctuple;		/* pointer to canonical tuple */
static char	AAccanon[MAXTUP];	/* canonical tuple buffer     */

/*
**	Put the canonical tuple in the position
**	on the current page specified by tid
*/
AAput_tuple(tid, tuple, length)
register struct tup_id	*tid;
char			*tuple;
register int		length;
{
	register char	*tp;
	extern char	*AAgetaddr();

#	ifdef xATR2
	if (AAtTfp(87, 0, "PUT_TUPLE: len=%d,", length))
		AAdumptid(tid);
#	endif

	/* get address in buffer */
	tp = AAgetaddr(tid);

	/* move the tuple */
	AAbmove(tuple, tp, length);

	/* mark page as dirty */
	AAcc_head->bufstatus |= BUF_DIRTY;
}


/*
**	Make the tuple canonical and return the length
**	of the tuple.
**
**	If the relation is compressed then the tuple in
**	compressed into the global area AAccanon.
**
**	As a side effect, the address of the tuple to be
**	inserted is placed in AAcctuple.
**
**	returns: length of canonical tuple
*/
AAcanonical(d, tuple)
register DESC	*d;
register char	*tuple;
{
	register int			i;

	if (ctoi(d->reldum.relspec) < 0)
	{
		/* compress tuple */
		i = AAcomp_tup(d, tuple);
		AAcctuple = AAccanon;
	}
	else
	{
		AAcctuple = tuple;
		i = d->reldum.relwid;
	}

	return (i);
}


/*
**	Compress the tuple into AAccanon. Compression is
**	done by copying INT and FLOAT as is.
**	For CHAR fields, the tuple is copied until a null
**	byte or until the end of the field. Then trailing
**	blanks are removed and a null byte is inserted at
**	the end if any trailing blanks were present.
*/
AAcomp_tup(d, src)
register DESC	*d;
register char	*src;
{
	register char		*dst;
	register int		len;
	register int		i;
	register int		j;
	register char		*save;
	register char		*domlen;
	register char		*domtype;

	dst = AAccanon;
	domlen = &d->relfrml[1];
	domtype = &d->relfrmt[1];

	for (i = 1; i <= d->reldum.relatts; i++)
	{
		len = ctou(*domlen++);
		if (*domtype++ == CHAR)
		{
			save = src;
			for (j = 1; j <= len; j++)
			{
				if (!(*dst++ = *src++))
				{
					dst--;
					break;
				}
			}

			while (j--)
				if (*--dst != ' ')
					break;

			if (j != len)
				*++dst = '\0';

			dst++;
			src = save + len;
		}
		else
			while (len--)
				*dst++ = *src++;
	}

	return (dst - AAccanon);
}


/*
**	Determine how much space remains on the page in
**	the current buffer. Included in computation
**	is the room for an additional line number
*/
AAspace_left(buf)
register ACCBUF		*buf;
{
	register int		nextoff;
	register short		i;

	nextoff = buf->nxtlino;
	for (i = 0; i < nextoff; i++)
	{
		if (!buf->linetab[LINETAB(i)])
		{
			/* found a free line number */
			i = (nextoff + 1) * sizeof (short);
			goto check_space;
		}
	}
	/* no free line numbers */
	if (nextoff > MAXTID)
		return (0);	/* linetab overflow */

	i = (nextoff + 2) * sizeof (short);

check_space:
	return (AApgsize - buf->linetab[LINETAB(nextoff)] - i);
}
