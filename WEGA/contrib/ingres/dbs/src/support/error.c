# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"

/*
**  PROCESS ERROR MESSAGE (Standalone override)
**
**	This routine replaces the "error" routine for use in
**	standalone routines such as creatdb and printr.  Its usage
**	is identical to that of normal "error".
**
**	This routine is assumed to be called by process five; hence,
**	all error messages it produces have a 5000 offset.
**
*/

# ifdef P8000
error(err, msg1, msg2, msg3, msg4, msg5, msg6)
# else
error(err, msg1)
# endif
int	err;
char	*msg1;
# ifdef P8000
char	*msg2;
char	*msg3;
char	*msg4;
char	*msg5;
char	*msg6;
# endif
{
	register int	c;
	register char	*p;
	register FILE	*iop;
	register char	**pv;
	short		i;
# ifdef SETBUF
	char		iob[BUFSIZ];
# endif
	char		buf[10];
	extern char	*AAerrfn();
# ifdef P8000
	char		*xx[6];

	pv = xx;
	*pv++ = msg1;
	*pv++ = msg2;
	*pv++ = msg3;
	*pv++ = msg4;
	*pv++ = msg5;
	*pv = msg6;
	pv = xx;
# else
	pv = &msg1;
# endif
	if ((iop = fopen(AAerrfn(5), "r")) == (FILE *) 0)
		AAsyserr(21052);
# ifdef SETBUF
	setbuf(iop, iob);
# endif

	/* read in the code and check for correct */
	for ( ; ; )
	{
		p = buf;
		while ((c = getc(iop)) != '\t')
		{
			if (c <= 0)
			{
				/* no code exists, print the first parm */
				printf("%d: %s\n\n", err, pv[0]);
				fclose(iop);
				return (err);
			}
			*p++ = c;
		}
		*p = 0;
		if (AA_atoi(buf, &i))
			AAsyserr(21053, err, buf);
		if (i != err)
		{
			while ((c = getc(iop)) != ERRDELIM)
				if (c <= 0)
					AAsyserr(21054, err);
			getc(iop);	/* throw out the newline */
			continue;
		}

		/* got the correct line, print it doing parameter substitution */
		printf("%d: ", err);
		c = '\n';
		for ( ; ; )
		{
			c = getc(iop);
			if (c == EOF || c == ERRDELIM)
			{
				putchar('\n');
				fclose(iop);
				return (err);
			}
			if (c == '%')
			{
				c = getc(iop);
				for (p = pv[c - '0']; c = *p; p++)
				{
					putchar(c);
				}
				continue;
			}
			putchar(c);
		}
	}
}
