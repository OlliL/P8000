# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"
# include	"../h/symbol.h"

/*
**	AAget_tuple - take the tuple specified
**	by tid and move it to "tuple"
*/
AAget_tuple(d, tid, tuple)
register DESC		*d;
struct tup_id		*tid;
register char		*tuple;
{
	register char			*cp;
	extern char			*AAgetaddr();

	cp = AAgetaddr(tid);

	if (ctoi(d->reldum.relspec) < 0)	/* compressed tuple */
		AAuncomp_tup(d, cp, tuple);
	else					/* uncompressed tuple */
		AAbmove(cp, tuple, d->reldum.relwid);
}


/*
**	AAgetint_tuple - get the tuple specified by
**	tid. If possible avoid moving the tuple.
**	Return value is address of tuple.
*/
char	*AAgetint_tuple(d, tid, tuple)
register DESC		*d;
struct tup_id		*tid;
register char		*tuple;
{
	register char			*cp;
	extern char			*AAgetaddr();

	cp = AAgetaddr(tid);

	if (ctoi(d->reldum.relspec) < 0)
	{
		AAuncomp_tup(d, cp, tuple);	/* compressed tuple */
		cp = tuple;
	}

	return (cp);
}


/*
**	Routine to compute the address of a tuple
**	within the current buffer.
*/
char	*AAgetaddr(tid)
register struct tup_id		*tid;
{
	register int		offset;

	if (!(offset = AAcc_head->linetab[LINETAB(ctou(tid->line_id))]))
		AAsyserr(10012, AAcc_head->rel_tupid);

	return ((char *) AAcc_head + offset);
}


/*
**	AAuncompress - decompress the tuple at address cp
**	according to descriptor.
*/
AAuncomp_tup(d, src, dst)
register DESC		*d;
register char		*src;
register char		*dst;
{
	register int			i;
	register int			j;

	/* for each domain, copy and blank pad if char */
	for (i = 1; i <= d->reldum.relatts; i++)
	{
		j = ctou(d->relfrml[i]);
		if (d->relfrmt[i] == CHAR)
		{
			while (j--)
				if (!(*dst++ = *src++))
				{
					/* back up one */
					dst--;
					j++;
					break;
				}

			/* blank pad if necessary */
			while (j-- > 0)
				*dst++ = ' ';
		}
		else
			while (j--)
				*dst++ = *src++;
	}
}


/*
**	Check if a line number is valid.
**	If linenumber is illegal return AMINVL_ERR
**	if Line has been deleted return 2
**	else return 0
*/
AAinvalid(tid)
register struct tup_id		*tid;
{
	register int		i;

	if ((i = ctou(tid->line_id)) >= AAcc_head->nxtlino)
		return (AAam_error(AMINVL_ERR));

	if (!AAcc_head->linetab[LINETAB(i)])
		return (2);

	return (0);
}
