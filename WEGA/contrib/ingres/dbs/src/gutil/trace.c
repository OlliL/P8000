# include	<stdio.h>
# include	"gen.h"

# define	tTNFUNC		100
# define	tTNCUTOFF	70

int		AAtTany;
short		AAtT[tTNFUNC];
static char	*AAtTp;
static char	AAtTsav;
static char	AAtTbuffer[100];

AAtTrace(argc, argv, flag)
int		*argc;
register char	**argv;
char		flag;
{
	register int	fno;
	register char	**ps;
	register int	f;
	register int	cnt;

	ps = argv;
	for (cnt = *argc; cnt > 0; cnt--)
	{
		if ((*ps)[0] != '-' || (*ps)[1] != flag)
		{
			*argv++ = *ps++;
			continue;
		}
		AAtTany++;
		AAtTp = AAtTbuffer;
		AAsmove((*ps) + 2, AAtTbuffer);
		(*argc)--;
		ps++;
		if (!*AAtTp)
		{
			for (fno = 0; fno < tTNCUTOFF; fno++)
				AAtTon(fno, -1);
			continue;
		}
		do
		{
			fno = AAtTnext();
			AAtTurn(fno);

			if (AAtTsav == '/')
			{
				f = fno + 1;
				fno = AAtTnext();
				while (f < fno)
					AAtTon(f++, -1);
				AAtTurn(fno);
			}
		} while (AAtTsav);
	}
	*argv = 0;
}

AAtTnext()
{
	register char	*c;
	short		i;

	c = AAtTp;
	while (*AAtTp >= '0' && *AAtTp <= '9')
		AAtTp++;
	AAtTsav = *AAtTp;
	*AAtTp++ = '\0';
	AA_atoi(c, &i);
	return (i);
}

AAtTurn(fno)
register int	fno;
{
	register int	pt;

	if (AAtTsav == '.')
	{
		while (AAtTsav == '.')
		{
			pt = AAtTnext();
			AAtTon(fno, pt);
		}
	}
	else
		AAtTon(fno, -1);
}


AAtTon(fun, pt)
register int	fun;
register int	pt;
{
	if (pt >= 0)
		AAtT[fun] |= (1 << pt % 16);
	else
		AAtT[fun] = 0177777;
}


/*
**  CHECK TRACE FLAG AND PRINT INFORMATION
**
**	This routine is equivalent to
**		if (AAtTf(m, n))
**			printf(a1, a2, a3, a4, a5, a6);
**
**	and can be called to reduce process space.  The return value
**	is the value of the flag.
*/
# ifdef P8000
AAtTfp(m, n, a0, a1, a2, a3, a4, a5, a6)
# else
AAtTfp(m, n, a0)
# endif
register int	m;
register int	n;
char		*a0;
# ifdef P8000
char		*a1;
char		*a2;
char		*a3;
char		*a4;
char		*a5;
char		*a6;
# endif
{
	register int	rtval;
# ifndef P8000
	register char	**a;

	a = &a0;
# endif
	if (rtval = AAtTf(m, n))
# ifdef P8000
		printf(a0, a1, a2, a3, a4, a5, a6);
# else
		printf(a[0], a[1], a[2], a[3], a[4], a[5], a[6]);
# endif
	fflush(stdout);
	return (rtval);
}
