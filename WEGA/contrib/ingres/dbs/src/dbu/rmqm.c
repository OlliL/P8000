# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"
# include	"../h/aux.h"
# include	"../h/bs.h"
# include	"../h/catalog.h"
# include	"../h/symbol.h"

/*
**  RMQM -- DBU to delete protection and integrity constraints
**
**	Defines:
**		dest_const
**		dest_integ
**		dest_prot
**		i_cat
**		dl_all
**		del_int
**		del_tree
**		chk_const
**		r_relstat
**		tree_const
**		tree_prot
**		prot_protree
**		int_inttree
**
**
**
**  DEST_CONST -- destroy constraints
**
**	Parameters:
**		argc -- number of parameters in argv
**		argv -- argv [0] == 5 destroy permission
**			         == 6 destroy integrity constraint
**		        argv [1]    relation from which to destroy constrain
**		        argv [2] == if (argc != 2) relation from which to delete
**				     	constraints
**		        argv[3] ... argv[argc - 1] == id of constraint
**
**	Side Effects:
**		destroys constraints. Involves activity on catalogs
**		AA_REL, AA_PROTECT, AA_INTEG, and AA_TREE.
*/
dest_const(argc, argv)
int		argc;
char		**argv;
{
	DESC			d;
	register int		i;
	short			mode;
	extern struct admin	AAdmin;

#	ifdef xZTR1
	if (AAtTfp(13, 0, "DEST_CONST\n"))
		AAprargs(argc, argv);
#	endif

	if (!(AAdmin.adhdr.adflags & A_QRYMOD))
		return (0);
	i = AArelopen(&d, -1, argv[1]);
	if (i < 0)
		AAsyserr(11138, argv[1], i);

	if (i == 1 || !AAbequal(AAusercode, d.reldum.relowner, 2))
	{
		error(5202, argv[1], (char *) 0);
		return (0);
	}
	argv[argc] = 0;
	if (i = AA_atoi(argv[0], &mode))
		AAsyserr(11139, argv[0]);

	if (mode == 5)
		dest_prot(&d, &argv[2]);
	else if (mode == 6)
		dest_integ(&d, &argv[2]);
	else
		AAsyserr(11140, mode);
	return (0);
}


/*
**  DEST_INTEG -- directs destruction of integrity constraints
**
**	Parameters:
**		desc -- descriptor for relation
**		intv -- 0 terminated list of id strings, if first element
**		        is 0 means "all"
**
**	Side Effects:
**		deletes integrity constraint. Activity on AA_REL, AA_INTEG,
**		and AA_TREE.
*/
dest_integ(desc, intv)
DESC			*desc;
char			*intv[];
{
	register DESC			*d;
	extern DESC			Intdes;
	struct integrity		tuple;
	struct integrity		key;
	struct tree			tkey;
	register int			i;
	register int			j;
	int				tree_const();
	int				int_inttree();

	d = desc;
#	ifdef xZTR1
	AAtTfp(13, 1, "DEST_INTEG: (%s, %s)...\n", d->reldum.relid, d->reldum.relowner);
#	endif

	i_cat(AA_INTEG, &Intdes, &key, d->reldum.relid, INTRELID,
		d->reldum.relowner, INTRELOWNER, mdINTEG, &tkey);

	if (!intv[0])
	{
		/* destroy integrity 'relation' ALL */
		if (!(d->reldum.relstat & S_INTEG))
			return (0);
		del_all(d, &Intdes, &key, &tuple, &tkey, S_INTEG,
			tree_const, int_inttree);
		return (0);
	}
	/* destroy integrity 'relation' int {, int} */
	for (i = 0; intv[i]; i++)
		del_int(&Intdes, &key, &tuple, &tkey, intv[i], INTTREE,
		tree_const, int_inttree);

	/* rescan to output error messages */
	for (j = 0; j < i; j++)
		if (*intv[j])
			error(5203, intv[j], (char *) 0);

	/* finally, check that there are still integrity constraints
	** on the relation, if not must reset the S_INTEG bit in the relation
	** relation tuple for that relation.
	*/
	chk_const(d, &Intdes, &key, &tuple, d->reldum.relid, INTRELID, d->reldum.relowner,
	INTRELOWNER, S_INTEG);
}


