# include	"amx.h"

static struct amx_key	AP_key[MAXDOM + 1];

/*
** STRATEGY
**
**	Attempts to limit access scan to less than the entire Source
**	relation by finding a key which can be used for associative
**	access to the Source reln or an index thereon.	The key is
**	constructed from domain-value specifications found in the
**	clauses of the qualification list.
*/
strategy(over)
AMX_REL		*over;
{
	register AMX_REL 	*r;
	register int		mode;
	register int		i;
	extern AMX_REL		*getrel();

	/* check user suggested access path */
	if (over)
	{
		if ((r = Relation) == over)
			goto ap_primary;

		while (r = getrel(r->rel_index, 0))
		{
			if (r == over)
			{
				AP_index = r->rel_uniq;
				i_param(r);
				if ((AP_mode = getkeys(r)) == AP_NOKEY)
					goto ap_primary;
				goto ap_done;
			}
		}
		amxerror(89, Relation->rel_id, over->rel_id);
	}

	AP_mode = AP_NOKEY;	/* assume a find mode with no key */

	if (!Domains)
	{	/* if no qualification then you must scan entire rel */
		AP_key[0].key_cond = 0; /* primary relation */
		AP_key[1].key_dom = 0;	/* no domains	    */
		goto ap_done;
	}

	/* copy structure of primary relation into AP_key */
	p_param(r = Relation);

	/* Try to find exact key on primary */
	if ((AP_mode = getkeys(r)) == AP_FULLKEY)
		goto ap_done;

	/* if primary has no sec index then give up */
	if (r->rel_index == MAX_RELS)
		goto ap_done;

	/* check indexed access paths */
	mode = AP_NOKEY;	/* assume found no useable index */
	while (r = getrel(r->rel_index, 0))
	{	/* copy structure of index relation into AP_key */
		i_param(r);
		if ((i = getkeys(r)) == AP_LOWKEY)
			i = AP_HIGHKEY;
		if (i > mode)
		{
			/* found better index */
			AP_index = r->rel_uniq;

			/* If an fullkey on a sec index was found, */
			/* look no more.			   */
			if ((mode = i) == AP_FULLKEY)
			{
				AP_mode = mode;
				goto ap_done;
			}
		}
	}

	/* Look for a range key on primary */
	/* or no indexed access path	   */
	if (AP_mode != AP_NOKEY || mode == AP_NOKEY)
	{
ap_primary:
		/* access via primary relation */
		AP_index = MAX_RELS;
		p_param(r = Relation);
	}
	else	/* access via index relation */
	{
		r = getrel(AP_index, 0);
		i_param(r);
	}
	AP_mode = getkeys(r);

ap_done:
	/*
	** At this point the strategy is determined.
	**
	** If AP_mode is AP_NOKEY, then a full scan will be performed
	*/

	p_len();
	if (AP_index != MAX_RELS)
	{
		r = getrel(AP_index, 0);
		i_len(r);
	}
}


static getkeys(r)
register AMX_REL 	*r;
{
	register AMX_KEY	*k;
	register int		key;
	register int		cond;

	/* if relation is unkeyed then give up */
	if (abs(ctoi(r->rel_spec)) == M_HEAP)
		return (AP_NOKEY);

	k = &AP_key[1];
	cond = ctou(k->key_cond);

	key = AP_FULLKEY;
	for ( ; k->key_dom; k++)
	{
		if (!bitset(AP_EQ, k->key_cond))
		{
			key = AP_NOKEY;
			break;
		}
	}
	if (key)
		return (AP_FULLKEY);

	/* if relation is hashed then give up */
	if (abs(ctoi(r->rel_spec)) == M_HASH)
		return (AP_NOKEY);

	if (bitset(AP_EQ, cond))
		return (AP_RANGEKEY | AP_LOWKEY | AP_HIGHKEY);

	if (bitset(AP_GT, cond))
		key = AP_LOWKEY;

	if (bitset(AP_LT, cond))
			return(key? (AP_RANGEKEY | AP_LOWKEY | AP_HIGHKEY): AP_HIGHKEY);

	return (key);
}


