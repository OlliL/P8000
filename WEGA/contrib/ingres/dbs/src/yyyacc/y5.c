# include	<stdio.h>
/* fake portable I/O routines, for those
    sites so backward as to not have the
     port. library */

FILE	*cin, *cout;
FILE	*Fin, *Fout;

FILE	*copen(s, c)
register char	*s;
register char	*c;
{
	register FILE	*f;

	if ((f = fopen(s, c)) == (FILE *) 0)
		error("cannot open %s", s);
	else if (*c == 'r')
		Fin = f;
	return (f);
}

cflush(x)
register FILE	*x;
{
	/* fake! sets file to x */
	fflush(Fout);
	Fout = x;
}

cexit(i)
register int	i;
{
	fflush(Fout);
	exit(i);
}