/*
**  DEST_PROT -- directs destruction of protection constraints
**
**	Parameters:
**		desc -- descriptor for relation
**		intv -- 0 terminated list of id strings, if first element
**		        is 0 means "all"
**
**	Side Effects:
**		deletes protection constraint. Activity on catalogs
**		AA_REL, AA_PROTECT, and AA_TREE.
*/
dest_prot(desc, intv)
DESC			*desc;
char			*intv[];
{
	register DESC			*d;
	extern DESC			Prodes;
	struct protect			tuple;
	struct protect			key;
	struct tree			tkey;
	register int			i;
	register int			j;
	short				propermid;
	int				prot_protree();
	int				tree_prot();

	d = desc;
#	ifdef xZTR1
	AAtTfp(13, 2, "DEST_PROT: (%s, %s)...\n", d->reldum.relid, d->reldum.relowner);
#	endif

	i_cat(AA_PROTECT, &Prodes, &key, d->reldum.relid, PRORELID, d->reldum.relowner,
	PRORELOWN, mdPROT, &tkey);

	if (!intv[0])
	{
		/* destroy permit 'relation' ALL */
		if (!(d->reldum.relstat & S_PROTRET) || !(d->reldum.relstat & S_PROTALL))
			r_relstat(d, S_PROTRET | S_PROTALL, 1);
		if (!(d->reldum.relstat & S_PROTUPS))
			return (0);
		del_all(d, &Prodes, &key, &tuple, &tkey, S_PROTUPS,
			tree_prot, prot_protree);
		return (0);
	}
	/* destroy permit 'relation' int {, int} */
	for (i = 0; intv[i]; i++)
	{
		if (j = AA_atoi(intv[i], &propermid))
			AAsyserr(11141, intv[i], j);
		if (!propermid)
		{
			if (!(d->reldum.relstat & S_PROTALL))
			{
				r_relstat(d, S_PROTALL, 1);
				*intv[i] = 0;
			}
			continue;
		}
		else if (propermid == 1)
		{
			if (!(d->reldum.relstat & S_PROTRET))
			{
				r_relstat(d, S_PROTRET, 1);
				*intv[i] = 0;
			}
			continue;
		}
		del_int(&Prodes, &key, &tuple, &tkey, intv[i], PROPERMID,
		tree_prot, prot_protree);
	}
	/* rescan to output error messages */
	for (j = 0; j < i; j++)
		if (*intv[j])
			error(5204, intv[j], (char *) 0);

	/* finally, check that there are still permissions
	** on the relation, if not must reset the S_PROTUPS bit in the relation
	** relation tuple for that relation's relstat.
	*/
	chk_const(d, &Prodes, &key, &tuple, d->reldum.relid, PRORELID,
	d->reldum.relowner, PRORELOWN, S_PROTUPS);
}


/*
**  I_CAT -- prepare catalogs for deletin of constraint
**
**	Initializes treerelid, treeowner, and treetype fields
**	of tree key. Also relation id and owner fields of
**	appropriate catalog c_desc, with key 'key'.
**
**	Parameters:
**		c_name -- name of catalog for opencatalog
**		c_desc -- descriptor of catalog
**		key -- key for catalog
**		relid -- relation.relid for relation to be de-constrained
**		id_attno -- attno of relid in constraint catalog c_desc
**		relowner -- relation.relowner for rel to be de-constrained
**		own_attno -- attno of owner in constrain catalog
**		type -- treetype for tree tuple (depends on catalog)
**		tkey -- key for AA_TREE catalog
**
**	Side Effects:
**		opencatalogs the constraint catalog c_desc, and the AA_TREE rel
**		for READ/WRITE. Sets keys.
*/
i_cat(c_name, c_desc, key, relid, id_attno, relowner, own_attno, type, tkey)
char			*c_name;
DESC			*c_desc;
char			*key;
char			*relid;
int			id_attno;
char			*relowner;
int			own_attno;
char			type;
struct tree		*tkey;
{
	extern DESC			Treedes;

#	ifdef xZTR1
	AAtTfp(13, 3, "I_CAT: c_name \"%s\", relid %s id_attno %d relowner %s own_attno %d type %d\n",
		c_name, relid, id_attno, relowner, own_attno, type);
#	endif

	opencatalog(AA_TREE, 2);
	AAam_setkey(&Treedes, tkey, relid, TREERELID);
	AAam_setkey(&Treedes, tkey, relowner, TREEOWNER);
	AAam_setkey(&Treedes, tkey, &type, TREETYPE);
	opencatalog(c_name, 2);
	AAam_clearkeys(c_desc);
	AAam_setkey(c_desc, key, relid, id_attno);
	AAam_setkey(c_desc, key, relowner, own_attno);
}


