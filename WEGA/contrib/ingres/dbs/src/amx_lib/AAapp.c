# include	"AA_liba.h"

AAappend(p, rel, ptup)
register DESCXX		*p;
register char		*rel;
register char		*ptup;
{
# ifdef AMX_TRACE
	AAtTfp(3, 0,
# ifdef MSDOS
		"append rel\t`%.10s'\n",
# else
		"append rel\t`%.14s'\n",
# endif
		rel);
# endif

	errno = 0;

	if (!AA_DESC)
		AAerror(111, "append");

	if (!p)
		AAerror(106, rel);

	if (!AA_NOLOCKS && !(p->xx_excl & M_EXCL))
		AAerror(20, rel);

	AArefresh(p);
	AA_on();
	AA = AAam_insert(p, &AA_TID, ptup, 1);

# ifdef AM_TRACE
	AAtTfp(3, 10, "##AM_INSERT\t%d\n", AA);
	if (AAtTfp(3, 11, "##\t\tnew"))
		AAdumptid(&AA_TID);
	if (AAtTfp(3, 12, "##\t\tnewtup"))
		AAprtup(p, ptup);
# endif

	if (AA < 0)
		AAerror(2, rel);

	if (ctoi(p->xx_desc.reldum.relindxd) == SECBASE)
	{
		AAp_desc = p;
		AAold_tuple = ptup;
		while (p = p->xx_index)
			AAsecappend(p);
	}

	AA_off();
}


AAsecappend(i)
register DESCXX		*i;
{
	char		itup[MAXTUP];
	long		tid;

# ifdef AMX_TRACE
	AAtTfp(4, 0,
# ifdef MSDOS
		"append index\t`%.10s'\n",
# else
		"append index\t`%.14s'\n",
# endif
		i->xx_desc.reldum.relid);
# endif

	errno = 0;
	AArefresh(i);
	AAsectup(itup, i->xx_indom, &AA_TID);
	/* set all keys on index descriptor */
	AAbmove(AA_SET_ALL_KEYS, &i->xx_desc.relgiven[1], MAXKEYS + 1);

	AA = AAam_insert(i, &tid, itup, 1);

# ifdef AM_TRACE
	AAtTfp(4, 10, "##AM_INSERT\t%d\n", AA);
	if (AAtTfp(4, 11, "##\t\tnew"))
		AAdumptid(&tid);
	if (AAtTfp(4, 12, "##\t\tnewtup"))
		AAprtup(i, itup);
# endif

	if (AA < 0)
		AAerror(2, i->xx_desc.reldum.relid);
}
