# include	"AA_liba.h"

# define	DUP_CHECK	1
# define	INVALID_TID	2

static int	AArep_code;	/* return code of AAam_replace()     */
static long	AArep_tid;	/* TID of AAam_replace()             */
static char	*AAnew_tuple;	/* global primary tuple pointer */

AAreplace(p, rel, tid, ptup, repl)
register DESCXX		*p;
char			*rel;
register long		*tid;
register char		*ptup;
register char		*repl;
{
# ifdef AMX_TRACE
	AAtTfp(7, 0,
# ifdef MSDOS
		"replace rel\t`%.10s'\n",
# else
		"replace rel\t`%.14s'\n",
# endif
		rel);
# endif

	errno = 0;

	if (!AA_DESC)
		AAerror(111, "replace");

	if (!p)
		AAerror(106, rel);

	if (!AA_NOLOCKS && !(p->xx_excl & M_EXCL))
		AAerror(22, rel);

	AA_on();

	AA_TID = *tid;
	AArep_tid = *tid;
	AA = AArep_code = AAam_replace(p, &AArep_tid, ptup, DUP_CHECK);

# ifdef AM_TRACE
	AAtTfp(7, 10, "##AM_REPLACE\t%d\n", AA);
	if (AAtTfp(7, 11, "##\t\told"))
		AAdumptid(tid);
	if (AAtTfp(7, 12, "##\t\tnew"))
		AAdumptid(&AArep_tid);
	if (AAtTfp(7, 13, "##\t\tnewtup"))
		AAprtup(p, ptup);
# endif

	if (AA < 0 || AA == INVALID_TID)
		AAerror(5, rel);

	if (ctoi(p->xx_desc.reldum.relindxd) == SECBASE)
	{
		AAp_desc = p;
		AAold_tuple = AA_STUP;
		AAnew_tuple = ptup;
		while (p = p->xx_index)
			AAsecreplace(p, *repl++);
	}

	AA_off();
}


AAsecreplace(i, repl)
register DESCXX		*i;
register int		repl;
{
	register char	*old_tup;
	long		tid;
	long		old_tid;
	char		itup[MAXTUP];
	char		ikey[MAXTUP];

# ifdef AMX_TRACE
	AAtTfp(8, 0,
# ifdef MSDOS
		"replace index\t`%.10s' (%d)\n",
# else
		"replace index\t`%.14s' (%d)\n",
# endif
		i->xx_desc.reldum.relid, repl);
# endif

	errno = 0;
	if (AArep_tid == AA_TID)	/* identical tid's */
	{
		if (AArep_code)
		{
# ifdef AMX_TRACE
			AAtTfp(8, 1, "\t\tidentical\n");
# endif
			return;	/* identical tuple */
		}
		else if (repl == '0')
		{
# ifdef AMX_TRACE
			AAtTfp(8, 2, "\t\tdoms unchanged\n");
# endif
			return;	/* index domains unchanged */
		}
	}

	AAsectup(ikey, i->xx_indom, &AA_TID);
	/* set all keys on index descriptor */
	AAbmove(AA_SET_ALL_KEYS, &i->xx_desc.relgiven[1], MAXKEYS + 1);

	AA = AAgetequal(i, ikey, itup, &tid);

# ifdef AM_TRACE
	AAtTfp(8, 6, "##AM_GETEQUAL\t%d\n", AA);
	if (AAtTfp(8, 7, "##\t\told"))
		AAdumptid(&tid);
	if (AAtTfp(8, 8, "##\t\toldkey"))
		AAprkey(i, ikey);
	if (AAtTfp(8, 9, "##\t\tnewtup"))
		AAprtup(i, itup);
# endif

	if (AA)
		AAerror(6, i->xx_desc.reldum.relid);

	if (AArep_code)
	{
# ifdef AMX_TRACE
		AAtTfp(8, 3, "\t\tdelete\n");
# endif
		AA = AAam_delete(i, &tid);

# ifdef AM_TRACE
	AAtTfp(8, 14, "##AM_DELETE\t%d\n", AA);
	if (AAtTfp(8, 15, "##\t\told"))
		AAdumptid(&tid);
# endif

		if (AA)
			AAerror(7, i->xx_desc.reldum.relid);

		return;
	}

# ifdef AMX_TRACE
	AAtTfp(8, 4, "\t\treplace\n");
# endif
	old_tid = tid;
	old_tup = AAold_tuple;
	AAold_tuple = AAnew_tuple;
	AAsectup(itup, i->xx_indom, &AArep_tid);
	AAold_tuple = old_tup;
	AA = AAam_replace(i, &tid, itup, DUP_CHECK);

# ifdef AM_TRACE
	AAtTfp(8, 10, "##AM_REPLACE\t%d\n", AA);
	if (AAtTfp(8, 11, "##\t\told"))
		AAdumptid(&old_tid);
	if (AAtTfp(8, 12, "##\t\tnew"))
		AAdumptid(&tid);
	if (AAtTfp(8, 13, "##\t\tnewtup"))
		AAprtup(i, itup);
# endif

	if (AA < 0 || AA == INVALID_TID)
		AAerror(5, i->xx_desc.reldum.relid);
}
