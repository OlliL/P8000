# include	"amx.h"

qual(dom, cond)
register int	dom;
register int	cond;
{
	register int		q;
	register int		i;
	char			q_buf[7];
	char			c_buf[7];
	extern char		*pr_qual();

	if (bitset(AP_EQ, (q = Dom_set[dom])))
		goto ill_qual;	/* second '==' */

	i = 0;
	if (bitset(AP_LT, q))
		i++;
	if (bitset(AP_GT, q))
		i++;
	if (bitset(AP_NE, q))
		i++;
	if (i > 1)
		goto ill_qual;	/* third condition */

	switch (cond)
	{
	  case AP_EQ:
	  case AP_LT:
	  case AP_LT | AP_LE:
		if (q)
		{		/* second '==' or '<[=]' */
ill_qual:
			amxerror(91, pr_qual(q, q_buf), pr_qual(cond, c_buf));
			return;
		}
		break;

	  case AP_GT:
	  case AP_GT | AP_GE:
		if (bitset(AP_GT, q))
			goto ill_qual;	/* second '>[=]' */
		/* fall trough, because qual must not be '!=' */

	  case AP_NE:
		if (bitset(AP_NE, q))
			goto ill_qual;	/* second '!=' */
	}

	q |= cond;
	Dom_set[dom] = q;

	if (cond != AP_NE)
		Domains++;	/* count of simple clausels */

	w_key(dom, q);		/* call AAsetkey(...); */
}


static int	w_key(dom, q)
int		dom;
register int	q;
{
	register AMX_FRM	*f;
	register int		cond;
	register AMX_ATT	*a;
	register int		dom_type;
	register int		pattern;
	extern char		*conv();
	extern char		*value();
	extern char		*addr();

	f = &Source;
	a = &Relation->rel_dom[dom - 1];
	dom_type = a->att_type;
	cond = ctou(a->att_len);

	/* don't mix CHAR and NUMERIC types */
	if (mixed(dom_type, f->type))
		return;

	pattern = 0;
	if (f->type == opSTRING)
	{
		pattern = f->off < 0;
		if (!pattern && cond > ctou(f->len))
		{
			/* scan dom to dom: frmt == opSTRING */
			/* AAlength(scan dom) < AAlength(dom): */
			/* --> setkey can't work           */
			amxerror(100, a->att_name);
			return;
		}
	}
	else
	{
		if (bitset(ODD, f->off) && dom_type != f->type)
		{
			/* scan dom to dom: frmt == NUMERIC */
			/* --> convert in a temp place      */
			fprintf(FILE_scn, msg(40), addr(f), ctou(f->len));
			fprintf(FILE_scn, msg(41), conv(dom_type, 0), conv(f->type, 1));
			goto mod_dom;
		}
		else if (f->off == C_CONST || dom_type != f->type)
		{
			/* inplace convert */
			fprintf(FILE_scn, msg(42), conv(dom_type, 0), value(f));
mod_dom:
			f->off = C_CONST;	/* force access to AA_var */
		}
	}

	/* extract key condition and key tuple */
	if (bitset(AP_NE, q))
	{
		cond = AP_NE;
		clrbit(AP_NE, q);
	}
	else if (bitset(AP_GT, q))
	{
		cond = AP_GT;
		clrbit(AP_GT, q);
		if (bitset(AP_GE, q))
		{
			cond |= AP_GE;
			clrbit(AP_GE, q);
		}
	}
	else
	{
		cond = q;
		q = 0;
	}

	/* call AAsetkey(desc, key_tup, key_val, dom, cond) */
	/*  or  AAsetpat(desc, key_tup, key_val, dom, cond) */
	fprintf(FILE_scn, msg(43), pattern? "pat": "key",
		Relation->rel_uniq, q? '2': '1', addr(f), dom, cond);
}


static char	*pr_qual(cond, buf)
register int	cond;
register char	*buf;
{
	register char		*p;

	p = buf;
	if (bitset(AP_EQ, cond))
	{
		*p++ = ' ';
		*p++ = '=';
		*p++ = '=';
	}
	if (bitset(AP_LT, cond))
	{
		*p++ = ' ';
		*p++ = '<';
		if (bitset(AP_LE, cond))
			*p++ = '=';
	}
	if (bitset(AP_GT, cond))
	{
		*p++ = ' ';
		*p++ = '>';
		if (bitset(AP_GE, cond))
			*p++ = '=';
	}
	if (bitset(AP_NE, cond))
	{
		*p++ = ' ';
		*p++ = '!';
		*p++ = '=';
	}
	*p = 0;
	return (buf);
}
