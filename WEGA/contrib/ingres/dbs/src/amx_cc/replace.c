# include	"amx.h"

bgn_replace()
{
	register AMX_REL	*r;
	register int		len;

	if (!(r = Scan_rel))
		return (amxerror(199));

	Qry_mode = AMX_REPLACE;
	Domains = 0;
	Relation = r;

	/* check REPLACE permission */
	qry_perm(r, "replace");

	fprintf(FILE_tmp, msg(52), ++AP_level);
# if (S_ALIGN + L_ALIGN + D_ALIGN) > 0
	fprintf(FILE_tmp, msg(120));
# endif
	fprintf(FILE_tmp, msg(152), ALIGNMENT(len = r->rel_wid));
	fprintf(FILE_tmp, msg(153), len);
}


end_replace()
{
	register AMX_REL	*r;
	register int		uniq;
	register char		*repl;
	char			buf[100];
	extern AMX_REL		*getrel();
	extern char		get_replace();

	r = Relation;
	repl = buf;
	Dom_set[Domains] = '\0';
	while (r = getrel(r->rel_index, 0))
		*repl++ = get_replace(r);
	*repl = 0;
	uniq = Relation->rel_uniq;
	fprintf(FILE_tmp, msg(176), uniq, uniq, buf);
	fprintf(FILE_tmp, msg(178), AP_level--);
}


static char	get_replace(r)
register AMX_REL	*r;
{
	register char		*pdom;
	register char		*idom;

	for (pdom = Dom_set; *pdom; pdom++)
		for (idom = r->ind_dom; *idom; )
			if (*idom++ == *pdom)
				return ('1');
	return ('0');
}
