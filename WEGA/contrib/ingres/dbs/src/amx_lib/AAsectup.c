# include	"AA_liba.h"

DESCXX		*AAp_desc;	/* global primary description   */
char		*AAold_tuple;	/* global primary tuple pointer */

char		AA_SET_ALL_KEYS[] = {1,1,1,1,1,1,1,0};


AAsectup(itup, pdom, tidp)
register char	*itup;	/* index tuple buffer */
register char	*pdom;	/* indexed domains */
long		*tidp;	/* value for "tidp" domain */
{
	register DESCXX		*p;	/* primary relation */
	register char		*poff;	/* offset in primary tuple */
	register int		len;

	p = AAp_desc;

# ifdef AMX_TRACE
	AAtTfp(20, 0,
# ifdef MSDOS
		"index tuple\t`%.10s'\n",
# else
		"index tuple\t`%.14s'\n",
# endif
		p->xx_desc.reldum.relid);
# endif

	do			/* copy value of included domains */
	{
		len = *pdom;
		poff = AAold_tuple + p->xx_desc.reloff[len];
		len = ctou(p->xx_desc.relfrml[len]);
		itup = AAbmove(poff, itup, len);
	} while (*++pdom);

	/* copy value for tidp */
	AAbmove(tidp, itup, sizeof AA_TID);
}
