# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/bs.h"
# include	"../h/catalog.h"

/*
**  USERDESTROY: -- auxiliary cleanup for destroy of a user relation
**
**	userdestroy is called during the destroy of a non system
**	relation. If the relation is indexed or is itself an index
**	then the appropriate action is taken. If it is indexed,
**	then all secondary indices on the relation are also destroyed.
**	If it is a secondary index, then the entry in the AA_INDEX relation
**	is removed and the "relindxd" bit on the primary relation is
**	cleared if this was the last index on the relation.
**
**	If the relation was a view or had integrity constraints or
**	protection constraints on it, then those definitions are
**	removed from the appropriate system catalogues.
**
**	Parameters:
**		reltup -- the relation relation tuple.
**
**	Side Effects:
**		zero or more system catalogues will be updated.
*/
userdestroy(reltup)
struct relation	*reltup;
{
	register int			i;
	register struct relation	*rel;
	struct tup_id			tid;
	struct tup_id			limtid;
	char				newrelname[MAXNAME + 3];
	extern DESC			Reldes;
	extern DESC			Attdes;
	extern DESC			Inddes;
	extern DESC			Treedes;
	extern DESC			Intdes;
	extern DESC			Prodes;
	struct relation			relt;
	struct relation			relk;
	struct index			indk;
	struct index			indt;
	extern char			*AArelpath();

	rel = reltup;

	/* handle special case of destroying a secondary index */
	if (ctoi(rel->relindxd) == SECINDEX)
	{
		opencatalog(AA_INDEX, 2);
		AAam_setkey(&Inddes, &indk, rel->relid, IRELIDI);
		AAam_setkey(&Inddes, &indk, rel->relowner, IOWNERP);
		if (i = AAgetequal(&Inddes, &indk, &indt, &tid))
			AAsyserr(11176, rel->relid, i);

		/* remove entry in AA_INDEX catalog */
		AAbmove(indt.irelidp, newrelname, MAXNAME);
		AAbmove(indt.iownerp, &newrelname[MAXNAME], 2);
		if (i = AAam_delete(&Inddes, &tid))
			AAsyserr(11177, rel->relid, i);
		AAam_clearkeys(&Inddes);
		AAam_setkey(&Inddes, &indk, newrelname, IRELIDP);
		AAam_setkey(&Inddes, &indk, &newrelname[MAXNAME], IOWNERP);

		/* reset relindxd field in AA_REL catalog if no other indexes exist on this primary */
		if (AAgetequal(&Inddes, &indk, &indt, &tid))
		{
			AAam_clearkeys(&Reldes);
			AAam_setkey(&Reldes, &relk, newrelname, RELID);
			AAam_setkey(&Reldes, &relk, &newrelname[MAXNAME], RELOWNER);
			if (i = AAgetequal(&Reldes, &relk, &relt, &tid))
				AAsyserr(11178, newrelname, i);
			relt.relindxd = 0;
			if (i = AAam_replace(&Reldes, &tid, &relt, 0))
				AAsyserr(11179, i);
		}
	}

	/* check special case of destroying primary relation */
	if (ctoi(rel->relindxd) == SECBASE)
	{
		opencatalog(AA_INDEX, 2);
		AAam_setkey(&Inddes, &indk, rel->relid, IRELIDP);
		AAam_setkey(&Inddes, &indk, rel->relowner, IOWNERP);
		if (i = AAam_find(&Inddes, EXACTKEY, &tid, &limtid, &indk))
			AAsyserr(11180, rel->relid, i);
		while (!(i = AAam_get(&Inddes, &tid, &limtid, &indt, TRUE)))
		{
			if (AAkcompare(&Inddes, &indk, &indt))
				continue;
			if (i = AAam_delete(&Inddes, &tid))
				AAsyserr(11177, rel->relid, i);
			AAam_clearkeys(&Reldes);
			purgetup(&Reldes, indt.irelidi, RELID, indt.iownerp, RELOWNER);
			if (i = AAam_flush_rel(&Reldes, FALSE))	/* flush for recovery & concurrency reasons */
				AAsyserr(11182, i);
			purgetup(&Attdes, indt.irelidi, ATTRELID, indt.iownerp, ATTOWNER);
			AAdbname(indt.irelidi, indt.iownerp, newrelname);
			if (unlink(AArelpath(newrelname)))
				AAsyserr(11183, newrelname);
		}
		if (i < 0)
			AAsyserr(11184, i);
	}

	/* if any integrity constraints exist, remove them */
	if (rel->relstat & S_INTEG)
	{
		opencatalog(AA_INTEG, 2);
		purgetup(&Intdes, rel->relid, INTRELID, rel->relowner, INTRELOWNER);
	}

	/* if any protection clauses exist, remove them */
	if (rel->relstat & S_PROTUPS)
	{
		opencatalog(AA_PROTECT, 2);
		purgetup(&Prodes, rel->relid, PRORELID, rel->relowner, PRORELOWN);
	}

	/* remove any trees associated with the relation */
	if (rel->relstat & (S_PROTUPS | S_VIEW | S_INTEG))
	{
		opencatalog(AA_TREE, 2);
		purgetup(&Treedes, rel->relid, TREERELID, rel->relowner, TREEOWNER);
	}
}
