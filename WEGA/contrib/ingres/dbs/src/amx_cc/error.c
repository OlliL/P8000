# include	"amx.h"

extern int	Xerrors;

char	*msg(no)
register int	no;
{
# define	UNIT		32
	static char		buf[(2 * UNIT)];
	register int		i;
	long			offset;
	extern int		errno;
	extern long		lseek();

	errno = 0;
	offset = UNIT * no;
	if (lseek(FILE_msg, offset, 0) != offset)
	{
		printf("SEEK ERROR <%d> to %ld\n", errno, offset);
		abort_amx();
	}
	if ((i = read(FILE_msg, buf, (2 * UNIT))) != (2 * UNIT))
	{
		printf("READ ERROR msg %d <%d> got %d\n", no, errno, i);
		abort_amx();
	}
	return (buf);
}


# ifdef P8000
amxerror(no, a0, a1, a2, a3)
register int	no;
register int	a0;
register int	a1;
register int	a2;
register int	a3;
{
# else
amxerror(no, av)
register int	no;
int		av;
{
	register int		*a;

	a = &av;
# endif

	Xerrors++;
# ifdef AMX_SC
	if (FILE_exp && Line)
		printf("IMPORT FILE:\t");
# endif
	if (Line)
		printf("%4d: ", Line);
	printf(msg(no),
# ifdef P8000
		a0, a1, a2, a3);
# else
		a[0], a[1], a[2], a[3]);
# endif
	putchar('\n');
	fflush(stdout);
	return (0);
}


# ifdef P8000
yyexit(a0, a1, a2, a3)
register int	a0;
register int	a1;
register int	a2;
register int	a3;
{
	amxerror(a0, a1, a2, a3);
# else
yyexit(av)
int	av;
{
	register int		*a;

	a = &av;
	amxerror(a[0], a[1], a[2], a[3]);
# endif
	abort_amx();
}


abort_amx()
{
	end_file();
	fflush(stdout);
	end_amx(1);
}


rubproc()
{
	yyexit(4);
}
