# include	"form.h"

MMnext()
{
	register int		character;
	register int		ret_val;

	MMerror(" RECHERCHE FORTSETZEN ? ");

	if (ret_val = ((character = MM_getchar()) != C_CONTINUE))
		MM_backup(character);
	else
		MMerror(" RECHERCHE LAEUFT ");

	return (ret_val);
}
