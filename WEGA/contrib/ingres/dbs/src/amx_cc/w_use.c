# include	"amx.h"

static int	Use_relation = MAX_RELS;
static int	Use_mode;

w_use(mode)
register int	mode;
{
	register AMX_REL	*r;
	register int		m;
	extern AMX_REL		*getrel();

# ifdef AMX_SC
	if (ExpImp > 0)
		return;
# endif

	m = Use_mode;
	Use_mode = mode;
	r = getrel(Use_relation, 0);
	if (mode == M_TE)
		Use_relation = MAX_RELS;
	else
	{
		Use_relation = Relation->rel_uniq;
		mode = 0;
	}

	if (!r)
	{
		if (!No_locks)
			fprintf(FILE_tmp, msg(208));
		return;
	}

	if (No_locks)
		return;

	mode |= M_TA;
	if (m < 0)
		m -= mode;
	else
		m |= mode;

	fprintf(FILE_tmp, msg(68), r->rel_uniq, r->rel_uniq, m | M_TA);
	if (mode & M_TE)
		fprintf(FILE_tmp, msg(209));
}


w_commit()
{
	if (No_locks)
		return;

	fprintf(FILE_tmp, msg(69));
}


# ifdef PAGE_LOCKING
w_abort()
{
	if (No_locks)
		return;

	fprintf(FILE_tmp, msg(118));
}
# endif