/*
**  DEL_ALL -- delete all constraints for a given relation
**
**	Deletes all constraints of a given type given by a constraint
**	catalog 'c_desc'. Note that Protection constraints 0 & 1, given
**	by relation.relstat field are not deleted here.
**
**	Parameters:
**		r_desc -- descriptor for relation to de-constrain (for
**			r_relstat)
**		c_desc -- constraint catalog descriptor
**		key -- c_desc's key
**		tuple -- c_desc's tuple (needed because sizeof tuple is not
**			known here, so must be allocated beforehand)
**		tkey -- tree key with TREERELID and TREERELOWNER setkeyed
**		bit -- bits in relstat to reset after deleting all constraints
**		tree_pred -- called with constraint tuple to determine
**			wether a tree tuple is present or not (as can happen
**			for AA_PROTECT catalog)
**		tree_field -- should return the treeid from tuple
*/
del_all(r_desc, c_desc, key, tuple, tkey, bit, tree_pred, tree_field)
DESC			*r_desc;
DESC			*c_desc;
char			*key;
char			*tuple;
struct tree		*tkey;
int			bit;
int			(*tree_pred)();
int			(*tree_field)();
{
	struct tup_id		lotid;
	struct tup_id		hitid;
	register int		i;

#	ifdef xZTR1
	AAtTfp(13, 4, "DEL_ALL: bit=0%o\n", bit);
#	endif

	if (i = AAam_find(c_desc, EXACTKEY, &lotid, &hitid, key))
		AAsyserr(11142, i);
	while (!(i = AAam_get(c_desc, &lotid, &hitid, tuple, TRUE)))
	{
		if (!AAkcompare(c_desc, tuple, key))
		{
			/* for each constraint of for a relation */
			if (i = AAam_delete(c_desc, &lotid))
				AAsyserr(11143, i);
			/* for crash recovery */
			if (i = AAam_flush_rel(c_desc, FALSE))
				AAsyserr(11144, i);
			/* if there is a tree tuple, destroy it */
			if ((*tree_pred)(tuple))
				del_tree(tkey, (*tree_field)(tuple));
		}
	}
	if (i != 1)
		AAsyserr(11145, i);
	/* turn off bit in relstat field */
	r_relstat(r_desc, bit, 0);
}


/*
**  DEL_INT -- delete from a constraint catalog a constraint
**
**	Parameters:
**		c_desc -- catalog descriptor
**		key -- catalog key
**		tuple -- catalog tuple (needed because tuple size unknown here)
**		tkey -- tree key with TREERELID and TREERELOWNER setkeyed
**		constid -- integer constraint id in string form
**		constattno -- attno of comstraint number in c_desc
**		tree_pred -- predicate on existence of tree tuple
**		tree_field -- returns treeid from constrain tuple
*/
del_int(c_desc, key, tuple, tkey, constid, constattno, tree_pred, tree_field)
DESC			*c_desc;
char			*key;
char			*tuple;
struct tree		*tkey;
char			*constid;
int			constattno;
int			(*tree_pred)();
int			(*tree_field)();
{
	struct tup_id		tid;
	register int		i;
	short			constnum;

#	ifdef xZTR1
	AAtTfp(13, 5, "DEL_INT: constid=%s, constattno=%d\n", constid, constattno);
#	endif

	if (i = AA_atoi(constid, &constnum))
		AAsyserr(11146, constid, i);
	AAam_setkey(c_desc, key, &constnum, constattno);
	if (!(i = AAgetequal(c_desc, key, tuple, &tid)))
	{
		if (i = AAam_delete(c_desc, &tid))
			AAsyserr(11147, constid, i);
		if ((*tree_pred)(tuple))
			del_tree(tkey, (*tree_field)(tuple));
		*constid = '\0';
		return;
	}
	else if (i != 1)
		AAsyserr(11148, i);
	/* bad constnum */
}


/*
**  DEST_TREE -- destroy a tree tuple with for a given treeid
**
**	Deletes all tuples from tree with 'treeid' and previously set
**	keys.
**
**	Parameters:
**		key -- tre key
**		treeid -- integer treeid
*/
del_tree(key, treeid)
struct tree	*key;
short		treeid;
{
	register int			i;
	register int			flag;
	struct tree			tuple;
	struct tup_id			lotid;
	struct tup_id			hitid;
	extern DESC			Treedes;

#	ifdef xZTR1
	AAtTfp(13, 6, "DEL_TREE: treeid=%d\n", treeid);
#	endif

	AAam_setkey(&Treedes, key, &treeid, TREEID);
	if (i = AAam_find(&Treedes, EXACTKEY, &lotid, &hitid, key))
		AAsyserr(11149, i, treeid);
	flag = 0;
	while (!(i = AAam_get(&Treedes, &lotid, &hitid, &tuple, TRUE)))
	{
		if (!AAkcompare(&Treedes, &tuple, key))
		{
			if (i = AAam_delete(&Treedes, &lotid))
				AAsyserr(11150, treeid, i);
			if (!flag)
				flag++;
		}
	}
	if (i != 1)
		AAsyserr(11151, i);
	if (!flag)
		AAsyserr(11152, treeid);
	if (i = AAam_flush_rel(&Treedes, FALSE))
		AAsyserr(11153, i);
}


