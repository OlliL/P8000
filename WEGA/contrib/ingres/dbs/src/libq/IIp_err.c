# include	<stdio.h>
# include	"gen.h"
# include	"../h/version.h"

char	IIversion[] = VERSION;

# define	AAbmove		IIbmove
# define	AAztack		IIconcatv
# define	AA_iocv		IIitos
# define	AApath		IIPathname
# define	AAerrfn		IIerrfilen
# define	AA_ERRFN	II_ERRFN
# define	AAversion	IIversion

# include	"../gutil/errfilen.c"
# include	"../h/pipes.h"
# include	"IIglobals.h"

# ifdef EBCDIC
# define	LAST_CHAR	0377
# else
# define	LAST_CHAR	0177
# endif

/*
**  IIP_ERR
**
**	This routine processes an IIerror call for printout.This involves doing
**	a lookup in the .../sys/etc/error? files, where ? is the thous-
**	ands digit of the error number.  The associated error message
**	then goes through parameter substitution and is printed.
**
**	In the current version, the error message is just printed.
*/
char	*IIp_err(err_num, argc, argv)
int		err_num;
int		argc;
char		**argv;
{
	register char		*p;
	register int		c;
	register FILE		*iop;
	short			i;
	char			buf[100];
	extern char		*IIerrfilen();

	/* open the appropriate error file */
	p = IIerrfilen(err_num / 1000);

#	ifdef xETR2
	if (IIdebug > 1)
		printf("P_ERR: file \"%s\"\n", p);
#	endif

	if (!(iop = fopen(p, "r")))
		return ((char *) 0);

	/* read in the code and check for correct */
	for ( ; ; )
	{
		p = buf;
		while ((c = getc(iop)) != '\t')
		{
			if (c <= 0)
			{
				fclose(iop);
				return ((char *) 0);
			}
			*p++ = c;
		}
		*p = 0;
		if (IIatoi(buf, &i))
			goto error;
		if (i != err_num)
		{
			while ((c = getc(iop)) != ERRDELIM)
				if (c <= 0)
				{
error:
					IIsyserr("P_ERR: Formatfehler im file %d", err_num);
				}
			getc(iop);	/* throw out the newline */
			continue;
		}

		/* got the correct line, print it doing parameter substitution */
		printf("%d: ", err_num);
		for ( ; ; )
		{
			c = getc(iop);
			if (c <= 0 || c == ERRDELIM)
			{
				putchar('\n');
				fclose(iop);
				return ((char *) 1);
			}
			if (c == '%')
			{
				c = getc(iop);
				for (p = argv[c - '0']; c = *p; p++)
				{
					if (c < ' ' || c >= LAST_CHAR)
						printf("\\0%o", c);
					else
						putchar(c);
				}
				continue;
			}
			putchar(c);
		}
	}
}
