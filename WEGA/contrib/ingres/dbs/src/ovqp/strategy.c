# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/catalog.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"
# include	"../h/pipes.h"
# include	"ovqp.h"
# include	"strategy.h"

int		Fmode;		/* find-mode determined by strategy	*/
DESC		*Scanr;		/* desc of reln to be scanned,		*/
				/* (i.e. either Srcdesc or Indesc)	*/
DESC		*Source;	/* 0 if no source for qry,		*/
				/* else points to Srcdesc		*/
DESC		*Result;	/* 0 if no result for qry,		*/
				/* else points to Reldesc		*/
int		Newq;		/* flags new user query to OVQP		*/
long		Lotid;		/* lo limits of scan in OVQP		*/
long		Hitid;		/* hi limits of scan in OVQP		*/

char		Keyl[MAXTUP];
char		Keyh[MAXTUP];
struct key	Lkey_struct[MAXDOM + 1];
struct key	Hkey_struct[MAXDOM + 1];

/*
** STRATEGY
**
**	Attempts to limit access scan to less than the entire Source
**	relation by finding a key which can be used for associative
**	access to the Source reln or an index thereon.  The key is
**	constructed from domain-value specifications found in the
**	clauses of the qualification list using sub-routine findsimp
**	in findsimp.c and other subroutines in file key.c
*/


strategy()
{
	register int			i;
	register int			allexact;
	struct accessparam		sourceparm;
	struct accessparam		indexparm;
	struct index			itup;
	struct index			rtup;
	struct key			lowikey[MAXKEYS + 1];
	struct key			highikey[MAXKEYS + 1];
	register DESC			*d;
	extern DESC			Inddes;
	extern DESC			*openindex();

#	ifdef xOTR1
	AAtTfp(31, 0, "STRATEGY\tSource=%.12s\tNewq=%d\n",
		Source ? Source->reldum.relid : "(none)", Newq);
#	endif

	while (Newq)	/* if Newq=TRUE then compute a new strategy */
			/* NOTE: This while loop is executed only once */
	{
		Scanr = Source;

		if (!Scanr)
			return (1);	/* return immediately if there is no source relation */

		Fmode = NOKEY;	/* assume a find mode with no key */

		if (!Qlist)
			break;	/* if no qualification then you must scan entire rel */

		/* copy structure of source relation into sourceparm */
		AAdparam(Source, &sourceparm);

		/* if source is unkeyed and has no sec index then give up */
		if (sourceparm.mode == NOKEY &&
		    ((i = ctoi(Source->reldum.relindxd)) == 0 || i == SECINDEX))
			break;

		/* find all simple clauses if any */
		if (!findsimps())
			break;	/* break if there are no simple clauses */

		/* Four steps are now performed to try and find a key.
		** First if the relation is hashed then an exact key is search for
		**
		** Second if there are secondary indexes, then a search is made
		** for an exact key. If that fails then a  check is made for
		** a range key. The result of the rangekey check is saved.
		**
		** Third if the relation is an ISAM a check is  made for
		** an exact key or a range key.
		**
		** Fourth if there is a secondary index, then if step two
		** found a key, that key is used.
		**
		**  Lastly, give up and scan the  entire relation
		*/

		/* step one. Try to find exact key on primary */
		if (exactkey(&sourceparm, Lkey_struct))
		{
			Fmode = EXACTKEY;
			break;
		}

		/* step two. If there is an index, try to find an exactkey on one of them */
		if (ctoi(Source->reldum.relindxd))
		{
			opencatalog(AA_INDEX, 0);
			AAam_setkey(&Inddes, &itup, Source->reldum.relid, IRELIDP);
			AAam_setkey(&Inddes, &itup, Source->reldum.relowner, IOWNERP);
			if (i = AAam_find(&Inddes, EXACTKEY, &Lotid, &Hitid, &itup))
				AAsyserr(17021, i);

			while (!(i = AAam_get(&Inddes, &Lotid, &Hitid, &itup, NXTTUP)))
			{
#				ifdef xOTR1
				if (AAtTf(31, 3))
					AAprtup(&Inddes, &itup);
#				endif
				if (!AAbequal(itup.irelidp, Source->reldum.relid, MAXNAME) ||
				    !AAbequal(itup.iownerp, Source->reldum.relowner, 2))
					continue;
				AAiparam(&itup, &indexparm);
				if (exactkey(&indexparm, Lkey_struct))
				{
					Fmode = EXACTKEY;
					d = openindex(itup.irelidi);
					/* temp check for 6.0 index */
					if (ctoi(d->reldum.relindxd) == -1)
						ov_err(BADSECINDX);
					Scanr = d;
					break;
				}
				if (Fmode == LRANGEKEY)
					continue;	/* a range key on a s.i. has already been found */
				if (allexact = rangekey(&indexparm, lowikey, highikey))
				{
					AAbmove(&itup, &rtup, sizeof itup);	/* save tuple */
					Fmode = LRANGEKEY;
				}
			}
			if (i < 0)
				AAsyserr(17022, i);
			/* If an exactkey on a secondary index was found, look no more. */
			if (Fmode == EXACTKEY)
				break;
		}


		/* step three. Look for a range key on primary */
		if (i = rangekey(&sourceparm, Lkey_struct, Hkey_struct))
		{
			if (i < 0)
				Fmode = EXACTKEY;
			else
				Fmode = LRANGEKEY;
			break;
		}

		/* last step. If a secondary index range key was found, use it */
		if (Fmode == LRANGEKEY)
		{
			if (allexact < 0)
				Fmode = EXACTKEY;
			d = openindex(rtup.irelidi);
			/* temp check for 6.0 index */
			if (ctoi(d->reldum.relindxd) == -1)
				ov_err(BADSECINDX);
			Scanr = d;
			AAbmove(lowikey, Lkey_struct, sizeof lowikey);
			AAbmove(highikey, Hkey_struct, sizeof highikey);
			break;
		}

		/* nothing will work. give up! */
		break;

	}

	/* check for Newq = FALSE and no source relation */
	if (!Scanr)
		return (1);
	/*
	** At this point the strategy is determined.
	**
	** If Fmode is EXACTKEY then Lkey_struct contains
	** the pointers to the keys.
	**
	** If Fmode is LRANGEKEY then Lkey_struct contains
	** the pointers to the low keys and Hkey_struct
	** contains pointers to the high keys.
	**
	** If Fmode is NOKEY, then a full scan will be performed
	*/
#	ifdef xOTR1
	AAtTfp(31, -1, "FMODE: %d\n", Fmode);
#	endif

	/* set up the key tuples */
	if (Fmode != NOKEY)
	{
		if (setallkey(Lkey_struct, Keyl))
			return (0);	/* query false. There is a simple
					** clause which can never be satisfied.
					** These simple clauses can be choosey!
					*/
	}

	if (i = AAam_find(Scanr, Fmode, &Lotid, &Hitid, Keyl))
		AAsyserr(17023, Scanr->reldum.relid, i);

	if (Fmode == LRANGEKEY)
	{
		setallkey(Hkey_struct, Keyh);
		if (i = AAam_find(Scanr, HRANGEKEY, &Lotid, &Hitid, Keyh))
			AAsyserr(17024, Scanr->reldum.relid, i);
	}

#	ifdef xOTR1
	if (AAtTfp(31, 1, "Lo"))
	{
		AAdumptid(&Lotid);
		printf("Hi");
		AAdumptid(&Hitid);
	}
#	endif

	return (1);
}
