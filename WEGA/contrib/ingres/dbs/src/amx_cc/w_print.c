# include	"amx.h"

w_print(dom)
register int	dom;
{
	register AMX_REL	*r;
	register AMX_ATT	*a;
	register int		all;
	register int		len;
	register int		mv;
	extern char		*conv();

	if (!(r = Scan_rel))
		return (amxerror(211));
	if (!(all = dom == 0))
		--dom;

	do
	{
		a = &r->rel_dom[dom++];
		len = ctou(a->att_len);

		if (mv = (a->att_type != opSTRING && bitset(ODD, a->att_off)))
		{
			/* move dom in a temp place */
			fprintf(FILE_tmp, msg(47), a->att_off, len);
		}
		fprintf(FILE_tmp, "printf(\"%12s:\\t%%", a->att_name);
		switch (a->att_type)
		{
		  case opSTRING:
			fprintf(FILE_tmp, ".%ds", len);
			break;

		  case opLONG:
			putc('l', FILE_tmp);
			/* fall through */

		  case opCHAR:
		  case opINT:
			putc('d', FILE_tmp);
			break;

# ifndef NO_F4
		  case opFLT:
# endif
		  case opDOUBLE:
			fprintf(FILE_tmp, "10.3f");
		}
		fprintf(FILE_tmp, "\\n\",%s", conv(a->att_type, 1));
		if (mv)
			fprintf(FILE_tmp, "&AA_var");
		else
			fprintf(FILE_tmp, "&AA_STUP[%d]", a->att_off);
		fprintf(FILE_tmp, "));\n");
	} while (all && dom < ctou(r->rel_atts));
}
