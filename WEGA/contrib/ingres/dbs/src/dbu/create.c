# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"
# include	"../h/aux.h"
# include	"../h/catalog.h"
# include	"../h/symbol.h"
# include	"../h/lock.h"

/*
**  CREATE -- create new relation
**
**	This module creates a brand new relation in the current
**	directory (database).  The relation is always created as
**	a paged heap.  It may not redefine an existing relation,
**	or rename a system catalog.
**
**	Defines:
**		create -- driver
**		chk_att -- check attribute for validity
**		ins_att -- insert attribute into AA_ATT catalog
**		dup_att -- check for duplicate attribute
**		initstructs -- initialize relation and attribute
**			tuples for insertion into catalog
**		formck -- check for valid attribute type.
*/

struct domain
{
	char	*name;
	char	frmt;
	char	frml;
};

/*
**  CREATE -- create new relation
**
**	This routine is the driver for the create module.
**
**	Parameters:
**		pc -- parameter count
**		pv -- parameter vector:
**			0 -- relation status (relstat) -- stored into
**				the 'relstat' field in the relation
**				relation, and used to determine the
**				caller.  Interesting bits are:
**
**				S_INDEX -- means called by the index
**					processor.  If set, the 'relindxd'
**					field will also be set to -1
**					(SECINDEX) to indicate that this
**					relation is a secondary index.
**				S_CATALOG -- this is a system catalog.
**					If set, this create was called
**					from creatdb, and the physical
**					file is not created.  Also, the
**					expiration date is set infinite.
**				S_VIEW -- this is a view.  Create has
**					been called by the 'define'
**					statement, rather than the
**					'create' statement.  The physical
**					file is not created.
**
**			1 -- relation name.
**			2 -- attname1
**			3 -- format1
**			4, etc -- attname, format pairs.
**
**	Returns:
**		zero -- successful create.
**		else -- failure somewhere.
*/
create(pc, pv)
int	pc;
char	**pv;
{
	register char			**pp;
	register int			i;
	int				bad;
	struct domain			domain[MAXDOM];
	struct domain			*dom;
	char				*relname;
	char				tempname[MAXNAME + 3];
	struct tup_id			tid;
	struct relation			rel;
	struct attribute		att;
	DESC				desr;
	extern char			*AAusercode;
	extern DESC			Reldes;
	extern DESC			Attdes;
	extern int			errno;
	register int			relstat;
	int				critical_section;
	long				npages;
	int				fdes;

#	ifdef xZTR1
	AAtTfp(2, -1, "CREATE: %s\n", pv[1]);
#	endif
	pp = pv;
	relstat = AA_oatoi(pp[0]);
	/*
	**	If this database has query modification, then default
	**	to denial on all user relations.
	**	(Since views cannot be protected, this doesn't apply to them)
	*/
	if ((AAdmin.adhdr.adflags & A_QRYMOD) && (!(relstat & S_VIEW)))
		relstat |= (S_PROTALL | S_PROTRET);
	relname = *(++pp);
	AApmove(relname, rel.relid, MAXNAME, ' ');
	AAbmove(AAusercode, rel.relowner, 2);
	AAbmove(rel.relid, att.attrelid, MAXNAME + 2);
	opencatalog(AA_REL, 2);

	/* check for duplicate relation name */
	if (!(relstat & S_CATALOG))
	{
		if (!AArelopen(&desr, -1, relname))
		{
			if (AAbequal(desr.reldum.relowner, rel.relowner, 2))
			{
				return (error(5102, relname, (char *) 0));	/* bad relname */
			}
			if (desr.reldum.relstat & S_CATALOG)
			{
				return (error(5103, relname, (char *) 0));	/* attempt to rename system catalog */
			}
		}
	}
	opencatalog(AA_ATT, 2);

	/* initialize structures for system catalogs */
	initstructs(&att, &rel);
	rel.relstat = relstat;
	if (relstat & S_CATALOG)
		rel.relsave = 0;
	else if ((relstat & S_INDEX))
		rel.relindxd = SECINDEX;

#	ifdef xZTR3
	if (AAtTf(2, 2))
		AAprtup(&Reldes, &rel);
#	endif

	/* check attributes */
	pp++;
	for (i = pc - 2; i > 0; i -= 2)
	{
		bad = chk_att(&rel, pp[0], pp[1], domain);
		if (bad)
		{
			return (error(bad, relname, pp[0], pp[1], (char *) 0));
		}
		pp += 2;
	}

	/*
	** Create files if appropriate. Concurrency control for
	** the create depends on the actual file. To prevent
	** to users with the same usercode from creating the
	** same relation at the same time, their is check
	** on the existence of the file. The important events are
	** (1) if a tuple exists in the relation relation then
	** the relation really exists. (2) if the file exists then
	** the relation is being created but will not exist for
	** use until the relation relation tuple is present.
	** For VIEWS, the file is used for concurrency control
	** during the create but is removed afterwards.
	*/
	if (!(relstat & S_CATALOG))
	{
		/* for non system named temporary relations
		** set a critical section lock while checking the
		** existence of a file.  If it exists, error return(5102)
		** else create file.
		*/
		critical_section = 0;
		if (!AAbequal(rel.relid, SYSNAME, s_SYSNAME))
		{
			critical_section++;
			if (AAsetcsl(NO_REL) < 0)
				return (error(5000, (char *) 0));
			if ((fdes = open(rel.relid, 0)) >= 0)
			{
				close(fdes);	/* file already exists */
				AAunlcs(NO_REL);
				return (error(5102, relname, (char *) 0));
			}
			errno = 0;	/* file doesn't exist */
		}
		AAdbname(rel.relid, rel.relowner, tempname);
		desr.relfp = creat(tempname, FILEMODE);
		if (critical_section)
			AAunlcs(NO_REL);
		if (desr.relfp < 0)
			AAsyserr(11012, rel.relid);
		desr.reltid = -1;	/* init reltid to unused */
		if (!(relstat & S_VIEW))
		{
			npages = 1;
			if (i = formatpg(&desr, npages))
				AAsyserr(11013, i);
		}

		close(desr.relfp);
	}

	/* insert attributes into attribute relation */
	pp = pv + 2;
	dom = domain;
	for (i = pc - 2; i > 0; i -= 2)
	{
		ins_att(&Attdes, &att, dom++);
		pp += 2;
	}

	/*
	** Flush the attributes. This is necessary for recovery reasons.
	** If for some reason the relation relation is flushed and the
	** machine crashes before the attributes are flushed, then recovery
	** will not detect the error.
	** The call below cannot be a "AAnoclose" without major changes to
	** creatdb.
	*/
	if (i = AApageflush((ACCBUF *) 0))
		AAsyserr(11014, i);

	if (i = AAam_insert(&Reldes, &tid, &rel, FALSE))
		AAsyserr(11015, rel.relid, i);

	/*
	** Flush the relation. This is necessary for recovery reasons.
	*/
	if (i = AApageflush((ACCBUF *) 0))
		AAsyserr(11034, i);

	if (relstat & S_VIEW)
		unlink(tempname);
	return (0);
}


