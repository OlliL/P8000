/*
** shared subroutines for relation backup
**	 extractr and insertr
*/
# include	"dmp.h"

# ifdef P8000
ret_buf		AAjmp_buf;
# else
jmp_buf		AAjmp_buf;
# endif

/* global declarations */
struct d_hdr	Hdr;
struct d_rel	Rel;
union d_tup	Tup;
int		Fh = -1;
int		Fp = -1;
int		No_qm = D_CATS;
int		D_flag;
static int	Copen;
extern DESC	Reldes;
extern DESC	Attdes;
extern DESC	Inddes;
extern DESC	Intdes;
extern DESC	Prodes;
extern DESC	Treedes;
DESC		*Cdesc[D_CATS] =
		{
			&Reldes,
			&Attdes,
			&Inddes,
			&Intdes,
			&Prodes,
			&Treedes
		};
char		*Catalog[D_CATS] =
		{
			AA_REL,
			AA_ATT,
			AA_INDEX,
			AA_INTEG,
			AA_PROTECT,
			AA_TREE
		};
short		Clen[D_CATS] =
		{
			ALIGNMENT(sizeof (struct relation)),
			ALIGNMENT(sizeof (struct attribute)),
			ALIGNMENT(sizeof (struct index)),
			ALIGNMENT(sizeof (struct integrity)),
			ALIGNMENT(sizeof (struct protect)),
			ALIGNMENT(sizeof (struct tree))
		};
extern int	Standalone;

/*
** OP_CAT	open all catalogs
*/
op_cat()
{
	register int		i;

	if (Copen++)
		return (0);

	for (i = D_REL; i < No_qm; i++)
		opencatalog(Catalog[i], 2);

	if (AAsetcsl(NO_REL) < 0)
		return (5000);

	Standalone = 0;
	return (0);
}

/*
** CL_CAT	close all catalogs
*/
cl_cat(rm)
register int	rm;
{
	register int		i;

	if (rm)
		unlink(Parmvect[2]);

	if (!Copen)
		return;

	closecatalog(TRUE);

	AAunlcs(NO_REL);

	/* close AA_USERS */
	getuser((char *) 0);
	Copen = 0;
}

/*
** RW_HDR	read/write dump header
*/
rw_hdr(rwfn)
int		(*rwfn)();
{
	register int		f;
	extern long		lseek();

	if (lseek(f = Fh, 0L, 0))
		return (5905);
	if ((*rwfn)(f, &Hdr, sizeof (struct d_hdr)) != sizeof (struct d_hdr))
		return (5906);
	return (0);
}

/*
** RW_REL	read/write relation header
*/
rw_rel(rel, rwfn)
register int	rel;
int		(*rwfn)();
{
	register int		f;
	register long		off;
	extern long		lseek();

	off = sizeof (struct d_hdr) + (rel * sizeof (struct d_rel));
	if (lseek(f = Fh, off, 0) != off)
		return (5907);
	if ((*rwfn)(f, &Rel, sizeof (struct d_rel)) != sizeof (struct d_rel))
		return (5908);
	return (0);
}

char	*AArelpath(rel)
register char	*rel;
{
	extern char		*AAztack();

	return (AAztack(AAztack(AAdbpath, "/"), rel));
}
