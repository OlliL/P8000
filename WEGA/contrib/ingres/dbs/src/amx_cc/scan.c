# include	"amx.h"

bgn_scan(retrieve)
int		retrieve;
{
	register AMX_REL	*r;
	register int		i;

	Qry_mode = AMX_RETRIEVE;
	r = Relation;

# ifdef AMX_QRYMOD
	/* check RETRIEVE permission */
	qry_perm(r, "retrieve");
# endif

	/* initialize access path informations */
	AP_index = MAX_RELS;
	AP_low = AP_high = 0;
	AP_ilow = AP_ihigh = 0;

	/* clear out domain table */
	Domains = 0;	/* count of simple clausels */
	for (i = MAXDOM - 1; i >= 0; --i)
		Dom_set[i] = 0;

	fprintf(FILE_tmp, msg(172), ++AP_level);
	fprintf(FILE_tmp, msg(174));
	fprintf(FILE_tmp, msg(175));
# if (S_ALIGN + L_ALIGN + D_ALIGN) > 0
	fprintf(FILE_tmp, msg(120));
# endif
	fprintf(FILE_tmp, msg(152), ALIGNMENT(r->rel_wid));

	if (!retrieve)
	{
		if (!No_locks)
			fprintf(FILE_tmp, msg(57), r->rel_uniq, r->rel_uniq);
		return;
	}

	fprintf(FILE_tmp, msg(51));
	fprintf(FILE_tmp, msg(187), ALIGNMENT(ctou(Relation->rel_atts)));

	/* create scan temp */
	if (!(FILE_scn = fopen(Filename, "w")))
		yyexit(19);
# ifdef SETBUF
	setbuf(FILE_scn, BUF_scn);
# endif
	if (!No_locks)
		fprintf(FILE_scn, msg(57), r->rel_uniq, r->rel_uniq);
	fprintf(FILE_scn, msg(180), r->rel_uniq, r->rel_uniq);
}


char	*scan(over, retrieve)
AMX_REL		*over;
int		retrieve;
{
	register AMX_REL	*scn;
	register int		c;
	register int		ap;
	register char		*rel_stack;
	int			low;
	int			high;
	DESC			d;
	extern AMX_REL		*getrel();

	/* setup ACCESS PATH */
	if (retrieve)
		strategy(over);

	if (AP_index != MAX_RELS)
	{
		/* indexed access path */
		scn = getrel(AP_index, 0);
		ap = 'i';
		low = AP_ilow;
		high = AP_ihigh;
		fprintf(FILE_tmp, msg(149), ALIGNMENT(ctou(scn->rel_atts)));
		fprintf(FILE_tmp, msg(181));
	}
	else
	{
		/* primary access path */
		scn = Relation;
		ap = 's';
		low = AP_low;
		high = AP_high;
	}

	if (AP_low)
		fprintf(FILE_tmp, msg(130), ALIGNMENT(AP_low));
	if (AP_high)
		fprintf(FILE_tmp, msg(121), ALIGNMENT(AP_high));
	if (AP_ilow)
		fprintf(FILE_tmp, msg(119), ALIGNMENT(AP_ilow));
	if (AP_ihigh)
		fprintf(FILE_tmp, msg(117), ALIGNMENT(AP_ihigh));

	if (retrieve)
	{
		/* write qualification out */
		fclose(FILE_scn);
		if (!(FILE_scn = fopen(Filename, "r")))
			yyexit(17);
# ifdef SETBUF
		setbuf(FILE_scn, BUF_scn);
# endif
		while ((c = getc(FILE_scn)) != EOF)
			putc(c, FILE_tmp);
		fclose(FILE_scn);
		FILE_scn = (FILE *) 0;
	}

	/* setup new SCAN */
	fprintf(FILE_tmp, msg(115));
	fprintf(FILE_tmp, msg(113));
	if (retrieve)
		fprintf(FILE_tmp, msg(147), Relation->rel_uniq,
			d.relgiven - ((char *) &d) + 1,
			ctou(Relation->rel_atts));
	if (AP_index != MAX_RELS)
		fprintf(FILE_tmp, msg(145), scn->rel_uniq,
			d.relgiven - ((char *) &d) + 1, ctou(scn->rel_atts));

	rel_stack = (char *) (Scan_rel? Scan_rel->rel_uniq: MAX_RELS);
	Scan_rel = getrel(Relation->rel_uniq, 2);

	if (!retrieve)
		goto do_scan;

	/* FIND with NOKEY */
	if (AP_mode == AP_NOKEY)
	{
		fprintf(FILE_tmp, msg(109));
		fprintf(FILE_tmp, msg(107));
		goto do_scan;
	}

	/* FIND with FULLKEY */
	if (AP_mode == AP_FULLKEY)
	{
		if (abs(ctoi(scn->rel_spec)) == M_HASH)
			fprintf(FILE_tmp, msg(143), scn->rel_uniq, ap, ap);
		else
			fprintf(FILE_tmp, msg(141), scn->rel_uniq, ap, ap, ap);
		goto do_scan;
	}

	/* FIND with LOWKEY or RANGEKEY */
	if (bitset(AP_LOWKEY, AP_mode))
	{
		fprintf(FILE_tmp, msg(139), scn->rel_uniq, ap, ap, ap);
		if (high)
			fprintf(FILE_tmp, ",AA_%c2key", ap);
		fprintf(FILE_tmp, ");\n");
	}
	else
		fprintf(FILE_tmp, msg(105), ap);

	/* FIND with HIGHKEY or RANGEKEY */
	if (bitset(AP_HIGHKEY, AP_mode))
		fprintf(FILE_tmp, msg(137), scn->rel_uniq, ap, ap);
	else
		fprintf(FILE_tmp, msg(107));

	/* SCAN */
do_scan:
	AP_label[AP_level] = AP_stmt++;
	if (retrieve)
	{
		fprintf(FILE_tmp, msg(202), 2 * AP_label[AP_level]);
		fprintf(FILE_tmp, msg(133), scn->rel_uniq, ap, ap);
		if (low)
			fprintf(FILE_tmp, ",AA_%c1key", ap);
		if (high)
			fprintf(FILE_tmp, ",AA_%c2key", ap);
	
		if (AP_index != MAX_RELS)
		{
			fprintf(FILE_tmp, msg(131), Scan_rel->rel_uniq);
			if (AP_low)
				fprintf(FILE_tmp, ",AA_s1key");
			if (AP_high)
				fprintf(FILE_tmp, ",AA_s2key");
		}
	}
	else
		fprintf(FILE_tmp, msg(213),
			Destination.c_var, Scan_rel->rel_uniq);
	fprintf(FILE_tmp, msg(203));
	return (rel_stack);
}


end_scan(rel_stack, retrieve)
register char	*rel_stack;
int		retrieve;
{
	register int		i;
	extern AMX_REL		*getrel();

	fprintf(FILE_tmp, msg(98));
	if (!retrieve)
		fprintf(FILE_tmp, msg(202), 2 * AP_label[AP_level]);
	fprintf(FILE_tmp, msg(202), 2 * AP_label[AP_level] + 1);
	fprintf(FILE_tmp, msg(95));
	if (!No_locks)
		fprintf(FILE_tmp, msg(25), Scan_rel->rel_uniq);
	fprintf(FILE_tmp, msg(26), AP_level--);

	/* restore previous scan */
	if ((i = (int) rel_stack) == MAX_RELS)
		Scan_rel = (AMX_REL *) 0;
	else
		Scan_rel = getrel(i, 2);
}