/*
**  CHK_ATT -- check attribute for validity
**
**	The attribute is checked to see if
**	* it's name is ok (within MAXNAME bytes)
**	* it is not a duplicate name
**	* the format specified is legal
**	* there are not a ridiculous number of attributes
**	  (ridiculous being defined as anything over MAXDOM - 1)
**	* the tuple is not too wide to fit on one page
**
**	Parameters:
**		r -- relation relation tuple for this relation.
**		attname -- tentative name of attribute.
**		format -- tentative format for attribute.
**		d -- a 'struct domain' used to determine dupli-
**			cation, and to store the resulting name and
**			format in.
**
**	Returns:
**		zero -- OK
**		5104 -- bad attribute name.
**		5105 -- duplicate attribute name.
**		5106 -- bad attribute format.
**		5107 -- too many attributes.
**		5108 -- tuple too wide.
**
**	Side Effects:
**		'r' has the relatts and relwid fields updated to
**		reflect the new attribute.
*/
chk_att(r, attname, format, d)
register struct relation	*r;
char				*attname;
char				*format;
register struct domain		*d;
{
	register int			i;

#	ifdef xZTR3
	if (AAtTfp(2, 1, "CHK_ATT: %s ", attname))
	{
		AAxputchar(*format);
		printf("%s\n", format + 1);
	}
#	endif

/*
**	if (AAsequal(attname, "tidp"))
**		return (5104);
*/
	if ((i = dup_att(attname, r->relatts, d)) < 0)
		return (5105);		/* duplicate attribute */
	if (formck(format, &d[i]))
		return (5106);		/* bad attribute format */
	r->relatts++;
	r->relwid += ctou(d[i].frml);
	if (r->relatts >= MAXDOM)
		return (5107);		/* too many attributes */
	if (r->relwid > AAmaxtup && !(r->relstat & S_VIEW))
		return (5108);		/* tuple too wide */
	return (0);
}


