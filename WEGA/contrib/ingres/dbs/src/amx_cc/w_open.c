# include	"amx.h"

w_open()
{
	register AMX_REL	*r;
	register int		uniq;
	extern AMX_REL		*getrel();

	if (bitset(AMX_INDEX, (r = Relation)->rel_status))
	{
		amxerror(102, r->rel_id);
		return;
	}

	if ((uniq = r->rel_uniq) > 2)
	{
		fprintf(FILE_tmp, msg(208));
		fprintf(FILE_tmp, msg(45), uniq, uniq);
		while (r = getrel(r->rel_index, 0))
		{
			uniq = r->rel_uniq;
			fprintf(FILE_tmp, msg(156), uniq, uniq, uniq);
		}
		fprintf(FILE_tmp, msg(209));
	}
}


w_close()
{
	register AMX_REL	*r;
	extern AMX_REL		*getrel();

	if (bitset(AMX_INDEX, (r = Relation)->rel_status))
	{
		amxerror(104, r->rel_id);
		return;
	}

	if (r->rel_uniq > 2)
	{
		fprintf(FILE_tmp, msg(208));
		fprintf(FILE_tmp, msg(46), r->rel_uniq, r->rel_uniq);
		while (r = getrel(r->rel_index, 0))
			fprintf(FILE_tmp, msg(200), r->rel_uniq);
		fprintf(FILE_tmp, msg(209));
	}
}
