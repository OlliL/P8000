# include	"amx.h"

rel_perm(r, pro_tups)
register AMX_REL	*r;
int			pro_tups;
{
# ifdef AMX_QRYMOD
	static DESC		*Prodes;
	register int		i;
	register AMX_QM		*qm;
	register int		noperm;
	register int		noday;
	register int		notime;
	register int		noterm;
	register int		nodom;
	struct protect		prokey;
	struct protect		protup;
	long			lotid;
	long			hitid;
	extern char		*talloc();

	if (!pro_tups)
# endif
		return (!bitset(AMX_RETRIEVE, r->rel_status));
# ifdef AMX_QRYMOD
	noperm = 1;

	/* check the AA_PROTECT catalog */
	if (!Prodes)
	{
		if (!(Prodes = (DESC *) talloc(sizeof (DESC))))
			yyexit(10, AA_PROTECT);
		if (AArelopen(Prodes, 0, AA_PROTECT))
			yyexit(8, AA_PROTECT);
	}
	AAam_setkey(Prodes, &prokey, r->rel_id, PRORELID);
	AAam_setkey(Prodes, &prokey, r->rel_owner, PRORELOWN);
	AAam_find(Prodes, EXACTKEY, &lotid, &hitid, &prokey);

	while (!(i = AAam_get(Prodes, &lotid, &hitid, &protup, 1)))
	{
		if (AAkcompare(Prodes, &prokey, &protup))
			continue;

		/* check if any qualification */
		if (protup.protree >= 0)
			continue;

		/* check for correct user*/
		if (!AAbequal("  ", protup.prouser, 2)
		 && !AAbequal(protup.prouser, AAusercode, 2))
			continue;

		/* check this operation as having been handled */
		i = r->rel_status & (AMX_RETRIEVE | AMX_REPLACE| AMX_DELETE| AMX_APPEND);
		clrbit(i, protup.proopset);
		if (!protup.proopset)
			continue;

		/* check if no special permit */
		noday = protup.prodowbgn == 0 && protup.prodowend == 6;
		notime = protup.protodbgn == 0 && protup.protodend == 1440;
		noterm = protup.proterm[0] == ' ';
		nodom = 1;
		if (protup.proopset == AMX_DELETE)
		{
			for (i = 0; i < 8; i++)
				protup.prodomset[i] = -1;
		}
		else
		{
			for (i = 0; i < 8; i++)
				if (protup.prodomset[i] != -1)
				{
					nodom = 0;
					break;
				}
		}
		if (noday && notime && noterm && nodom)
			setbit(protup.proopset, r->rel_status);

		/* if all permits given, search no more */
		if (setbit(AMX_RETRIEVE | AMX_REPLACE| AMX_DELETE| AMX_APPEND, r->rel_status))
			return (0);

		/* create new permit */
		if (!(qm = (AMX_QM *) talloc(sizeof (AMX_QM))))
			yyexit(58, r->rel_id);
		noperm = 0;
		qm->qm_next = r->rel_qm;
		r->rel_qm = qm;
		qm->qm_opset = protup.proopset;
		if (noday)
			qm->qm_dowbgn = -1;
		else
		{
			qm->qm_dowbgn = protup.prodowbgn;
			qm->qm_dowend = protup.prodowend;
		}
		if (notime)
			qm->qm_todbgn = -1;
		else
		{
			qm->qm_todbgn = protup.prodowbgn;
			qm->qm_todend = protup.prodowend;
		}
		if (noterm)
			qm->qm_term[0] = 0;
		else
			AAbmove(protup.proterm, qm->qm_term, PROTERM);
		AAbmove(protup.prodomset, qm->qm_term, sizeof qm->qm_term);
	}

	/* test 'AAam_get' return code */
	if (i < 0)
		yyexit(54);

	/* see if no tuples applied for some operation */
	return (noperm);
# endif
}


qry_perm(r, qry_mode)
register AMX_REL	*r;
register char		*qry_mode;
{
	register int		status;

# ifdef AMX_QRYMOD
# else
	if (!bitset(Qry_mode, r->rel_status))
	{
		/* check for SYSTEM CATALOG */
		if (bitset(AMX_CATALOG, status = r->rel_status))
			yyexit(94, r->rel_id, qry_mode);

		/* check for INDEX */
		if (bitset(AMX_INDEX, status))
			yyexit(96, r->rel_id, qry_mode);

		/* PROTECTION violation */
		yyexit(97, r->rel_id, qry_mode);
	}
# endif
}


# ifdef AMX_QRYMOD
dom_perm(r, qry_mode)
register AMX_REL	*r;
register int		qry_mode;
{
}
# endif
