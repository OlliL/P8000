/*
** ---  RG_ERROR.C ------ ERROR MESSAGE PRINTER  ------------------------------
**
**	Version: @(#)rg_error.c		4.0	02/05/89
**
*/

# include	<stdio.h>
# include	"rg.h"

/*ARGSUSED*/
# ifdef P8000
rg_error(msg, a0, a1, a2, a3, a4, a5)
register char	*msg;
register int	a0;
register int	a1;
register int	a2;
register int	a3;
register int	a4;
register int	a5;
{
	extern int	errno;
# else
rg_error(msg, av)
register char	*msg;
int		av;
{
	register int	*a;
	extern int	errno;

	a = &av;
# endif

	rg_flush();
	fprintf(stderr, "\n******* PQR ---\tPAGE %3d\tLINE %3d\tTUPLE %3d\n\t",
		SYS_page, SYS_line, SYS_tuple);

# ifdef P8000
	fprintf(stderr, msg, a0, a1, a2, a3, a4, a5);
# else
	fprintf(stderr, msg, a[0], a[1], a[2], a[3], a[4], a[5]);
# endif

	fprintf(stderr, "\007\n");

	if (errno)
	{
		fprintf(stderr, "\tBS	error %d\n", errno);
		errno =	0;
	}
}
