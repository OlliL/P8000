# include	"amx.h"

bgn_append(scan_rel)
register int	scan_rel;
{
	register AMX_REL	*r;

	Qry_mode = AMX_APPEND;
	Domains = 0;
	if (scan_rel)
	{
		if (Scan_rel)
			Relation = Scan_rel;
		else
			return (amxerror(189));
	}

	/* check APPEND permission */
	qry_perm(r = Relation, "append");

	fprintf(FILE_tmp, msg(150), ++AP_level);
# if (S_ALIGN + L_ALIGN + D_ALIGN) > 0
	fprintf(FILE_tmp, msg(120));
# endif
	fprintf(FILE_tmp, msg(152), ALIGNMENT(r->rel_wid));
	if (scan_rel)
		fprintf(FILE_tmp, msg(153), r->rel_wid);
	else
		fprintf(FILE_tmp, msg(183), r->rel_uniq, r->rel_uniq);
}


end_append()
{
	register AMX_REL	*r;

	r = Relation;
	fprintf(FILE_tmp, msg(154), r->rel_uniq, r->rel_uniq);
	fprintf(FILE_tmp, msg(158), AP_level--);
}
