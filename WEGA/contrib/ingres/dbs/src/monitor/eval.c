# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"monitor.h"

/*
**  DO MACRO EVALUATION OF QUERY BUFFER
**
**	The logical query buffer is read and passed through the macro
**	processor.  The main purpose of this is to evaluate {define}'s.
**	If the 'pr' flag is set, the result is printed on the terminal,
**	and so becomes a post-evaluation version of print.
*/
eval(pr)
register int	pr;
{
	register FILE	*tfile;
	register int	c;
# ifdef SETBUF
	char		tbuf[BUFSIZ];
# endif
	char		tfilename[QRYNAME];
	extern int	fgetc();

	Autoclear = 0;
	clrline(TRUE);

	/* open temp file and rewind query buffer */
	if (!pr)
	{
		Notnull = 0;
		AAconcat(QRYTBUF, Fileset, tfilename);
		if (!(tfile = fopen(tfilename, "w")))
			AAsyserr(16004, tfilename);
# ifdef SETBUF
		setbuf(tfile, tbuf);
# endif
	}
	if (!freopen(Qbname, "r", Qryiop))
		AAsyserr(16005);

	/* COPY FILE */
	macinit(fgetc, Qryiop, FALSE);
	while ((c = macgetch()) > 0)
	{
		if (pr)
			putchar(c);
		else
		{
			if (c != ' ' && c != '\n' && c != '\t')
				Notnull++;
			if (putc(c, tfile) == EOF)
				AAsyserr(16006);
		}
	}

	if (!pr)
	{
		/* link temp file back to query buffer */
		fclose(tfile);
		unlink(Qbname);
		if (link(tfilename, Qbname))
			AAsyserr(16007);
		unlink(tfilename);
	}

	/* reopen query buffer (now evaluated) */
	if (!(Qryiop = freopen(Qbname, "a", Qryiop)))
		AAsyserr(16008);

	cgprompt();
}
