# include	"amx.h"

/* LOOKUP RELATION "name" */
rel_lookup(rel, status)
register char	*rel;
int		status;
{
	register AMX_REL 	*r;
	register AMX_ATT 	*a;
	register int		i;
	register int		j;
	register DESC		*d;
	register char		*p;
# ifdef AMX_SC
	int			decl;
# endif
	struct relation 	rel_key;
	struct relation 	rel_tup;
	struct attribute	dom_key;
	struct attribute	dom_tup;
	struct index		ind_key;
	struct index		ind_tup;
	struct tup_id		tid1;
	struct tup_id		tid2;
	int			primary;
	extern int		write();
	extern AMX_REL		*getrel();
	extern AMX_REL		*talloc();

	r = (AMX_REL *) 0;
	for (p = Reltab, i = 0; i < Relcount; i++, p += MAXNAME + 1)
		if (AAsequal(p, rel))
		{
			r = getrel(i, 1);
			break;
		}
	if (r)
	{
		if (!status && bitset(AMX_INTERNAL, r->rel_status))
			yyexit(12, rel);
		Relation = r;
		return;
	}


/* CREATE a new relation entry */

	/* Step 1: get the relation tuple	*/
	d = &AAdmin.adreld;
	AAam_clearkeys(d);
	AAam_setkey(d, &rel_key, rel, RELID);
	AAam_setkey(d, &rel_key, AAusercode, RELOWNER);
	if ((i = AAgetequal(d, &rel_key, &rel_tup, &tid1)) == 1)
	{
		/* not a user relation, try owned by dba */
		AAam_setkey(d, &rel_key, AAdmin.adhdr.adowner, RELOWNER);
		i = AAgetequal(d, &rel_key, &rel_tup, &tid1);
	}
	if (i)
		yyexit((i == 1)? 82: 8, rel);

	/* Step 2: allocate symbol space	*/
	r = Temp_rel;
	if (Relcount >= MAX_RELS)
		yyexit(10, rel);

	/* Step 3: fill in the relation part	*/
	AAsmove(rel, &Reltab[Relcount * (MAXNAME + 1)]);
	r->rel_index = MAX_RELS;
# ifdef AMX_QRYMOD
	r->rel_qm = (AMX_QM *) 0;
	AAbmove(rel_tup.relowner, r->rel_owner, 2);
# endif
	r->rel_uniq = Relcount++;
	AAsmove(rel, r->rel_id);
	r->rel_status = status;
	r->rel_spec = rel_tup.relspec;
	r->rel_wid = rel_tup.relwid;
	r->rel_wid = ALIGNMENT(r->rel_wid);
	r->ind_wid = 0;
	r->rel_atts = rel_tup.relatts;

	/* Step 4: check for valid relation	*/
	i = rel_tup.relstat;
	if (status)
		goto internal;

	/* ------- check for protection tuples	*/
	if (!AAbequal(AAusercode, rel_tup.relowner, 2))
	{
		if (!bitset(S_PROTALL, i))
			goto all_perm;
		if (!bitset(S_PROTRET, i))
			r->rel_status |= AMX_RETRIEVE;
		if (rel_perm(r, rel_tup.relstat & S_PROTUPS))
			yyexit(12, rel);
	}
	else
	{
all_perm:
		r->rel_status |= AMX_RETRIEVE | AMX_REPLACE | AMX_DELETE | AMX_APPEND;
	}

	/* ------- check for integrity tuples	*/
	if (bitset(S_INTEG, i))
		yyexit(14, rel);

	/* ------- view check			*/
	if (bitset(S_VIEW, i))
		yyexit(16, rel);

internal:
# ifdef AMX_SC
	decl = ExpImp < 0;
# endif

	/* ------- check for system catalog	*/
	if (bitset(S_CATALOG, i))
	{
		r->rel_status |= AMX_CATALOG;
		clrbit(AMX_REPLACE | AMX_DELETE | AMX_APPEND, r->rel_status);
	}

	/* ------- check for index relation	*/
	else if (bitset(S_INDEX, i) || ctoi(rel_tup.relindxd) == SECINDEX)
	{
		r->rel_status |= AMX_INDEX;
		clrbit(AMX_RETRIEVE | AMX_REPLACE | AMX_DELETE | AMX_APPEND, r->rel_status);
		Relation->rel_index = r->rel_uniq;
		swaprel(Relation, write);
	}

	else	/* user relation */
	{
		r = getrel(r->rel_uniq, 1);
		Temp_rel[0].rel_uniq = MAX_RELS;
		Relation = r;
		if (bitset(S_NOUPDT, i))
			clrbit(AMX_REPLACE | AMX_DELETE | AMX_APPEND, r->rel_status);
		if (ctoi(rel_tup.relindxd) == SECBASE)
		{	/* relation has indexes  */
			r->rel_status |= AMX_PRIMARY;
			d = &Indes;
			AAam_clearkeys(d);
			AAam_setkey(d, &ind_key, rel, IRELIDP);
			AAam_setkey(d, &ind_key, rel_tup.relowner, IOWNERP);
			if (AAam_find(d, EXACTKEY, &tid1, &tid2, &ind_key))
				yyexit(18, rel);
			while (!(i = AAam_get(d, &tid1, &tid2, &ind_tup, TRUE)))
			{
				if (AAkcompare(d, &ind_tup, &ind_key))
					continue;

				/* found one			 */
				/* make a string,		 */
				/* overwriting irelspeci	 */
				ind_tup.irelspeci = ' ';
				for (p = ind_tup.irelidi; *p != ' '; p++)
					continue;
				*p = '\0';
				/* save primary relation */
				swaprel(r, write);
				primary = r->rel_uniq;
				rel_lookup(ind_tup.irelidi, AMX_INTERNAL);
				/* restore primary relation */
				r = getrel(primary, 0);
				if (r->ind_wid < Relation->rel_wid)
					r->ind_wid = Relation->rel_wid;
				for (i = 0; i < MAXKEYS && ind_tup.idom[i]; )
					i++;
				/* make a string */
				AAbmove(ind_tup.idom, Relation->ind_dom, i);
				Relation->ind_dom[i] = '\0';
				/* complete INDEX-relation */
				swaprel(Relation, write);
				fprintf(FILE_amx,
# ifdef AMX_SC
					msg(166 + decl), Relation->rel_uniq);
				if (decl)
					continue;
# else
					msg(166), Relation->rel_uniq);
# endif
				for (j = 0; j < i; j++)
					fprintf(FILE_amx, "\\%o", ctou(Relation->ind_dom[j]));
				fprintf(FILE_amx, "\";\n");
			}
			if (i < 0)
				yyexit(20, rel);
		}
	}

	/* Step 5: get the attribute tuples	*/
	d = &AAdmin.adattd;
	AAam_clearkeys(d);
	AAam_setkey(d, &dom_key, rel_tup.relid, ATTRELID);
	AAam_setkey(d, &dom_key, rel_tup.relowner, ATTOWNER);
	if (AAam_find(d, EXACTKEY, &tid1, &tid2, &dom_key))
		yyexit(22, rel);

	/* zero out all format types */
	i = rel_tup.relatts;
	for (a = r->rel_dom; i; --i, a++)
		a->att_type = 0;

	i = rel_tup.relatts;
	while (i && !AAam_get(d, &tid1, &tid2, &dom_tup, TRUE))
	{
		/* does this attribute belong ? */
		if (AAkcompare(d, &dom_tup, &dom_key))
			continue;

		/* yes, it belongs */
		if (dom_tup.attid < 1 || dom_tup.attid > rel_tup.relatts)
			yyexit(24, dom_tup.attid, rel);
		j = dom_tup.attid;
		a = &r->rel_dom[j - 1];
		if (a->att_type)
			yyexit(23, dom_tup.attid, rel);

		AAbmove(dom_tup.attname, p = a->att_name, MAXNAME);
		a->att_name[MAXNAME] = ' ';
		while (*p != ' ')
			p++;
		*p = '\0';

		a->att_id = dom_tup.attid;
		a->att_key = dom_tup.attxtra;
		switch (dom_tup.attfrmt)
		{
		  case INT:
			if (dom_tup.attfrml == sizeof (char))
				a->att_type = opCHAR;
			else if (dom_tup.attfrml == sizeof (short))
				a->att_type = opINT;
			else
				a->att_type = opLONG;
			break;

		  case FLOAT:
# ifndef NO_F4
			if (dom_tup.attfrml == sizeof (float))
				a->att_type = opFLT;
			else
# endif
				a->att_type = opDOUBLE;
			break;

		  default:
			a->att_type = opSTRING;
		}
		a->att_len = dom_tup.attfrml;
		a->att_off = dom_tup.attoff;
		--i;
	}
	if (i)
		yyexit(21, i, rel);

	/* create the global relation pointer */
	swaprel(r, write);
	r = getrel(r->rel_uniq, 1);
	Temp_rel[0].rel_uniq = MAX_RELS;
# ifdef AMX_SC
	fprintf(FILE_amx, msg(168 + decl), r->rel_uniq);
	fprintf(FILE_amx, msg(170 + decl), r->rel_uniq, r->rel_id);
# else
	fprintf(FILE_amx, msg(168), r->rel_uniq);
	fprintf(FILE_amx, msg(170), r->rel_uniq, r->rel_id);
# endif
	Relation = r;
}
