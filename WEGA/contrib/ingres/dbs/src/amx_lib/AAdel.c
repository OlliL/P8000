# include	"AA_liba.h"

AAdelete(p, rel, tid)
register DESCXX		*p;
register char		*rel;
register long		*tid;
{
# ifdef AMX_TRACE
	AAtTfp(5, 0,
# ifdef MSDOS
		"delete rel\t`%.10s'\n",
# else
		"delete rel\t`%.14s'\n",
# endif
		rel);
# endif

	errno = 0;

	if (!AA_DESC)
		AAerror(111, "delete");

	if (!p)
		AAerror(106, rel);

	if (!AA_NOLOCKS && !(p->xx_excl & M_EXCL))
		AAerror(21, rel);

	AA_on();

	AA_TID = *tid;
	AA = AAam_delete(p, tid);

# ifdef AM_TRACE
	AAtTfp(5, 14, "##AM_DELETE\t%d\n", AA);
	if (AAtTfp(5, 15, "##\t\told"))
		AAdumptid(tid);
# endif

	if (AA < 0)
		AAerror(3, rel);

	if (ctoi(p->xx_desc.reldum.relindxd) == SECBASE)
	{
		AAp_desc = p;
		AAold_tuple = AA_STUP;
		while (p = p->xx_index)
			AAsecdelete(p);
	}

	AA_off();
}


AAsecdelete(i)
register DESCXX		*i;
{
	char		itup[MAXTUP];
	char		ikey[MAXTUP];
	long		tid;

# ifdef AMX_TRACE
	AAtTfp(6, 0,
# ifdef MSDOS
		"delete index\t`%.10s'\n",
# else
		"delete index\t`%.14s'\n",
# endif
		i->xx_desc.reldum.relid);
# endif

	errno = 0;
	AAsectup(ikey, i->xx_indom, &AA_TID);
	/* set all keys on index descriptor */
	AAbmove(AA_SET_ALL_KEYS, &i->xx_desc.relgiven[1], MAXKEYS + 1);

	AA = AAgetequal(i, ikey, itup, &tid);

# ifdef AM_TRACE
	AAtTfp(6, 6, "##AM_GETEQUAL\t%d\n", AA);
	if (AAtTfp(6, 7, "##\t\told"))
		AAdumptid(&tid);
	if (AAtTfp(6, 8, "##\t\toldkey"))
		AAprkey(i, ikey);
	if (AAtTfp(6, 9, "##\t\tnewtup"))
		AAprtup(i, itup);
# endif

	if (AA)
		AAerror(4, i->xx_desc.reldum.relid);

	AA = AAam_delete(i, &tid);

# ifdef AM_TRACE
	AAtTfp(6, 14, "##AM_DELETE\t%d\n", AA);
	if (AAtTfp(6, 15, "##\t\told"))
		AAdumptid(&tid);
# endif

	if (AA < 0)
		AAerror(3, i->xx_desc.reldum.relid);
}
