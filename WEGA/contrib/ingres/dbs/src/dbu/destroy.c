# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/catalog.h"
# include	"../h/lock.h"

extern int	Standalone;

/*
**  DESTROY RELATION
**
**	The relations in pv are destroyed.  This involves three steps:
**	1 - remove tuple from relation relation
**	2 - remove tuples from attribute relation
**	3 - unlink physical file
**
**	If the relation is a secondary index, the entry is removed
**	from the index relation, and the primary relation is set to
**	be "not indexed" (unless there is another index on it).
**
**	If the relation has an index, all its indexes are also
**	destroyed.
**
**	If any errors occured while destroying the relations,
**	then the last error number is returned, otherwise
**	0 is returned.
**
**	If any query modification was defined on the relation,
**	the qrymod catalogues are updated.
**
**	If the Standalone variable is non-zero, relation names to
**	destroy include the owner, and you need not own the relation.
**	This is used by purge.
*/
destroy(pc, pv)
int	pc;
char	**pv;
{
	register int	i;
	register int	ret;
	register char	*name;

	opencatalog(AA_REL, 2);
	opencatalog(AA_ATT, 2);

	for (ret = 0; (name = *pv++) != (char *) -1; )
	{
		if (i = des(name))
			ret = i;
	}
	return (ret);
}


des(name)
char	*name;
{
	register int		i;
	register char		*relname;
	struct tup_id		tid;
	char			newrelname[MAXNAME + 3];
	extern DESC		Reldes;
	extern DESC		Attdes;
	extern DESC		Inddes;
	extern DESC		Treedes;
	struct relation		relt;
	struct relation		relk;
	extern char		*AArelpath();

	relname = name;
#	ifdef xZTR1
	AAtTfp(3, -1, "DESTROY: %s\n", relname);
#	endif

	newrelname[MAXNAME + 2] = 0;

	/* get the tuple from relation relation */
	AAam_setkey(&Reldes, &relk, relname, RELID);
	if (Standalone)
		AAam_setkey(&Reldes, &relk, &relname[MAXNAME], RELOWNER);
	else
		AAam_setkey(&Reldes, &relk, AAusercode, RELOWNER);
	if (i = AAgetequal(&Reldes, &relk, &relt, &tid))
	{
		if (i < 0)
			AAsyserr(11019, relname, i);
		/* nonexistant relation */
		return (error(5202, relname, (char *) 0));
	}

	/* don't allow a system relation to be destroyed */
	if (relt.relstat & S_CATALOG)
	{
		/* attempt to destroy system catalog */
		return (error(5201, relname, (char *) 0));
	}

	/* check if relation in use */
	if (AAsetrll(A_RTN, tid, M_EXCL) < 0)
		goto use_error;
	if (AAsetusl(A_RTN, tid, M_EXCL) < 0)
	{
		AAunlrl(tid);
use_error:
		return (error(5205, relname, (char *) 0));
	}

	if (i = AAam_delete(&Reldes, &tid))
		AAsyserr(11020, i);

	/*
	** for concurrency reasons, flush the relation-relation page
	** where the tuple was just deleted. This will prevent anyone
	** from being able to "AArelopen" the relation while it is being
	** destroyed. It also allows recovery to finish the destroy
	** it the system crashes during this destroy.
	*/
	if (i = AAam_flush_rel(&Reldes, FALSE))
		AAsyserr(11021, i);
	AAunlus(tid);
	AAunlrl(tid);

	purgetup(&Attdes, relt.relid, ATTRELID, relt.relowner, ATTOWNER);

	/*
	**	If this is a user relation, then additional processing
	**	might be needed to clean up indicies, protection constraints
	**	etc.
	*/
	if (!(relt.relstat & S_CATALOG))
		userdestroy(&relt);

	if (!(relt.relstat & S_VIEW))
	{
		if (Standalone)
			AAbmove(relname, newrelname, MAXNAME + 2);
		else
			AAdbname(relname, AAusercode, newrelname);
		if (unlink(AArelpath(newrelname)) < 0)
			AAsyserr(11022, newrelname);
	}
	return (0);
}
