# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"monitor.h"

FILE		*Trapfile;
int		Trap;

/*
**  TRAPQUERY -- trap queries which succeeded.
**
**	This module traps the current query into the file
**	specified by Trapfile.  It will open Trapfile if
**	it is not already open.
**
**	Parameters:
**		retcode -- the return code of the query.
**		name -- the name of the file in which to dump the
**			query buffer.
**
**	Bugs:
**		If 'name' (that is, the QUERYTRAP macro) changes
**			during a run, the output file does not change.
*/
trapquery(retcode, name)
struct retcode	*retcode;
char		*name;
{
	register int		c;
	register FILE		*iop;
	register char		*cp;
	long			timevec;
# ifdef SETBUF
	static char		trap_buf[BUFSIZ];
	char			iop_buf[BUFSIZ];
# endif
	char			buf[MAXLINE + 1];
	extern			fgetc();
	extern long		time();
	extern char		*ctime();
	extern char		*AAdatabase;
	extern char		*AAusercode;

	if (Trap < 0)
		return;

	if (!Trapfile)
	{
		if (!(Trapfile = fopen(name, "a")))
		{
			printf("%s open TRAP file '%s'\n", ERROR, name);
			Trap = -1;
			return;
		}
# ifdef SETBUF
		setbuf(Trapfile, trap_buf);
# endif
	}

	if (!Trap++)
	{
		time(&timevec);
# ifdef ESER_VMX
		fprintf(Trapfile, "\\r\n\/\*\n** TIME   %s", ctime(timevec));
# else
		fprintf(Trapfile, "\\r\n\/\*\n** TIME   %s", ctime(&timevec));
# endif
		fprintf(Trapfile, "**  DB    %s\n", AAdatabase);
		getuser((char *) -1);	/*  init getuser */
		if (getuser(AAusercode, buf))
			*buf = 0;	/* cant find user code */
		else
		{
			for (cp = buf; *cp != ':'; cp++)
				continue;
			*cp = '\0';
		}
		getuser((char *) 0);	/* close getuser */
		fprintf(Trapfile, "** USER   %s (%.2s)\n", buf, AAusercode);
		fprintf(Trapfile, "** TRAP   %s\n\*\/\n\n\n", name);
	}

	if (!(iop = fopen(Qbname, "r")))
		AAsyserr(16024);
# ifdef SETBUF
	setbuf(iop, iop_buf);
# endif

	macinit(fgetc, iop, TRUE);

	while ((c = macgetch()) > 0)
		putc(c, Trapfile);

	time(&timevec);
# ifdef ESER_VMX
	fprintf(Trapfile, "\/\*\n** %s", ctime(timevec));
# else
	fprintf(Trapfile, "\/\*\n** %s", ctime(&timevec));
# endif

	if (retcode->rc_status == RC_OK)
		fprintf(Trapfile, "** %ld tuples touched\n", retcode->rc_tupcount);

	fprintf(Trapfile, "\*\/\n\\p\\g\\r\n\n");

	fflush(Trapfile);
	fclose(iop);
}
