# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"monitor.h"

/*
**  PRINT QUERY BUFFER
**
**	The logical query buffer is printed on the terminal regardless
**	of the "Nodayfile" mode.  Autoclear is reset, so the query
**	may be rerun.
*/
print(pr)
register int	pr;
{
	register int	c;
	register FILE	*iop;
# ifdef SETBUF
	char		iop_buf[BUFSIZ];
# endif

	if (pr)
	{
		/* BACK UP FILE & UPDATE LAST PAGE */
		Autoclear = 0;
		clrline(TRUE);
		fflush(Qryiop);
	}

	if (!(iop = fopen(Qbname, "r")))
		AAsyserr(16021, Qbname);
# ifdef SETBUF
	setbuf(iop, iop_buf);
# endif

	/* list file on standard output */
	Notnull = 0;
	while ((c = getc(iop)) > 0)
	{
		if (pr)
			putchar(c);
		if (c != ' ' && c != '\n' && c != '\t')
			Notnull++;
	}

	fclose(iop);
	cgprompt();
}
