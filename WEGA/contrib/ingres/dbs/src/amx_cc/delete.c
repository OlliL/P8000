# include	"amx.h"

delete()
{
	register AMX_REL	*r;

	if (!(r = Scan_rel))
		return (amxerror(190));

	Qry_mode = AMX_DELETE;

	/* check DELETE permission */
	qry_perm(r, "delete");

	fprintf(FILE_tmp, msg(160), ++AP_level);
	fprintf(FILE_tmp, msg(162), r->rel_uniq, r->rel_uniq);
	fprintf(FILE_tmp, msg(164), AP_level--);
}


scan_goto(val)
register int	val;
{
	register AMX_REL	*r;

	if (!(r = Scan_rel))
		return (amxerror(122 + 2 * val));

	fprintf(FILE_tmp, msg(99), 2 * AP_label[AP_level] + val);
}
