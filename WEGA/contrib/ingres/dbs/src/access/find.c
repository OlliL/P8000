# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/symbol.h"
# include	"../h/access.h"

/*
**	AAam_find - determine limits for scan of a relation
**
**	AAam_find determines the values of an initial TID
**	and an ending TID for scanning a relation.
**	The possible calls to find are:
**
**	AAam_find(desc, NOKEY, lotid, hightid)
**		sets tids to scan entire relation
**
**	AAam_find(desc, EXACTKEY, lotid, hightid, key)
**		sets tids according to structure
**		of the relation. Key should have
**		been build using setkey.
**
**	AAam_find(desc, LRANGEKEY, lotid, hightid, keylow)
**		Finds lotid less then or equal to keylow
**		for isam relations. Otherwise scans whole relation.
**		This call should be followed by a call with HRANGEKEY.
**
**	AAam_find(desc, HRANGEKEY, lotid, hightid, keyhigh)
**		Finds hightid greater than or equal to
**		keyhigh for isam relations. Otherwise sets
**		hightid to maximum scan.
**
**	AAam_find(desc, FULLKEY, lotid, hightid, key)
**		Same as find with EXACTKEY and all keys
**		provided. This mode is used only by AAfindbest
**		and replace.
**
**	returns:
**		<0 fatal error
**		 0 success
*/
AAam_find(d, mode, lotid, hightid, key)
register DESC		*d;
int			mode;
register struct tup_id	*lotid;
struct tup_id		*hightid;
char			*key;
{
	register int			ret;
	register int			keyok;
	long				pageid;
	long				AArhash();

#	ifdef xATR1
	if (AAtTf(92, 0))
	{
		printf("FIND: m%d,s%d,%.14s\n", mode, ctoi(d->reldum.relspec), d->reldum.relid);
		if (mode != NOKEY)
			AAprkey(d, key);
	}
#	endif

	ret = 0;	/* assume successful return */
	keyok = FALSE;

	switch (mode)
	{
	  case EXACTKEY:
		keyok = AAfullkey(d);
		break;

	  case FULLKEY:
		keyok = TRUE;

	  case NOKEY:
	  case LRANGEKEY:
	  case HRANGEKEY:
		break;

	  default:
		AAsyserr(10011, mode);
	}

	/* set lotid for beginning of scan */
	if (mode != HRANGEKEY)
	{
		pageid = 0L;
		AAstuff_page(lotid, &pageid);
		lotid->line_id = SCANTID;
	}

	/* set hitid for end of scan */
	if (mode != LRANGEKEY)
	{
		pageid = -1L;
		AAstuff_page(hightid, &pageid);
		hightid->line_id = SCANTID;
	}

	if (mode != NOKEY)
	{
		switch (abs(ctoi(d->reldum.relspec)))
		{
		  case M_HEAP:
			break;

		  case M_ISAM:
			if (mode != HRANGEKEY)
			{
				/* compute lo limit */
				if (ret = AAndxsearch(d, lotid, key, -1, keyok))
					break;	/* fatal error */
			}

			/* if the full key was provided and mode is exact, then done */
			if (keyok)
			{
				AAbmove(lotid, hightid, sizeof *lotid);
				break;
			}

			if (mode != LRANGEKEY)
				ret = AAndxsearch(d, hightid, key, 1, keyok);
			break;

		  case M_HASH:
			if (!keyok)
				break;		/* can't do anything */
			pageid = AArhash(d, key);
			AAstuff_page(lotid, &pageid);
			AAstuff_page(hightid, &pageid);
			break;

		  default:
			ret = AAam_error(AMFIND_ERR);
		}
	}

#	ifdef xATR2
	if (AAtTfp(92, 1, "FIND: ret %d\nlow", ret))
	{
		AAdumptid(lotid);
		printf("hi");
		AAdumptid(hightid);
	}
#	endif

	return (ret);
}


/*
 * This routine will check that enough of the tuple has been specified
 * to enable a key access.
 *
 */
AAfullkey(d)
register DESC	*d;
{
	register int			i;

	for (i = 1; i <= d->reldum.relatts; i++)
		if (d->relxtra[i] && !d->relgiven[i])
			return (FALSE);
	return (TRUE);
}
