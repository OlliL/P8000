# include	"AA_liba.h"

# ifndef MSDOS
exit(code)
register int	code;
{
	extern int		(*AAEXIT)();

	AAexit();
	if (AAEXIT)
		(*AAEXIT)(code);
	_cleanup();
	_exit(code);
}
# endif


AAexit()
{
	DESCXX			*d;

# ifdef AMX_TRACE
	AAtTfp(19, 0, "----- exit -----\n");
# endif

	errno = 0;

	if (!AA_DESC)
		return;

	/* close the world */
	while (d = AA_DESC->xx_next)
		AAcloser(&d, d->xx_desc.reldum.relid);

	/* close catalog AA_REL */
	AArelclose(AA_DESC);

	/* logout database */
	AA_DESC = (DESCXX *) 0;
	AAunlall();
# ifdef MSDOS
	AAunldb();
# endif

	/* close catalogs AA_REL, AA_ATT and concurrency device */
	AAam_exit();
}