/*
**  CHK_CONST -- check constraint catlg for tuples for a rel, and reset relatin.relstat
**
**	Parameters:
**		r_desc -- relation desc for de-constrained relation
**		c_desc -- catalog desc
**		key -- catalog key (here unknown size)
**		tuple -- " tuple space " " " " "
**		relid -- relation name
**		id_attno -- attno of relid
**		relowner -- relation owner
**		own_attno -- relowner attno
**		bit -- bits to reset in relstat if there are no constraints left
**
**	Side Effects:
**		reads catalog, maybe changes relstat field of AA_REL
**		r_desc tuple
*/
chk_const(r_desc, c_desc, key, tuple, relid, id_attno, relowner, own_attno, bit)
DESC			*r_desc;
DESC			*c_desc;
char			*key;
char			*tuple;
char			*relid;
int			id_attno;
char			*relowner;
int			own_attno;
int			bit;
{
	struct tup_id		tid;
	register int		i;


#	ifdef xZTR1
	AAtTfp(13, 7, "CHK_CONST: relid %s id_attno %d relowner %s own_attno %d bit 0%o)\n",
		relid, id_attno, relowner, own_attno, bit);
#	endif

	AAam_clearkeys(c_desc);
	AAam_setkey(c_desc, key, relid, id_attno);
	AAam_setkey(c_desc, key, relowner, own_attno);
	if ((i = AAgetequal(c_desc, key, tuple, &tid)) == 1)
		r_relstat(r_desc, bit, 0);
	else if (i < 0)
		AAsyserr(11154, i);
}


/*
**  R_RELSTAT -- set or reset bits in the relation.relstat field
**
**	Does the above for relation described by desc.
**
**	Parameters:
**		desc -- relation to have relation.relstat field changed
**		bit -- bits to set or reset
**		action -- 0 reset, 1 set
**
**	Side Effects:
**		relation is opened for READ/WRITE, relstat changed
*/
r_relstat(desc, bit, action)
DESC		*desc;
int		bit;
int		action;
{
	struct relation			tuple;
	struct relation			key;
	struct tup_id			tid;
	register int			i;
	extern DESC			Reldes;
	register DESC			*d;

	d = desc;
#	ifdef xZTR1
	AAtTfp(13, 8, "R_RELSTAT: bit=0%o, action %d\n", bit, action);
#	endif

	opencatalog(AA_REL, 2);
	AAam_clearkeys(&Reldes);
	AAam_setkey(&Reldes, &key, d->reldum.relid, RELID);
	AAam_setkey(&Reldes, &key, d->reldum.relowner, RELOWNER);
	if (i = AAgetequal(&Reldes, &key, &tuple, &tid))
		AAsyserr(11155, d->reldum.relid, d->reldum.relowner, i);
	if (action)
	{
		if (tuple.relstat == (i = tuple.relstat | bit))
			return;
		tuple.relstat = i;
	}
	else
	{
		if (tuple.relstat == (i = tuple.relstat & ~bit))
			return;
		tuple.relstat = i;
	}
	if ((i = AAam_replace(&Reldes, &tid, &tuple, 0)) < 0)
		AAsyserr(11156, i);
	if (i = AAam_flush_rel(&Reldes, FALSE))
		AAsyserr(11157, i);
}


/*
**  TREE_CONST -- True predicate
**
**	Called indirectly by routines wishing to know if
**	a integrity constraint has an associated tree tuple.
**	As this is always the case, returns TRUE always.
**
**	Parameters:
**		i -- integrity tuple
*/
tree_const()
{
#	ifdef xZTR1
	AAtTfp(13, 9, "TREE_CONST\n");
#	endif

	return (TRUE);
}


/*
**  TREE_PROT -- Protection tuple tree predicate
**
**	Called indirectly by routines wishing to know if
**	a protection constraint has an associated tree tuple.
**
**	Parameters:
**		p -- protection tuple
**
**	Returns:
**		TRUE -- if p->protree != -1
**		FLASE -- otherwise
*/
tree_prot(p)
struct protect		*p;
{
#	ifdef xZTR1
	AAtTfp(13, 9, "TREE_PROT: p->protree=%d\n", p->protree);
#	endif

	if (p->protree == -1)
		return (FALSE);
	else
		return (TRUE);
}


/*
**  PROT_PROTREE -- get protree field of a protection tuple
**
**	Parameters:
**		p -- protection tuple
**
**	Returns:
**		p->protree
*/
prot_protree(p)
struct protect	*p;
{
#	ifdef xZTR1
	AAtTfp(13, 9, "PROT_PROTREE: protree=%d\n", p->protree);
#	endif

	return (p->protree);
}


/*
**  INT_INTTREE -- get inttree field of a integrity tuple
**
**	Parameters:
**		i -- integrity tuple
**
**	Returns:
**		i->inttree
*/
int_inttree(i)
struct integrity	*i;
{
#	ifdef xZTR1
	AAtTfp(13, 9, "INT_INTTREE: inttree=%d\n", i->inttree);
#	endif

	return (i->inttree);
}