/*
**  INS_ATT -- insert attribute into attribute relation
**
**	Parameters:
**		des -- relation descriptor for the AA_ATT catalog.
**		att -- attribute tuple, preinitialized with all sorts
**			of good stuff (everything except 'attname',
**			'attfrmt', and 'attfrml'; 'attid' and 'attoff'
**			must be initialized to zero before this routine
**			is called the first time.
**		dom -- 'struct domain' -- the information needed about
**			each domain.
**
**	Side Effects:
**		The 'att' tuple is updated in the obvious ways.
**		A tuple is added to the AA_ATT catalog.
*/
ins_att(des, att, dom)
DESC			*des;
struct attribute	*att;
struct domain		*dom;
{
	struct tup_id		tid;
	register struct domain	*d;

	d = dom;

	AApmove(d->name, att->attname, MAXNAME, ' ');
	att->attfrmt = d->frmt;
	att->attfrml = d->frml;
	att->attid++;
	if (AAam_insert(des, &tid, att, FALSE))
		AAsyserr(11016, d->name);
	att->attoff += ctou(att->attfrml);
}


/*
**  DUP_ATT -- check for duplicate attribute
**
**	The attribute named 'name' is inserted into the 'attalias'
**	vector at position 'count'.  'Count' should be the count
**	of existing entries in 'attalias'.  'Attalias' is checked
**	to see that 'name' is not already present.
**
**	Parameters:
**		name -- the name of the attribute.
**		count -- the count of attributes so far.
**		domain -- 'struct domain' -- the list of domains
**			so far, names and types.
**
**	Returns:
**		-1 -- attribute name is a duplicate.
**		else -- index in 'domain' for this attribute (also
**			the attid).
**
**	Side Effects:
**		The 'domain' vector is extended.
*/
dup_att(name, count, domain)
char		*name;
int		count;
struct domain	domain[];
{
	register struct domain	*d;
	register int		lim;
	register int		i;

	lim = count;
	d = domain;

	for (i = 0; i < lim; i++)
		if (AAsequal(name, d++->name))
			return (-1);
	if (count < MAXDOM)
		d->name = name;
	return (i);
}


/*
**  INITSTRUCTS -- initialize relation and attribute tuples
**
**	Structures containing images of AA_REL relation and
**	AA_ATT relation tuples are initialized with all the
**	information initially needed to do the create.  Frankly,
**	the only interesting part is the the expiration date
**	computation; 604800 is exactly the number
**	of seconds in one week.
**
**	Parameters:
**		att -- attribute relation tuple.
**		rel -- relation relation tuple.
**
**	Side Effects:
**		'att' and 'rel' are initialized.
*/
initstructs(att1, rel1)
struct attribute	*att1;
struct relation		*rel1;
{
	register struct relation	*rel;
	register struct attribute	*att;

	rel = rel1;
	att = att1;

	/* setup expiration date (today + one week) */
	time(&rel->relsave);
	rel->relsave += 604800L;
	rel->reltups = 0;
	rel->relatts = 0;
	rel->relwid = 0;
	rel->relprim = 1;
	rel->relspec = M_HEAP;
	rel->relindxd = 0;
	rel->relspare = 0;

	att->attxtra = 0;
	att->attid = 0;
	att->attoff = 0;
}


/*
**  CHECK ATTRIBUTE FORMAT AND CONVERT
**
**	The string 'a' is checked for a valid attribute format
**	and is converted to internal form.
**
**	zero is returned if the format is good; one is returned
**	if it is bad.  If it is bad, the conversion into a is not
**	made.
**
**	A format of CHAR can be length zero but a format
**	of 'c' cannot.
*/
formck(a, dom)
char		*a;
struct domain	*dom;
{
	short			len;
	register int		i;
	char			c;
	register char		*p;
	register struct domain	*d;

	p = a;
	c = *p++;
	d = dom;

	if (AA_atoi(p, &len))
		return (1);
	i = len;

	switch (c)
	{
	  case INT:
	  case 'i':
		if (i == sizeof (char) || i == sizeof (short) || i == sizeof (long))
		{
			d->frmt = INT;
			d->frml = i;
			return (0);
		}
		return (1);

	  case FLOAT:
	  case 'f':
		if
		(
# ifndef NO_F4
			i == sizeof (float) ||
# endif
			i == sizeof (double)
		)
		{
			d->frmt = FLOAT;
			d->frml = i;
			return (0);
		}
		return (1);

	  case 'c':
		if (!i)
			return (1);
	  case CHAR:
		if (i > MAXFIELD || i < 0)
			return (1);
		d->frmt = CHAR;
		d->frml = i;
		return (0);
	}
	return (1);
}
