# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/access.h"
# include	"../h/lock.h"

/*
**  AArelopen -- Open a relation into a descriptor
**
**	AArelopen will open the named relation into the given descriptor
**	according to the mode specified. When searching for a name,
**	a relation owner by the current user will be searched for first.
**	If none is found then one owned by the DBA will be search for.
**
**	There are several available modes for opening a relation. The
**	most common are
**		mode 0 -- open for reading
**		mode 2 -- open for writing (mode 1 works identically).
**	Other modes which can be used to optimize performance:
**		mode -1 -- get catalog AA_REL tuple and tid only.
**      			Does not open the relation.
**		mode -2 -- open relation for reading after a previous
**      			call of mode -1.
**		mode -3 -- open relation for writing after a previous
**      			call of mode -1.
**		mode -4 -- open relation for reading. Assumes that relation
**      			was previously open (eg relation & attributed
**      			have been filled) and file was closed by AArelclose.
**		mode -5 -- open relation for writing. Same assumptions as
**      			mode -4.
**
**	Parameters:
**		d   - a pointer to a DESC		 (defined in dbs.h)
**		mode - can be 2, 0, -1, -2, -3, -4, -5
**		name - a null terminated name (only first 12 chars looked at)
**
**	Returns:
**		1 - relation does not exist
**		0 - ok
**		<0 - error. Refer to the error codes in access.h
**
**	Side Effects:
**		Opens the physical file if required. Fill the
**		descriptor structure. Initializes the access methods
**		if necessary.
*/
AArelopen(d, mode, name)
register DESC		*d;
int			mode;
char			*name;
{
	register int			retval;
	register int			filemode;
	char				filename[MAXNAME + 3];
	extern char			*AArelpath();

#	ifdef xATR1
	AAtTfp(90, -1, "OPENR: %.12s,%d\n", name, mode);
#	endif
#	ifdef xATM
	if (AAtTf(76, 2))
		timtrace(21, 0);
#	endif

	AAam_start();

	/* process according to mode */
	filemode = 0;
	switch (mode)
	{
	  case -1:
		retval = AAgetrel_tuple(d, name);
		break;

	  case 1:
	  case 2:
		filemode = 2;

	  case 0:
		if (retval = AAgetrel_tuple(d, name))
			break;

	  case -2:
	  case -3:
		if (retval = AAgetatt_tuples(d))
			break;

	  case -5:
		if (mode == -3 || mode == -5)
			filemode = 2;

	  case -4:
		AAam_clearkeys(d);
		/* descriptor is filled. open file */
		AAdbname(d->reldum.relid, d->reldum.relowner, filename);
		/* can't open a view */
		if (d->reldum.relstat & S_VIEW)
		{
			retval = AAam_error(AMOPNVIEW_ERR);	/* view */
			break;
		}
		if ((d->relfp = open(AArelpath(filename), filemode)) < 0)
		{
			retval = AAam_error(AMNOFILE_ERR);	/* can't open file */
			break;
		}
		if (filemode == 2)
		{
			d->relopn = RELWOPEN(d->relfp);
			d->reldum.relock[1] = P_EXCL;
		}
		else
		{
			d->relopn = RELROPEN(d->relfp);
			d->reldum.relock[1] = P_SHR;
		}
		d->reldum.relock[0] = d->reldum.relock[1];
		d->reladds = 0;
		retval = 0;
		break;

	  default:
		AAsyserr(10017, mode, name);
	}

#	ifdef xATR1
	if (AAtTf(90, 4) && mode != -1 && retval != 1)
		AAprdesc(d);
	AAtTfp(90, -1, "OPENR: rets %d\n", retval);
#	endif
#	ifdef xATM
	if (AAtTf(76, 2))
		timtrace(22, 0);
#	endif

	return (retval);
}


/*
**  AAgetrel_tuple
**	Get the tuple for the relation specified by 'name'
**	and put it in the descriptor 'd'.
**
**	First a relation named 'name' owned
**	by the current user is searched for. If that fails,
**	then a relation owned by the dba is searched for.
*/
AAgetrel_tuple(d, name)
register DESC		*d;
register char		*name;
{
	register int			i;
	struct relation			rel;

	AAam_clearkeys(&AAdmin.adreld);

	/* make believe catalog AA_REL is read only for concurrency	*/
	/* that means, readed pages are not locked			*/
	AAdmin.adreld.relopn = RELROPEN(AAdmin.adreld.relfp);

	/* catalog AA_REL is open. Search for relation 'name' */
	AAam_setkey(&AAdmin.adreld, &rel, name, RELID);
	AAam_setkey(&AAdmin.adreld, &rel, AAusercode, RELOWNER);

	if ((i = AAgetequal(&AAdmin.adreld, &rel, d, &d->reltid)) == 1)
	{
		/* not a user relation. try relation owner by dba */
		AAam_setkey(&AAdmin.adreld, &rel, AAdmin.adhdr.adowner, RELOWNER);
		i = AAgetequal(&AAdmin.adreld, &rel, d, &d->reltid);
	}

	/* free all buffers connected with catalog AA_REL */
	AAam_flush_rel(&AAdmin.adreld, TRUE);

#	ifdef xATR1
	AAtTfp(90, 1, "GET_RELTUPLE: %d\n", i);
#	endif

	/* restore catalog AA_REL to read/write mode */
	AAdmin.adreld.relopn = RELWOPEN(AAdmin.adreld.relfp);
	return (i);
}


AAgetatt_tuples(d)
register DESC		*d;
{
	register int			i;
	register int			dom;
	register int			numatts;
	struct attribute		attr;
	struct attribute		attkey;
	struct tup_id			tid1;
	struct tup_id			tid2;

	AAam_clearkeys(&AAdmin.adattd);

	/* zero all format types */
	for (i = 0; i <= d->reldum.relatts; i++)
		d->relfrmt[i] = 0;

	/* prepare to scan attribute relation */
	AAam_setkey(&AAdmin.adattd, &attkey, d->reldum.relid, ATTRELID);
	AAam_setkey(&AAdmin.adattd, &attkey, d->reldum.relowner, ATTOWNER);
	if (i = AAam_find(&AAdmin.adattd, EXACTKEY, &tid1, &tid2, &attkey))
		return (i);

	numatts = d->reldum.relatts;

	while (numatts && !AAam_get(&AAdmin.adattd, &tid1, &tid2, &attr, TRUE))
	{
		/* does this attribute belong? */
		if (AAbequal(&attr, &attkey, MAXNAME + 2))
		{
			/* this attribute belongs */
			dom = attr.attid;	/* get domain number */

			if (d->relfrmt[dom])
				break;	/* duplicate attribute. force error */

			numatts--;
			d->reloff[dom] = attr.attoff;
			d->relfrmt[dom] = attr.attfrmt;
			d->relfrml[dom] = attr.attfrml;
			d->relxtra[dom] = attr.attxtra;
		}
	}

	/* make sure all the atributes were there */
	for (dom = 1; dom <= d->reldum.relatts; dom++)
		if (!d->relfrmt[dom])
			numatts = 1;	/* force an error */
	if (numatts)
		i = AAam_error(AMNOATTS_ERR);

	AAam_flush_rel(&AAdmin.adattd, TRUE);

#	ifdef xATR1
	AAtTfp(90, 3, "GET_ATTR: ret %d\n", i);
#	endif

	return (i);
}
