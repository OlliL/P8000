# include	"amx.h"

/* LOOKUP domain by name */
att_lookup(att_name, scan_att)
char	*att_name;
int	scan_att;
{
	register AMX_REL	*r;
	register AMX_ATT	*a;
	register int		i;
	register AMX_FRM	*f;
	register char		*name;

	if (scan_att)
	{
		if (!(r = Scan_rel))
		{
			amxerror(75, att_name);
			return (0);
		}
		f = &Source;
	}
	else
	{
		r = Relation;
		f = &Destination;
	}

	name = att_name;
	i = ctou(r->rel_atts);
	for (a = r->rel_dom; i; --i, a++)
		if (AAsequal(a->att_name, name))
			break;
	Dom = ctou(a->att_id);
	if (!i)
	{
		f->type = NOTYPE;
		amxerror(77, r->rel_id, name);
		return (0);
	}
	f->type = a->att_type;
	f->len = a->att_len;
	f->off = a->att_off;

# ifdef AMX_QRYMOD
	dom_perm(r, scan_att? AMX_RETRIEVE: Qry_mode);
# endif

	return (Dom);
}


/* LOOKUP domain by number */
dom_lookup(attid, scan_dom)
int	attid;
int	scan_dom;
{
	register AMX_REL	*r;
	register AMX_ATT	*a;
	register int		i;
	register AMX_FRM	*f;
	register int		id;

	if (scan_dom)
	{
		if (!(r = Scan_rel))
		{
			amxerror(78, attid);
			return (0);
		}
		f = &Source;
	}
	else
	{
		r = Relation;
		f = &Destination;
	}
	id = attid;
	i = ctou(r->rel_atts);
	for (a = r->rel_dom; i; --i, a++)
		if (ctou(a->att_id) == id)
			break;
	Dom = id;
	if (!i)
	{
		f->type = NOTYPE;
		amxerror(80, r->rel_id, id);
		return (0);
	}
	f->type = a->att_type;
	f->len = a->att_len;
	f->off = a->att_off;

# ifdef AMX_QRYMOD
	dom_perm(r, scan_dom? AMX_RETRIEVE: Qry_mode);
# endif

	return (Dom);
}
