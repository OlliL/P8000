# include	<stdio.h>
# include	"gen.h"

/*
**  AAsyserr -- SYStem ERRor message print and abort
**
**	AAsyserr acts like a printf with up to five arguments.
**
**	If the first argument to AAsyserr is not zero,
**	the error messages are prepended.
**
**	If the extern variable 'AAproc_name' is assigned to a
**	string, that string is prepended to the message.
**
**	All arguments must be null-terminated.
**
**	The function pointed to by 'AAexitfn' is then called.
**	It is initialized to be 'exit'.
*/

char	*AAproc_name;
int	AAccerror;
extern	exit();
int	(*AAexitfn)() =	exit;

# ifdef P8000
AAsyserr(err, p0, p1, p2, p3, p4, p5)
# else
AAsyserr(err, p0)
# endif
register int	err;
char		*p0;
# ifdef P8000
char		*p1;
char		*p2;
char		*p3;
char		*p4;
char		*p5;
# endif
{
	register int	exitvalue;
	char		buf[256];
	extern int	errno;
	extern char	*AAgetformat();
# ifndef P8000
	register char	**p;

	p = &p0;
# endif
	exitvalue = -1;

	putchar('\n');
	if (err)
	{
		if (AAproc_name)
			printf("%s\n", AAproc_name);
		printf("\007DB\terror\t%d\n", err);
		if (errno)
			printf("BS\terror\t%d\n", exitvalue = errno);
		if (AAccerror)
			printf("ACCESS\terror\t%d\n", AAccerror);
# ifdef P8000
		printf(AAgetformat(err, buf), p0, p1, p2, p3, p4, p5);
# else
		printf(AAgetformat(err, buf), p[0], p[1], p[2], p[3], p[4], p[5]);
# endif
	}
	else
# ifdef P8000
		printf(p0, p1, p2, p3, p4, p5);
# else
		printf(p[0], p[1], p[2], p[3], p[4], p[5]);
# endif
	printf("\007\n");
	AAunlall();
	fflush(stdout);
	sync();
	(*AAexitfn)(exitvalue);
}

char	*AAgetformat(err, buf)
int	err;
char	*buf;
{
	register int		c;
	register char		*p;
	register FILE		*iop;
	short			i;
# ifdef SETBUF
	char			iobufx[BUFSIZ];
# endif
	extern char		*AAerrfn();

	/* open the appropriate error file */
	if (!(iop = fopen(AAerrfn(err / 1000), "r")))
		return ("");
# ifdef SETBUF
	setbuf(iop, iobufx);
# endif

	/* read in the code and check for correct */
	for ( ; ; )
	{
		p = buf;
		while ((c = getc(iop)) != '\t')
		{
			if (c <= 0)
				/* no code exists */
				goto fmt_err;
			*p++ = c;
		}
		*p = 0;
		if (AA_atoi(buf, &i))
			goto fmt_err;
		if (i != err)
		{
			while ((c = getc(iop)) != ERRDELIM)
				if (c <= 0)
					goto fmt_err;
			getc(iop);	/* throw out the newline */
			continue;
		}

		/* got the correct line */
		p = buf;
		while ((c = getc(iop)) != ERRDELIM)
		{
			if (c <= 0)
			{
				fmt_err:
					fclose(iop);
					return ("");
			}
			*p++ = c;
		}
		*p = 0;
		fclose(iop);
		return (buf);
	}
}