static p_param(r)
register AMX_REL 	*r;
{
	register AMX_KEY	*k;
	register int		dom;

	for (dom = 0, k = AP_key; dom <= MAXDOM; dom++, k++)
		k->key_dom = k->key_cond = 0;

	for (dom = 1; dom <= ctou(r->rel_atts); dom++)
	{
		k = &AP_key[r->rel_dom[dom - 1].att_key];
		k->key_dom = dom;
		k->key_cond = Dom_set[dom];
	}
}


static p_len()
{
	register AMX_REL 	*r;
	register AMX_ATT 	*a;
	register int		dom;
	register int		qual;
	register int		len;

	AP_low = AP_high = 0;
	AP_ilow = AP_ihigh = 0;
	r = Relation;

	for (dom = 1; dom < MAXDOM; dom++)
	{
		if (!(qual = Dom_set[dom]))
			continue;

		a = &r->rel_dom[dom - 1];
		len = a->att_off + ctou(a->att_len);
		if (len > AP_low)	/* update length of first key tuple */
			AP_low = len;
		if ((bitset(AP_LT, qual) && bitset(AP_GT, qual)) ||
		    (bitset(AP_LT, qual) && bitset(AP_NE, qual)) ||
		    (bitset(AP_GT, qual) && bitset(AP_NE, qual)))
			if (len > AP_high)	/* update length of second key tuple */
				AP_high = len;
	}
}


static i_param(r)
register AMX_REL 	*r;
{
	register AMX_KEY	*k;
	register int		dom;
	register char		*p;

	k = AP_key;
	k->key_dom = 0;
	k++->key_cond = 1;	/* secondary index */

	for (p = r->ind_dom; dom = ctou(*p); p++)
	{
		k->key_dom = dom;
		k++->key_cond = Dom_set[dom];
	}
	k->key_dom = 0;
}


static i_len(r)
register AMX_REL 	*r;
{
	register AMX_ATT 	*a;
	register char		*dom;
	register int		len;
	register int		qual;
	register int		qual2;
	register int		i;

	AP_ilow = AP_ihigh = 0;
	fprintf(FILE_scn, msg(180), r->rel_uniq, r->rel_uniq);

	for (dom = r->ind_dom; *dom; dom++)
	{
		i = ctou(*dom);
		if (!(qual = Dom_set[i]))
			continue;

		qual2 = 0;
		a = &r->rel_dom[dom - r->ind_dom];
		len = a->att_off + ctou(a->att_len);
		if (len > AP_ilow)	/* update length of first key tuple */
			AP_ilow = len;
		if ((bitset(AP_LT, qual) && bitset(AP_GT, qual)) ||
		    (bitset(AP_LT, qual) && bitset(AP_NE, qual)) ||
		    (bitset(AP_GT, qual) && bitset(AP_NE, qual)))
		{
			if (len > AP_ihigh)	/* update length of second key tuple */
				AP_ihigh = len;

			if (bitset(AP_NE, qual))
			{
				clrbit(AP_NE, qual);
				qual2 = AP_NE;
			}
			else if (bitset(AP_GT, qual))
			{
				clrbit(AP_GT, qual);
				qual2 = AP_GT;
				if (bitset(AP_GE, qual))
				{
					clrbit(AP_GE, qual);
					qual2 |= AP_GE;
				}
			}
		}

		/* call AAsetind(desc, key_tup, key_val, dom, qual) */
		i = ctou(*dom);
		a = &Relation->rel_dom[i - 1];
		len = a->att_off;
		fprintf(FILE_scn, msg(55), r->rel_uniq, len, dom - r->ind_dom + 1, qual);
		if (qual2)
			fprintf(FILE_scn, msg(64), r->rel_uniq, len, dom - r->ind_dom + 1, qual2);
	}
}
